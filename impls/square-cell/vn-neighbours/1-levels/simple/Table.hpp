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

#ifndef _CA_TABLE_HPP_
#define _CA_TABLE_HPP_


//! \file Table.hpp
//! Define table data, i.e. a constant array.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2013-11


#include"Grid.hpp"
#include<limits>
#include<iostream>
#include<cstdlib>
#include"caapi2D.hpp"


namespace CA {
  

  //! Define a class that manage the data as in a table format, i.e. a
  //! constant array where a value can be identified by index. The
  //! array is constant during a CA function execution. It can/must be
  //! populated in the main code.
  //! \attention the index in the table is different than a CA_INDEX.
  template<typename T>  
  class Table: public CA::Uncopyable
  {
  public:

    //! Create the table data. It is possible to have implementation
    //! specific options set by using the options list. \attention Do
    //! not destroy the grid before destroying this buffer.
    //! \param grid        The Grid
    //! \param num         The number of elements.
    //! \param options     The list of implementation specific options.
    Table(Grid& grid, Unsigned num, const Options& options = Options());


    //! Destroy the table.
    virtual ~Table();


    //! Return the specific options about the Table object and this
    //! implementation.
    static Options options();


    //! Update the specific values of the table from the data of the
    //! given memory location.  The specific value are identified by a
    //! starting location and a end location. The memory is identified
    //! by a size.
    //! \param start         The startiong position where to update table.
    //! \param stop          The ending position where to update table (not included).
    //! \param mem           The pointer to the memory location where to read data.
    //! \param mem_num       The number of elements in memory location.
    void update(Unsigned start, Unsigned stop, const T* mem, Unsigned mem_num);


    // ---- DEBUG ----
      
    //! Dump the buffer (with borders) into the given output stream in a
    //! readable form. The method print the buffer starting from the
    //! top/left corner.
    //! \param out         The output stream/
    //! \param x_sep       The separator of values in the X dimension.
    //! \param y_sep       The separator of values in the Y dimension.
    void dump(std::ostream& out, String x_sep=" ", String y_sep="\n");    


    // ---------  Implementation dependent --------------

    // Convert the buffer in the CA_TABLE_??? used in the CA function
    operator const T*()  const { return _buff; }

    
  private:

    //! The reference to the grid.
    Grid& _grid;

    //! The number of elements in the openCL buffer that contains the table.
    _caUnsigned    _buff_num;
    
    //! The size of the buffer in bytes.
    _caUnsigned    _buff_size;

    //! The pointer to the data.
    T* _buff;
  };


  /// ----- Inline implementation ----- ///

  
  template<typename T>
  inline Table<T>::Table(Grid& grid, Unsigned num, const Options& options):
    _grid(grid),
    _buff_num(num),
    _buff_size(sizeof(T) * _buff_num),
    _buff()
  {    
    _buff = static_cast<T*>(calloc(_buff_num, sizeof(T)));  
  }
  

  template<typename T>
  inline Table<T>::~Table()
  {
  }


  template<typename T>
  inline Options Table<T>::options()
  {
    Options options;

    // Add the optional arguments. The tag start from higher number
    // for safety reason.
    CA::Unsigned na = 6000;
    
    // Example
    //options.push_back(new Arguments::Arg(na++,"option-name", "Option desctiption","value",true, true, false));

    return options;
  }


  template<typename T>
  inline void Table<T>::update(Unsigned start, Unsigned stop, const T* mem, Unsigned mem_num)
  {
    if(std::min(stop-start,mem_num)<=0)
      return;

    // Write the data into the buffer.
    Unsigned mem_size = std::min(std::min(stop-start,mem_num)*sizeof(T), _buff_size);
    memcpy(&_buff[start],mem, mem_size);    
  }


  template<typename T>
  inline void Table<T>::dump(std::ostream& out, String x_sep, String y_sep)
  {
    // Cycle through the table values
    for(_caUnsigned i=0; i<_buff_num; ++i)
    {
      out<<_buff[static_cast<size_t>(i)];	
      out<<x_sep;
    }
    out<<y_sep;   
  }

}



#endif	// _CA_TABLE_HPP_
