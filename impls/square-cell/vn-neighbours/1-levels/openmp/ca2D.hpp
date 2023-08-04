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

//! \def CAAPI_2D_MAGIC 
//! Define a magic number that uniquely identify this grid
//! implementation. 
#if !defined CAAPI_2D_MAGIC
#define CAAPI_2D_MAGIC    0xFFFB
#endif


//! \file ca.hpp 
//! Main include file with the classes and methods to generate a CA
//! algorithm.
//! \author Mike Gibson, University of Exeter, 
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2013-05


//! \def CA2D_OPENMP
//! Define that this CA2D implementation is an OPENMP implementation
#define CA2D_OPENMP 1

// Include the openmp library header.
#include<omp.h>

// Include the specific header file for this impelemntation.
#include"caapi2D.hpp"
#include"Grid.hpp"
#include"CellBuff.hpp"
#include"EdgeBuff.hpp"
#include"Alarms.hpp"
#include"Functions.hpp"


// The rest is the same of the simple implementation
#include"../simple/ca2D.hpp"
