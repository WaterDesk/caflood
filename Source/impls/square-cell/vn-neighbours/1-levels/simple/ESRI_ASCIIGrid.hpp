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

#ifndef _CA_ESRI_ASCIIGRID_HPP_
#define _CA_ESRI_ASCIIGRID_HPP_


//! \file AsciiGrid.hpp
//! Contains the structures and methods to read a ARC/INFO ASCII GRID
//! format file.
//! \author Michele Guidolin, University of Exeter, 
//! \author Mike Gibson, University of Exeter, 
//! contact: m.j.gibson [at] exeter.ac.uk
//! \date 2012-01


#include "AsciiGrid.hpp"


#ifdef CAAPI_GDAL_OPTION
//#include "GDAL_Functions.hpp"
#include "gdal_priv.h"

#endif


namespace CA {

    template<typename T>
    class ESRI_ASCIIGrid
        : public CA::AsciiGridGeneral<T>
    {
    public:

        //public ESRI_ASCIIGrid<T>() : CA::AsciiGridGeneral<T>(){
        //};

#ifdef CAAPI_GDAL_OPTION
        //static std::string driver = "AAIGrid";
        static std::string driver;
        //static int epsgCode = 27700;
        static int epsgCode;
        static bool createCapable;
        static bool createCopyCapable;

#endif


        //! Read token and value pair from a line of the header of an ARC/INFO
        //! ASCII GRID file.
        template<typename A>
        inline
            void readAsciiGridHeaderLine(const std::string& filename, std::ifstream& infile, A& value,
                const char* check, bool substring = false)
        {
            std::string token;
            std::string strvalue;

            // Read the token and value. Check that the token is
            // the desire one. Add the value into grid.ncols.
            if (!(infile >> token) || !(infile >> strvalue))
                throw std::runtime_error(std::string("Error reading the file: ") + filename);

            if (!CA::compareCaseInsensitive(token, check, substring))
                throw std::runtime_error(std::string("Error not an ARC/INFO ASCII GRID file: ") + filename);

            if (!CA::fromString(value, strvalue))
                throw std::runtime_error(std::string("Error converting the string ") + strvalue
                    + std::string(" into a value from the file: ") + filename);
        }

        //! Read a ARC/INFO ASCII GRID format file
        //template<typename T>
        inline  void readAsciiGrid(const std::string& filename, bool print = false)
        {
            // Get starting time.
            //Clock total_timer;

#ifndef CAAPI_GDAL_OPTION
            //std::cout << "READING DATA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

            // Open file
            std::ifstream infile(filename.c_str());

            // Check if the file was open.
            if (!infile)
                throw std::runtime_error(std::string("Error opening file: ") + filename);

            // Read the header  
            readAsciiGridHeaderLine<CA::Unsigned>(filename, infile, AsciiGridGeneral<T>::ncols, "ncols", false);
            readAsciiGridHeaderLine<CA::Unsigned>(filename, infile, AsciiGridGeneral<T>::nrows, "nrows", false);
            readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::xllcorner, "xllcorner", false);
            readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::yllcorner, "yllcorner", false);
            readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::cellsize, "cellsize", false);
            readAsciiGridHeaderLine<T>(filename, infile, AsciiGridGeneral<T>::nodata, "nodata_value", true);

            if (print)
            {
                std::cout << "ncols \t\t" << AsciiGridGeneral<T>::ncols << std::endl;
                std::cout << "nrows \t\t" << AsciiGridGeneral<T>::nrows << std::endl;
                std::cout << "xllcorner \t" << AsciiGridGeneral<T>::xllcorner << std::endl;
                std::cout << "yllcorner \t" << AsciiGridGeneral<T>::yllcorner << std::endl;
                std::cout << "cellsize \t" << AsciiGridGeneral<T>::cellsize << std::endl;
                std::cout << "nodata \t\t" << AsciiGridGeneral<T>::nodata << std::endl;
                std::cout << std::endl;
            }

            // Allocate data and set the default value to nodata.
            AsciiGridGeneral<T>::data.clear();
            AsciiGridGeneral<T>::data.resize(AsciiGridGeneral<T>::ncols * AsciiGridGeneral<T>::nrows, AsciiGridGeneral<T>::nodata);

            // Read data.
            T value;
            size_t  i = 0;
            while (infile >> value && i < AsciiGridGeneral<T>::data.size())
            {
                if (infile.fail())
                    throw std::runtime_error(std::string("Error converting a data string into a value"));

                AsciiGridGeneral<T>::data[i] = value;
                i++;
            }

            // Close the file.
            infile.close();

            //std::cout<<"Read run time taken (s) = " << total_timer.millisecond()/1000.0 << std::endl;
#else
            GDALAllRegister();

            GDALDataset* dataSet;

            dataSet = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);

            // ulx, xres, xskew, uly, yskew, yres 
            double adfGeoTransform[6];
            dataSet->GetGeoTransform(adfGeoTransform);

            AsciiGridGeneral<T>::ncols = dataSet->GetRasterXSize();
            AsciiGridGeneral<T>::nrows = dataSet->GetRasterYSize();
            AsciiGridGeneral<T>::xllcorner = adfGeoTransform[0];
            // check for that negative?
            AsciiGridGeneral<T>::yllcorner = adfGeoTransform[3] + (dataSet->GetRasterYSize() * adfGeoTransform[5]);

            // check for negative and that they are both the same value?
            AsciiGridGeneral<T>::cellsize = adfGeoTransform[1];

            GDALRasterBand* poBand;

            // might be zero in c++?
            poBand = dataSet->GetRasterBand(1);

            AsciiGridGeneral<T>::nodata = poBand->GetNoDataValue();

            // Allocate data and set the default value to nodata.
            AsciiGridGeneral<T>::data.clear();
            AsciiGridGeneral<T>::data.resize(AsciiGridGeneral<T>::ncols * AsciiGridGeneral<T>::nrows, AsciiGridGeneral<T>::nodata);

            CA::Real* pafScanline;
            int   nXSize = poBand->GetXSize();
            int   nYSize = poBand->GetYSize();
            //pafScanline = (CA::Real*)CPLMalloc(sizeof(CA::Real) * nXSize);

            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize,
                static_cast<void*>(AsciiGridGeneral<T>::data.data()), nXSize, nYSize, GDT_Float64,
                0, 0);

            //delete(poBand);
            delete(dataSet);
#endif
        }//end of readAsciigrid ----------------------------------------------------------------------


        //! Read a ARC/INFO ASCII GRID format file without loading teh data.
        //template<typename T>
        inline  void readAsciiGridHeader(const std::string& filename, bool print = false)
        {
#ifndef CAAPI_GDAL_OPTION
            // Open file
            std::ifstream infile(filename.c_str());

            // Check if the file was open.
            if (!infile)
                throw std::runtime_error(std::string("Error opening file: ") + filename);

            // Read the header  
            readAsciiGridHeaderLine<CA::Unsigned>(filename, infile, AsciiGridGeneral<T>::ncols, "ncols", false);
            readAsciiGridHeaderLine<CA::Unsigned>(filename, infile, AsciiGridGeneral<T>::nrows, "nrows", false);
            readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::xllcorner, "xllcorner", false);
            readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::yllcorner, "yllcorner", false);
            readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::cellsize, "cellsize", false);
            readAsciiGridHeaderLine<T>(filename, infile, AsciiGridGeneral<T>::nodata, "nodata_value", true);

            if (print)
            {
                std::cout << "ncols \t\t" << AsciiGridGeneral<T>::ncols << std::endl;
                std::cout << "nrows \t\t" << AsciiGridGeneral<T>::nrows << std::endl;
                std::cout << "xllcorner \t" << AsciiGridGeneral<T>::xllcorner << std::endl;
                std::cout << "yllcorner \t" << AsciiGridGeneral<T>::yllcorner << std::endl;
                std::cout << "cellsize \t" << AsciiGridGeneral<T>::cellsize << std::endl;
                std::cout << "nodata \t\t" << AsciiGridGeneral<T>::nodata << std::endl;
                std::cout << std::endl;
            }

            // Close the file.
            infile.close();
#else
            GDALAllRegister();

            GDALDataset* dataSet;

            dataSet = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);

            // ulx, xres, xskew, uly, yskew, yres 
            double adfGeoTransform[6];
            dataSet->GetGeoTransform(adfGeoTransform);

            AsciiGridGeneral<T>::ncols = dataSet->GetRasterXSize();
            AsciiGridGeneral<T>::nrows = dataSet->GetRasterYSize();
            AsciiGridGeneral<T>::xllcorner = adfGeoTransform[0];
            // check for that negative?
            AsciiGridGeneral<T>::yllcorner = adfGeoTransform[3] - (dataSet->GetRasterYSize() * abs(adfGeoTransform[5]));

            // check for negative and that they are both the same value?
            AsciiGridGeneral<T>::cellsize = adfGeoTransform[1];

            GDALRasterBand* poBand;

            // might be zero in c++?
            poBand = dataSet->GetRasterBand(1);

            AsciiGridGeneral<T>::nodata = poBand->GetNoDataValue();

            //delete(poBand);
            delete(dataSet);
#endif

            // Empty the data
            AsciiGridGeneral<T>::data.clear();

        }// end of readAsciiGridHeader


        void writeAAIGrid(const std::string& filename,
            int decimal_places = 6, bool print = false)
        {
            // Open file
            std::ofstream onfile(filename.c_str());

            // Check if the file was open.
            if (!onfile)
                throw std::runtime_error(std::string("Error opening file: " + filename));

            // Set  manipulators 
            onfile.unsetf(std::ios::floatfield);
            onfile.precision(12);

            // Write the header 
            onfile << "ncols \t\t" << AsciiGridGeneral<T>::ncols << std::endl;
            onfile << "nrows \t\t" << AsciiGridGeneral<T>::nrows << std::endl;
            onfile << "xllcorner \t" << AsciiGridGeneral<T>::xllcorner << std::endl;
            onfile << "yllcorner \t" << AsciiGridGeneral<T>::yllcorner << std::endl;
            onfile << "cellsize \t" << AsciiGridGeneral<T>::cellsize << std::endl;
            onfile << "NODATA_value \t\t" << AsciiGridGeneral<T>::nodata << std::endl;
            onfile << std::endl;

            // Make sure the decimal places is not zeros.
            if (decimal_places <= 0)
                decimal_places = 6;

            onfile.unsetf(std::ios::floatfield);
            onfile << std::fixed;
            onfile.precision(decimal_places);

            // Loop through the data.
            for (unsigned int i = 0; i < AsciiGridGeneral<T>::ncols* AsciiGridGeneral<T>::nrows; ++i)
            {
                if (i % AsciiGridGeneral<T>::ncols == 0)
                    onfile << "\n";
                onfile << AsciiGridGeneral<T>::data[i] << " ";
            }

            // End of line.
            onfile << std::endl;

            // Close the file.
            onfile.close();
        }


        //! Write a ARC/INFO ASCII GRID format file
        //template<typename T>
        inline void writeAsciiGrid(const std::string& filename,
            int decimal_places = 6, bool print = false)
        {
            // Get starting time.
            //Clock total_timer;

#ifndef CAAPI_GDAL_OPTION

#ifdef CA2D_MPI
            //MPI_Comm_size(MPI_COMM_WORLD, &_commSize);
            int _rank;

            //MPI_Comm_size(MPI_COMM_WORLD, &_cagrid.comm_size);

            //MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
            MPI_Comm_rank(MPI_COMM_WORLD, &_rank);

            std::stringstream subGridIdStream;
            subGridIdStream << _rank;

            // Add file extension
            String _filename = filename + "_" + subGridIdStream.str() + ".asc";
#else
            // Add file extension
            String _filename = filename + ".asc";
#endif // CA2D_MPI
            writeAAIGrid(_filename, decimal_places, print);
#else
            /*
            if (filename.compare("AAIGrid") == 0) {

                // Add file extension
                String _filename = filename + ".asc";
                writeAAIGrid(_filename, decimal_places, print);
                return;
            }*/

            GDALAllRegister();
            GDALDriver* poDriver;
            std::string extension = ".";

            if (ESRI_ASCIIGrid<T>::createCapable == false && ESRI_ASCIIGrid<T>::createCopyCapable) {
                poDriver = GetGDALDriverManager()->GetDriverByName("MEM");
            }
            else {
                poDriver = GetGDALDriverManager()->GetDriverByName(ESRI_ASCIIGrid<T>::driver.c_str());
                extension += poDriver->GetMetadataItem(GDAL_DMD_EXTENSION);
            }

            // Add file extension
            String _filename = filename + extension;

            GDALDataset* dataSet;
            char** papszOptions = NULL;
            dataSet = poDriver->Create(_filename.c_str(), AsciiGridGeneral<T>::ncols, AsciiGridGeneral<T>::nrows, 1,
                GDT_Float64,
                papszOptions);

            // ulx, xres, xskew, uly, yskew, yres 
            double adfGeoTransform[6];
            //dataSet->GetGeoTransform(adfGeoTransform);

            adfGeoTransform[1] = AsciiGridGeneral<T>::cellsize;
            adfGeoTransform[5] = -AsciiGridGeneral<T>::cellsize;

            adfGeoTransform[0] = AsciiGridGeneral<T>::xllcorner;

            adfGeoTransform[3] = AsciiGridGeneral<T>::yllcorner + (AsciiGridGeneral<T>::nrows* AsciiGridGeneral<T>::cellsize);

            // check for negative and that they are both the same value?
            //adfGeoTransform[1] = AsciiGridGeneral<T>::cellsize;
            //adfGeoTransform[4] = AsciiGridGeneral<T>::cellsize;
            adfGeoTransform[2] = 0;
            adfGeoTransform[4] = 0;

            dataSet->SetGeoTransform(adfGeoTransform);

            GDALRasterBand* poBand;

            // might be zero in c++?
            poBand = dataSet->GetRasterBand(1);

            poBand->SetNoDataValue(AsciiGridGeneral<T>::nodata);

            // Allocate data and set the default value to nodata.
            //AsciiGridGeneral<T>::data.clear();
            //AsciiGridGeneral<T>::data.resize(AsciiGridGeneral<T>::ncols * AsciiGridGeneral<T>::nrows, AsciiGridGeneral<T>::nodata);

            int   nXSize = poBand->GetXSize();
            int   nYSize = poBand->GetYSize();
            //pafScanline = (CA::Real*)CPLMalloc(sizeof(CA::Real) * nXSize);

            OGRSpatialReference oSRS;
            //OSRImportFromEPSG(osr, AsciiGridGeneral<T>::epsgCode);
            char* pszSRS_WKT = NULL;

            //oSRS.SetWellKnownGeogCS("NAD27");
            oSRS.importFromEPSG(ESRI_ASCIIGrid<T>::epsgCode);
            oSRS.exportToWkt(&pszSRS_WKT);
            dataSet->SetProjection(pszSRS_WKT);

            free(pszSRS_WKT);

            poBand->RasterIO(GDALRWFlag::GF_Write, 0, 0, nXSize, nYSize,
                static_cast<void*>(AsciiGridGeneral<T>::data.data()), nXSize, nYSize, GDT_Float64,
                0, 0);

            //GDAlClose((GDALDataSet)dataSet);

            poBand->FlushCache();

            dataSet->FlushCache();

            if (ESRI_ASCIIGrid<T>::createCapable == false && ESRI_ASCIIGrid<T>::createCopyCapable) {
                poDriver = GetGDALDriverManager()->GetDriverByName(ESRI_ASCIIGrid<T>::driver.c_str());
                extension += poDriver->GetMetadataItem(GDAL_DMD_EXTENSION);
                _filename = filename + extension;

                GDALCreateCopy(poDriver, _filename.c_str(), dataSet, false, NULL, NULL, NULL);
            }

            //delete(poBand);
            delete(dataSet);
            //delete(poDriver);
#endif
                        //std::cout<<"Write run time taken (s) = " << total_timer.millisecond()/1000.0 << std::endl;    
        }// end of write function


        //------------------------------------------------------------------------------------------------------------------------------
        //! Add another ascii gri dof data to this grid of data
        inline void addAsciiGrid(AsciiGrid<T>& grid2, bool print = false) {
            // find how many cells between bottom left of each terrain.
            int colsDiff = 0;
            int rowsDiff = 0;

            if (print)
                std::cout << "BLAH" << std::endl;

            // used to be a utility for converting real to unsigned, bewarned linux ppl

            colsDiff = ((CA::Unsigned)grid2.xllcorner) - ((CA::Unsigned)AsciiGridGeneral<T>::xllcorner);
            rowsDiff = ((CA::Unsigned)grid2.yllcorner) - ((CA::Unsigned)AsciiGridGeneral<T>::yllcorner);

            // check for case without alignment?

            if (print) {
                std::cout << "This grid:" << std::endl;
                std::cout << "This grid: xllCorner: " << AsciiGridGeneral<T>::xllcorner << std::endl;
                std::cout << "This grid: yllCorner: " << AsciiGridGeneral<T>::yllcorner << std::endl;
                std::cout << "This grid: nrows    : " << AsciiGridGeneral<T>::nrows << std::endl;
                std::cout << "This grid: ncols    : " << AsciiGridGeneral<T>::ncols << std::endl;

                std::cout << "That grid:" << std::endl;
                std::cout << "That grid: xllCorner: " << grid2.xllcorner << std::endl;
                std::cout << "That grid: yllCorner: " << grid2.yllcorner << std::endl;
                std::cout << "That grid: nrows    : " << grid2.nrows << std::endl;
                std::cout << "That grid: ncols    : " << grid2.ncols << std::endl;

                std::cout << "Cols Diff    : " << colsDiff << std::endl;
                std::cout << "Rows Diff    : " << rowsDiff << std::endl;
            }

            // check for overlap in x direction
            CA::Unsigned thisXLeft = (CA::Unsigned)AsciiGridGeneral<T>::xllcorner;
            CA::Unsigned thisXRight = ((CA::Unsigned)AsciiGridGeneral<T>::xllcorner) + AsciiGridGeneral<T>::ncols;
            CA::Unsigned thatXLeft = (CA::Unsigned)grid2.xllcorner;
            CA::Unsigned thatXRight = ((CA::Unsigned)grid2.xllcorner) + grid2.ncols;

            CA::Unsigned thisYBottom = (CA::Unsigned)AsciiGridGeneral<T>::yllcorner;
            CA::Unsigned thisYTop = ((CA::Unsigned)AsciiGridGeneral<T>::yllcorner) + AsciiGridGeneral<T>::nrows;
            CA::Unsigned thatYBottom = (CA::Unsigned)grid2.yllcorner;
            CA::Unsigned thatYTop = ((CA::Unsigned)grid2.yllcorner) + grid2.nrows;

            if (print) {
                std::cout << "thisXLeft    : " << thisXLeft << std::endl;
                std::cout << "thatXLeft    : " << thatXLeft << std::endl;
                std::cout << std::endl;
                std::cout << "thisXRight    : " << thisXRight << std::endl;
                std::cout << "thatXRight    : " << thatXRight << std::endl;
                std::cout << std::endl;
                std::cout << "thisYBottom    : " << thisYBottom << std::endl;
                std::cout << "thatYBottom    : " << thatYBottom << std::endl;
                std::cout << std::endl;
                std::cout << "thisYTop    : " << thisYTop << std::endl;
                std::cout << "thatYTop    : " << thatYTop << std::endl;
                std::cout << std::endl;
            }

            //          if( ( (thatXleft >= thisXleft) && (thatXLeft < thisXRight) ) ||
            //              ( (thatXRight >= thisXleft) && (thatXRight < thisXRight))
            //              )

            // check for not overlap
            if (
                ((thisXRight < thatXLeft) || (thatXRight < thisXLeft)) // not overlap in x direction...
                || // if no overlap in either or both then not overlapping area
                ((thisYTop < thatYBottom) || (thatYTop < thisYBottom)) // not overlap in y direction...
                )
            {
                if (print) {
                    std::cout << "No Overlapiing area, so returning from adding therrain data." << std::endl;
                }

                // no overlapping area, bail out, return message somehow...
                return; // bail as nothing to do here
            }

            // otherwise there is overlap

            // calculate the range, i.e. distance of overlaps
            CA::Unsigned colsOverlap = 0;
            CA::Unsigned rowsOverlap = 0;

            if (thatXLeft < thisXLeft) {
                // then we start on this grid left most cells.
                // next we check to see if that grid totally covers the x direction of this grid
                if (thatXRight > thisXRight) {
                    colsOverlap = AsciiGridGeneral<T>::ncols;
                }
                else {
                    // otherwise that grid, end somewhere before this grids x extent.
                    colsOverlap = thatXRight - thisXLeft;
                }
            }
            else {
                //otherwise, that grids left most, starts inside this grids extent
                if (thatXRight < thisXRight)
                {
                    // therefore that grids extent is totaly within this grid
                    colsOverlap = grid2.ncols;
                }
                else {
                    // otherwise this grid ends before that one
                    colsOverlap = thisXRight - thatXLeft;
                }
            }

            if (thatYBottom < thisYBottom) {
                // then we start on this grid Bottom most cells.
                // next we check to see if that grid totally covers the y direction of this grid
                if (thatYTop > thisYTop) {
                    rowsOverlap = AsciiGridGeneral<T>::nrows;
                }
                else {
                    // otherwise that grid, end somewhere before this grids y extent.
                    rowsOverlap = thatYTop - thisYBottom;
                }
            }
            else {
                //otherwise, that grids bottom most, starts inside this grids extent
                if (thatYTop < thisYTop)
                {
                    // therefore that grids extent is totaly within this grid
                    rowsOverlap = grid2.nrows;
                }
                else {
                    // otherwise this grid ends before that one
                    rowsOverlap = thisYTop - thatYBottom;
                }
            }

            // so we can now loop over the area (columns and rows) of the overlapping area.

            // NOTE: that geo co-ordinates start at bottom left, and overlap calculations are based on these, therefore
            // a conversion to the local co-ordinates which are top left are required.

            CA::Unsigned thisXInset = 0;
            CA::Unsigned thisYInset = 0;
            CA::Unsigned thatXInset = 0;
            CA::Unsigned thatYInset = 0;

            if (colsDiff >= 0) {
                thisXInset = colsDiff;
            }
            else {
                thatXInset = -colsDiff;
            }

            if (rowsDiff >= 0) {
                thisYInset = rowsDiff;
            }
            else {
                thatYInset = -rowsDiff;
            }

            // now we know the inset due to over lap, based on bottom left.

            for (CA::Unsigned col = 0; col < colsOverlap; ++col) {
                for (CA::Unsigned row = 0; row < rowsOverlap; ++row) {
                    // convert to local cordinates (from bottom left to top left)
                    CA::Unsigned thisGridXCord = col + thisXInset;
                    CA::Unsigned thisGridYCord = (AsciiGridGeneral<T>::nrows - 1) - (row + thisYInset);
                    CA::Unsigned thatGridXCord = col + thatXInset;
                    CA::Unsigned thatGridYCord = (grid2.nrows - 1) - (row + thatYInset);

                    //T thisDataPoint = AsciiGridGeneral<T>::data[thisGridXCord + (thisGridYCord * AsciiGridGeneral<T>::ncols)];
                    //T thatDataPoint = grid2.data[thatGridXCord + (thatGridYCord * grid2.ncols)];

                    //------------------------------------

                    AsciiGridGeneral<T>::data[thisGridXCord + (thisGridYCord * AsciiGridGeneral<T>::ncols)] =
                        grid2.data[thatGridXCord + (thatGridYCord * grid2.ncols)];
                }// end of looping overlap grids rows
            }// end of looping overlap grids coloumns

        }// end of addAsciiGrid

        //#######################################################################################################################################
        //###########################################   comparitor functions            ########################################################
        //#######################################################################################################################################

        //--------------------------------------------------------------------------------------------------------------
        //! Compare the RMSE of two ascii grids, and return the errors
        //! if the same cell from either grid is no data, then no comparison is made
        //! note this object is considered the observation (truth) to compare to the other simulation.
        inline  Errors compareAsciiGrid(AsciiGrid<T>& grid2, T tolerance = 0.0, bool print = false)
        {
            //
            //double result = 0.0;
            //int count = 0;

            // note needs checking for cell size usuage?!.............

            // erros structure used, to reduce multiple loops
            Errors result;

            result.RMSE = 0.0;
            result.RMSE_wet_both = 0.0;
            result.RMSE_wet_either = 0.0;

            result.meanError = 0.0;
            result.meanError_wet_both = 0.0;
            result.meanError_wet_either = 0.0;

            result.accuracy = 0.0;
            result.sensitivity = 0.0;
            result.precision = 0.0;
            result.precentage = 0.0;

            result.nashSutcliffe = 0.0;

            result.truePositive = 0;
            result.falsePositive = 0;
            result.trueNegative = 0;
            result.falseNegative = 0;

            result.thisWettedCount = 0;
            result.thatWettedCount = 0;
            result.combinedWettedCount = 0;
            result.allDataCells = 0;
            result.eitherWetted = 0;

            // for nash sutcliffe we need the mean of the observed (truth), i.e. this ascii grids data
            double meanObserved = 0.0;
            double nashSumSquaredErrors = 0.0;
            double nashSumDataVariance = 0.0;

            // do a check for size mismatch here.
            // due to geo-cor-ordinates, we must check only the region that overlaps

            // find how many cells between bottom left of each terrain.
            int colsDiff = 0;
            int rowsDiff = 0;

            // used to be a utility for converting real to unsigned, bewarned linux ppl

            colsDiff = ((CA::Unsigned)grid2.xllcorner) - ((CA::Unsigned)AsciiGridGeneral<T>::xllcorner);
            rowsDiff = ((CA::Unsigned)grid2.yllcorner) - ((CA::Unsigned)AsciiGridGeneral<T>::yllcorner);

            // check for case without alignment?

            // check for overlap in x direction
            CA::Unsigned thisXLeft = (CA::Unsigned)AsciiGridGeneral<T>::xllcorner;
            CA::Unsigned thisXRight = ((CA::Unsigned)AsciiGridGeneral<T>::xllcorner) + AsciiGridGeneral<T>::ncols;
            CA::Unsigned thatXLeft = (CA::Unsigned)grid2.xllcorner;
            CA::Unsigned thatXRight = ((CA::Unsigned)grid2.xllcorner) + grid2.ncols;

            CA::Unsigned thisYBottom = (CA::Unsigned)AsciiGridGeneral<T>::yllcorner;
            CA::Unsigned thisYTop = ((CA::Unsigned)AsciiGridGeneral<T>::yllcorner) + AsciiGridGeneral<T>::nrows;
            CA::Unsigned thatYBottom = (CA::Unsigned)grid2.yllcorner;
            CA::Unsigned thatYTop = ((CA::Unsigned)grid2.yllcorner) + grid2.nrows;

            //          if( ( (thatXleft >= thisXleft) && (thatXLeft < thisXRight) ) ||
            //              ( (thatXRight >= thisXleft) && (thatXRight < thisXRight))
            //              )

                        // check for not overlap
            if (
                ((thisXRight < thatXLeft) || (thatXRight < thisXLeft)) // not overlap in x direction...
                || // if no overlap in either or both then not overlapping area
                ((thisYTop < thatYBottom) || (thatYTop < thisYBottom)) // not overlap in y direction...
                )
            {
                // no overlapping area, bail out, return message somehow...
            }

            // otherwise there is overlap

            // calculate the range, i.e. distance of overlaps
            CA::Unsigned colsOverlap = 0;
            CA::Unsigned rowsOverlap = 0;

            if (thatXLeft < thisXLeft) {
                // then we start on this grid left most cells.
                // next we check to see if that grid totally covers the x direction of this grid
                if (thatXRight > thisXRight) {
                    colsOverlap = AsciiGridGeneral<T>::ncols;
                }
                else {
                    // otherwise that grid, end somewhere before this grids x extent.
                    colsOverlap = thatXRight - thisXLeft;
                }
            }
            else {
                //otherwise, that grids left most, starts inside this grids extent
                if (thatXRight < thisXRight)
                {
                    // therefore that grids extent is totaly within this grid
                    colsOverlap = grid2.ncols;
                }
                else {
                    // otherwise this grid ends before that one
                    colsOverlap = thisXRight - thatXLeft;
                }
            }

            if (thatYBottom < thisYBottom) {
                // then we start on this grid Bottom most cells.
                // next we check to see if that grid totally covers the y direction of this grid
                if (thatYTop > thisYTop) {
                    rowsOverlap = AsciiGridGeneral<T>::nrows;
                }
                else {
                    // otherwise that grid, end somewhere before this grids y extent.
                    rowsOverlap = thatYTop - thisYBottom;
                }
            }
            else {
                //otherwise, that grids bottom most, starts inside this grids extent
                if (thatYTop < thisYTop)
                {
                    // therefore that grids extent is totaly within this grid
                    rowsOverlap = grid2.nrows;
                }
                else {
                    // otherwise this grid ends before that one
                    rowsOverlap = thisYTop - thatYBottom;
                }
            }

            // so we can now loop over the area (columns and rows) of the overlapping area.

            // NOTE: that geo co-ordinates start at bottom left, and overlap calculations are based on these, therefore
            // a conversion to the local co-ordinates which are top left are required.

            CA::Unsigned thisXInset = 0;
            CA::Unsigned thisYInset = 0;
            CA::Unsigned thatXInset = 0;
            CA::Unsigned thatYInset = 0;

            if (colsDiff >= 0) {
                thisXInset = colsDiff;
            }
            else {
                thatXInset = -colsDiff;
            }

            if (rowsDiff >= 0) {
                thisYInset = rowsDiff;
            }
            else {
                thatYInset = -rowsDiff;
            }

            // now we know the inset due to over lap, based on bottom left.

            for (CA::Unsigned col = 0; col < colsOverlap; ++col) {
                for (CA::Unsigned row = 0; row < rowsOverlap; ++row) {
                    // convert to local cordinates (from bottom left to top left)
                    CA::Unsigned thisGridXCord = col + thisXInset;
                    CA::Unsigned thisGridYCord = (AsciiGridGeneral<T>::nrows - 1) - (row + thisYInset);
                    CA::Unsigned thatGridXCord = col + thatXInset;
                    CA::Unsigned thatGridYCord = (grid2.nrows - 1) - (row + thatYInset);

                    T thisDataPoint = AsciiGridGeneral<T>::data[thisGridXCord + (thisGridYCord * AsciiGridGeneral<T>::ncols)];
                    T thatDataPoint = grid2.data[thatGridXCord + (thatGridYCord * grid2.ncols)];

                    //------------------------------------

                    ++result.allDataCells;

                    bool thisWet = thisDataPoint > tolerance;
                    bool thatWet = thatDataPoint > tolerance;

                    double error = std::abs(thisDataPoint - thatDataPoint);

                    double squareError = error * error;

                    // all cells
                    result.RMSE += squareError;
                    result.meanError += error;

                    if (thisWet) {
                        // this cell is thus wet
                        ++result.thisWettedCount;
                    }

                    if (thatWet) {
                        // that other grids cell is thus wet
                        ++result.thatWettedCount;
                    }

                    if (thisWet && thatWet) {
                        ++result.combinedWettedCount;
                        result.RMSE_wet_both += squareError;
                        result.meanError_wet_both += error;
                        // also a true positive
                        ++result.truePositive;
                    }
                    else {
                        if (thisWet || thatWet) {
                            // one is both not both, so therefore
                            // so either area;
                            result.RMSE_wet_either += squareError;
                            result.meanError_wet_either += error;
                            ++result.eitherWetted;
                            meanObserved += thisDataPoint;
                            nashSumSquaredErrors += squareError;
                            if (thisWet) {
                                // this is but noth both, FalseNega
                                ++result.falseNegative;
                            }
                            else {
                                // so this not wet, and other is
                                ++result.falsePositive;
                            }
                        }
                        else {
                            // so noth either, and not both means neither, means neither
                            ++result.trueNegative;
                        }
                    }
                }// end of looping overlap grids rows
            }// end of looping overlap grids coloumns

            /*
            for(unsigned int i = 0; i < AsciiGridGeneral<T>::ncols * AsciiGridGeneral<T>::nrows ; ++i){
                if (grid2.data[i] == grid2.nodata || AsciiGridGeneral<T>::data[i] == AsciiGridGeneral<T>::nodata) {
                    continue;
                }

                ++result.allDataCells;

                bool thisWet = AsciiGridGeneral<T>::data[i] > tolerance;
                bool thatWet = grid2.data[i] > tolerance;

                double error = std::abs(AsciiGridGeneral<T>::data[i] - grid2.data[i]);

                double squareError = error * error;

                // all cells
                result.RMSE += squareError;
                result.meanError += error;

                if (thisWet) {
                    // this cell is thus wet
                    ++result.thisWettedCount;
                }

                if (thatWet) {
                    // that other grids cell is thus wet
                    ++result.thatWettedCount;
                }

                if (thisWet && thatWet) {
                    ++result.combinedWettedCount;
                    result.RMSE_wet_both += squareError;
                    result.meanError_wet_both += error;
                    // also a true positive
                    ++result.truePositive;
                }
                else {
                    if (thisWet || thatWet) {
                        // one is both not both, so therefore
                        // so either area;
                        result.RMSE_wet_either += squareError;
                        result.meanError_wet_either += error;
                        ++result.eitherWetted;
                        meanObserved += AsciiGridGeneral<T>::data[i];
                        nashSumSquaredErrors += squareError;
                        if (thisWet) {
                            // this is but noth both, FalseNega
                            ++result.falseNegative;
                        }
                        else {
                            // so this not wet, and other is
                            ++result.falsePositive;
                        }
                    }
                    else {
                        // so noth either, and not both means neither, means neither
                        ++result.trueNegative;
                    }
                }
            }// end of looping values
            */

            // divide by total, to get mean.


            // alot of possible divide by zero's, all protected ^_^

            if (result.allDataCells != 0.0) {
                result.RMSE /= result.allDataCells;
            }
            if (result.combinedWettedCount != 0.0) {
                result.RMSE_wet_both /= result.combinedWettedCount;
            }
            if (result.eitherWetted != 0.0) {
                result.RMSE_wet_either /= result.eitherWetted;
            }

            if (result.allDataCells != 0.0) {
                result.meanError /= result.allDataCells;
            }
            if (result.allDataCells != 0.0) {
                result.meanError_wet_both /= result.combinedWettedCount;
            }
            if (result.eitherWetted != 0.0) {
                result.meanError_wet_either /= result.eitherWetted;
                meanObserved /= result.eitherWetted;
            }

            // percentage of correctly guessed 100.0 * (trues/alls)
            result.precentage = 100.0 * ((result.truePositive + result.trueNegative) / (double)result.allDataCells);

            // square root it for RMSE
            //result = std::sqrt(result);
            result.RMSE = std::sqrt(result.RMSE);
            result.RMSE_wet_both = std::sqrt(result.RMSE_wet_both);
            result.RMSE_wet_either = std::sqrt(result.RMSE_wet_either);

            // Accuracy = (TP + TN) / (TP + TN + FP + FN)
            result.accuracy = 0;
            double accNumerator = (result.truePositive + result.trueNegative);
            double accDenominator = (result.truePositive + result.trueNegative + result.falsePositive + result.falseNegative);
            if (accDenominator != 0.0) {
                result.accuracy = accNumerator / accDenominator;
            }

            // Sensitivity (True Positive Rate) = TP / (TP + FN)
            result.sensitivity = 0;
            double senseNumerator = result.truePositive;
            double senseDenominator = (result.truePositive + result.falseNegative);
            if (senseDenominator != 0.0) {
                result.sensitivity = senseNumerator / senseDenominator;
            }

            // Precision (positive predictive value) = TP / (TP + FP)
            result.precision = 0;
            double precNumerator = result.truePositive;
            double precDenominator = (result.truePositive + result.falsePositive);
            if (precDenominator != 0.0) {
                result.precision = precNumerator / precDenominator;
            }

            // second loop for nash sutcliffe
            for (unsigned int i = 0; i < AsciiGridGeneral<T>::ncols * AsciiGridGeneral<T>::nrows; ++i) {
                if (grid2.data[i] == grid2.nodata || AsciiGridGeneral<T>::data[i] == AsciiGridGeneral<T>::nodata) {
                    continue;
                }

                bool thisWet = AsciiGridGeneral<T>::data[i] > tolerance;
                bool thatWet = grid2.data[i] > tolerance;

                //double error = std::abs(AsciiGridGeneral<T>::data[i] - grid2.data[i]);

                //double squareError = error * error;
                if (thisWet || thatWet) {
                    double diffFromMean = AsciiGridGeneral<T>::data[i] - meanObserved;
                    nashSumDataVariance += (diffFromMean * diffFromMean);
                }
            }// end of looping values

            result.nashSutcliffe = 0;
            if (nashSumDataVariance != 0.0) {
                result.nashSutcliffe = 1 - (nashSumSquaredErrors / nashSumDataVariance);
            }

            return result;
        }// end of compareAsciiGridHeader
        //--------------------------------------------------------------------------------------------------------------


        //--------------------------------------------------------------------------------------------------------------
        //std::ostream & operator<<(std::ostream & Str, ESRI_ASCIIGrid<CA::Real> const & value) {
        inline std::string getString() {
            // print something from v to str, e.g: Str << v.getX();
            std::ostringstream   Str;
            std::string output;

            Str << "n cols: " << AsciiGridGeneral<T>::ncols << std::endl;
            Str << "n rows: " << AsciiGridGeneral<T>::nrows << std::endl;
            Str << "xllcorner: " << AsciiGridGeneral<T>::xllcorner << std::endl;
            Str << "yllcorner: " << AsciiGridGeneral<T>::yllcorner << std::endl;
            Str << "no_data: " << AsciiGridGeneral<T>::nodata << std::endl;

            output = Str.str();
            return output;
        }
        //--------------------------------------------------------------------------------------------------------------

        inline void copyData(ESRI_ASCIIGrid inGrid)
        {
            AsciiGridGeneral<T>::ncols = inGrid.ncols;
            AsciiGridGeneral<T>::nrows = inGrid.nrows;
            AsciiGridGeneral<T>::xllcorner = inGrid.xllcorner;
            AsciiGridGeneral<T>::yllcorner = inGrid.yllcorner;
            AsciiGridGeneral<T>::cellsize = inGrid.cellsize;
            AsciiGridGeneral<T>::nodata = inGrid.nodata;

            AsciiGridGeneral<T>::data.resize(inGrid.data.size());

            for (int counter = 0; counter < inGrid.data.size(); ++counter) {

                AsciiGridGeneral<T>::data[counter] = inGrid.data[counter];
            }
        }// end of copyData
        //--------------------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------
        inline T getCellByCords(double XCord, double YCord)
        {
            int XCol = int(XCord - AsciiGridGeneral<T>::xllcorner) / AsciiGridGeneral<T>::cellsize;

            // note , cords go from bottom left and rows,cols go grom top left, therefore need to flip this...

            //int YRow = int(YCord - AsciiGridGeneral<T>::yllcorner) / AsciiGridGeneral<T>::cellsize;
            int YRow = AsciiGridGeneral<T>::nrows - (int(YCord - AsciiGridGeneral<T>::yllcorner) / AsciiGridGeneral<T>::cellsize);

            T returnResult = AsciiGridGeneral<T>::data[XCol + (AsciiGridGeneral<T>::ncols * YRow)];

            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "Local Cords [" << XCol << "," << YRow << "]: " << returnResult << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

            return returnResult;
        }// end of getCellByCords
        //--------------------------------------------------------------------------------------------------------------

    };// end of class

#ifdef CAAPI_GDAL_OPTION
    template<typename T>
    std::string CA::ESRI_ASCIIGrid<T>::driver = "AAIGrid";
    template<typename T>
    int CA::ESRI_ASCIIGrid<T>::epsgCode = 27700;
    template<typename T>
    bool CA::ESRI_ASCIIGrid<T>::createCapable = false;
    template<typename T>
    bool CA::ESRI_ASCIIGrid<T>::createCopyCapable = true;
#endif

}// end of namespace

#endif  // _CA_ESRI_ASCIIGRID_HPP_
