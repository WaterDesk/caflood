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
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"Grid.hpp"
#include<limits>
#include<iostream>
#include<cstdlib>
#include<cstring>
#include"caapi2D.hpp"


namespace CA {

    //! Define the buffer which contains a value (real/state) for each
    //! cell in a square regular grid.

    //! This is a simple implementations with  an unique buffer.

    //! \attention The CA_CELLBUFF_???? item used in the CA function can be
    //! different from this class. 

    //! \warning The buffer is bigger than the Grid since it has to
    //! account of border cells. The number of border cells depends on
    //! the number of neighbours' levels.

    //! \attention This file contains also the code for the Moore
    //! neighbourhood using the CA2D_MOORE macro.

    template<typename T>
    class CellBuff : public CA::Uncopyable
    {
    public:

        //! Create the buffer. It is possible to have implementation
        //! specific options set by using the options list. \attention Do
        //! not destroy the grid before destroying this buffer.
        //! The buffer is set to zero.
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
        void clear(const T& value = T());

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
        //! \attention           This function uses a single box instead than a list of boxes.
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
        //! \param mem           The pointer to the memory location where to upload
        //!                      the read data.
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

        //! Read the values of the cells identified in the list of points
        //! and upload the data into the given memory buffer. The number
        //! of elements in the buffer (size) must be at least the number
        //! of points in the list. The order in the buffer is the same one
        //! of the list of points.
        void retrievePoints(const PointList& pl, T* mem, Unsigned mem_size);

        //! Write the values of the cells identified in the list of points
        //! by using the data of the given memory buffer. The number
        //! of elements in the buffer (size) must be at least the number
        //! of points in the list. The order in the buffer is the same one
        //! of the list of points.
        void insertPoints(const PointList& pl, const T* mem, Unsigned mem_size);

        //! Save the values of the cells identified in the list of points
        //! in the DataDir using an unique main id (filename / buffername)
        //! and unique sub id (time step / checkpoint) using as direct and
        //! quick I/O technique as possible.  
        //! \attention How the points are saved is implementation dependant
        //! \return true if successful.
        bool savePoints(const PointList& pl, const std::string& mainid, const std::string& subid);

        //! Execute the given sequential operator on the values of all the cells of
        //! the given region of the grid and return the result of the operation.
        //! \param bl            Identifies the region of the grid from a list of
        //!                      boxes to execute the function.
        //! \param value         The results.
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
        void dump(std::ostream& out, String x_sep = " ", String y_sep = "\n");

        // ---------  Implementation dependent --------------

        // Convert the buffer in the CA_CELLBUFF_??? used in the CA function
        operator const T*()  const { return _buff; }
        operator T*() { return _buff; }

    protected:

        //! Method that perform borders values depending on the operator.
        template<typename Op>
        void bordersValueOp(const Borders& bound, T value, Op op);

        //! Method that perform a sequential operator in each cell values
        //! depending on the operator.
        template<typename Op>
        void sequentialOperationOp(const BoxList& bl, T& value, Op op) const;

    private:

        //! The reference to the grid.
        Grid& _grid;

        //! The local copy of the caGrid from Grid.
        _caGrid _cagrid;

        //! The size of the buffer in bytes.
        _caUnsigned    _buff_size;

        //! The pointer to the data.
        T* _buff;
    };


    /// ----- Inline implementation ----- ///


    template<typename T>
    inline CellBuff<T>::CellBuff(Grid& grid, const Options& options) :
        _grid(grid),
        _cagrid(grid.caGrid()),
        _buff_size(),
        _buff()
    {
        // Allocate the buffer for the cell.  
        _buff_size = _cagrid.cb_x_size * _cagrid.cb_y_size * sizeof(T);
        _buff = static_cast<T*>(calloc(_cagrid.cb_x_size * _cagrid.cb_y_size, sizeof(T)));
    }


    template<typename T>
    inline CellBuff<T>::~CellBuff()
    {
        if (_buff)
        {
            free(_buff);
            _buff = 0;
        }
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
        std::fill(&_buff[0], &_buff[_cagrid.cb_x_size * _cagrid.cb_y_size], value);
    }


    template<typename T>
    inline void CellBuff<T>::copy(const CellBuff<T>& src)
    {
        // Check that the two buffer belong to the same grid. If it not
        // the case. Do nothing.
        if (&_grid != &src._grid)
            return;

        // Check that both buffer are allocated.
        if (!_buff || !src._buff)
            return;

        // Copy the momory.
        memcpy(_buff, src._buff, _buff_size);
    }


    template<typename T>
    inline void CellBuff<T>::retrieveData(const Box& box, T* mem, Unsigned mem_x_size, Unsigned mem_y_size)
    {
        // The borders are not read.

        // Check that the given box is inside the domain of the grid.
        if (!_grid.box().inside(box))
            return;

        // Check that the given memory location is big enaough to contain
        // the data of the region.
        if (box.w() > mem_x_size || box.h() > mem_y_size)
            return;

        // Cycle through the region of cells to read. i_reg and j_reg are
        // the region indices. i_mem and j_mem are the memory indices. The
        // _border value is used to avoid reading the border.
        for (Unsigned j_reg = box.y() + _cagrid.cb_border, j_mem = 0;
            j_reg < box.h() + box.y() + _cagrid.cb_border; ++j_reg, ++j_mem)
        {
            for (Unsigned i_reg = box.x() + _cagrid.cb_border, i_mem = 0;
                i_reg < box.w() + box.x() + _cagrid.cb_border; ++i_reg, ++i_mem)
            {
                mem[j_mem * mem_x_size + i_mem] = _buff[j_reg *_cagrid.cb_x_size + i_reg];
            }
        }
    }


    template<typename T>
    inline void CellBuff<T>::insertData(const Box& box, const T* mem, Unsigned mem_x_size, Unsigned mem_y_size)
    {
        // The borders are not written.

        // Check that the given box is inside the domain of the grid.
        if (!_grid.box().inside(box))
            return;

        // Check that the given memory location is big enough to contain
        // the data of the region.
        if (box.w() < mem_x_size || box.h() < mem_y_size)
            return;

        if (box.w() > mem_x_size || box.h() > mem_y_size)
        {
            // upsample, assuming it is a clean upsampling

            int x_scale = box.w() / mem_x_size;
            int y_scale = box.h() / mem_y_size;

            double x_scale_check = box.w() / mem_x_size;
            double y_scale_check = box.h() / mem_y_size;

            if (x_scale != x_scale_check)
                return;

            if (y_scale != y_scale_check)
                return;

            //std::cout << "x_scale: " << x_scale << std::endl;
            //std::cout << "y_scale: " << y_scale << std::endl;

            // Cycle through the region of cells to read. i_reg and j_reg are
            // the region indices. i_mem and j_mem are the memory indices. The
            // _border value is used to avoid reading the border.
            // i_box and j_box, are the region indicies within the box locally
            for (Unsigned j_reg = box.y() + _cagrid.cb_border, j_mem = 0, j_box = 0;
                j_reg < box.h() + box.y() + _cagrid.cb_border; ++j_reg, ++j_box)
            {
                if (j_box % y_scale == 0 && j_box != 0)
                    ++j_mem;

                for (Unsigned i_reg = box.x() + _cagrid.cb_border, i_mem = 0, i_box = 0;
                    i_reg < box.w() + box.x() + _cagrid.cb_border; ++i_reg, ++i_box)

                {
                    if (i_box % x_scale == 0 && i_box != 0)
                        ++i_mem;

                    _buff[j_reg * _cagrid.cb_x_size + i_reg] = mem[j_mem * mem_x_size + i_mem];
                }
            }
        }
        else {
            // Cycle through the region of cells to read. i_reg and j_reg are
            // the region indices. i_mem and j_mem are the memory indices. The
            // _border value is used to avoid reading the border.
            for (Unsigned j_reg = box.y() + _cagrid.cb_border, j_mem = 0;
                j_reg < box.h() + box.y() + _cagrid.cb_border; ++j_reg, ++j_mem)
            {
                for (Unsigned i_reg = box.x() + _cagrid.cb_border, i_mem = 0;
                    i_reg < box.w() + box.x() + _cagrid.cb_border; ++i_reg, ++i_mem)

                {
                    _buff[j_reg * _cagrid.cb_x_size + i_reg] = mem[j_mem * mem_x_size + i_mem];
                }
            }
        }
    }// end of insertData


    template<typename T>
    inline bool CellBuff<T>::saveData(const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = _grid.dataDir() + mainid + "_" + subid + "_" + caImplShortName + ".CB";

        // Create the file in the data dir.
        std::ofstream file(filename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

        // If the file is not good, problem!!!
        if (!file.good())
            return false;

        // Write the magic value!
        unsigned int magic = CAAPI_2D_MAGIC;
        file.write(reinterpret_cast<char*>(&magic), sizeof(unsigned int));

        // Write the file in a go!
        file.write(reinterpret_cast<char*>(_buff), _buff_size);
        bool ret = file.good();

        return ret;
    }


    template<typename T>
    inline bool CellBuff<T>::loadData(const std::string& mainid, const std::string& subid, bool remove)
    {
        // Create the filename
        std::string filename = _grid.dataDir() + mainid + "_" + subid + "_" + caImplShortName + ".CB";

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
        file.read(reinterpret_cast<char*>(_buff), _buff_size);
        bool ret = file.good() && (_buff_size == file.gcount()) && (file.peek() == EOF);

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
    inline bool CellBuff<T>::removeData(const std::string& datadir,
        const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = datadir + mainid + "_" + subid + "_" + caImplShortName + ".CB";

        // Remove it.
        if (std::remove(filename.c_str()) != 0)
            return false;

        return true;
    }


    template<typename T>
    inline bool CellBuff<T>::existData(const std::string& datadir,
        const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = datadir + mainid + "_" + subid + "_" + caImplShortName + ".CB";

        // Open with a filestream
        std::ifstream file(filename.c_str());

        return file.good();
    }


    template<typename T>
    inline void CellBuff<T>::retrievePoints(const PointList& pl, T* mem, Unsigned mem_size)
    {
        // Check that all the points arte inside the domain of the grid.
        if (!_grid.box().inside(pl.extent()))
            return;

        // Check that the size of the momory location is big enough for the point list.
        if (pl.size() > mem_size)
            return;

        // Cycle through the point list and retrieve the value of the
        // points.
        Unsigned i_mem = 0;
        for (PointList::ConstIter i_p = pl.begin(); i_p != pl.end(); ++i_p)
        {
            Unsigned x = (*i_p).x() + _cagrid.cb_border;
            Unsigned y = (*i_p).y() + _cagrid.cb_border;

            mem[i_mem] = _buff[y*_cagrid.cb_x_size + x];

            i_mem++;
        }
    }


    template<typename T>
    inline void CellBuff<T>::insertPoints(const PointList& pl, const T* mem, Unsigned mem_size)
    {
        // Check that all the points arte inside the domain of the grid.
        if (!_grid.box().inside(pl.extent()))
            return;

        // Check that the size of the momory location is big enough for the point list.
        if (pl.size() > mem_size)
            return;

        // Cycle through the point list and retrieve the value of the
        // points.
        Unsigned i_mem = 0;
        for (PointList::ConstIter i_p = pl.begin(); i_p != pl.end(); ++i_p)
        {
            Unsigned x = (*i_p).x() + _cagrid.cb_border;
            Unsigned y = (*i_p).y() + _cagrid.cb_border;

            _buff[y*_cagrid.cb_x_size + x] = mem[i_mem];

            i_mem++;
        }
    }


    template<typename T>
    inline void CellBuff<T>::sequentialOp(const BoxList& bl, T& value, CA::Seq::Operator op) const
    {
        switch (op)
        {
        case CA::Seq::Add:
            value = 0;
            sequentialOperationOp(bl, value, CA::OP::AddEqual<T>());
            break;

        case CA::Seq::Mul:
            value = 1;
            sequentialOperationOp(bl, value, CA::OP::MulEqual<T>());
            break;

        case CA::Seq::Min:
            value = std::numeric_limits<T>::max();
            sequentialOperationOp(bl, value, CA::OP::MinEqual<T>());
            break;

        case CA::Seq::MinAbs:
            value = std::numeric_limits<T>::max();
            sequentialOperationOp(bl, value, CA::OP::MinAbsEqual<T>());
            break;

        case CA::Seq::Max:
            value = std::numeric_limits<T>::min();
            sequentialOperationOp(bl, value, CA::OP::MaxEqual<T>());
            break;

        case CA::Seq::MaxAbs:
            value = 0;
            sequentialOperationOp(bl, value, CA::OP::MaxAbsEqual<T>());
            break;
        }
    }


    template<typename T>
    template<typename Op>
    inline void CellBuff<T>::sequentialOperationOp(const BoxList& bl, T& result, Op op) const
    {
        // Check that the extent of the boxlist is inside the domain of
        // the grid.
        if (!_grid.box().inside(bl.extent()))
            return;

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            const Box box(*ibox);

#ifdef CA2D_OPENMP

            // The size of the chunk for each thread.
            Unsigned chunksize = box.h() / omp_get_num_procs() + 1;

#pragma omp parallel default(shared)
            {
                T threadvalue = result;

                // Cycle through the region of cells to read.  The _border value
                // is used to avoid writing to the border.
#pragma omp for schedule(static,chunksize)
                for (int j_reg = static_cast<int>(box.y() + _cagrid.cb_border); j_reg < box.h() + box.y() + _cagrid.cb_border; ++j_reg)
                {
                    for (int i_reg = static_cast<int>(box.x() + _cagrid.cb_border); i_reg < box.w() + box.x() + _cagrid.cb_border; ++i_reg)
                    {
                        T value = _buff[j_reg *_cagrid.cb_x_size + i_reg];

                        op(threadvalue, value);
                    }
                }

#pragma omp critical
                {
                    op(result, threadvalue);
                }
            } // end paralle.
#else
            // Cycle through the region of cells to read.  The _border value
            // is used to avoid writing to the border.
            for (Unsigned j_reg = box.y() + _cagrid.cb_border; j_reg < box.h() + box.y() + _cagrid.cb_border; ++j_reg)
            {
                for (Unsigned i_reg = box.x() + _cagrid.cb_border; i_reg < box.w() + box.x() + _cagrid.cb_border; ++i_reg)
                {
                    T value = _buff[j_reg *_cagrid.cb_x_size + i_reg];

                    op(result, value);
                }
            }
#endif
        }
    }

    template<typename T>
    inline void CellBuff<T>::fill(const BoxList& bl, T value)
    {
        // The borders are not written.

        // Check that the extent of the boxlist is inside the domain of
        // the grid.
        if (!_grid.box().inside(bl.extent()))
            return;

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            Box box(*ibox);

            // Cycle through the region of cells to read. The
            // _border value is used to avoid writing the border.
            for (Unsigned j_reg = box.y() + _cagrid.cb_border; j_reg < box.h() + box.y() + _cagrid.cb_border; ++j_reg)
            {
                for (Unsigned i_reg = box.x() + _cagrid.cb_border; i_reg < box.w() + box.x() + _cagrid.cb_border; ++i_reg)
                {
                    _buff[j_reg *_cagrid.cb_x_size + i_reg] = value;
                }
            }
        }
    }


    template<typename T>
    inline void CellBuff<T>::bordersValue(const Borders& bound, T value, CA::Bdr::Operator op)
    {
        switch (op)
        {
        case CA::Bdr::Equal:
            bordersValueOp(bound, value, CA::OP::Equal<T>());
            break;

        case CA::Bdr::Add:
            bordersValueOp(bound, value, CA::OP::AddEqual<T>());
            break;

        case CA::Bdr::Sub:
            bordersValueOp(bound, value, CA::OP::SubEqual<T>());
            break;

        case CA::Bdr::Mul:
            bordersValueOp(bound, value, CA::OP::MulEqual<T>());
            break;

        case CA::Bdr::Div:
            bordersValueOp(bound, value, CA::OP::DivEqual<T>());
            break;
        }
    }


    //////////////////////////////////////////////////////////
    // Attention this is tested only whith a border of one one.
    ///////////////////////////////////////////////////////////
    template<typename T> template <typename Op>
    inline void CellBuff<T>::bordersValueOp(const Borders& bound, T value, Op op)
    {
        // Compute base index
        const Unsigned top = 0;
        const Unsigned bottom = (_cagrid.y_size + _cagrid.cb_border) * _cagrid.cb_x_size;
        const Unsigned left = 0;
        const Unsigned right = _cagrid.x_size + _cagrid.cb_border;

        // Cycle through the segments
        for (int s = 0; s < bound.numSegments(); ++s)
        {
            // Retrive segment
            Borders::Segment seg = bound.segment(s);

            // Execute action depend on segment.
            switch (seg.type)
            {
            case Top:
                // Check the end of the segment
                if (seg.stop > _cagrid.x_size)
                    seg.stop = _cagrid.x_size;

                // Set the value.
                for (Unsigned i = seg.start + _cagrid.cb_border; i < seg.stop + _cagrid.cb_border; ++i)
                    op(_buff[top + i], value);
                break;

            case Bottom:
                // Check the end of the segment
                if (seg.stop > _cagrid.x_size)
                    seg.stop = _cagrid.x_size;

                // Set the value.	
                for (Unsigned i = seg.start + _cagrid.cb_border; i < seg.stop + _cagrid.cb_border; ++i)
                    op(_buff[bottom + i], value);
                break;

            case Left:
                // Check the end of the segment
                if (seg.stop > _cagrid.y_size)
                    seg.stop = _cagrid.y_size;

                // Set the value.	
                for (Unsigned j = seg.start + _cagrid.cb_border; j < seg.stop + _cagrid.cb_border; ++j)
                    op(_buff[j*_cagrid.cb_x_size + left], value);
                break;

            case Right:
                // Check the end of the segment
                if (seg.stop > _cagrid.y_size)
                    seg.stop = _cagrid.y_size;

                // Set the value.	
                for (Unsigned j = seg.start + _cagrid.cb_border; j < seg.stop + _cagrid.cb_border; ++j)
                    op(_buff[j*_cagrid.cb_x_size + right], value);
                break;
            }
        }

        // Cycle through the corners
        for (int c = 0; c < bound.numCorners(); ++c)
        {
            // Retrive segment
            Corner corner = bound.corner(c);

            // Execute action depend on segment.
            switch (corner)
            {
            case TopLeft:
                op(_buff[top + left], value);
                break;

            case TopRight:
                op(_buff[top + right], value);
                break;

            case BottomLeft:
                op(_buff[bottom + left], value);
                break;

            case BottomRight:
                op(_buff[bottom + right], value);
                break;
            }
        }
    }


    template<typename T>
    inline void CellBuff<T>::bordersShift(const Borders& bound)
    {
        // Compute base index
        const Unsigned top = 0;
        const Unsigned bottom = (_cagrid.y_size + _cagrid.cb_border) * _cagrid.cb_x_size;
        const Unsigned left = 0;
        const Unsigned right = _cagrid.x_size + _cagrid.cb_border;

        const Unsigned xone = 1;
        const Unsigned yone = _cagrid.cb_x_size;

        // Cycle through the segments
        for (int s = 0; s < bound.numSegments(); ++s)
        {
            // Retrive segment
            Borders::Segment seg = bound.segment(s);

            // Execute action depend on segment.
            switch (seg.type)
            {
            case Top:
                // Check the end of the segment
                if (seg.stop > _cagrid.x_size)
                    seg.stop = _cagrid.x_size;

                // Set the value.
                for (Unsigned i = seg.start + _cagrid.cb_border; i < seg.stop + _cagrid.cb_border; ++i)
                    _buff[top + i] = _buff[top + yone + i];
                break;

            case Bottom:
                // Check the end of the segment
                if (seg.stop > _cagrid.x_size)
                    seg.stop = _cagrid.x_size;

                // Set the value.	
                for (Unsigned i = seg.start + _cagrid.cb_border; i < seg.stop + _cagrid.cb_border; ++i)
                    _buff[bottom + i] = _buff[bottom - yone + i];
                break;

            case Left:
                // Check the end of the segment
                if (seg.stop > _cagrid.y_size)
                    seg.stop = _cagrid.y_size;

                // Set the value.	
                for (Unsigned j = seg.start + _cagrid.cb_border; j < seg.stop + _cagrid.cb_border; ++j)
                    _buff[j*_cagrid.cb_x_size + left] = _buff[j*_cagrid.cb_x_size + left + xone];
                break;

            case Right:
                // Check the end of the segment
                if (seg.stop > _cagrid.y_size)
                    seg.stop = _cagrid.y_size;

                // Set the value.	
                for (Unsigned j = seg.start + _cagrid.cb_border; j < seg.stop + _cagrid.cb_border; ++j)
                    _buff[j*_cagrid.cb_x_size + right] = _buff[j*_cagrid.cb_x_size + right - xone];
                break;
            }
        }

        // Cycle through the corners
        for (int c = 0; c < bound.numCorners(); ++c)
        {
            // Retrive segment
            Corner corner = bound.corner(c);

            // Execute action depend on segment.
            switch (corner)
            {
            case TopLeft:
                _buff[top + left] = _buff[top + yone + left + xone];
                break;

            case TopRight:
                _buff[top + right] = _buff[top + yone + right - xone];
                break;

            case BottomLeft:
                _buff[bottom + left] = _buff[bottom - yone + left + xone];;
                break;

            case BottomRight:
                _buff[bottom + right] = _buff[bottom - yone + right - xone];;
                break;
            }
        }
    }


    template<typename T>
    inline void CellBuff<T>::dump(std::ostream& out, String x_sep, String y_sep)
    {
        // Cycle through the region of cells to print. 
        for (Unsigned j_reg = 0; j_reg < _cagrid.cb_y_size; ++j_reg)
        {
            for (Unsigned i_reg = 0; i_reg < _cagrid.cb_x_size; ++i_reg)
            {
                out << _buff[j_reg *_cagrid.cb_x_size + i_reg];
                out << x_sep;
            }
            out << y_sep;
        }
    }

}

#endif  // _CA_CELLBUFF_HPP_
