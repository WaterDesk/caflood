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

#ifndef _CA_BASETYPES_HPP_
#define _CA_BASETYPES_HPP_

//! \file BaseTypes.hpp 
//! Contains the typedefs that define the base
//! types used in the CAAPI.
//! \author Michele  Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-07


#ifndef NOMINMAX
#define NOMINMAX          //!< This macro stop a name clash.
#endif


#include<string>
#include<utility>
#include<vector>

#if defined _WIN32 
#include<memory>
#else //GCC
#include<tr1/memory>
#endif

//! The namespace cpp11 contains all the element that are new to the
//! new standard std:cpp11. \attention Tested only on Linux.
#if defined _WIN32 
namespace cpp11 = std;
#else //GCC
namespace cpp11 = std::tr1;
#endif


//! \def CA_REAL_FLOAT
//! Define the value that identify the real type as float
#define  CA_REAL_FLOAT 0

//! \def CA_REAL_DOUBLE
//! Define the value that identify the real type as double
#define  CA_REAL_DOUBLE 1


//! \namespace CA 
//! Main namespace of the CA which contains all the classes and
//! methods used to generate the CA algorithm.
namespace CA {

    //! Define the type of a string.
    typedef std::string   String;

    //! Define the type of real values (floating point).
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
    typedef float         Real;
#elif CA_REAL_PRECISION == CA_REAL_DOUBLE
    typedef double        Real;
#else
#error Undefined Real precision 
#endif

  //! Define the type of state values (discrite states).
    typedef int           State;

    //! Define the type of an unsigned large  quantity.
    typedef std::size_t   Unsigned;
    //typedef unsigned int   Unsigned;

    //! Define the type of a byte.
    typedef char          Byte;


    namespace Bdr {
        //! Enum which identifies the possible operators that can be used on
        //! setting a border value.
        enum Operator
        {
            Equal,
            Add,
            Sub,
            Mul,
            Div
        };
    }

    namespace Seq {
        //! Enum which identifies the possible operators that can be used on
        //! a sequential operation.
        enum Operator
        {
            Add,
            Mul,
            Min,
            MinAbs,
            Max,
            MaxAbs
        };
    }
}

#endif// _CA_BASETYPES_HPP_
