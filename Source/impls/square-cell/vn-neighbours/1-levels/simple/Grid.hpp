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

#ifndef _CA_GRID_HPP_
#define _CA_GRID_HPP_


//! \file Grid.hpp
//! Contains the class that identifies the square regular grid of
//! the CA algorithm.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"caapi2D.hpp"


namespace CA {

    //! The class that define the square regular grid where the CA
    //! algorithm is executed. This grid is used to retrive input and
    //! output data.

    //! \attention The CA_GRID item used in the CA function can be
    //! different from this class. The CA_GRID can be of any type and shape
    //! (square/hexagonal/triangular, structured/unstructured).

    //! \warning The point (0,0) in the Grid correspond to the top-left
    //! corner of the grid. While the coordinate (0.0,0.0) in the Grid
    //! correspond to the bottom-left corner of the grid.

    //! \attention This file contains also the code for the Moore
    //! neighbourhood using the CA2D_MOORE macro.

    class Grid
    {
    public:

        Grid();

        //! Create the square regular grid of the CA. It is possible to
        //! have implementation specific options set by using the options
        //! list.
        //! \param x_num   The number of cells in the X dimension.
        //! \param y_num   The number of cells in the Y dimension.
        //! \param length  The length of the cell in the X and Y coordinate dimension (square).
        //! \param x_coo   The x coordinate of the bottom-left corner.
        //! \param y_coo   The y coordinate of the bottom-left corner.
        //! \param options The list of implementation specific options.
        Grid(Unsigned x_num, Unsigned y_num, Real length, Real x_coo, Real y_coo,
            const Options& options = Options());

        //! Create the square regular grid of the CA from a previusly
        //! saved one in the DataDir using the given unique
        //! main id and unique sub id.
        //! It is possible to have implementation specific options set by
        //! using the options list.
        //! \param datadir The directory where the data will be loaded and saved.
        //! \param mainid  The main id of the Grid to load.
        //! \param subid   The main id of the Grid to load.
        //! \param options The list of implementation specific options.
        Grid(const std::string& datadir, const std::string& mainid, const std::string& subid,
            const Options& options = Options());

        //! Destroy the grid.
        ~Grid();

        //! Return the specific options about the Grid object and this
        //! implementation.
        static Options options();

        //! Return the number of cells in the X dimension.    
        Unsigned xNum() const;

        //! Return the number of cells in the Y dimension.
        Unsigned yNum() const;

        //! Return the lenght of the edges of the cells.
        Real length() const;

        //! Returns the area of a cell.
        Real area() const;

        //! Return the x coordinate of the bottom-left corner.
        Real xCoo() const;

        //! Return the y coordinate of the bottom-left corner.
        Real yCoo() const;

        //! Return the total number of cells in the grid.
        Unsigned num() const;

        //! Return the rectangular box of the grid with the given
        //! coordinates. \attention X and Y starting position are going to
        //! be (0,0).
        Box box() const;

        //! Print information about the local implementation.
        void printInfo(std::ostream& out);

        //! If set to true, the print methods in the ca function will
        //! work. If set to false there will be no printing.
        void setCAPrint(bool print);

        //! Set the data directory where the various buffer(s) will be
        //! saved/load when the direct I/O methods are used. \attention
        //! Default is the local directory.
        void setDataDir(const std::string& datadir);

        //! Return the data directory.
        std::string dataDir() const;

        //! Save the information of the Grid in the DataDir using an
        //! unique man id (filename/ grid type) and unique sub id (size of
        //! the grid / version of the grid).
        //! \attention How the grid is saved is implementation dependant.
        //! \warning The data of the buffer is not saved whit this method.
        //! \return true if successful.
        bool save(const std::string& mainid, const std::string& subid);

        //! Remove the information of the Grid from the DataDir of the given unique
        //! main id (filename / buffername) and of the unique sub id (time
        //! step / checkpoint) using as direct and quick I/O technique as
        //! possible.
        static bool remove(const std::string& datadir, const std::string& mainid, const std::string& subid);

        //! Check if the information of the Grid of the given unique main
        //! id and of the unique sub id exist in the DataDir.
        //! \return true if data exist.
        static bool exist(const std::string& datadir, const std::string& mainid, const std::string& subid);

        // ---------  Implementation dependent --------------

        //! Return the structure shared with the CA function which
        //! contaions all the values needed to access and manage GRID data
        //! and the vairous buffer.
        const _caGrid caGrid() const;

        //! Convert the square regolar grid in the CA_GRID
        operator const _caGrid&() const;
        operator _caGrid&();

    protected:

    private:

        //! The CA_GRID used in the CA function.
        _caGrid _cagrid;

        //! String with the direcotry where the data is saved/loaded when
        //! using direct I/O.
        std::string _datadir;
    };


    /// ----- Inline implementation ----- ///

    inline Grid::Grid()
    {
    }

    inline Grid::Grid(Unsigned x_num, Unsigned y_num, Real length, Real x_coo, Real y_coo,
        const Options& options) :
        _cagrid(),
#if defined _WIN32 || defined __CYGWIN__   
        _datadir(".\\")
#else
        _datadir("./")
#endif
    {
        // This object initialise also the values needed by the
        // cell/edge/vertex buffers. These values are used to create these
        // buffers. 

        // Set the dimensions of the Grid.
        _cagrid.x_size = x_num;
        _cagrid.y_size = y_num;

        // Set the cell lenght and area.
        _cagrid.length = length;
        _cagrid.area = length * length;

        // Set the distance between cells.
        _cagrid.distance = length;
#ifdef CA2D_MOORE
        _cagrid.distance_diag = std::sqrt(2 * _cagrid.distance*_cagrid.distance);
#endif

        // Set the coordinates.
        _cagrid.x_coo = x_coo;
        _cagrid.y_coo = y_coo;
        _cagrid.y_coo_top = _cagrid.y_coo + _cagrid.y_size*_cagrid.length;

        // The box inside _caGrid is going to be set each time a kernel is
        // called since it indicates the are to compute.
        _cagrid.bx_lx = 0;
        _cagrid.bx_ty = 0;
        _cagrid.bx_rx = 0;
        _cagrid.bx_by = 0;

        // Set the dimensions of an CellBuffer. The buffer has caLevels_*2
        // more cells in the X and Y dimensions to store the borders cells
        // in each side of the grid.
        _cagrid.cb_border = caLevels;

        _cagrid.cb_x_size = _cagrid.x_size + _cagrid.cb_border * 2;
        _cagrid.cb_y_size = _cagrid.y_size + _cagrid.cb_border * 2;

        // Set the dimensions of an EdgeBuffer.  The north/south buffer
        // has caLevels*2 border in the Y direction and no border in the X
        // direction. The west/east buffer has caLevels*2 border in the X
        // direction and no border in the Y direction.

        // Furthermore, the north/south buffer need one more space in the
        // Y direction and none in the X, while the west/east buffer need
        // one more space in the X direction and none in the Y.

        _cagrid.eb_ns_y_border = caLevels;
        _cagrid.eb_we_x_border = caLevels;

        _cagrid.eb_ns_x_size = _cagrid.x_size;
        _cagrid.eb_ns_y_size = _cagrid.y_size + _cagrid.eb_ns_y_border * 2 + 1; // Notice the +1.

        _cagrid.eb_we_x_size = _cagrid.x_size + _cagrid.eb_we_x_border * 2 + 1;
        _cagrid.eb_we_y_size = _cagrid.y_size;

        // Set the starting point for the two sub-buffers in the main buffer.
        _cagrid.eb_ns_start = 0; // The north/South is first.
        _cagrid.eb_we_start = _cagrid.eb_ns_x_size * _cagrid.eb_ns_y_size;

#ifdef CA2D_MOORE
        // Eventual Moore neighbourhood
        // The nort-west/south-east buffer has caLevels*2 border in the X and Y direction.
        // The nort-east/south-west buffer has caLevels*2 border in the X and Y direction.

        // Furthermore, these buffers need one more extra space in the X
        // and Y direction.

        // Given the fact that these two diagonal buffers have the same
        // dimensions, we use generic _diag_ variables.

        _cagrid.eb_diag_y_border = caLevels;
        _cagrid.eb_diag_x_border = caLevels;

        _cagrid.eb_diag_x_size = _cagrid.x_size + _cagrid.eb_diag_x_border * 2 + 1; // Notice the +1.
        _cagrid.eb_diag_y_size = _cagrid.y_size + _cagrid.eb_diag_y_border * 2 + 1; // Notice the +1.

        // Set the starting point for the two extra sub-buffers in the main buffer.
        _cagrid.eb_nwse_start = _cagrid.eb_we_start + (_cagrid.eb_we_x_size * _cagrid.eb_we_y_size);
        _cagrid.eb_nesw_start = _cagrid.eb_nwse_start + (_cagrid.eb_diag_x_size * _cagrid.eb_diag_y_size);

#endif

        _cagrid.print = false;
    }


    inline Grid::Grid(const std::string& datadir, const std::string& mainid, const std::string& subid,
        const Options& options) :
        _cagrid(),
        _datadir(datadir)
    {
        // Create the filename
        std::string filename = _datadir + mainid + "_" + subid + "_" + caImplShortName + ".GD";

        // Create the file in the data dir.
        std::ifstream file(filename.c_str(), std::ofstream::in | std::ofstream::binary);

        // If the file is not good, problem!!!
        if (!file.good())
            throw std::runtime_error(std::string("Error opening the Grid file: ") + filename);

        // Check the magic value!
        unsigned int magic = 0;
        file.read(reinterpret_cast<char*>(&magic), sizeof(unsigned int));

        if (magic != CAAPI_2D_MAGIC)
            throw std::runtime_error(std::string("Wrong type of Grid file: ") + filename);

        // Read the file in a go!
        file.read(reinterpret_cast<char*>(&_cagrid), sizeof(_caGrid));
        bool ret = file.good() && (sizeof(_caGrid) == file.gcount()) && (file.peek() == EOF);

        if (!ret)
            throw std::runtime_error(std::string("Error loading data from Grid file: ") + filename);

        // Close the file.
        file.close();
    }


    inline Grid::~Grid()
    {
    }


    inline Options Grid::options()
    {
        Options options;

        // Add the optional arguments. The tag start from higher number
        // for safety reason.
        CA::Unsigned na = 2000;

        // Example
        //options.push_back(new Arguments::Arg(na++,"option-name", "Option desctiption","value",true, true, false));

        return options;
    }


    inline Unsigned Grid::xNum() const
    {
        return static_cast<Unsigned>(_cagrid.x_size);
    }


    inline Unsigned Grid::yNum() const
    {
        return static_cast<Unsigned>(_cagrid.y_size);
    }


    inline Real Grid::length() const
    {
        return _cagrid.length;
    }

    inline Real Grid::area() const
    {
        return _cagrid.area;
    }


    inline Real Grid::xCoo() const
    {
        return _cagrid.x_coo;
    }


    inline Real Grid::yCoo() const
    {
        return _cagrid.y_coo;
    }


    inline Unsigned Grid::num() const
    {
        return xNum()*yNum();
    }


    inline Box Grid::box() const
    {
        Box box(0, 0, xNum(), yNum());
        box.setCoo((*this));
        return box;
    }


    inline void Grid::printInfo(std::ostream& out)
    {
        out << "CA API Version     : " << caVersion << std::endl;
        out << "       Impl Name   : " << caImplName << std::endl;
        out << "       Impl Version: " << caImplVersion << std::endl;
        out << "Grid               : " << std::endl;
        out << "       xNum        : " << xNum() << std::endl;
        out << "       yNum        : " << yNum() << std::endl;
        out << "       length      : " << length() << std::endl;
        out << "       xCoo        : " << xCoo() << std::endl;
        out << "       yCoo        : " << yCoo() << std::endl;
    }

    inline const _caGrid Grid::caGrid() const
    {
        return _cagrid;
    }


    inline Grid::operator const _caGrid&() const
    {
        return _cagrid;
    }


    inline Grid::operator _caGrid&()
    {
        return _cagrid;
    }


    inline void Grid::setCAPrint(bool print)
    {
        _cagrid.print = print;
    }


    inline void Grid::setDataDir(const std::string& datadir)
    {
        _datadir = datadir;
    }


    inline std::string Grid::dataDir() const
    {
        return _datadir;
    }


    inline bool Grid::save(const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = _datadir + mainid + "_" + subid + "_" + caImplShortName + ".GD";

        // Create the file in the data dir.
        std::ofstream file(filename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

        // If the file is not good, problem!!!
        if (!file.good())
        {
            file.close();
            return false;
        }

        // Write the magic value!
        unsigned int magic = CAAPI_2D_MAGIC;
        file.write(reinterpret_cast<char*>(&magic), sizeof(unsigned int));

        // Write the data in a go!
        file.write(reinterpret_cast<char*>(&_cagrid), sizeof(_caGrid));
        bool ret = file.good();

        // Close the file.
        file.close();

        return ret;
    }

    inline bool Grid::remove(const std::string& datadir, const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = datadir + mainid + "_" + subid + "_" + caImplShortName + ".GD";

        // Remove it.
        if (std::remove(filename.c_str()) != 0)
            return false;

        return true;
    }


    inline bool Grid::exist(const std::string& datadir, const std::string& mainid, const std::string& subid)
    {
        // Create the filename
        std::string filename = datadir + mainid + "_" + subid + "_" + caImplShortName + ".GD";

        // Open with a filestream
        std::ifstream file(filename.c_str());

        return file.good();
    }

}

#endif  // _CA_GRID_HPP_
