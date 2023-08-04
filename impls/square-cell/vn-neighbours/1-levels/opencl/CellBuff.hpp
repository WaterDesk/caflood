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

#ifndef _CA_CELLBUFF_HPP_
#define _CA_CELLBUFF_HPP_


//! \file CellBuff.hpp
//! Contains the common class of the buffer which can contains a value
//! (Real/State) for each cell in the grid. 
//! This implementation uses OpenCL.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-12


#include"Grid.hpp"
#include<limits>
#include<iostream>
#include<vector>


// Get rid of the annoying visual studio warning 4244 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif // _MSC_VER


namespace CA {
  

  //! Define the buffer which contains a value (real/state) for each
  //! cell in a square regular grid.

  //! This is the OpenCL implementation. The buffer is created
  //! directly into the (single) device memory. At the moment this
  //! object uses a single event that indicates the last command that
  //! used (or is working on) the buffer.

  //! \attention The CA_CELLBUFF_???? item used in the CA function can be
  //! different from this class. 
  
  //! \warning The buffer is bigger than the Grid since it has to
  //! account of border cells. The number of border cells depends on
  //! the number of neighbours' levels.

  template<typename T>
  class CellBuff: public CA::Uncopyable
  {
  public:

    //! Create the buffer. It is possible to have implementation
    //! specific options set by using the options list. \attention Do
    //! not destroy the grid before destroying this buffer.  The
    //! buffer is set to zero.
    //! \param grid    The Grid
    //! \param options The list of implementation specific options.
    CellBuff(Grid& grid, const Options& options = Options());


    //! Destroy the buffer.
    virtual ~CellBuff();


    //! Return the specific options about the CellBuff object and this
    //! implementation.
    static Options options();


    //! Clear the buffer, i.e. set a value in all the elements
    //! (borders included). The default is zero.
    void clear(const T& value=T());

    
    //! Copy all the value of the given buffer into the object buffer
    void copy(const CellBuff<T>& src);


    //! Read the values of the cells from a given region of the grid
    //! and upload the data into the given memory location. The region
    //! of the grid is identifies by a list of rectangular boxes where X and Y
    //! are the offset respectively to the left and top corner of the
    //! region of the grid to be access. If X and Y are zero the
    //! region start from the top/left side. The width and height of
    //! the box represent the number of cells in the X and Y
    //! directions of the region of the grid to access.
    //! \attention This function uses a single box instead than a list of boxes.
    //! \param box           Identifies the region of the grid to read.
    //! \param mem           The pointer to the memory location where to upload
    //!                      the read data.
    //! \param mem_x_size    The size of the memmory location on the 
    //!                      X dimension.
    //! \param mem_y_size    The size of the memmory location on the 
    //!                      Y dimension.
    void retrieveData(const Box& box, T* mem, Unsigned mem_x_size, Unsigned mem_y_size);

    
    //! Write the values of the cells into a region of the grid from
    //! the data of the given memory location.The region of the grid
    //! is identifies by a rectangular box where X and Y are the
    //! offset respectively to the left and top corner of the region of
    //! the grid to be access. If X and Y are zero the region start
    //! from the top/left side. The width and height of the box
    //! represent the number of cells in the X and Y directions of the
    //! region of the grid to access.
    //! \attention This function uses a single box instead than a list of boxes.
    //! \param box           Identifies the region of the grid to write.
    //! \param mem           The pointer to the memory location where to retrieve
    //!                      the data to insert.
    //! \param mem_x_size    The size of the memmory location on the 
    //!                      X dimension.
    //! \param mem_y_size    The size of the memmory location on the 
    //!                      Y dimension.
    void insertData(const Box& box, const T* mem, Unsigned mem_x_size, Unsigned mem_y_size);


    //! Save the entire buffer in the DataDir using an unique main id
    //! (filename / buffername) and unique sub id (time step /
    //! checkpoint) using as direct and quick I/O technique as
    //! possible.
    //! \attention How the buffer is saved is implementation
    //! dependant.
    //! \return true if successful.
    bool saveData(const std::string& mainid, const std::string& subid);


    //! Load the entire buffer from the DataDir of the given unique
    //! main id (filename / buffername) and of the unique sub id (time
    //! step / checkpoint) using as direct and quick I/O technique as
    //! possible.
    //! If the remove arguments is true, the file/data is delete from
    //! the DataDir once opened.
    //! \attention How the buffer is loaded is implementation
    //! dependant.
    //! \warning The existing data will be overwitten.
    //! \return true if successful.
    bool loadData(const std::string& mainid, const std::string& subid, bool remove = false);


    //! Remove the buffer data from the DataDir of the given unique
    //! main id (filename / buffername) and of the unique sub id (time
    //! step / checkpoint) using as direct and quick I/O technique as
    //! possible.
    static bool removeData(const std::string& datadir, const std::string& mainid, const std::string& subid);


    //! Check if a buffer data of the given unique main id and of the
    //! unique sub id exist in the DataDir.
    //! \return true if data exist.
    static bool existData(const std::string& datadir, const std::string& mainid, const std::string& subid);


    //! Read the values of the cells indetified in the list of points
    //! and upload the data into the given memory buffer. The number
    //! of elements in the buffer (size) must be at least the number
    //! of points in the list. The order in the buffer is the same one
    //! of the list of points.
    void retrievePoints(const PointList& pl, T* mem, Unsigned size); 
    

    //! Write the values of the cells indetified in the list of points
    //! by using the data of the given memory buffer. The number
    //! of elements in the buffer (size) must be at least the number
    //! of points in the list. The order in the buffer is the same one
    //! of the list of points.
    void insertPoints(const PointList& pl, const T* mem, Unsigned size); 


    //! Execute the given sequential operator on the values of all the cells of
    //! the given region of the grid and return the result of the operation.
    //! For example if the operator is CA::add it returns cell 1 + cell 2 + ....
    //! \attention This method can only use commutative operation Add/Mul.
    //! \param bl            Identifies the region of the grid from a list of
    //!                      boxes to execute the function.
    //! \param value         The starting value of the operation and the
    //!                      results.
    //! \param op            The commutative operator to execute in each cell .
    void sequentialOp(const BoxList& bl, T& value, CA::Seq::Operator op) const;


    //! Fill all values of the cells of the given region of the grid
    //! with the given value. The region of the grid is identifies by
    //! a list of boxes.
    //! \param bl            Identifies the region of the grid to write from a  
    //!                      list of boxes.
    //! \param value         The value to fill the cells.
    void fill(const BoxList& bl, T value);
    
    
    // ---- Borders ----

    //! Set into the given borders the given value.
    void bordersValue(const Borders& bound, T value, CA::Bdr::Operator op = CA::Bdr::Equal);


    //! Shifht values from inner zone into the given borders.
    void bordersShift(const Borders& bound);


    // ---- DEBUG ----
      
    //! Dump the buffer (with borders) into the given output stream in a
    //! readable form. The method print the buffer starting from the
    //! top/left corner.
    //! \param out         The output stream/
    //! \param x_sep       The separator of values in the X dimension.
    //! \param y_sep       The separator of values in the Y dimension.
    void dump(std::ostream& out, String x_sep=" ", String y_sep="\n");    


    // ---------  Implementation dependent --------------


    //! Return the event that was generated by the last command that
    //! used (or is using) the buffer.
    cl::Event event() const;


    //! Set the event that was generated by the last command.
    void setEvent(cl::Event& event);


    //! Return the OpenCl buffer.
    cl::Buffer buffer() const;


  protected:
    
    //! Internal method that fill the given region of a cell buffer
    //! with the given real value.
    void fillBox(const Box& box, Real value, std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that fill the given region of a cell buffer
    //! with the given real value.
    void fillBox(const Box& box, State value, std::vector<cl::Event>* wait_events, cl::Event* e);

    
    //! Internal method that copy a 2D device buffer into the cell buffer.
    void copy2DBuffToBox(const Box& box, Real value, 
			 cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
			 std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy a 2D device buffer into the cell buffer.
    void copy2DBuffToBox(const Box& box, State value, 
			 cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
			 std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy the cell buff into a 2D device buffer.
    void copyBoxTo2DBuff(const Box& box, Real value, 
			 cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
			 std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy the cell buff into a 2D device buffer.
    void copyBoxTo2DBuff(const Box& box, State value, 
			 cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
			 std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy the given points of the cell buff
    //! into a 1D device buffer.
    void copyPointsTo1DBuff(const Box& box, Real value, 
			    cl::Buffer& tmp_buff, _caUnsigned tmp_size,
			    cl::Buffer& xy_buff,  _caUnsigned xy_size,
			    std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy the given points of the cell buff
    //! into a 1D device buffer.
    void copyPointsTo1DBuff(const Box& box, State value, 
			    cl::Buffer& tmp_buff, _caUnsigned tmp_size,
			    cl::Buffer& xy_buff,  _caUnsigned xy_size,
			    std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy the 1D device buffer into the given
    //! points of the cell buff 
    void copy1DBuffToPoints(const Box& box, Real value, 
			    cl::Buffer& tmp_buff, _caUnsigned tmp_size,
			    cl::Buffer& xy_buff,  _caUnsigned xy_size,
			    std::vector<cl::Event>* wait_events, cl::Event* e);


    //! Internal method that copy the 1D device buffer into the given
    //! points of the cell buff 
    void copy1DBuffToPoints(const Box& box, State value, 
			    cl::Buffer& tmp_buff, _caUnsigned tmp_size,
			    cl::Buffer& xy_buff,  _caUnsigned xy_size,
			    std::vector<cl::Event>* wait_events, cl::Event* e);

  private:


    //! The reference to the grid.
    Grid& _grid;


    //! The local copy of the caGrid_short from Grid which is going to
    //! be used in the kernels.
    _caGrid_short _cagrid_short;


    //! The number of elements in the openCL buffer.
    _caUnsigned    _buff_num;


    //! The size of the OnpeCL buffer in bytes.
    _caUnsigned    _buff_size;
    

    //! OpenCL buffer with the cell values data.
    mutable cl::Buffer _buff;

    
    //! The last event generated by a command that was using this
    //! buffer. This can be used to synchonyse different command.
    mutable cl::Event  _event;

    
    //! Internal kernels.
    cl::Kernel _kernel_copyCellBuffRealTo2DBuff;
    cl::Kernel _kernel_copyCellBuffStateTo2DBuff;
    cl::Kernel _kernel_copy2DBuffToCellBuffReal;
    cl::Kernel _kernel_copy2DBuffToCellBuffState;
    cl::Kernel _kernel_setValueCellBuffReal;
    cl::Kernel _kernel_setValueCellBuffState;
    cl::Kernel _kernel_copyPLCellBuffRealTo1DBuff;
    cl::Kernel _kernel_copyPLCellBuffStateTo1DBuff;
    cl::Kernel _kernel_copy1DBuffToPLCellBuffReal;
    cl::Kernel _kernel_copy1DBuffToPLCellBuffState;

    //! Buffer used for direct I/O.
    std::vector<T> mem_io;
  };


  /// ----- Inline implementation ----- ///

  
  template<typename T>
  inline CellBuff<T>::CellBuff(Grid& grid, const Options& options):
    _grid(grid),
    _cagrid_short(grid.caGridShort()),
    _buff_num(),
    _buff_size(),
    _buff(),
    _event(),
    _kernel_copyCellBuffRealTo2DBuff(),
    _kernel_copyCellBuffStateTo2DBuff(),
    _kernel_copy2DBuffToCellBuffReal(),
    _kernel_copy2DBuffToCellBuffState(),
    _kernel_setValueCellBuffReal(),
    _kernel_setValueCellBuffState(),
    _kernel_copyPLCellBuffRealTo1DBuff(),
    _kernel_copyPLCellBuffStateTo1DBuff(),
    _kernel_copy1DBuffToPLCellBuffReal(),
    _kernel_copy1DBuffToPLCellBuffState(),
	mem_io()

  {
    // Create the buffer in the device memory of GRID context.  The x
    // size of the buffer is the stride in order to keep the memory
    // coalesced.

    _buff_num  = _grid.caGrid().cb_stride * _grid.caGrid().cb_y_size;

    _buff_size = sizeof(T) * _buff_num;

    _buff = cl::Buffer(_grid.context(), CL_MEM_READ_WRITE, static_cast<std::size_t>(_buff_size) );

    // Used to identify kernel in  the error.
    unsigned int i = 0;
    try{
      // Retrieve the internal kernels.
      _kernel_copyCellBuffRealTo2DBuff    = cl::Kernel(_grid.kernelsProgram(), "copyCellBuffRealTo2DBuff");
      ++i;//1
      _kernel_copyCellBuffStateTo2DBuff   = cl::Kernel(_grid.kernelsProgram(), "copyCellBuffStateTo2DBuff");
      ++i;//2
      _kernel_copy2DBuffToCellBuffReal    = cl::Kernel(_grid.kernelsProgram(), "copy2DBuffToCellBuffReal");
      ++i;//3
      _kernel_copy2DBuffToCellBuffState   = cl::Kernel(_grid.kernelsProgram(), "copy2DBuffToCellBuffState");
      ++i;//4
      _kernel_setValueCellBuffReal        = cl::Kernel(_grid.kernelsProgram(), "setValueCellBuffReal");
      ++i;//5
      _kernel_setValueCellBuffState       = cl::Kernel(_grid.kernelsProgram(), "setValueCellBuffState");
      ++i;//6
      _kernel_copyPLCellBuffRealTo1DBuff  = cl::Kernel(_grid.kernelsProgram(), "copyPLCellBuffRealTo1DBuff");
      ++i;//7
      _kernel_copyPLCellBuffStateTo1DBuff = cl::Kernel(_grid.kernelsProgram(), "copyPLCellBuffStateTo1DBuff");
      ++i;//8
      _kernel_copy1DBuffToPLCellBuffReal  = cl::Kernel(_grid.kernelsProgram(), "copy1DBuffToPLCellBuffReal");
      ++i;//9
      _kernel_copy1DBuffToPLCellBuffState = cl::Kernel(_grid.kernelsProgram(), "copy1DBuffToPLCellBuffState");
      ++i;//10
    }catch(cl::Error e){
      std::cout << "CellBuff creation error: " << e.what() << "(" << e.err() << ")" <<std::endl;
      std::cout << "i = " << i <<std::endl;
      
      
    }
	
    // Initialise the buffer to zero and store the event.
#ifdef  CA_OCL_USE_EVENTS 
    _grid.fill1DBuff(T(),_buff, 0,_buff_num, 0, &_event);
#else
    _grid.fill1DBuff(T(),_buff, 0,_buff_num, NULL, NULL);
#endif
  }


  template<typename T>
  inline CellBuff<T>::~CellBuff()
  {
    //_event.wait();
  }


  template<typename T>
  inline Options CellBuff<T>::options()
  {
    Options options;

    // Add the optional arguments. The tag start from higher number
    // for safety reason.
    CA::Unsigned na = 3000;
    
    // Example
    //options.push_back(new Arguments::Arg(na++,"option-name", "Option desctiption","value",true, true, false));

    return options;
  }


  template<typename T>
  inline void CellBuff<T>::clear(const T& value)
  {    
#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events(1,_event);   

    // Fill the buffer with the given value.
    _grid.fill1DBuff(value, _buff, 0,_buff_num, &wait_events, &_event);    
#else
    _grid.fill1DBuff(value, _buff, 0,_buff_num, NULL, NULL);    
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copy(const CellBuff<T>& src)
  {    
    // Check that the two buffer belong to the same grid. If it not
    // the case. Do nothing.
    if(&_grid!=&src._grid)
      return;

    // Check that both buffer are allocated.
    if(!_buff() || !src._buff())
      return;

#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events;   
    
    // Populate it with the events of the two buffers.
    wait_events.push_back(_event);
    wait_events.push_back(src._event);

    // Copy the buffer using the device memory.
    _grid.queue().enqueueCopyBuffer(src._buff, _buff, 0,  0, _buff_size, &wait_events, &_event);
#else
    _grid.queue().enqueueCopyBuffer(src._buff, _buff, 0,  0, _buff_size, NULL, NULL);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::retrieveData(const Box& box, T* mem, Unsigned mem_x_size, Unsigned mem_y_size)
  {    
    // The borders are not read.

    // Check that the given box is inside the domain of the grid.
    if(!_grid.box().inside( box ) )
       return;

    // Check that the given memory location is big enought to contain
    // the data of the region.
    if(box.w() > mem_x_size || box.h() > mem_y_size)
      return;

    // Create a temporary buffer in the device that is assigned from the given memory.
    size_t  mem_size = sizeof(T) * mem_x_size * mem_y_size;
    cl::Buffer tmp_buff(_grid.context(), CL_MEM_WRITE_ONLY, mem_size);

#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events(1,_event);
    
    // Now copy the CellBuff box into the temporary buffer and keep the event.
    copyBoxTo2DBuff(box,T(),tmp_buff,mem_x_size,mem_y_size,&wait_events,&_event);

    // Wait for the compy to finish.
    _event.wait();
#else
    // Now copy the CellBuff box into the temporary buffer and keep the event.
    copyBoxTo2DBuff(box,T(),tmp_buff,mem_x_size,mem_y_size,NULL,NULL);
#endif    

    // Copy the temporary buffer into the memory
	_grid.queue().enqueueReadBuffer(tmp_buff, CL_TRUE, 0, mem_size, mem, NULL, NULL);
  }


  template<typename T>
  inline void CellBuff<T>::insertData(const Box& box, const T* mem, Unsigned mem_x_size, Unsigned mem_y_size)
  {    
    // The borders are not written.

    // Check that the given box is inside the domain of the grid.
    if(! _grid.box().inside( box ) )
       return;   

    // Check that the given memory location is big enought to contain
    // the data of the region.
    if(box.w() > mem_x_size || box.h() > mem_y_size)
      return;

    // It is not possible to make the copy from mem to _buff non
    // blocking since the given memory can be reused immediately when
    // this function end. Thus, the memory is copyed in a temporary
    // buffer in one go, and then copyed from the temporary buffer to
    // the real buffer.

    // Create the temporary buffer as big as the given memory. 
    size_t  mem_size = sizeof(T) * mem_x_size * mem_y_size;
    cl::Buffer tmp_buff(_grid.context(), CL_MEM_READ_ONLY, mem_size);

#ifdef  CA_OCL_USE_EVENTS 
    // Copy the mem to the temporary buffer (non blocking) and save the event.
    cl::Event e_mem_to_tmp;
    _grid.queue().enqueueWriteBuffer(tmp_buff, CL_FALSE, 0, mem_size, mem, 0, &e_mem_to_tmp);   

    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events;   
    wait_events.push_back(e_mem_to_tmp);
    wait_events.push_back(_event);

    // Now copy the temporary buffer into the CellBuff and keep the event.
    copy2DBuffToBox(box,T(),tmp_buff,mem_x_size,mem_y_size,&wait_events,&_event);

    // Wait for the copy from mem to temporary buffer to finish before
    // returning. The temporary buffer should be removed automatically
    // (hopefully).
    e_mem_to_tmp.wait();
#else
    // Copy the mem to the temporary buffer (blocking) and save the event.
    _grid.queue().enqueueWriteBuffer(tmp_buff, CL_TRUE, 0, mem_size, mem, 0, NULL);   

    // Now copy the temporary buffer into the CellBuff.
    copy2DBuffToBox(box,T(),tmp_buff,mem_x_size,mem_y_size,NULL,NULL);
#endif
  }


  template<typename T>
  inline bool CellBuff<T>::saveData(const std::string& mainid, const std::string& subid)
  {    
    // Create the filename
    std::string filename = _grid.dataDir()+mainid+"_"+subid+"_"+caImplShortName+".CB"; 

    // Create the file in the data dir.
    std::ofstream file(filename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc );

    // If the file is not good, problem!!!
    if(!file.good())
      return false;
    
    // Check if the static variable was initialised.
    if(mem_io.size()==0)
    {
      // Create a STATIC memory buffer that can hold the full devide
      // buffer. ATTENTION being static this memory buffer is shared
      // betwenn various cellbuffer.
      mem_io.resize(_buff_num );
    }

#ifdef  CA_OCL_USE_EVENTS 
    // Wait for a possible event.
    _event.wait();
#endif

    // Copy the buffer into the memory 
	_grid.queue().enqueueReadBuffer(_buff, CL_TRUE, 0, _buff_size, &mem_io[0], NULL, NULL);

    // Write the magic value!
    unsigned int magic = CAAPI_2D_MAGIC;
    file.write(reinterpret_cast<char*>(&magic), sizeof(unsigned int));
    
    // Write the file in a go!
    file.write(reinterpret_cast<char*>(&mem_io[0]), _buff_size);
    bool ret = file.good();

    return ret;
  }


  template<typename T>
  inline bool CellBuff<T>::loadData(const std::string& mainid, const std::string& subid, bool remove)
  {    
    // Create the filename
    std::string filename = _grid.dataDir()+mainid+"_"+subid+"_"+caImplShortName+".CB"; 
    
    // Create the file in the data dir.
    std::ifstream file(filename.c_str(), std::ofstream::in | std::ofstream::binary);
   
    // If the file is not good, problem!!!
    if(!file.good())
      return false;

    // Check if the static variable was initialised.
    if(mem_io.size()==0)
    {
      // Create a STATIC memory buffer that can hold the full devide
      // buffer. ATTENTION being static this memory buffer is shared
      // betwenn various cellbuffer.
      mem_io.resize(_buff_num );
    }
    
    // Check the magic value!
    unsigned int magic = 0;
    file.read(reinterpret_cast<char*>(&magic), sizeof(unsigned int));
    
    if(magic!=CAAPI_2D_MAGIC)
      return false;

    // Read the file in a go!
    file.read(reinterpret_cast<char*>(&mem_io[0]), _buff_size);
    bool ret = file.good() && (_buff_size == file.gcount()) && (file.peek() == EOF);
    
#ifdef  CA_OCL_USE_EVENTS 
    // Wait for a possible event.
    _event.wait();
#endif

    // Copy the mem to the buffer (blocking).
	_grid.queue().enqueueWriteBuffer(_buff, CL_TRUE, 0, _buff_size, &mem_io[0], NULL, NULL);
   
    // If the operation was succesful and the file need to be removed,
    // then delete the file.
    if(ret && remove)
    {
      // Close the file
      file.close();
      std::remove(filename.c_str());
    }

    return ret;
  }

  
  template<typename T>
  inline bool CellBuff<T>::removeData(const std::string& datadir,
				      const std::string& mainid, const std::string& subid)
  {
    // Create the filename
    std::string filename = datadir+mainid+"_"+subid+"_"+caImplShortName+".CB"; 

    // Remove it.
    if(std::remove(filename.c_str())!=0)
      return false;

    return true;
  }


  template<typename T>
  inline bool CellBuff<T>::existData(const std::string& datadir, 
				     const std::string& mainid, const std::string& subid)
  {
    // Create the filename
    std::string filename = datadir+mainid+"_"+subid+"_"+caImplShortName+".CB"; 
    
    // Open with a filestream
    std::ifstream file(filename.c_str());

    return file.good();
  }

  
  template<typename T>
  inline void CellBuff<T>::retrievePoints(const PointList& pl, T* mem, Unsigned size)
  {

    // Check that all the points arte inside the domain of the grid.
    if(! _grid.box().inside( pl.extent() ) )
       return;   

    // Check that the size of the momory location is big enough for the point list.
    if( pl.size() > size)
      return;

    // If the point list does not contains any point ... do nothing.
    if( pl.size()==0)
      return;

    // Retrieve the extent
    Box extent(pl.extent());

    //! ATTENTION This is a hack to solve a nasty problem of when the
    // extent has width or heigh equal to zero.  The extent box must
    // not have an empty size.
    if( extent.w() == 0)  extent.setW(3);
    if( extent.h() == 0)  extent.setH(3);

    // Create a temporary buffer in the device that is assigned from the given memory.
    size_t  tmp_mem_size = sizeof(T) * size;
    cl::Buffer tmp_buff(_grid.context(), CL_MEM_WRITE_ONLY, tmp_mem_size);

    // Create a temporary buffer in the device that will have the x/y coordinate of the points.
    size_t  xy_mem_size = sizeof(_caUnsigned2) * pl.size();
    cl::Buffer xy_buff(_grid.context(), CL_MEM_READ_ONLY, xy_mem_size);

    // Create a temporary buffer that will have the x/y coordinates of the points.
    std::vector<_caUnsigned2> xy_mem(pl.size()); 
    
    // Populate the memory buffer with the x/y coordinates.
    for(int i =0; i< pl.size(); i++)
    {
      xy_mem[i].s[0] = pl[i].x();
      xy_mem[i].s[1] = pl[i].y();
    }

#ifdef  CA_OCL_USE_EVENTS 
    // Copy the mem to the temporary x/y buffer (non blocking) and save the event.
    cl::Event e_mem_to_xy;
    _grid.queue().enqueueWriteBuffer(xy_buff, CL_FALSE, 0, xy_mem_size, &xy_mem[0], 0, &e_mem_to_xy);   

    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events;
    wait_events.push_back(e_mem_to_xy);
    wait_events.push_back(_event);

    // Now copy the Points into the buffer and keep the events.
    copyPointsTo1DBuff(extent,T(),tmp_buff,size, xy_buff, pl.size(), &wait_events,&_event);
    
    // Wait for the compy to finish.
    _event.wait();
#else

    // Copy the mem to the temporary x/y buffer (blocking).
    _grid.queue().enqueueWriteBuffer(xy_buff, CL_TRUE, 0, xy_mem_size, &xy_mem[0], 0, NULL);   

    // Now copy the Points into the buffer
    copyPointsTo1DBuff(extent,T(),tmp_buff,size, xy_buff, pl.size(), NULL, NULL);
#endif
    
    // Copy the temporary buffer into the memory
	_grid.queue().enqueueReadBuffer(tmp_buff, CL_TRUE, 0, tmp_mem_size, mem, NULL, NULL);
  }


  template<typename T>
  inline void CellBuff<T>::insertPoints(const PointList& pl, const T* mem, Unsigned size)
  {

    // Check that all the points arte inside the domain of the grid.
    if(! _grid.box().inside( pl.extent() ) )
       return;   

    // Check that the size of the momory location is big enough for the point list.
    if( pl.size() > size)
      return;

    // If the point list does not contains any point ... do nothing.
    if( pl.size()==0)
      return;

    // Retrieve the extent
    Box extent(pl.extent());

    // The extent box must not have an empty size.
    if( extent.w() == 0)  extent.setW(3);
    if( extent.h() == 0)  extent.setH(3);

    ///////// ATTENTION ------------> NON TESTED!

    // Create a temporary buffer in the device that is assigned from the given memory.
    size_t  tmp_mem_size = sizeof(T) * size;
    cl::Buffer tmp_buff(_grid.context(), CL_MEM_WRITE_ONLY, tmp_mem_size);

#ifdef  CA_OCL_USE_EVENTS 
    // Copy the mem to the temporary buffer (non blocking) and save the event.
    cl::Event e_mem_to_tmp;
    _grid.queue().enqueueWriteBuffer(tmp_buff, CL_FALSE, 0, tmp_mem_size, mem, 0, &e_mem_to_tmp);   
#else
    // Copy the mem to the temporary buffer (blocking)
    _grid.queue().enqueueWriteBuffer(tmp_buff, CL_TRUE, 0, tmp_mem_size, mem, 0, NULL);   
#endif    

    // Create a temporary buffer in the device that will have the x/y coordinate of the points.
    size_t  xy_mem_size = sizeof(_caUnsigned2) * pl.size();
    cl::Buffer xy_buff(_grid.context(), CL_MEM_READ_ONLY, xy_mem_size);

    // Create a temporary buffer that will have the x/y coordinates of the points.
    std::vector<_caUnsigned2> xy_mem(pl.size()); 
    
    // Populate the memory buffer with the x/y coordinates.
    for(int i =0; i< pl.size(); i++)
    {
      xy_mem[i].x = pl[i].x();
      xy_mem[i].y = pl[i].y();
    }

#ifdef  CA_OCL_USE_EVENTS 
    // Copy the mem to the temporary x/y buffer (non blocking) and save the event.
    cl::Event e_mem_to_xy;
    _grid.queue().enqueueWriteBuffer(xy_buff, CL_FALSE, 0, xy_mem_size, &xy_mem[0], 0, &e_mem_to_xy);   

    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events;
    wait_events.push_back(e_mem_to_tmp);
    wait_events.push_back(e_mem_to_xy);
    wait_events.push_back(_event);

    // Now copy the Points into the buffer and keep the events.
    copy1DBuffToPoints(extent,T(),tmp_buff,size, xy_buff, pl.size(), &wait_events,&_event);
    
    // Wait for the copy from mem to temporary buffer to finish before
    // returning. The temporary buffer should be removed automatically
    // (hopefully).
    e_mem_to_tmp.wait();
    e_mem_to_xy.wait();
#else
    // Copy the mem to the temporary x/y buffer (blocking)
    _grid.queue().enqueueWriteBuffer(xy_buff, CL_TRUE, 0, xy_mem_size, &xy_mem[0], 0, NULL);   

    // Now copy the Points into the buffer.
    copy1DBuffToPoints(extent,T(),tmp_buff,size, xy_buff, pl.size(), NULL,NULL);    
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copy2DBuffToBox(const Box& box, Real value, 
					   cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
					   std::vector<cl::Event>* wait_events, cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();

    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    // Set the arguments of the kernel.
    _kernel_copy2DBuffToCellBuffReal.setArg(0,_cagrid_short);
    _kernel_copy2DBuffToCellBuffReal.setArg(1,tmp_buff);
    _kernel_copy2DBuffToCellBuffReal.setArg(2,x_size);
    _kernel_copy2DBuffToCellBuffReal.setArg(3,y_size);
    _kernel_copy2DBuffToCellBuffReal.setArg(4,_buff);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy2DBuffToCellBuffReal, offset, global, cl::NDRange(), wait_events, e);
#else
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy2DBuffToCellBuffReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copy2DBuffToBox(const Box& box, State value, 
					   cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
					   std::vector<cl::Event>* wait_events, cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();
    
    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    // Set the arguments of the kernel.
    _kernel_copy2DBuffToCellBuffState.setArg(0,_cagrid_short);
    _kernel_copy2DBuffToCellBuffState.setArg(1,tmp_buff);
    _kernel_copy2DBuffToCellBuffState.setArg(2,x_size);
    _kernel_copy2DBuffToCellBuffState.setArg(3,y_size);
    _kernel_copy2DBuffToCellBuffState.setArg(4,_buff);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy2DBuffToCellBuffState,offset,global,cl::NDRange(),wait_events,e);  
#else
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy2DBuffToCellBuffState, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copyBoxTo2DBuff(const Box& box, Real value, 
					   cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
					   std::vector<cl::Event>* wait_events, cl::Event* e)
  {
  // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();
    
    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    //cl::NDRange global(box.w(), box.h());
    
    // Set the arguments of the kernel.
    _kernel_copyCellBuffRealTo2DBuff.setArg(0,_cagrid_short);
    _kernel_copyCellBuffRealTo2DBuff.setArg(1,_buff);
    _kernel_copyCellBuffRealTo2DBuff.setArg(2,tmp_buff);
    _kernel_copyCellBuffRealTo2DBuff.setArg(3,x_size);
    _kernel_copyCellBuffRealTo2DBuff.setArg(4,y_size);
    
    
	
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyCellBuffRealTo2DBuff,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyCellBuffRealTo2DBuff, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copyBoxTo2DBuff(const Box& box, State value, 
					   cl::Buffer& tmp_buff, _caUnsigned x_size, _caUnsigned y_size,
					   std::vector<cl::Event>* wait_events, cl::Event* e)
  {
  // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();
    
    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    // Set the arguments of the kernel.
    _kernel_copyCellBuffStateTo2DBuff.setArg(0,_cagrid_short);
    _kernel_copyCellBuffStateTo2DBuff.setArg(1,_buff);
    _kernel_copyCellBuffStateTo2DBuff.setArg(2,tmp_buff);
    _kernel_copyCellBuffStateTo2DBuff.setArg(3,x_size);
    _kernel_copyCellBuffStateTo2DBuff.setArg(4,y_size);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyCellBuffStateTo2DBuff,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel that copy the temporary buffer into the given
	// region of the cell buffer.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyCellBuffStateTo2DBuff, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copyPointsTo1DBuff(const Box& box, Real value, 
					      cl::Buffer& tmp_buff, _caUnsigned tmp_size,
					      cl::Buffer& xy_buff,  _caUnsigned xy_size,
					      std::vector<cl::Event>* wait_events, cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();   

    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    _kernel_copyPLCellBuffRealTo1DBuff.setArg(0,_cagrid_short);
    _kernel_copyPLCellBuffRealTo1DBuff.setArg(1,_buff);
    _kernel_copyPLCellBuffRealTo1DBuff.setArg(2,tmp_buff);
    _kernel_copyPLCellBuffRealTo1DBuff.setArg(3,tmp_size);
    _kernel_copyPLCellBuffRealTo1DBuff.setArg(4,xy_buff);
    _kernel_copyPLCellBuffRealTo1DBuff.setArg(5,xy_size);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyPLCellBuffRealTo1DBuff,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyPLCellBuffRealTo1DBuff, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }
  

  template<typename T>
  inline void CellBuff<T>::copyPointsTo1DBuff(const Box& box, State value, 
					      cl::Buffer& tmp_buff, _caUnsigned tmp_size,
					      cl::Buffer& xy_buff,  _caUnsigned xy_size,
					      std::vector<cl::Event>* wait_events, cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();   
    
    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    _kernel_copyPLCellBuffStateTo1DBuff.setArg(0,_cagrid_short);
    _kernel_copyPLCellBuffStateTo1DBuff.setArg(1,_buff);
    _kernel_copyPLCellBuffStateTo1DBuff.setArg(2,tmp_buff);
    _kernel_copyPLCellBuffStateTo1DBuff.setArg(3,tmp_size);
    _kernel_copyPLCellBuffStateTo1DBuff.setArg(4,xy_buff);
    _kernel_copyPLCellBuffStateTo1DBuff.setArg(5,xy_size);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyPLCellBuffStateTo1DBuff,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copyPLCellBuffStateTo1DBuff, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }
  

  template<typename T>
  inline void CellBuff<T>::copy1DBuffToPoints(const Box& box, Real value, 
					      cl::Buffer& tmp_buff, _caUnsigned tmp_size,
					      cl::Buffer& xy_buff,  _caUnsigned xy_size,
					      std::vector<cl::Event>* wait_events, cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();   

    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    _kernel_copy1DBuffToPLCellBuffReal.setArg(0,_cagrid_short);
    _kernel_copy1DBuffToPLCellBuffReal.setArg(1,tmp_buff);
    _kernel_copy1DBuffToPLCellBuffReal.setArg(2,tmp_size);
    _kernel_copy1DBuffToPLCellBuffReal.setArg(3,_buff);
    _kernel_copy1DBuffToPLCellBuffReal.setArg(4,xy_buff);
    _kernel_copy1DBuffToPLCellBuffReal.setArg(5,xy_size);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy1DBuffToPLCellBuffReal,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy1DBuffToPLCellBuffReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::copy1DBuffToPoints(const Box& box, State value, 
					      cl::Buffer& tmp_buff, _caUnsigned tmp_size,
					      cl::Buffer& xy_buff,  _caUnsigned xy_size,
					      std::vector<cl::Event>* wait_events, cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();   

    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    _kernel_copy1DBuffToPLCellBuffState.setArg(0,_cagrid_short);
    _kernel_copy1DBuffToPLCellBuffState.setArg(1,tmp_buff);
    _kernel_copy1DBuffToPLCellBuffState.setArg(2,tmp_size);
    _kernel_copy1DBuffToPLCellBuffState.setArg(3,_buff);
    _kernel_copy1DBuffToPLCellBuffState.setArg(4,xy_buff);
    _kernel_copy1DBuffToPLCellBuffState.setArg(5,xy_size);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy1DBuffToPLCellBuffState,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel.
	_grid.queue().enqueueNDRangeKernel(_kernel_copy1DBuffToPLCellBuffState, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }
    

  template<typename T>
  inline void CellBuff<T>::sequentialOp(const BoxList& bl, T& value, CA::Seq::Operator op) const
  {
    // Check that the extent of the boxlist is inside the domain of
    // the grid.
    if(! _grid.box().inside( bl.extent() ) )
       return;
    
    // Compute base index
    const _caUnsigned border  = _grid.caGrid().cb_border;
    const _caUnsigned xoffset = _grid.caGrid().cb_x_offset;


#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events(1,_event);   


    std::vector<cl::Event> *pwe = &wait_events;
#else
    std::vector<cl::Event> *pwe = NULL;
#endif

    // Cycle through the boxes.
    for(BoxList::ConstIter ibox = bl.begin(); ibox!=bl.end(); ++ibox)
    {      
      Box box(*ibox);

      _grid.seq2DBuff(value, _buff, 
		      0,_grid.caGrid().cb_stride, 
		      box.x()+ xoffset, box.w(),
		      box.y()+border, box.h(),
		      op, pwe, 0);      
    }

    _grid.queue().flush();
  }

  
  template<typename T>
  inline void CellBuff<T>::fill(const BoxList& bl, T value)
  {    
    // The borders are not written.
    
    // Check that the extent of the boxlist is inside the domain of
    // the grid.
    if(! _grid.box().inside( bl.extent() ) )
       return;

#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events(1,_event);   


    std::vector<cl::Event> *pwe = &wait_events;
#else
    std::vector<cl::Event> *pwe = NULL;
#endif

    // Cycle through the boxes.
    for(BoxList::ConstIter ibox = bl.begin(); ibox!=bl.end(); ++ibox)
    {      
      Box box(*ibox);
      
      // Call the specific method. Do not store the event.
      fillBox(box,value, pwe,0);
    }

	_grid.queue().flush();

#ifdef  CA_OCL_USE_EVENTS 
    // Set the event of this buffer to wait for all the previous
    // events (copy) to finish.
    
    _grid.queue().enqueueMarker(&_event);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::fillBox(const Box& box, Real value, std::vector<cl::Event>* wait_events,cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();

    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));

    // Set the arguments of the kernel.
    _kernel_setValueCellBuffReal.setArg(0,_cagrid_short);
    _kernel_setValueCellBuffReal.setArg(1,_buff);
    _kernel_setValueCellBuffReal.setArg(2,value);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel that set a value into the given region. 
	_grid.queue().enqueueNDRangeKernel(_kernel_setValueCellBuffReal,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel that set a value into the given region. 
	_grid.queue().enqueueNDRangeKernel(_kernel_setValueCellBuffReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }

  
  template<typename T>
  inline void CellBuff<T>::fillBox(const Box& box, State value, std::vector<cl::Event>* wait_events,cl::Event* e)
  {
    // Set the value of the box into the local copy of the cagrid.
    _cagrid_short.bx_lx = box.x();
    _cagrid_short.bx_ty = box.y();
    _cagrid_short.bx_rx = box.w() + box.x();
    _cagrid_short.bx_by = box.h() + box.y();

    // Set the NDrange for the global workspace and offset from the
    // given box. The size of the global workspace is a multiple of
    // warp.
    cl::NDRange offset(box.x(), box.y());   
    cl::NDRange global(computeStride(box.w(),_grid.warp()), computeStride(box.h(),_grid.warp()));
    
    // Set the arguments of the kernel.
    _kernel_setValueCellBuffState.setArg(0,_cagrid_short);
    _kernel_setValueCellBuffState.setArg(1,_buff);
    _kernel_setValueCellBuffState.setArg(2,value);
    
    
#ifdef  CA_OCL_USE_EVENTS 
	// Lunch the kernel that set a value into the given region. 
	_grid.queue().enqueueNDRangeKernel(_kernel_setValueCellBuffState,offset,global,cl::NDRange(),wait_events,e);
#else
	// Lunch the kernel that set a value into the given region. 
	_grid.queue().enqueueNDRangeKernel(_kernel_setValueCellBuffState, offset, global, cl::NDRange(), NULL, NULL);
#endif
  }


  template<typename T> 
  inline void CellBuff<T>::bordersValue(const Borders& bound, T value, CA::Bdr::Operator op)
  {
    // Compute base index
    const _caUnsigned border  = _grid.caGrid().cb_border;
    const _caUnsigned xoffset = _grid.caGrid().cb_x_offset;

#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events(1,_event);


    std::vector<cl::Event> *pwe = &wait_events;
#else
    std::vector<cl::Event> *pwe = NULL;
#endif

    // Cycle through the segments
    for(int s=0; s<bound.numSegments(); ++s)
    {
      // Retrive segment
      Borders::Segment seg = bound.segment(s);
      
      // Execute action depend on segment.
      switch(seg.type)
      {
      case Top:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().x_size)
	    seg.stop = _grid.caGrid().x_size;

	  _grid.op2DBuff(value, _buff,
			 _grid.caGrid().cb_stride,
			 seg.start + xoffset, seg.stop - seg.start, 
			 0, border, 
			 op, pwe, 0);
	}
	break;
	
      case Bottom:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().x_size)
	    seg.stop = _grid.caGrid().x_size;
	  
	  _grid.op2DBuff(value, _buff,
			 _grid.caGrid().cb_stride,
			 seg.start + xoffset, seg.stop - seg.start, 
			 _grid.caGrid().cb_y_size - border, border, 
			 op, pwe, 0);
	}

	break;
	
      case Left:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().y_size)
	    seg.stop = _grid.caGrid().y_size;

	  _grid.op2DBuff(value, _buff,       
			 _grid.caGrid().cb_stride,
			 0, xoffset, 
			 seg.start + border, seg.stop - seg.start, 
			 op, pwe, 0);
	}

	break;

      case Right:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().y_size)
	    seg.stop = _grid.caGrid().y_size;
	  
	  _grid.op2DBuff(value, _buff,
			 _grid.caGrid().cb_stride,
			 _grid.caGrid().cb_x_size - border, border, 
			 seg.start + border, seg.stop - seg.start, 
			 op,pwe, 0);
	} 
	break;
      }
    }
    
    // Cycle through the corners
    for(int c=0; c<bound.numCorners(); ++c)
    {
      // Retrive segment
      Corner corner = bound.corner(c);

      // Execute action depend on segment.
      switch(corner)
      {
      case TopLeft:
	_grid.op2DBuff(value, _buff,
		       _grid.caGrid().cb_stride,
		       0, xoffset, 
		       0, border, 
		       op, pwe, 0);
	break;
	
      case TopRight:
	_grid.op2DBuff(value, _buff,
		       _grid.caGrid().cb_stride,
		       _grid.caGrid().cb_x_size - border, border, 
		       0, border, 
		       op, pwe, 0);
	break;

      case BottomLeft:
	_grid.op2DBuff(value, _buff,
		       _grid.caGrid().cb_stride,
		       0, xoffset, 
		       _grid.caGrid().cb_y_size - border, border, 
		       op, pwe, 0);
	break;

      case BottomRight:
	_grid.op2DBuff(value, _buff,
		       _grid.caGrid().cb_stride,
		       _grid.caGrid().cb_x_size - border, border, 		 
		       _grid.caGrid().cb_y_size - border, border, 
		       op, pwe, 0);
	
	break;
      }
    }    
    
	_grid.queue().flush();

#ifdef  CA_OCL_USE_EVENTS 
    // Set the event of this buffer to wait for all the previous
    // events to finish.
    
    _grid.queue().enqueueMarker(&_event);
#endif
  }
    

  template<typename T>
  inline void CellBuff<T>::bordersShift(const Borders& bound)
  {  
    // Compute base index
    const _caUnsigned border  = _grid.caGrid().cb_border;
    const _caUnsigned xoffset = _grid.caGrid().cb_x_offset;

#ifdef  CA_OCL_USE_EVENTS 
    // Create the list of event ot wait.
    std::vector<cl::Event> wait_events(1,_event);


    std::vector<cl::Event> *pwe = &wait_events;
#else
    std::vector<cl::Event> *pwe = NULL;
#endif

    // The segment are copied using the copy(H,V)Line methods. These
    // methods work by identifying a vertical or horizonatal line and
    // the copy this line into the vertical or horizontal lines of a
    // given area. In this case the area are the borders.

    // Cycle through the segments
    for(int s=0; s<bound.numSegments(); ++s)
    {
      // Retrive segment
      Borders::Segment seg = bound.segment(s);
      
      // Execute action depend on segment.
      switch(seg.type)
      {
      case Top:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().x_size)
	    seg.stop = _grid.caGrid().x_size;

	  _grid.copy2DBuffHLine(T(), _buff,
				border, 
				_grid.caGrid().cb_stride,
				seg.start + xoffset, seg.stop - seg.start, 
				0, border, 
				pwe, 0);
	}
	break;

      case Bottom:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().x_size)
	    seg.stop = _grid.caGrid().x_size;
	  
	  _grid.copy2DBuffHLine(T(), _buff,
				_grid.caGrid().cb_y_size - border - border, 
				_grid.caGrid().cb_stride,
				seg.start + xoffset, seg.stop - seg.start, 
				_grid.caGrid().cb_y_size - border, border, 
				pwe, 0);
	}

	break;
	
      case Left:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().y_size)
	    seg.stop = _grid.caGrid().y_size;

	  _grid.copy2DBuffVLine(T(), _buff,
				xoffset, 
				_grid.caGrid().cb_stride,
				0, xoffset, 
				seg.start + border, seg.stop - seg.start, 
				pwe, 0);
	}

	break;

      case Right:
	{
	  // Check the end of the segment
	  if(seg.stop > _grid.caGrid().y_size)
	    seg.stop = _grid.caGrid().y_size;

	  _grid.copy2DBuffVLine(T(), _buff,
				_grid.caGrid().cb_x_size - border - border, 
				_grid.caGrid().cb_stride,
				_grid.caGrid().cb_x_size - border, border, 
				seg.start + border, seg.stop - seg.start, 
				pwe, 0);
	} 
	break;
      }
    }
    
    // Cycle through the corners
    for(int c=0; c<bound.numCorners(); ++c)
    {
      // Retrive segment
      Corner corner = bound.corner(c);
      
      // Execute action depend on segment.
      switch(corner)
      {
      case TopLeft:
	_grid.copy2DBuffPoint(T(), _buff,
			      xoffset, border, 
			      _grid.caGrid().cb_stride,
			      0, xoffset, 
			      0, border, 
			      pwe, 0);
	break;
	
      case TopRight:
	_grid.copy2DBuffPoint(T(), _buff,
			      _grid.caGrid().cb_x_size - border - border, border, 
			      _grid.caGrid().cb_stride,
			      _grid.caGrid().cb_x_size - border, border, 
			      0, border, 
			      pwe, 0);
	break;

      case BottomLeft:
	_grid.copy2DBuffPoint(T(), _buff,
			      xoffset, _grid.caGrid().cb_y_size - border - border, 
			      _grid.caGrid().cb_stride,
			      0, xoffset, 
			      _grid.caGrid().cb_y_size - border, border, 
			      pwe, 0);
	break;

      case BottomRight:
	_grid.copy2DBuffPoint(T(), _buff,
			      _grid.caGrid().cb_x_size - border - border, _grid.caGrid().cb_y_size - (2*border),
			      _grid.caGrid().cb_stride,
			      _grid.caGrid().cb_x_size - border, border, 		 
			      _grid.caGrid().cb_y_size - border, border, 
			      pwe, 0);
	
	break;
      }
    }    

	_grid.queue().flush();

#ifdef  CA_OCL_USE_EVENTS 
    // Set the event of this buffer to wait for all the previous
    // events to finish.
    
    _grid.queue().enqueueMarker(&_event);
#endif
  }


  template<typename T>
  inline void CellBuff<T>::dump(std::ostream& out, String x_sep, String y_sep)
  {
    // Create a temporary array to keep the buffer.
    std::vector<T> mem(static_cast<size_t>( _buff_num ));

#ifdef  CA_OCL_USE_EVENTS 
    // Wait for the last command issued that uses this bufer to
    // finish.
    _event.wait();

    // Copy the buffer to memory (blocking) abd keep store the event.
    _grid.queue().enqueueReadBuffer(_buff, CL_TRUE, 0, static_cast<size_t>(_buff_size), &mem[0], 0, &_event);   

#else    
    _grid.queue().enqueueReadBuffer(_buff, CL_TRUE, 0, static_cast<size_t>(_buff_size), &mem[0], 0, NULL);   
#endif    

    // Cycle through the region of cells to print. 
    for(_caUnsigned j_reg=0; j_reg<_grid.caGrid().cb_y_size; ++j_reg)
    {      
      //_caUnsigned start = 0;
      _caUnsigned start = _grid.caGrid().cb_x_offset-_grid.caGrid().cb_border; // Consider the X offset
      
      for(_caUnsigned i_reg=start; i_reg<_grid.caGrid().cb_x_size; ++i_reg)
      {
	out<<mem[static_cast<size_t>(j_reg *_grid.caGrid().cb_stride + i_reg)];	
	out<<x_sep;
      }
      out<<y_sep;
    }       

#ifdef  CA_OCL_USE_EVENTS 
    // Set the event of this buffer to wait for all the previous
    // events (copy) to finish.
    _grid.queue().enqueueMarker(&_event);
#endif
  }


  template<typename T>
  inline cl::Event CellBuff<T>::event() const
  {
    return _event;
  }



  template<typename T>
  inline void CellBuff<T>::setEvent(cl::Event& event)
  {
    _event = event;
  }


  template<typename T>
  inline cl::Buffer CellBuff<T>::buffer() const
  {
    return _buff;
  }


}


#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER


#endif	// _CA_CELLBUFF_HPP_
