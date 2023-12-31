/*

Copyright (c) 2013 Centre for Water Systems,
                   University of Exeter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

//! \file preProc.cpp
//! Perform the pre processing of the data for a CA 2D model.
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2013-07


#include"ca2D.hpp"
#include"ArgsData.hpp"
#include"Setup.hpp"


//! Perform the pre processing of the data for a CA 2D model. It
//! mainly save the elevation file.
//! \attention The GRID size add an extra set of cells in each directions.
int preProc(const ArgsData& ad, const Setup& setup, const std::string& ele_file)
{
    if (setup.output_computation)
    {
        std::cout << "Pre-processing : " << setup.sim_name << std::endl;
        std::cout << "------------------------------------------" << std::endl;
    }

    // ---- Timer ----

    // Get starting time.
    CA::Clock total_timer;

    // Check if pre-process data is already there
    if (CA::Grid::exist(ad.data_dir, setup.preproc_name + "_Grid", "0") &&
        CA::CellBuffReal::existData(ad.data_dir, setup.preproc_name + "_ELV", "0"))
    {
        if (setup.output_console)
            std::cout << "Pre-proc data already exist" << std::endl;
    }
    else
    {
        // -- ELEVATION FILE(s) --
        CA::AsciiGrid<CA::Real> eg;
        eg.readAsciiGrid(ele_file);

        // -- EXTRA CELLS --

        // Given the way the boundary cells are computed (see later), we
        // need to add an extra set of cells in each direction.
        CA::AsciiGrid<CA::Real> grid;
        grid.ncols = eg.ncols + 2;
        grid.nrows = eg.nrows + 2;
        grid.xllcorner = eg.xllcorner - eg.cellsize; // ATTENTION HERE!
        grid.yllcorner = eg.yllcorner - eg.cellsize; // ATTENTION HERE!
        grid.cellsize = eg.cellsize;
        grid.nodata = eg.nodata;

        // ---- CA GRID ----

        // Create the square regular CA grid of the extended size of the DEM.
        // The internal implementation could be different than a square regular grid.
        CA::Grid  GRID(grid.ncols, grid.nrows, grid.cellsize, grid.xllcorner, grid.yllcorner, ad.args.active());

        // Set if to print debug information on CA function.
        GRID.setCAPrint(false);

        // Set the place where the direct I/O data is going to be saved.
        GRID.setDataDir(ad.data_dir);

        // Save the data of the Grid
        if (!GRID.save(setup.preproc_name + "_Grid", "0"))
        {
            std::cerr << "Error while saving the GRID information" << std::endl;
            return 1;
        }

        if (setup.output_console)
            std::cout << "Saved Grid information" << std::endl;

        // Create the full (extended) computational domain of CA grid. 
        CA::BoxList  fulldomain;
        CA::Box      fullbox = GRID.box();
        fulldomain.add(fullbox);

        // Create a borders object that contains all the borders and
        // corners of the grid.
        CA::Borders borders;

        // Create the real computational domain of CA grid, i.e. the
        // original DEM size not the extended one.
        CA::BoxList  realdomain;
        CA::Box      realbox(GRID.box().x() + 1, GRID.box().y() + 1, GRID.box().w() - 2, GRID.box().h() - 2);
        realdomain.add(realbox);

        // -- INITIALISE ELEVATION ---

        // Create the elevation cell buffer.
        // It contains a "real" value in each cell of the grid.
        CA::CellBuffReal  ELV(GRID);

        // Set the border of the elevation buffer to be no data. 
        ELV.bordersValue(borders, grid.nodata);

        // Se the default value of the elevation to be nodata.
        ELV.fill(fulldomain, grid.nodata);

        // Insert the DEM data into the elevation cell buffer. The first
        // parameter is the region of the buffer to write the data which is
        // the real domain, i.e. the original non extended domain.
        ELV.insertData(realbox, &eg.data[0], eg.ncols, eg.nrows);

        // Save the data of the Elevation
        if (!ELV.saveData(setup.preproc_name + "_ELV", "0"))
        {
            std::cerr << "Error while saving the Elevation data" << std::endl;
            return 1;
        }

        if (setup.output_console)
            std::cout << "Saved Elevation data" << std::endl;
    }

    // ---- TIME OUTPUT ----

    if (setup.output_computation)
    {
        std::cout << "-----------------" << std::endl;
        std::cout << "Total run time taken (s) = " << total_timer.millisecond() / 1000.0 << std::endl;
        std::cout << "-----------------" << std::endl;
    }

    return 0;
}


int preProc_2(const std::string& data_dir, const Setup& setup, const CA::AsciiGrid<CA::Real>& eg, FILE* rptFile)
{
    if (rptFile)
    {
        fprintf(rptFile, "Pre-processing : %s\n", setup.sim_name.c_str());
        fprintf(rptFile, "------------------------------------------\n");
    }

    // ---- Timer ----

    // Get starting time.
    CA::Clock total_timer;

    // Check if pre-process data is already there
    if (CA::Grid::exist(data_dir, setup.preproc_name + "_Grid", "0") &&
        CA::CellBuffReal::existData(data_dir, setup.preproc_name + "_ELV", "0"))
    {
        if (setup.output_console)
            std::cout << "Pre-proc data already exist" << std::endl;
    }
    else
    {
        // -- EXTRA CELLS --

        // Given the way the boundary cells are computed (see later), we
        // need to add an extra set of cells in each direction.
        CA::AsciiGrid<CA::Real> grid;
        grid.ncols = eg.ncols + 2;
        grid.nrows = eg.nrows + 2;
        grid.xllcorner = eg.xllcorner - eg.cellsize; // ATTENTION HERE!
        grid.yllcorner = eg.yllcorner - eg.cellsize; // ATTENTION HERE!
        grid.cellsize = eg.cellsize;
        grid.nodata = eg.nodata;

        // ---- CA GRID ----

        // Create the square regular CA grid of the extended size of the DEM.
        // The internal implementation could be different than a square regular grid.
        CA::Grid  GRID(grid.ncols, grid.nrows, grid.cellsize, grid.xllcorner, grid.yllcorner);

        // Set if to print debug information on CA function.
        GRID.setCAPrint(false);

        // Set the place where the direct I/O data is going to be saved.
        GRID.setDataDir(data_dir);

        // Save the data of the Grid
        if (!GRID.save(setup.preproc_name + "_Grid", "0"))
        {
            std::cerr << "Error while saving the GRID information" << std::endl;
            return 1;
        }

        if (rptFile)
            fprintf(rptFile, "Saved Grid information\n");

        // Create the full (extended) computational domain of CA grid. 
        CA::BoxList  fulldomain;
        CA::Box      fullbox = GRID.box();
        fulldomain.add(fullbox);

        // Create a borders object that contains all the borders and
        // corners of the grid.
        CA::Borders borders;

        // Create the real computational domain of CA grid, i.e. the
        // original DEM size not the extended one.
        CA::BoxList  realdomain;
        CA::Box      realbox(GRID.box().x() + 1, GRID.box().y() + 1, GRID.box().w() - 2, GRID.box().h() - 2);
        realdomain.add(realbox);

        // -- INITIALISE ELEVATION ---

        // Create the elevation cell buffer.
        // It contains a "real" value in each cell of the grid.
        CA::CellBuffReal  ELV(GRID);

        // Set the border of the elevation buffer to be no data. 
        ELV.bordersValue(borders, grid.nodata);

        // Se the default value of the elevation to be nodata.
        ELV.fill(fulldomain, grid.nodata);

        // Insert the DEM data into the elevation cell buffer. The first
        // parameter is the region of the buffer to write the data which is
        // the real domain, i.e. the original non extended domain.
        ELV.insertData(realbox, &eg.data[0], eg.ncols, eg.nrows);

        // Save the data of the Elevation
        if (!ELV.saveData(setup.preproc_name + "_ELV", "0"))
        {
            std::cerr << "Error while saving the Elevation data" << std::endl;
            return 1;
        }

        if (rptFile)
            fprintf(rptFile, "Saved Elevation data\n");
    }

    // ---- TIME OUTPUT ----

    if (rptFile)
    {
        fprintf(rptFile, "Total run time taken (s) = %f\n", total_timer.millisecond() / 1000.0);
        fprintf(rptFile, "-----------------\n");
    }

    return 0;
}

