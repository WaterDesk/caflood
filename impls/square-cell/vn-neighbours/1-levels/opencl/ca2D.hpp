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
#define CAAPI_2D_MAGIC    0xCCCC
#endif

#ifndef _CA2D_HPP_
#define _CA2D_HPP_

//! \file ca.hpp 
//! Main include file with the classes and methods to generate a CA
//! algorithm.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-07


#if !defined __cplusplus
#error This CAAPI  implementation requires a C++ compiler
#endif


#include"caapi2D.hpp"
#include"cabuffs2D.hpp"
#include"caexec2D.hpp"
#include"Decomposition.hpp"

#include "ESRI_ASCIIGrid.hpp"


//! \def CA_2D_INCLUDE
//! Macro that include a CA 2D function.   
#ifdef  CA_2D_INCLUDE
#undef  CA_2D_INCLUDE
#endif
#define  CA_2D_INCLUDE(name) CA_QUOTE(name.ca.hpp)


namespace CA {

    //! Initialise the 2D caAPI environment. This method must be called as
    //! soon as possible in the main file.
    inline void init2D(int *argc, char ***argv)
    {
    }


    //! Terminates the 2D caAPI environment. This method must be called
    //! before closing the main file.
    inline void finalise2D()
    {
    }


    /// ----- Tools ----- ///

    //! Compute a possible domain decomposition and data partitioning
    //! using a CellBuffer and a given nodata value.
    //! This method do not decomposed the Grid.
    //!\param[in]  box      Identifies the region of the grid to decomposed
    //!\param[in]  cellbuff The buffer to analyse.
    //!\param[in]  nodata   The value that identifies the non-domain cells.
    //!\param[in]  thrd     The efficiency threshould (number of domain points/ number of points).
    //!\param[in]  nlmin    Define the minimum nuber of lines in a side of the box.
    //!\param[out] bl       The boxlist with the decomposed domain. Any previous
    //!                     values in the list are removed.
    inline void domainDecomposition(const Box& box, CellBuffReal& cb, Real nodata, Real thrd, Unsigned nlmin,
        BoxList& bl)
    {
        // It uses the common split/block method. Different implementation
        // can use different methods.
        splitBlockDecomposition<Real>(box, cb, nodata, thrd, nlmin, bl);
    }

} // Namespace CA


// Include the file that contains the methods and classes that need
// the element of ca2D.hpp to be declared.
#include"endca2D.hpp"


#endif  // _CA2D_HPP_
