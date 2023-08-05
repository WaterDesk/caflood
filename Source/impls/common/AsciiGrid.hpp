/*

Copyright (c) 2016 Centre for Water Systems,
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

#ifndef _CA_ASCIIGRID_HPP_
#define _CA_ASCIIGRID_HPP_


//! \file AsciiGrid.hpp
//! Contains the structures and methods to read a generic grid of data
//! which is then implemented within each implementation
//! \author Mike Gibson, University of Exeter, 
//! contact: m.j.gibson [at] exeter.ac.uk
//! \date 2016-01


#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<stdexcept>
#include"Utilities.hpp"
#include"BaseTypes.hpp"
//#include"Clock.hpp"

namespace CA {

    //! structure to contian various diiferent error metrics and measures
    struct Errors {

        double RMSE;
        double RMSE_wet_both;
        double RMSE_wet_either;

        double meanError;
        double meanError_wet_both;
        double meanError_wet_either;

        double accuracy;
        double sensitivity;
        double precision;
        double precentage;

        double nashSutcliffe;

        int truePositive;
        int falsePositive;
        int trueNegative;
        int falseNegative;

        int thisWettedCount;
        int thatWettedCount;

        int combinedWettedCount;
        int allDataCells;
        int eitherWetted;
    };

    template<typename T>
    class AsciiGridGeneral {
    public:

        //! Structure contains the data of an ARC/INFO ASCII GRID format file.
        //! In the data pointer the (0,0) point is in the top/left corner.
        Unsigned ncols;
        Unsigned nrows;
        double xllcorner;
        double yllcorner;
        double cellsize;
        T nodata;

        std::vector<T> data;

        AsciiGridGeneral() {};

        //! Read token and value pair from a line of the header of an ARC/INFO
        //! ASCII GRID file.
        virtual void readAsciiGridHeaderLine(const std::string& filename, std::ifstream& infile, T& value,
            const char* check, bool substring = false) {
            return;
        };

        //! Read a ARC/INFO ASCII GRID format file
        virtual void readAsciiGrid(const std::string& filename, bool print = false) { return; };

        //! Read a ARC/INFO ASCII GRID format file without loading teh data.
        virtual void readAsciiGridHeader(const std::string& filename, bool print = false) { return; };

        // compare this and other one....
        //template<typename T>
        //virtual double compareAsciiGrid(AsciiGrid<T>& grid1, AsciiGrid<T>& grid2, bool print = false);

        //! Write a ARC/INFO ASCII GRID format file
        //template<typename T>
        virtual void writeAsciiGrid(const std::string& filename,
            int decimal_places = 6, bool print = false) {
            return;
        };

    };// end of ASCIIGrid class

}// end of namespace


#endif  // _CA_ASCIIGRID_HPP_
