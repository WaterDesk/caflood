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


namespace CA {

    //! Define the buffer which contains a value (real/state) for each
    //! edge of a cell in a square regular grid. Edge betwen two cells
    //! share the same value.

    //! This is a simple implementations with two unique buffers. One
    //! per the North-South shared edges the other for West-East shared
    //! edges.

    //! \attention The CA_EDGEBUFF_??? item used in the CA function can be
    //! different from this class. 

    //! \warning The buffer is bigger than the Grid since it has to
    //! account of border edges. The number of border edges depends on
    //! the number of neighbours' levels.

    //! \attention This file contains also the code for the Moore
    //! neighbourhood using the CA2D_MOORE macro.  In the case of Moore
    //! neighbourhood there are two extra unique buffers. One per the
    //! NW-SE shared edges the other for the NE-SW shared edges.


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
        //! \param bl            Identifies the region of the grid from a list of
        //!                      boxes to execute the function.
        //! \param values        The results.
        //! \param op            The commutative operator to execute in each cell .
        void sequentialOp(const BoxList& bl, std::vector<T>& values, CA::Seq::Operator op) const;

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

        // Convert the buffer in the CA_EDGEBUFF_??? used in the CA function
        operator const T*()  const { return _buff; }
        operator T*() { return _buff; }

    protected:

        //! Method that perform a sequential operator in each edge values
        //! depending on the operator.
        template<typename Op>
        void sequentialOperationOp(const BoxList& bl, std::vector<T>& values, Op op) const;

    private:

        //! The reference to the grid.
        Grid& _grid;

        //! The local copy of the caGrid from Grid.
        _caGrid _cagrid;

        //! The number of elements in the North/South sub-buffer
        _caUnsigned    _ns_buff_num;

        //! The number of elements in the West/East sub-buffer
        _caUnsigned    _we_buff_num;

#ifdef CA2D_MOORE
        //! The number of elements in the NW-SE sub-buffer and NE-SW
        //! sub-buffer. They diagonal buffers have the same number of
        //! elements.
        _caUnsigned    _diag_buff_num;
#endif

        //! The total number (sum of all directiosn buffers) of elements
        //! in the buffer.
        _caUnsigned    _buff_num;

        //! The size of the OnpeCL buffer in bytes.
        _caUnsigned    _buff_size;

        //! The pointer to the data of the north/south and west/east edges.
        //! Added the diagonal buffer in the case of moore neighbourh.
        T *_buff;
    };


    /// ----- Inline implementation ----- ///


    template<typename T>
    inline EdgeBuff<T>::EdgeBuff(Grid& grid, const Options& options) :
        _grid(grid),
        _cagrid(grid.caGrid()),
        _ns_buff_num(),
        _we_buff_num(),
#ifdef CA2D_MOORE
        _diag_buff_num(),
#endif
        _buff_num(),
        _buff_size(),
        _buff()
    {
        _ns_buff_num = _cagrid.eb_ns_x_size * _cagrid.eb_ns_y_size;
        _we_buff_num = _cagrid.eb_we_x_size * _cagrid.eb_we_y_size;

        _buff_num = _ns_buff_num + _we_buff_num;

#ifdef CA2D_MOORE
        // Add the extra size for the two extra sub-buffer.
        _diag_buff_num = _cagrid.eb_diag_x_size * _cagrid.eb_diag_y_size;
        _buff_num += _diag_buff_num * 2;
#endif

        _buff_size = _buff_num * sizeof(T);
        _buff = static_cast<T*>(calloc(_buff_num, sizeof(T)));
    }


    template<typename T>
    inline EdgeBuff<T>::~EdgeBuff()
    {
        if (_buff)
            free(_buff);
        _buff = 0;
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
        std::fill(&_buff[0], &_buff[_buff_num], value);
    }


    template<typename T>
    inline void EdgeBuff<T>::copy(const EdgeBuff<T>& src)
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
    inline bool EdgeBuff<T>::saveData(const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = _grid.dataDir() + mainid + "_" + subid + "_" + caImplShortName + ".EB";

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
        switch (op)
        {
        case CA::Seq::Add:
            for (int i = 0; i < caEdges + 1; ++i)
                values[i] = 0;
            sequentialOperationOp(bl, values, CA::OP::AddEqual<T>());
            break;

        case CA::Seq::Mul:
            for (int i = 0; i < caEdges + 1; ++i)
                values[i] = 1;
            sequentialOperationOp(bl, values, CA::OP::MulEqual<T>());
            break;

        case CA::Seq::Min:
            for (int i = 0; i < caEdges + 1; ++i)
                values[i] = std::numeric_limits<T>::max();
            sequentialOperationOp(bl, values, CA::OP::MinEqual<T>());
            break;

        case CA::Seq::MinAbs:
            for (int i = 0; i < caEdges + 1; ++i)
                values[i] = std::numeric_limits<T>::max();
            sequentialOperationOp(bl, values, CA::OP::MinAbsEqual<T>());
            break;

        case CA::Seq::Max:
            for (int i = 0; i < caEdges + 1; ++i)
                values[i] = std::numeric_limits<T>::min();
            sequentialOperationOp(bl, values, CA::OP::MaxEqual<T>());
            break;

        case CA::Seq::MaxAbs:
            for (int i = 0; i < caEdges + 1; ++i)
                values[i] = 0;
            sequentialOperationOp(bl, values, CA::OP::MaxAbsEqual<T>());
            break;
        }
    }


    template<typename T>
    template<typename Op>
    inline void EdgeBuff<T>::sequentialOperationOp(const BoxList& bl, std::vector<T>& results, Op op) const
    {
        // Check that the extent of the boxlist is inside the domain of
        // the grid.
        if (!_grid.box().inside(bl.extent()))
            return;

#ifdef CA2D_MOORE
        const CA::Unsigned idx_ns_1 = 3;
        const CA::Unsigned idx_ns_2 = 7;
        const CA::Unsigned idx_we_1 = 1;
        const CA::Unsigned idx_we_2 = 5;
#else  // CA2D_VN
        const CA::Unsigned idx_ns_1 = 2;
        const CA::Unsigned idx_ns_2 = 4;
        const CA::Unsigned idx_we_1 = 1;
        const CA::Unsigned idx_we_2 = 3;
#endif    

#ifdef CA2D_OPENMP

        // ATTENTION NOT TESTED!!!

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            const Box box(*ibox);

            // The size of the chunk for each thread.
            Unsigned chunksize = box.h() / omp_get_num_procs() + 1;

#pragma omp parallel default(shared)
            {
                std::vector<T>  threadvalues(results);

#pragma omp for schedule(static,chunksize)
                // Max north/south edges.
                for (int j = box.y() + _cagrid.eb_ns_y_border; j < box.h() + box.y() + _cagrid.eb_ns_y_border + 1; ++j)
                {
                    for (int i = box.x(); i < box.w() + box.x(); ++i) // No X border for N/S buffer
                    {
                        T buffvalue = _buff[j*_cagrid.eb_ns_x_size + i];

                        op(threadvalues[idx_ns_1], buffvalue);
                        op(threadvalues[idx_ns_2], buffvalue);
                    }
                }

#pragma omp for schedule(static,chunksize)
                // Max west/east edges.
                for (int j = box.y(); j < box.h() + box.y(); ++j) // No Y border for W/E buffer
                {
                    for (int i = box.x() + _cagrid.eb_we_x_border; i < box.w() + box.x() + _cagrid.eb_we_x_border + 1; ++i)
                    {
                        T buffvalue = _buff[j*_cagrid.eb_we_x_size + i + _cagrid.eb_we_start];

                        op(threadvalues[idx_we_1], buffvalue);
                        //op(threadvalues[idx_ns_2], buffvalue);
                        op(threadvalues[idx_we_2], buffvalue);
                    }
                }

#ifdef CA2D_MOORE
#pragma omp for schedule(static,chunksize)
                // NWSE and NESW edges
                for (Unsigned j = box.y() + _cagrid.eb_diag_y_border; j < box.h() + box.y() + _cagrid.eb_diag_y_border + 1; ++j)
                {
                    for (Unsigned i = box.x() + _cagrid.eb_diag_x_border; i < box.w() + box.x() + _cagrid.eb_diag_x_border + 1; ++i)
                    {
                        T buffvalue_nwse = _buff[j*_cagrid.eb_diag_x_size + i + _cagrid.eb_nwse_start];
                        T buffvalue_nesw = _buff[j*_cagrid.eb_diag_x_size + i + _cagrid.eb_nesw_start];
                        op(threadvalues[4], buffvalue_nwse);
                        op(threadvalues[8], buffvalue_nwse);
                        op(threadvalues[2], buffvalue_nesw);
                        op(threadvalues[6], buffvalue_nesw);
                    }
                }
#endif // CA2D_MOORE

#pragma omp critical
                for (size_t i = 0; i < results.size(); i++)
                {
                    op(results[i], threadvalues[i]);
                }

            } // opm parallel

        } // box list

#else // NO CA2D_OPENMP

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            Box box(*ibox);

            // Max north/south edges.
            for (Unsigned j = box.y() + _cagrid.eb_ns_y_border; j < box.h() + box.y() + _cagrid.eb_ns_y_border + 1; ++j)
            {
                for (Unsigned i = box.x(); i < box.w() + box.x(); ++i) // No X border for N/S buffer
                {
                    T buffvalue = _buff[j*_cagrid.eb_ns_x_size + i];

                    op(results[idx_ns_1], buffvalue);
                    op(results[idx_ns_2], buffvalue);
                }
            }

            // Max west/east edges.
            for (Unsigned j = box.y(); j < box.h() + box.y(); ++j) // No Y border for W/E buffer
            {
                for (Unsigned i = box.x() + _cagrid.eb_we_x_border; i < box.w() + box.x() + _cagrid.eb_we_x_border + 1; ++i)
                {
                    T buffvalue = _buff[j*_cagrid.eb_we_x_size + i + _cagrid.eb_we_start];

                    op(results[idx_we_1], buffvalue);
                    op(results[idx_we_2], buffvalue);
                }
            }

#ifdef CA2D_MOORE
            // NWSE and NESW edges
            for (Unsigned j = box.y() + _cagrid.eb_diag_y_border; j < box.h() + box.y() + _cagrid.eb_diag_y_border + 1; ++j)
            {
                for (Unsigned i = box.x() + _cagrid.eb_diag_x_border; i < box.w() + box.x() + _cagrid.eb_diag_x_border + 1; ++i)
                {
                    T buffvalue_nwse = _buff[j*_cagrid.eb_diag_x_size + i + _cagrid.eb_nwse_start];
                    T buffvalue_nesw = _buff[j*_cagrid.eb_diag_x_size + i + _cagrid.eb_nesw_start];
                    op(results[4], buffvalue_nwse);
                    op(results[8], buffvalue_nwse);
                    op(results[2], buffvalue_nesw);
                    op(results[6], buffvalue_nesw);
                }
            }
#endif // CA2D_MOORE
        } // box list

#endif // CA2S_OPENMP 
    }


    template<typename T>
    inline void EdgeBuff<T>::fill(const BoxList& bl, T value)
    {
        // Check that the extent of the boxlist is inside the domain of
        // the grid.
        if (!_grid.box().inside(bl.extent()))
            return;

        // The borders are not written.

        // Cycle through the boxes.
        for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
        {
            Box box(*ibox);

            // Min north/south edges.
            for (Unsigned j = box.y() + _cagrid.eb_ns_y_border; j < box.h() + box.y() + _cagrid.eb_ns_y_border + 1; ++j)
            {
                for (Unsigned i = box.x(); i < box.w() + box.x(); ++i) // No X border for N/S buffer
                {
                    _buff[j*_cagrid.eb_ns_x_size + i] = value;
                }
            }

            // Min west/east edges.
            for (Unsigned j = box.y(); j < box.h() + box.y(); ++j) // No Y border for W/E buffer
            {
                for (Unsigned i = box.x() + _cagrid.eb_we_x_border; i < box.w() + box.x() + _cagrid.eb_we_x_border + 1; ++i)
                {
                    _buff[j*_cagrid.eb_we_x_size + i + _cagrid.eb_we_start] = value;
                }
            }

#ifdef CA2D_MOORE
            // NWSE and NESW edges
            for (Unsigned j = box.y() + _cagrid.eb_diag_y_border; j < box.h() + box.y() + _cagrid.eb_diag_y_border + 1; ++j)
            {
                for (Unsigned i = box.x() + _cagrid.eb_diag_x_border; i < box.w() + box.x() + _cagrid.eb_diag_x_border + 1; ++i)
                {
                    _buff[j*_cagrid.eb_diag_x_size + i + _cagrid.eb_nwse_start] = value;
                    _buff[j*_cagrid.eb_diag_x_size + i + _cagrid.eb_nesw_start] = value;
                }
            }
#endif
        }
    }


    template<typename T>
    inline void EdgeBuff<T>::dump(std::ostream& out, String x_sep, String y_sep)
    {
        for (Unsigned j_reg = 0; j_reg < _cagrid.eb_ns_y_size; ++j_reg)
        {
            for (Unsigned i_reg = 0; i_reg < _cagrid.eb_ns_x_size; ++i_reg)
            {
                out << _buff[j_reg *_cagrid.eb_ns_x_size + i_reg];
                out << x_sep;
            }
            out << y_sep;
        }
        out << std::endl << std::endl;
        for (Unsigned j_reg = 0; j_reg < _cagrid.eb_we_y_size; ++j_reg)
        {
            for (Unsigned i_reg = 0; i_reg < _cagrid.eb_we_x_size; ++i_reg)
            {
                out << _buff[j_reg *_cagrid.eb_we_x_size + i_reg + _cagrid.eb_we_start];
                out << x_sep;
            }
            out << y_sep;
        }
#ifdef CA2D_MOORE
        out << std::endl << std::endl;
        for (Unsigned j_reg = 0; j_reg < _cagrid.eb_diag_y_size; ++j_reg)
        {
            for (Unsigned i_reg = 0; i_reg < _cagrid.eb_diag_x_size; ++i_reg)
            {
                out << _buff[j_reg *_cagrid.eb_diag_x_size + i_reg + _cagrid.eb_nwse_start];
                out << x_sep;
            }
            out << y_sep;
        }

        out << std::endl << std::endl;
        for (Unsigned j_reg = 0; j_reg < _cagrid.eb_diag_y_size; ++j_reg)
        {
            for (Unsigned i_reg = 0; i_reg < _cagrid.eb_diag_x_size; ++i_reg)
            {
                out << _buff[j_reg *_cagrid.eb_diag_x_size + i_reg + _cagrid.eb_nesw_start];
                out << x_sep;
            }
            out << y_sep;
        }
#endif
    }

}

#endif  // _CA_EDGEBUFF_HPP_
