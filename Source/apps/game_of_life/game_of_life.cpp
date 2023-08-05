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

//! \file game_of_life.cpp
//! Simple command line game of life simulation using CA algorithms.
//! \author Mike Gibson, University of Exeter, 
//! contact: mg278 [at] exeter.ac.uk
//! \date 2012-06

#include<cstdlib>
#include<iostream>
#include<vector>
#include<fstream>
#include<stdexcept>

// Add the header files of the CA API.
#include"ca2D.hpp"
#include"Arguments.hpp"
#include"Options.hpp"


// Check if the version of the CAAPI is supported.
#if !CAAPI_VERSION >= 100
#error "CAAPI Version is not supported, i.e. less than 100."
#endif

// ----------------------------//
// Include the CA 2D functions //
// ----------------------------//
#include CA_2D_INCLUDE(computeStatesN1)


//! Print the version info to std output.
inline void version(std::ostream& out)
{
    out << "Copyright 2013 Centre for Water Systems, University of Exeter" << std::endl;
    out << "App                  : " << CA_QUOTE_MACRO(CAAPI_APP_NAME) << " ver. " << CAAPI_APP_VERSION << std::endl;
    out << "CA API Version       : " << caVersion << std::endl;
    out << "       Impl Name     : " << caImplName << std::endl;
    out << "       Impl Short    : " << caImplShortName << std::endl;
    out << "       Impl Version  : " << caImplVersion << std::endl;
    out << "       Impl Precision: " << caImplPrecision << std::endl;
}


// Execute the main loop of the Game of Life.
void run_ca(std::string asciigrid_filename,
    int max_iterations,
    double& run_time,
    std::ostream* out,
    bool results)
{
    // ------  PREPROCESS ARGUMENTS ------
    if (asciigrid_filename.empty())
        asciigrid_filename = "basic_grid_data.txt";

    // by default, do not allow 0 iteration, deafult instead to 10...
    if (max_iterations == 0)
        max_iterations = 10;

    if (out)
    {
        version((*out));
    }

    // ----  INPUT DATA ----

    // Read the Basic Grid in ARC/INFO ASCII GRID format
    CA::AsciiGrid<CA::State> asciigrid;
    asciigrid.readAsciiGrid(asciigrid_filename);

    // ----  Timer ----

    // Get starting time.
    CA::Clock total_timer;

    // ----  CA GRID ----

    // Create the square regular CA grid of the same size of the basic ascii grid.
    // The internal implementation could be different than a square regular grid.
    CA::Grid  GRID(asciigrid.ncols, asciigrid.nrows,
        asciigrid.cellsize, asciigrid.xllcorner, asciigrid.yllcorner);

    // Set if to print debug information on CA function.
    GRID.setCAPrint(false);

    // Create the computational domain of CA grid. This is a list of
    // boxes (areas) where to compute the CA function.
    CA::BoxList  domain;

    // In this case, the domain is composed of a box of the same size
    // of the DEM data.
    domain.add(GRID.box());

    // Create a borders object that contains all the borders and
    // corners of the grid.
    CA::Borders borders;

    borders.addSegment(CA::Top);
    borders.addSegment(CA::Bottom);
    borders.addSegment(CA::Right);
    borders.addSegment(CA::Left);

    borders.addCorner(CA::TopLeft);
    borders.addCorner(CA::TopRight);
    borders.addCorner(CA::BottomLeft);
    borders.addCorner(CA::BottomRight);

    // ----  CELL BUFFERS ----

    // Implementing a two grid system, which in each generation only
    // processes once from one to other, and then vice versa.

    // grid A
    CA::CellBuffState grid_A(GRID);

    // grid B
    CA::CellBuffState grid_B(GRID);


    // ----  SCALAR VALUES ----

    CA::Unsigned nnn = max_iterations;    // Maximum number of iterations.
    CA::Unsigned n = 1;         // The iteration number.

    CA::State nodata = asciigrid.nodata;


    // -- INITIALISE ---

    // Write the basic grid data into the asciigrid cell buffer. The first
    // parameter is the region of the buffer to write the data. In
    // this case all the CA grid.

    // Insert the data into the grid A, then on the first we'll compute the states
    // and write to grid B, then vice verse, and so on...
    grid_A.insertData(GRID.box(), &asciigrid.data[0], asciigrid.ncols, asciigrid.nrows);

    //---------------------- border setting here ---------

    // Set a static value of dead(0).
    grid_A.bordersValue(borders, 0);
    grid_B.bordersValue(borders, 0);


    // ------------------------- MAIN LOOP -------------------------------  
    while (n <= nnn)
    {
        //-------------------- process each generation -------------

        // Call the CA Function that change the process one iteration of
        // the game of life.
        // NOTE: that interation/generation number starts at 1.
        if (n % 2 == 1)
        {
            // A->B
            CA::Execute::function(domain, computeStatesN1, GRID, grid_A, grid_B);
        }
        else
        {
            // B->A
            CA::Execute::function(domain, computeStatesN1, GRID, grid_B, grid_A);
        }

        // -------------- Increse iteration.----------------
        ++n;

    }// end of iteration loop.......
    // ------------------------- MAIN LOOP END ---------------------------

    // ----  produce end result ------

    // NOTE: this function therefore only reads the final data back to the
    // grid if the results arguement is passed....

    if (results)
    {
        // Write the data back from the CAAPI buffer to the local asciigrid.
        // NOTE: we must now dettermine which buffer we wrote the last generation/
        // iteration too.
        if (nnn % 2 == 1)
        {
            // process a gen/it therefore retrieve from B...
            grid_A.retrieveData(GRID.box(), &asciigrid.data[0], asciigrid.ncols, asciigrid.nrows);
        }
        else
        {
            // otherwise vice verse(still need to free the other grid) from A...
            grid_B.retrieveData(GRID.box(), &asciigrid.data[0], asciigrid.ncols, asciigrid.nrows);
        }

        asciigrid.nodata = nodata;

        // Write the result into an ASCII/GRID file
        asciigrid.writeAsciiGrid("result.txt");
    }

    // ----  Timer ----

    // Calculate total time taken in second.  
    run_time = total_timer.millisecond() / 1000.0;

}// end of run_ca


//! Separate output text.
const std::string dash_line("-------------------------------------------------------------------------------");


// Main function
int main(int argc, char*argv[])
{
    // Initialise the 2D caAPI.
    CA::init2D(&argc, &argv);

    // Create the arguments list and define the prefix which identifies
    // the arguments that are optional. This prefix differs between
    // windows and unix.
#if defined _WIN32 || defined __CYGWIN__   
    CA::Arguments args("/");
#else
    CA::Arguments args("-");
#endif

    // There are none compulsory arguments.
    // Add optional arguments.
    args.add(0, "help", "Display the help and exit.", "", true, false, true);
    args.add(1, "data", "The initial state data in ARC/INFO ASCII GRID input file.", "", true, true);
    args.add(2, "niters", "The maximum number of iterations.", "0", true, true);
    args.add(3, "nruns", "The number of runs of the CA algorithm.", "1", true, true);
    args.add(4, "results", "Produce the results files of the CA algorithm.", "", true, false);
    args.add(5, "output", "Output to screen.", "", true, false);
    args.add(6, "version", "Show the version of the code.", "", true, false, true);

    // Add the options from the CA implementation
    args.addList(CA::options());

    // Parse the argument list, if the parse fail (it returns false) then 
    // exit!
    if (args.parse(argc, argv, std::cout) == false)
    {
#if defined _WIN32 || defined __CYGWIN__   
        std::cout << "Usage: /help option to show help" << std::endl;
        std::cout << "Press 'Return' to continue" << std::endl;
        std::cin.get();
#else
        std::cout << "Usage: -help option to show help" << std::endl;
        std::cin.get();
#endif
        return 1;
    }

    // Variables used in the arguments.
    std::string   asciigrid_filename("");
    int           niters = 0;
    int           nruns = 1;
    bool          results = false;
    std::ostream* out = 0;

    // Cycle throught the agument that were activated
    for (CA::Arguments::List::const_iterator i = args.active().begin(); i != args.active().end(); ++i)
    {
        // Execute the action depending on the argument.    
        if ((*i)->name == "help")
        {
            args.help(std::cout, true);
            return (EXIT_SUCCESS);
        }

        if ((*i)->name == "data")
        {
            asciigrid_filename = (*i)->value;
        }

        if ((*i)->name == "niters")
        {
            if (!CA::fromString(niters, (*i)->value))
            {
                std::cout << "Error: need a number after option'" << (*i)->name << "'" << std::endl;
                return (EXIT_FAILURE);
            }
            niters = std::max(0, niters);
        }

        if ((*i)->name == "nruns")
        {
            if (!CA::fromString(nruns, (*i)->value))
            {
                std::cout << "Error: need a number after option'" << (*i)->name << "'" << std::endl;
                return (EXIT_FAILURE);
            }
            nruns = std::max(1, nruns);
        }

        if ((*i)->name == "results")
        {
            results = true;
        }

        if ((*i)->name == "output")
        {
            out = &std::cout;
        }

        if ((*i)->name == "version")
        {
            version(std::cout);
            return EXIT_SUCCESS;
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // main loop/function call

    // Store the total run time of all the runs.
    double total_run_time = 0.0;

    for (unsigned int test = 0; test < static_cast<unsigned int>(nruns); test++)
    {
        // Store the run time of the single run.
        double run_time = 0.0;

        std::cout << dash_line << std::endl;
        std::cout << "Run [" << test << "] of CA algorithm: " << std::endl;
        std::cout << dash_line << std::endl;

        try
        {
            // Run the ca.
            run_ca(asciigrid_filename, niters, run_time, out, results);
        }
        catch (std::exception& e)
        {
            std::cout << "Error: " << std::endl;
            std::cout << e.what() << std::endl;
            return (EXIT_FAILURE);
        }

        std::cout << dash_line << std::endl;
        std::cout << "End [" << test << "] on " << run_time << " (s)" << std::endl;

        total_run_time += run_time;
    }

    // Compute run time.

    double average_time = total_run_time / nruns;

    std::cout << dash_line << std::endl;
    std::cout << "Total run time taken   (s)   = " << total_run_time << std::endl;
    std::cout << "Average run time taken (s)   = " << average_time << std::endl;
    std::cout << dash_line << std::endl;

    // Close the caAPI.
    CA::finalise2D();

    return EXIT_SUCCESS;
}
