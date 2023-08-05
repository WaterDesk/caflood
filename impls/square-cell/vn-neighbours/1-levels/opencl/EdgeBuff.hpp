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

#ifndef _CA_EDGEBUFF_HPP_
#define _CA_EDGEBUFF_HPP_


//! \file EdgeBuff.hpp
//! Contains the common class of the buffer which contains a value
//! (Real/State) for each edge in the grid. Edge betwen two cells
//! share the same value.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"Grid.hpp"
#include<algorithm>
#include<limits>


// Get rid of the annoying visual studio warning 4244 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif // _MSC_VER


namespace CA {

    //! Define the buffer which contains a value (real/state) for each
    //! edge of a cell in a square regular grid. Edge betwen two cells
    //! share the same value.

    //! This is the OpenCL implementation. The object contains a single
    //! buffer which is composed of two sub-buffers: One per the
    //! North-South shared edges the other for West-East shared
    //! edges. The buffer is created directly into the (single) device
    //! memory. At the moment this object uses a single event that
    //! indicates the last command that used (or is working on) the
    //! buffer.

    //! \attention The CA_EDGEBUFF_??? item used in the CA function can be
    //! different from this class. 

    //! \warning The buffer is bigger than the Grid since it has to
    //! account of border edges. The number of border edges depends on
    //! the number of neighbours' levels.

    template<typename T>
    class EdgeBuff
    {
    public:

        //! Create the buffer. It is possible to have implementation
        //! specific options set by using the options list. \attention Do
        //! not destroy the grid before destroying this buffer.
        //! \param grid    The grid
        //! \param options The list of implementation specific options.
        EdgeBuff(Grid& grid, const Options& options = Options());

        //! Destroy the buffer.
        virtual ~EdgeBuff();

        //! Return the specific options about the EdgeBuff object and this
        //! implementation.
        static Options options();

        //! Clear the buffer, i.e. set a value in all the elements
        //! (borders included). The default is zero.
        void clear(const T& value = T());

        //! Copy all the value of the given buffer into the object buffer
        void copy(const EdgeBuff<T>& src);

        //! Save the entire buffer in the DataDir using an unique main id
        //! (filename / buffername) and unique sub id (time step /
        //! checkpoint) using as direct and quick I/O technique as
        //! possible.
        //! \attention How the buffer is saved is implementation
        //! dependant.
        //! \return true if successful.
        bool saveData(const std::string& mainid, const std::string& subid);

        //! Load the entire buffer from teh DataDir of the given unique
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

        //! Execute the given sequential operator on the values of all the
        //! edges of the given region of the grid and return the result of
        //! the operation in an array of edges.
        //! For example if the operator is CA::add it returns 
        //! array[0] = edge 0 of cell 1 + edge 0 of cell 2 + ....
        //! array[1] = edge 1 of cell 1 + edge 1 of cell 2 + ....
        //! \attention This method can only use commutative operation Add/Mul.
        //! \param bl            Identifies the region of the grid from a list of
        //!                      boxes to execute the function.
        //! \param values        The starting value of the operation and the
        //!                      results.
        //! \param op            The commutative operator to execute in each cell .
        void sequentialOp(const BoxList& bl, std::vector<T>& values, CA::Seq::Operator op) const;

        //! Set the given array with the maximum value of each edge of all
        //! visible cells in given region of the grid identified by a list
        //! of boxes.
        //! \param bl     Identifies the region of the grid to read
        //!               from a list of boxes.
        //! \param values The array with starting value and maximum results.
        //! \param abs    If true use the absolute value.
        void max(const BoxList& bl, T values[], bool abs = false) const;

        //! Set the given array with the maximum value of each edge of all
        //! visible cells in given region of the grid identified by a list
        //! of boxes.
        //! \param bl     Identifies the region of the grid to read
        //!               from a list of boxes.
        //! \param values The array with starting value and minimum results.
        //! \param abs    If true use the absolute value.
        void min(const BoxList& bl, T values[], bool abs = false) const;

        //! Fill all values of the edges of the given region of the grid
        //! with the given value. The region of the grid is identifies by
        //! a list of boxes.
        //! \param bl            Identifies the region of the grid to write 
        //!                      from a list of boxes.
        //! \param value         The value to fill the cells.
        void fill(const BoxList& bl, T value);

        // ---- DEBUG ----

        //! Dump the buffer (with borders) into the given output stream in a
        //! readable form. The method print the buffer starting from the
        //! top/left corner.
        //! \param out         The output stream/
        //! \param x_sep       The separator of values in the X dimension.
        //! \param y_sep       The separator of values in the Y dimension.
        void dump(std::ostream& out, String x_sep = " ", String y_sep = "\n");

        // ---------  Implementation dependent --------------

        //! Return the event that was generated by the last command that
        //! used (or is using) the buffer.
        cl::Event event() const;

        //! Set the event that was generated by the last command.
        void setEvent(cl::Event& event);

        //! Return the OpenCl buffer.
        cl::Buffer buffer() const;

    protected:

        //! Internal method that fill all values of the edges of the given
        //! region for a real buffer.
        void fillBox(const Box& box, Real value, std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that fill all values of the edges of the given
        //! region for a state buffer.
        void fillBox(const Box& box, State value, std::vector<cl::Event>* wait_events, cl::Event* e);

    private:

        //! The reference to the grid.
        Grid& _grid;

        //! The local copy of the caGrid_short from Grid which is going to
        //! be used in the kernels.
        _caGrid_short _cagrid_short;

        //! The number of elements in the North/South sub-buffer
        _caUnsigned    _ns_buff_num;

        //! The number of elements in the West/East sub-buffer
        _caUnsigned    _we_buff_num;

        //! The number of elements in the openCL buffer.
        _caUnsigned    _buff_num;

        //! The size of the OnpeCL buffer in bytes.
        _caUnsigned    _buff_size;

        //! OpenCL buffer with the Edge values data. This buffer is the
        //! summ of two sub-buffers.
        mutable cl::Buffer _buff;

        //! The last event generated by a command that was using this
        //! buffer. This can be used to synchonyse different command.
        mutable cl::Event  _event;

        //! Internal kernels.
        cl::Kernel _kernel_setValueEdgeBuffReal;
        cl::Kernel _kernel_setValueEdgeBuffState;

        //! Buffer used for direct I/O.
        std::vector<T> mem_io;
    };


    /// ----- Inline implementation ----- ///

    template<typename T>
    inline EdgeBuff<T>::EdgeBuff(Grid& grid, const Options& options) :
        _grid(grid),
        _cagrid_short(grid.caGridShort()),
        _ns_buff_num(),
        _we_buff_num(),
        _buff_num(),
        _buff_size(),
        _buff(),
        _event(),
        _kernel_setValueEdgeBuffReal(),
        _kernel_setValueEdgeBuffState(),
        mem_io()
    {
        // Create the buffer in the device memory of GRID context. The
        // buffer is the sum of the two sub-buffers.  The x size of the
        // sub-buffers is the stride in order to keep the memory
        // coalesced.

        _ns_buff_num = _grid.caGrid().eb_ns_stride * _grid.caGrid().eb_ns_y_size;
        _we_buff_num = _grid.caGrid().eb_we_stride * _grid.caGrid().eb_we_y_size;

        _buff_num = _ns_buff_num + _we_buff_num;

        _buff_size = sizeof(T) * _buff_num;

        _buff = cl::Buffer(_grid.context(), CL_MEM_READ_WRITE, static_cast<std::size_t>(_buff_size));

        // Retrieve the internal kernels.
        _kernel_setValueEdgeBuffReal = cl::Kernel(_grid.kernelsProgram(), "setValueEdgeBuffReal");
        _kernel_setValueEdgeBuffState = cl::Kernel(_grid.kernelsProgram(), "setValueEdgeBuffState");

#ifdef  CA_OCL_USE_EVENTS
        // Initialise the buffer to zero and store the event.
        _grid.fill1DBuff(T(), _buff, 0, _buff_num, 0, &_event);
#else
        // Initialise the buffer to zero and store the event.
        _grid.fill1DBuff(T(), _buff, 0, _buff_num, 0, NULL);
#endif
    }


    template<typename T>
    inline EdgeBuff<T>::~EdgeBuff()
    {
#ifdef  CA_OCL_USE_EVENTS
        _event.wait();
#endif
    }


    template<typename T>
    inline Options EdgeBuff<T>::options()
    {
        Options options;

        // Add the optional arguments. The tag start from higher number
        // for safety reason.
        CA::Unsigned na = 4000;

        // Example
        //options.push_back(new Arguments::Arg(na++,"option-name", "Option desctiption","value",true, true, false));

        return options;
    }


    template<typename T>
    inline void EdgeBuff<T>::clear(const T& value)
    {
#ifdef  CA_OCL_USE_EVENTS 
        // Create the list of event ot wait.
        std::vector<cl::Event> wait_events(1, _event);

        // Fill the buffer with the given value.
        _grid.fill1DBuff(value, _buff, 0, _buff_num, &wait_events, &_event);
#else
        // Fill the buffer with the given value.
        _grid.fill1DBuff(value, _buff, 0, _buff_num, NULL, NULL);
#endif
    }


    template<typename T>
    inline void EdgeBuff<T>::copy(const EdgeBuff<T>& src)
    {
        // Check that the two buffer belong to the same grid. If it not
        // the case. Do nothing.
        if (&_grid != &src._grid)
            return;

        // Check that both buffer are allocated.
        if (!_buff() || !src._buff())
            return;
#ifdef  CA_OCL_USE_EVENTS 
        // Create the list of event ot wait.
        std::vector<cl::Event> wait_events;

        // Populate it with the events of the two buffers.
        wait_events.push_back(_event);
        wait_events.push_back(src._event);

        // Copy the buffer using the device memory.
        _grid.queue().enqueueCopyBuffer(src._buff, _buff, 0, 0, _buff_size, &wait_events, &_event);
#else
        // Copy the buffer using the device memory.
        _grid.queue().enqueueCopyBuffer(src._buff, _buff, 0, 0, _buff_size, NULL, NULL);
#endif
    }


    template<typename T>
    inline bool EdgeBuff<T>::saveData(const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = _grid.dataDir() + mainid + "_" + subid + "_" + caImplShortName + ".EB";

        // Create the file in the data dir.
        std::ofstream file(filename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

        // If the file is not good, problem!!!
        if (!file.good())
            return false;

        // Check if the static variable was initialised.
        if (mem_io.size() == 0)
        {
            // Create a STATIC memory buffer that can hold the full devide
            // buffer. ATTENTION being static this memory buffer is shared
            // betwenn various cellbuffer.
            mem_io.resize(_buff_num);
        }

        // Wait for a possible event.
        //_event.wait();

        // Copy the buffer into the memory BLOCKING
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
    inline bool EdgeBuff<T>::loadData(const std::string& mainid, const std::string& subid, bool remove)
    {
        // Create the filename
        std::string filename = _grid.dataDir() + mainid + "_" + subid + "_" + caImplShortName + ".EB";

        // Create the file in the data dir.
        std::ifstream file(filename.c_str(), std::ofstream::in | std::ofstream::binary);

        // If the file is not good, problem!!!
        if (!file.good())
            return false;

        // Check the magic value!
        unsigned int magic = 0;
        file.read(reinterpret_cast<char*>(&magic), sizeof(unsigned int));

        if (magic != CAAPI_2D_MAGIC)
            return false;

        // Read the file in a go!
        file.read(reinterpret_cast<char*>(&mem_io[0]), _buff_size);
        bool ret = file.good() && (_buff_size == file.gcount()) && (file.peek() == EOF);

        // Wait for a possible event.
        //_event.wait();

        // Copy the mem to the buffer (blocking).
        _grid.queue().enqueueWriteBuffer(_buff, CL_TRUE, 0, _buff_size, &mem_io[0], NULL, NULL);

        // If the operation was succesful and the file need to be removed,
        // then delete the file.
        if (ret && remove)
        {
            // Close the file
            file.close();
            std::remove(filename.c_str());
        }

        return ret;
    }


    template<typename T>
    inline bool EdgeBuff<T>::removeData(const std::string& datadir,
        const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = datadir + mainid + "_" + subid + "_" + caImplShortName + ".EB";

        // Remove it.
        if (std::remove(filename.c_str()) != 0)
            return false;

        return true;
    }


    template<typename T>
    inline bool EdgeBuff<T>::existData(const std::string& datadir,
        const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = datadir + mainid + "_" + subid + "_" + caImplShortName + ".EB";

        // Open with a filestream
        std::ifstream file(filename.c_str());

        return file.good();
    }


    template<typename T>
    inline void EdgeBuff<T>::sequentialOp(const BoxList& bl, std::vector<T>& values, CA::Seq::Operator op) const
    {
        // Check that the extent of the boxlist is inside the domain of
        // the grid.
        if (!_grid.box().inside(bl.extent()))
            return;

        // Compute base index
        const _caUnsigned ns_y_border = _grid.caGrid().eb_ns_y_border;
        const _caUnsigned we_x_border = _grid.caGrid().eb_we_x_border;

        const _caUnsigned we_x_offset = _grid.caGrid().eb_we_x_offset;

        const _caUnsigned ns_start = _grid.caGrid().eb_ns_start;
        const _caUnsigned we_start = _grid.caGrid().eb_we_start;

        // Create the list of event ot wait.
        std::vector<cl::Event> wait_events(1, _event);

#ifdef  CA_OCL_USE_EVENTS 
        std::vector<cl::Event> *pwe = &wait_events;
#else
        std::vector<cl::Event> *pwe = NULL;
#endif

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            Box box(*ibox);

            // Noth South!
            _grid.seq2DBuff(values[2], _buff,
                ns_start, _grid.caGrid().eb_ns_stride,
                box.x(), box.w(),
                box.y() + ns_y_border, box.h(),
                op, pwe, 0);
            // West East!
            _grid.seq2DBuff(values[1], _buff,
                we_start, _grid.caGrid().eb_we_stride,
                box.x() + we_x_offset, box.w(),
                box.y(), box.h(),
                op, pwe, 0);
        }

        values[3] = values[1];
        values[4] = values[2];
    }


    template<typename T>
    inline void EdgeBuff<T>::fill(const BoxList& bl, T value)
    {
        // The borders are not written.

        // Check that the extent of the boxlist is inside the domain of
        // the grid.
        if (!_grid.box().inside(bl.extent()))
            return;

        // Create the list of event ot wait.
        std::vector<cl::Event> wait_events(1, _event);

#ifdef  CA_OCL_USE_EVENTS 
        std::vector<cl::Event> *pwe = &wait_events;
#else
        std::vector<cl::Event> *pwe = NULL;
#endif

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            Box box(*ibox);

            // Call the specific method. Do not store the event.
            fillBox(box, value, pwe, 0);
        }

        _grid.queue().flush();

#ifdef  CA_OCL_USE_EVENTS 
        // Set the event of this buffer to wait for all the previous
        // events (copy) to finish.
        _grid.queue().enqueueMarker(&_event);
#endif
    }


    template<typename T>
    inline void EdgeBuff<T>::fillBox(const Box& box, Real value, std::vector<cl::Event>* wait_events, cl::Event* e)
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
        cl::NDRange global(computeStride(box.w(), _grid.warp()), computeStride(box.h(), _grid.warp()));

        // Set the arguments of the kernel.
        _kernel_setValueEdgeBuffReal.setArg(0, _cagrid_short);
        _kernel_setValueEdgeBuffReal.setArg(1, _buff);
        _kernel_setValueEdgeBuffReal.setArg(2, value);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel that set a value into the given region. 
        _grid.queue().enqueueNDRangeKernel(_kernel_setValueEdgeBuffReal, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel that set a value into the given region. 
        _grid.queue().enqueueNDRangeKernel(_kernel_setValueEdgeBuffReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    template<typename T>
    inline void EdgeBuff<T>::fillBox(const Box& box, State value, std::vector<cl::Event>* wait_events, cl::Event* e)
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
        cl::NDRange global(computeStride(box.w(), _grid.warp()), computeStride(box.h(), _grid.warp()));

        // Set the arguments of the kernel.
        _kernel_setValueEdgeBuffState.setArg(0, _cagrid_short);
        _kernel_setValueEdgeBuffState.setArg(1, _buff);
        _kernel_setValueEdgeBuffState.setArg(2, value);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel that set a value into the given region. 
        _grid.queue().enqueueNDRangeKernel(_kernel_setValueEdgeBuffState, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel that set a value into the given region. 
        _grid.queue().enqueueNDRangeKernel(_kernel_setValueEdgeBuffState, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    template<typename T>
    inline void EdgeBuff<T>::dump(std::ostream& out, String x_sep, String y_sep)
    {
        // Create a temporary array to keep the buffer.
        std::vector<T> mem(static_cast<size_t>(_buff_num));

        // Wait for the last command issued that uses this bufer to
        // finish.
#ifdef  CA_OCL_USE_EVENTS 
        _event.wait();
#endif

        // Copy the buffer to memory (blocking) and keep store the event.
#ifdef  CA_OCL_USE_EVENTS 
        _grid.queue().enqueueReadBuffer(_buff, CL_TRUE, 0, static_cast<size_t>(_buff_size), &mem[0], 0, &_event);
#else
        _grid.queue().enqueueReadBuffer(_buff, CL_TRUE, 0, static_cast<size_t>(_buff_size), &mem[0], 0, NULL);
#endif

        // Cycle through the Nort/South sub-buffers region to print. 
        for (Unsigned j_reg = 0; j_reg < _grid.caGrid().eb_ns_y_size; ++j_reg)
        {
            for (Unsigned i_reg = 0; i_reg < _grid.caGrid().eb_ns_x_size; ++i_reg)
            {
                out << mem[static_cast<size_t>(j_reg *_grid.caGrid().eb_ns_stride + i_reg + _grid.caGrid().eb_ns_start)];
                out << x_sep;
            }
            out << y_sep;
        }
        out << std::endl << std::endl;

        // Cycle through the West/East sub-buffers region to print. 
        for (Unsigned j_reg = 0; j_reg < _grid.caGrid().eb_we_y_size; ++j_reg)
        {
            //_caUnsigned start = 0;
            _caUnsigned start = _grid.caGrid().eb_we_x_offset - _grid.caGrid().eb_we_x_border; // Consider the X offset

            for (Unsigned i_reg = start; i_reg < _grid.caGrid().eb_we_x_size; ++i_reg)
            {
                out << mem[static_cast<size_t>(j_reg *_grid.caGrid().eb_we_stride + i_reg + _grid.caGrid().eb_we_start)];
                out << x_sep;
            }
            out << y_sep;
        }

        // Set the event of this buffer to wait for all the previous
        // events (copy) to finish.
#ifdef  CA_OCL_USE_EVENTS 
        _grid.queue().enqueueMarker(&_event);
#endif
    }


    template<typename T>
    inline cl::Event EdgeBuff<T>::event() const
    {
        return _event;
    }


    template<typename T>
    inline void EdgeBuff<T>::setEvent(cl::Event& event)
    {
        _event = event;
    }


    template<typename T>
    inline cl::Buffer EdgeBuff<T>::buffer() const
    {
        return _buff;
    }

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER


#endif  // _CA_EDGEBUFF_HPP_
