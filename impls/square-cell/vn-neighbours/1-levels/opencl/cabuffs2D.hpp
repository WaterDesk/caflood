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

#ifndef _CABUFFS2D_H_
#define _CABUFFS2D_H_

//! \file cabuffs2d.h 
//! Contains the declaration of the 2D buffers used in the OpenCL
//! implementation of the caAPI.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2012-02


#include"Grid.hpp"
#include"CellBuff.hpp"
#include"EdgeBuff.hpp"
#include"Table.hpp"


// Get rid of the annoying visual studio warning 4244 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif // _MSC_VER


namespace CA {


  //! Identifies the buffer which contains a real value for each cell in
  //! the grid. 
  class CellBuffReal: public CellBuff<Real>
  {
  public:
    CellBuffReal(Grid& grid, const Options& options = Options()):
      CellBuff<Real>(grid, options)
    {

    }
    
    virtual ~CellBuffReal(){}

    
  private:
    

  };
  

  //! Identifies the buffer which contains a state value for each cell in
  //! the grid.  
  class CellBuffState: public CellBuff<State>
  {
  public:
    CellBuffState(Grid& grid, const Options& options = Options()):
      CellBuff<State>(grid, options)
    {
  
    }
    
    virtual ~CellBuffState(){}


  private:
    
  };
  

  //! Identifies the buffer which contains a real value for each edge
  //! of each cell in the grid.
  class EdgeBuffReal: public EdgeBuff<Real>
  {
  public:
    EdgeBuffReal(Grid& grid, const Options& options = Options()):
      EdgeBuff<Real>(grid, options)
    {

    }
    
    virtual ~EdgeBuffReal(){}


  private:
    

  };
  

  //! Identifies the buffer which contains a state value for each
  //! edge of each cell in the grid.
  class EdgeBuffState: public EdgeBuff<State>
  {
  public:
    EdgeBuffState(Grid& grid, const Options& options = Options()):
      EdgeBuff<State>(grid, options)
    {

    }
    
    virtual ~EdgeBuffState(){}

  
  private:
    
  };


  //! Identifies the tabel which contains multiple real values for
  //! multiple indices.
  class TableReal: public Table<Real>
  {
  public:
    TableReal(Grid& grid, Unsigned num, const Options& options = Options()):
      Table<Real>(grid,num,options)
    {

    }
    
    virtual ~TableReal(){}


  private:
    

  };

  //! Identifies the tabel which contains multiple real values for
  //! multiple indices.
  class TableState: public Table<State>
  {
  public:
    TableState(Grid& grid, Unsigned num, const Options& options = Options()):
      Table<State>(grid,num,options)
    {

    }
    
    virtual ~TableState(){}

  private:
    
  };
  
    

} // Namespace CA


#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER


#endif	// _CABUFFS2D_HPP_
