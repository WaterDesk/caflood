/*

Copyright (c) 2013 Centre for Water Systems, University of Exeter

Copyright (c) 2016 EAWAG, Department Urban Water Management

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

#ifndef _CA_HEXASCIIGRID_HPP_
#define _CA_HEXASCIIGRID_HPP_


//! \file HexAsciiGrid.hpp
//! Contains the structures and methods to read an hexagonal raster grid
//! in ASCII format.
//! The file must be structured according to this grammar:
//! https://github.com/ldesousa/HexAsciiBNF
//! \author Michele Guidolin, University of Exeter, 
//! \author Mike Gibson, University of Exeter,
//! \author Luís de Sousa, EAWAG,  
//! contact: luis.desousa [at] eawag.ch
//! \date 2016-01

#include "AsciiGrid.hpp"

namespace CA {


	template<typename T>
	class HexAsciiGrid 
		: public CA::AsciiGridGeneral<T> 
	{

	public:

		static const char* key_ncols;
		static const char* key_nrows;
		static const char* key_xll;
		static const char* key_yll;
		static const char* key_side;
		static const char* key_nodata;
		static const std::string file_extension;

	  //! Read token and value pair from a line of the header of an hexagonal ASCII raster file.
	  //! .
		template<typename A>
		inline 
			void readAsciiGridHeaderLine(const std::string& filename, std::ifstream& infile, A& value,
			const char* check, bool substring = false, bool optional = false)
		{
			std::string token;
			std::string strvalue;

			// Read the token and value. Check that the token is
			// the desire one. Add the value into grid.ncols.
			if (!(infile >> token) || !(infile >> strvalue))
				throw std::runtime_error(std::string("Error reading the file: ") + filename);

			if (!CA::compareCaseInsensitive(token, check, substring))
			{
				if (!optional)
					throw std::runtime_error(
							std::string("Error, not an hexagonal ASCII raster file: ") + filename +
							std::string(" - Expected ") + check + std::string(" but read ") + token);
				else return;
			}

			if (!CA::fromString(value, strvalue))
				throw std::runtime_error(std::string("Error converting the string ") + strvalue
					+ std::string(" into a value from the file: ") + filename);
		}

		//! Reads the header of an hexagonal ASCII raster file
		inline  void loadAsciiGridHeader(const std::string& filename, std::ifstream& infile, bool print = false)
		{
			readAsciiGridHeaderLine<CA::Unsigned>(filename, infile, AsciiGridGeneral<T>::ncols, key_ncols, false);
			readAsciiGridHeaderLine<CA::Unsigned>(filename, infile, AsciiGridGeneral<T>::nrows, key_nrows, false);
			readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::xllcorner, key_xll, false);
			readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::yllcorner, key_yll, false);
			readAsciiGridHeaderLine<double>(filename, infile, AsciiGridGeneral<T>::cellsize, key_side, false);
			readAsciiGridHeaderLine<T>(filename, infile, AsciiGridGeneral<T>::nodata, key_nodata, true, true);

			if (print)
			{
				std::cout << key_ncols << " \t" << AsciiGridGeneral<T>::ncols << std::endl;
				std::cout << key_nrows << " \t" << AsciiGridGeneral<T>::nrows << std::endl;
				std::cout << key_xll << " \t" << AsciiGridGeneral<T>::xllcorner << std::endl;
				std::cout << key_yll << " \t" << AsciiGridGeneral<T>::yllcorner << std::endl;
				std::cout << key_side << " \t" << AsciiGridGeneral<T>::cellsize << std::endl;
				std::cout << key_nodata << " \t" << AsciiGridGeneral<T>::nodata << std::endl;
				std::cout << std::endl;
			}
		}


		//! Read an hexagonal ASCII raster file
		//template<typename T>
		inline  void readAsciiGrid(const std::string& filename, bool print = false)
		{
			// Get starting time.
			//Clock total_timer;

			// Open file
			std::ifstream infile(filename.c_str());

			// Check if the file was open.
			if (!infile)
				throw std::runtime_error(std::string("Error opening file: ") + filename);

			// Read the header  
			loadAsciiGridHeader(filename, infile, print);

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
		}


		//! Read an hexagonal ASCII raster file without loading the data.
		//template<typename T>
		inline  void readAsciiGridHeader(const std::string& filename, bool print = false)
		{
			// Open file
			std::ifstream infile(filename.c_str());

			// Check if the file was open.
			if (!infile)
				throw std::runtime_error(std::string("Error opening file: ") + filename);

			// Read the header  
			loadAsciiGridHeader(filename, infile, print);

			// Empty the data
			AsciiGridGeneral<T>::data.clear();

			// Close the file.
			infile.close();
		}

		//! Write an hexagonal ASCII raster file format file
		//template<typename T>
		inline void writeAsciiGrid(const std::string& filename,
			int decimal_places = 6, bool print = false)
		{
			// Get starting time.
			//Clock total_timer;

			// Open file
			std::ofstream onfile((filename + file_extension).c_str());

			// Check if the file was open.
			if (!onfile)
				throw std::runtime_error(std::string("Error opening file: " + filename));

			// Set  manipulators 
			onfile.unsetf(std::ios::floatfield);
			onfile.precision(12);

			// Write the header 
			onfile << key_ncols << " \t\t" << AsciiGridGeneral<T>::ncols << std::endl;
			onfile << key_nrows << " \t\t" << AsciiGridGeneral<T>::nrows << std::endl;
			onfile << key_xll << " \t\t" << AsciiGridGeneral<T>::xllcorner << std::endl;
			onfile << key_yll << " \t\t" << AsciiGridGeneral<T>::yllcorner << std::endl;
			onfile << key_side << " \t\t" << AsciiGridGeneral<T>::cellsize << std::endl;
			onfile << key_nodata << " \t\t" << AsciiGridGeneral<T>::nodata << std::endl;
			onfile << std::endl;

			// Make sure the decimal places is not zeros.
			if (decimal_places <= 0)
				decimal_places = 6;

			onfile.unsetf(std::ios::floatfield);
			onfile.precision(decimal_places);

			// Loop through the data.
			for (unsigned int i = 0; i < AsciiGridGeneral<T>::ncols * AsciiGridGeneral<T>::nrows; ++i)
			{
				if (i%AsciiGridGeneral<T>::ncols == 0)
					onfile << "\n";
				onfile << AsciiGridGeneral<T>::data[i] << " ";
			}

			// End of line.
			onfile << std::endl;

			// Close the file.
			onfile.close();

			//std::cout<<"Write run time taken (s) = " << total_timer.millisecond()/1000.0 << std::endl;    
		}


		//------------------------------------------------------------------------------------------------------------------------------
		//! Add another ascii gri dof data to this grid of data
		inline void addAsciiGrid(AsciiGrid<T>& grid2, bool print = false) {
			// find how many cells between bottom left of each terrain.
			int colsDiff = 0;
			int rowsDiff = 0;

			if (print)
				std::cout << "BLAH" << std::endl;

			// TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// NEEDS IMPLMENTING FOR HEX GRIDS

		}
		//------------------------------------------------------------------------------------------------------------------------------


		//#######################################################################################################################################
		//###########################################	comparitor functions			 ########################################################
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

			// TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// NEEDS IMPLMENTING FOR HEX GRIDS

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


			// TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// NEEDS IMPLMENTING FOR HEX GRIDS

			return result;

		}// end of compareAsciiGridHeader
		 //--------------------------------------------------------------------------------------------------------------

		 //--------------------------------------------------------------------------------------------------------------
		 //std::ostream & operator<<(std::ostream & Str, ESRI_ASCIIGrid<CA::Real> const & value) {
		inline std::string getString() {
			// print something from v to str, e.g: Str << v.getX();
			std::ostringstream   Str;
			std::string output;

			Str << "n cols: " << ncols << std::endl;
			Str << "n rows: " << nrows << std::endl;
			Str << "xllcorner: " << xllcorner << std::endl;
			Str << "yllcorner: " << yllcorner << std::endl;
			Str << "no_data: " << nodata << std::endl;


			output = Str.str();
			//return Str;
			return output;
		}
		//--------------------------------------------------------------------------------------------------------------

		inline void copyData(HexAsciiGrid inGrid)
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




		}


	};// end of class

	template<typename T>
	const char* HexAsciiGrid<T>::key_ncols  = "ncols";
	template<typename T>
	const char* HexAsciiGrid<T>::key_nrows  = "nrows";
	template<typename T>
	const char* HexAsciiGrid<T>::key_xll    = "xll";
	template<typename T>
	const char* HexAsciiGrid<T>::key_yll    = "yll";
	template<typename T>
	const char* HexAsciiGrid<T>::key_side   = "side";
	template<typename T>
	const char* HexAsciiGrid<T>::key_nodata = "no_data";
	template<typename T>
	const std::string HexAsciiGrid<T>::file_extension = ".hasc";

	


}// end of namespace


#endif	// _CA_HexAsciiGrid_HPP_
