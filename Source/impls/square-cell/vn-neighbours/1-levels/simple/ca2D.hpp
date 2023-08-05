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
#define CAAPI_2D_MAGIC    0xFFFA
#endif

#ifndef _CA2D_HPP_
#define _CA2D_HPP_

//! \file ca2D.hpp 
//! Main include file with the classes and methods to generate a CA
//! algorithm of 2D lattice.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-07


#if !defined __cplusplus
#error This CAAPI  implementation requires a C++ compiler
#endif


#include"caapi2D.hpp"
#include"Grid.hpp"
#include"CellBuff.hpp"
#include"EdgeBuff.hpp"
#include"Alarms.hpp"
#include"Table.hpp"
#include"Functions.hpp"
#include"Decomposition.hpp"

#include "ESRI_ASCIIGrid.hpp"



//! \def CA_2D_INCLUDE
//! Macro that include a CA 2D function.   
#ifdef  CA_2D_INCLUDE
#undef  CA_2D_INCLUDE
#endif
#define CA_2D_INCLUDE(name) CA_QUOTE(name.ca)


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


    //! Identifies the buffer which contains a real value for each cell in
    //! the grid. 
    class CellBuffReal : public CellBuff<Real>
    {
    public:
        CellBuffReal(Grid& grid, const Options& options = Options()) :
            CellBuff<Real>(grid, options)
        {
        }

        virtual ~CellBuffReal() {}

    private:

    };


    //! Identifies the buffer which contains a state value for each cell in
    //! the grid.  
    class CellBuffState : public CellBuff<State>
    {
    public:
        CellBuffState(Grid& grid, const Options& options = Options()) :
            CellBuff<State>(grid, options)
        {
        }

        virtual ~CellBuffState() {}

    private:

    };


    //! Identifies the buffer which contains a real value for each edge
    //! of each cell in the grid.
    class EdgeBuffReal : public EdgeBuff<Real>
    {
    public:
        EdgeBuffReal(Grid& grid, const Options& options = Options()) :
            EdgeBuff<Real>(grid, options)
        {
        }

        virtual ~EdgeBuffReal() {}

    private:

    };


    //! Identifies the buffer which contains a state value for each
    //! edge of each cell in the grid.
    class EdgeBuffState : public EdgeBuff<State>
    {
    public:
        EdgeBuffState(Grid& grid, const Options& options = Options()) :
            EdgeBuff<State>(grid, options)
        {
        }

        virtual ~EdgeBuffState() {}

    private:

    };


    //! Identifies the tabel which contains multiple real values for
    //! multiple indices.
    class TableReal : public Table<Real>
    {
    public:
        TableReal(Grid& grid, Unsigned num, const Options& options = Options()) :
            Table<Real>(grid, num, options)
        {
        }

        virtual ~TableReal() {}

    private:

    };

    //! Identifies the tabel which contains multiple real values for
    //! multiple indices.
    class TableState : public Table<State>
    {
    public:
        TableState(Grid& grid, Unsigned num, const Options& options = Options()) :
            Table<State>(grid, num, options)
        {
        }

        virtual ~TableState() {}

    private:

    };


    //! Execute a CA functions.
    template<typename Func>
    inline void execute(const BoxList& bl, Func f, Grid& grid)
    {
#ifdef CA2D_OPENMP
        _caGrid   _cagrid = grid; // by value!!
#else
        _caGrid&  _cagrid = grid; // by reference!!!
#endif

    // Check that the extent of the boxlist is inside the domain of
    // the grid.
        if (!grid.box().inside(bl.extent()))
            return;

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            const Box box(*ibox);

#ifdef CA2D_OPENMP
            Unsigned chunksize = box.h() / omp_get_num_procs() + 1;
            // Cycle through the region of cells to read. 
            // And set the nev value of the __cagrid.
#pragma omp parallel for default(shared) firstprivate(f,_cagrid) schedule(static,chunksize)
      // Cycle through the region of cells to read. 
      // And set the nev value of the __cagrid.
            for (int j_reg = static_cast<int>(box.y()); j_reg < box.h() + box.y(); ++j_reg)
            {
                for (int i_reg = static_cast<int>(box.x()); i_reg < box.w() + box.x(); ++i_reg)
                {
                    // Set the new cell to visit.
                    _cagrid.main_x = i_reg;
                    _cagrid.main_y = j_reg;

                    // Set the box.
                    _cagrid.bx_lx = box.x();
                    _cagrid.bx_ty = box.y();
                    _cagrid.bx_rx = box.w() + box.x();
                    _cagrid.bx_by = box.h() + box.y();

                    f(_cagrid);
                }
            }
#else
            // Cycle through the region of cells to read. 
            // And set the nev value of the __cagrid.
            for (Unsigned j_reg = box.y(); j_reg < box.h() + box.y(); ++j_reg)
            {
                for (Unsigned i_reg = box.x(); i_reg < box.w() + box.x(); ++i_reg)
                {
                    // Set the new cell to visit.
                    _cagrid.main_x = i_reg;
                    _cagrid.main_y = j_reg;

                    // Set the box.
                    _cagrid.bx_lx = box.x();
                    _cagrid.bx_ty = box.y();
                    _cagrid.bx_rx = box.w() + box.x();
                    _cagrid.bx_by = box.h() + box.y();

                    f(_cagrid);
                }
            }
#endif
        }
    }


    //! Define the class that execute a CA Function.  This class works
    //! only with static methods. It is impossible to create a normal
    //! objects since the construct is private.
    class Execute : public CA::Uncopyable
    {
    private:
        Execute() {}
        ~Execute() {}

    public:

        template<typename Func, typename A1>
        static void function(const BoxList& bl, Func f, Grid& g, A1& a1)
        {
            CA::execute(bl, CA::makeFunction(f, a1), g);
        }


        template<typename Func, typename A1, typename A2>
        static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2), g);
        }


        template<typename Func, typename A1, typename A2, typename A3>
        static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3), g);
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4>
        static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4), g);
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5>
        static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5), g);
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
        static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6), g);
        }

        // seven
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7 >
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7), g);
        }

        // eight
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8), g);
        }

        // nine
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9), g);
        }

        // ten
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), g);
        }

        // eleven
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11), g);
        }

        // twleve
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12), g);
        }

        // thirteen
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12,
            typename A13>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13), g);
        }

        // fourtheen
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12,
            typename A13, typename A14>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14), g);
        }

        // fofiftenn
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12,
            typename A13, typename A14, typename A15>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15), g);
        }


        // 16
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12,
            typename A13, typename A14, typename A15, typename A16>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15,
                A16& a16)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16), g);
        }


        // 17
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12,
            typename A13, typename A14, typename A15, typename A16, typename A17>
            static void function(const BoxList& bl, Func f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6,
                A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15,
                A16& a16, A17& a17)
        {
            CA::execute(bl, CA::makeFunction(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17), g);
        }
    };

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
