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
//! This is the OpenCL implementation
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-12


#include"caapi2D.hpp"
#include<iostream>
#include<vector>
#include<map>
#include<stdexcept>
#include<cstring>
#include<cmath>


// Get rid of the annoying visual studio warning 4244 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif // _MSC_VER


#define CA_GRID_READ_TOKEN(_test,_var,_tok,_sect)       \
   {                                                    \
   if(!CA::fromString(_var,_tok))                       \
   {                                                    \
     std::cerr<<"Error reading '"<<CA::trimToken(_sect)<<"' element"<<std::endl; \
     return 1;                                          \
   }                                                    \
   _test = true;                                        \
   }


namespace CA {

    //! The class that define the square regular grid where the CA
    //! algorithm is executed. This grid is used to retrive input and
    //! output data.

    //! This is the OpenCL implementation. At the moment this object
    //! create only one context from one type of device (CPU or
    //! GPU). Furthermore it has only one queue and thus it uses only
    //! one device of the specific type. In the future it could have
    //! multiple different contexts in order to use hybrid cpu/gpu
    //! computation and it could have multiple queues thus use multiple
    //! devices of the same type.

    //! This OpenCL implementation try to use cl::Events in order to
    //! syncronize between cell/edege/vertex buffer management and CA
    //! functions execution.

    //! \attention The CA_GRID item used in the CA function can be
    //! different from this class. The CA_GRID can be of any type and shape
    //! (square/hexagonal/triangular, structured/unstructured).

    //! \warning The position (0,0) in the Grid correspond to the
    //! top-left corner of the grid. While the coordinate (0.0,0.0) in
    //! the Grid correspond to the bottom-left corner of the grid.

    class Grid
    {
    public:

        Grid();

        //! Create the square regular grid of the CA. It is possible to
        //! have implementation specific options set by using the options
        //! list.
        //! \param x_num   The number of cells in the X dimension.
        //! \param y_num   The number of cells in the Y dimension.
        //! \param length  The length of the cell in the X and Y dimension (square).
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
        //! coordinates. \attention X and Y
        //! starting position are going to be (0,0).
        Box box() const;

        //! Print information about the local implementation.
        void printInfo(std::ostream& out);
        void printInfo(FILE* rptFile);

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

        //! This is the optimise version of _caGrid for the kernel computation.
        const _caGrid_short caGridShort() const;

        //! Return the OpenCL context;
        const cl::Context& context() const;

        //! Return the OpenCL queue;
        const std::vector<cl::Device>& devices() const;

        //! Return the OpenCL queue;
        const cl::CommandQueue& queue() const;

        //! Return the program with the internal helper/utility kernels.
        const cl::Program& kernelsProgram() const;

        //! Return the program given the CA function.
        template<typename Func>
        const cl::Program& getProgram(Func& f);

        //! Return the warp/wafront value.
        Unsigned warp() const;

        //! Internal method that set the given real one dimentional buffer
        //! to the given real value.
        void fill1DBuff(Real value, cl::Buffer& tmp_buff, _caUnsigned start, _caUnsigned stop,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that set the given state one dimentional
        //! buffer to the given state value.
        void fill1DBuff(State value, cl::Buffer& tmp_buff, _caUnsigned start, _caUnsigned stop,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that performe the given operator into the
        //! values of the given region of two dimensional buffer.
        void op2DBuff(Real value, cl::Buffer& tmp_buff,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            CA::Bdr::Operator op,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that performe the given operator into the
        //! values of the given region of two dimensional buffer.
        void op2DBuff(State value, cl::Buffer& tmp_buff,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            CA::Bdr::Operator op,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that perform the given commutative operator
        //! sequentially into the values of the given region of two
        //! dimensional buffer.
        void seq2DBuff(Real& value, const cl::Buffer& tmp_buff,
            _caUnsigned x_offset, _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            CA::Seq::Operator op,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that perform the given commutative operator
        //! sequentially into the values of the given region of two
        //! dimensional buffer.
        void seq2DBuff(State& value, const cl::Buffer& tmp_buff,
            _caUnsigned x_offset, _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            CA::Seq::Operator op,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that copy a given horizontal line into the
        //! horizontal lines of a given region of two dimensional buffer.
        void copy2DBuffHLine(Real value, cl::Buffer& tmp_buff, _caUnsigned l_start,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that copy a given vertical line into the
        //! vertical lines of a given region of two dimensional buffer.
        void copy2DBuffVLine(Real value, cl::Buffer& tmp_buff, _caUnsigned l_start,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that copy a given horizontal line into the
        //! horizontal lines of a given region of two dimensional buffer.
        void copy2DBuffHLine(State value, cl::Buffer& tmp_buff, _caUnsigned l_start,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that copy a given vertical line into the
        //! vertical lines of a given region of two dimensional buffer.
        void copy2DBuffVLine(State value, cl::Buffer& tmp_buff, _caUnsigned l_start,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that copy a given point into the given region
        //! of two dimensional buffer.
        void copy2DBuffPoint(Real value, cl::Buffer& tmp_buff, _caUnsigned p_x, _caUnsigned p_y,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            std::vector<cl::Event>* wait_events, cl::Event* e);

        //! Internal method that copy a given point into the given region
        //! of two dimensional buffer.
        void copy2DBuffPoint(State value, cl::Buffer& tmp_buff, _caUnsigned p_x, _caUnsigned p_y,
            _caUnsigned x_stride,
            _caUnsigned x_start, _caUnsigned x_num,
            _caUnsigned y_start, _caUnsigned y_num,
            std::vector<cl::Event>* wait_events, cl::Event* e);

    protected:

        //! Mnage the options passed when the object was created.
        void manageOptions(const Options& options);

        //! Read the extra configuration parameters from a configuration
        //! file.
        int readConfigCSV(const std::string filename);

        //! Initialise the OpenCL objects and build the internal kernels
        //! used internally by Cell/Edge/Vertex buffers.
        void initOpenCL();

    protected:

        //! Structure shared with the CA functions which contains all the
        //! values needed to access GRID data and the various BUFFER
        //! (Cell/Edge/Vertex) data.
        _caGrid _cagrid;

        //! This is the short version of _caGrid which is used to save
        //! registers and thus is specialised to be used inside the
        //! kernels.
        _caGrid_short _cagrid_short;


        //! String with the direcotry where the data is saved/loaded when
        //! using direct I/O.
        std::string _datadir;

        //! This variable return true if the openCL system was initialised.
        bool initcl;

        //! The short name of the OpenCL platform to use. If empty, the first
        //! platform with the righ device type.
        std::string _platform_name;

        //! The type of OpenCL devices. At the moment is chosen at compile
        //! time. It should be possible in the future to choose it at
        //! run-time using Options.
        // DEVICE SELECTION HERE!!!!!!!!!!!!!!!!!!!!
        cl_device_type _device_type;

        //! The number of device to use in OpenCL when there are more than
        //! one device to use.
        int _devices_num;

        //! If true a device fission was perfored.
        bool _device_fission;

        //! The number of compute unit in a fission. If it is zero the
        //! fission is not performed.
        cl_device_partition_property_ext  _device_cu;

        //! Size of a wavefront/warp. This depend on the type of device
        //! used. It is used to define the stride and its alignment.  It
        //! should be a multiple of 2 and more than 16 if // the device is
        //! GPU otherwise it should be 1.
        _caUnsigned _warp;

        //! The OpenCl platforms.
        std::vector<cl::Platform> _platforms;
        _caUnsigned  _platforms_num;

        //! The OpenCL context.
        cl::Context _context;

        //! The OpenCL devices of a specific type available.  \attention
        //! This list should contain only one device. The one identified
        //! by _devices_num.
        std::vector<cl::Device> _devices;

        //! The queue of the single (and only) device where the various
        //! OpenCL executions are made.
        cl::CommandQueue _queue;

        //! The program that contains the utility/helper kernels which are
        //! used internally by the Cell/Edge/Vertex buffers.
        cl::Program _kernels_program;

        //! The building options.
        std::string _building_options;

        //! The options to use when create the queue.
        cl_command_queue_properties _queue_properties;

        //! Show build info.
        bool _build_info;

        //! Internal kernels.
        cl::Kernel _kernel_setValueReal;
        cl::Kernel _kernel_setValueState;
        cl::Kernel _kernel_opValueReal;
        cl::Kernel _kernel_opValueState;
        cl::Kernel _kernel_reduceReal;
        cl::Kernel _kernel_reduceState;
        cl::Kernel _kernel_copyHLineReal;
        cl::Kernel _kernel_copyVLineReal;
        cl::Kernel _kernel_copyHLineState;
        cl::Kernel _kernel_copyVLineState;
        cl::Kernel _kernel_copyPointReal;
        cl::Kernel _kernel_copyPointState;

        //! The name of the config file read with extra configuration parameters.
        std::string _config_filename;

        //! Maps the NDRanges program of a CA function to the relative CA
        //! function name.
        std::map<std::string, cl::NDRange> _hash_ranges;

        std::map<std::string, cl::Program>	_programsMap;
    };


    /// ----- Inline implementation ----- ///

    inline Grid::Grid():
        _cagrid(),
        _cagrid_short(),
        // former static variables.
        initcl(false),
        _warp(32),
        _context(),
        _platforms(),
        _platforms_num(),
        _devices(),
        _platform_name(),
        _device_type(CL_DEVICE_TYPE_GPU),
        _devices_num(0),
        _device_fission(false),
        _device_cu(0),
        _queue(),
        _kernels_program(),
        _building_options(),
        _queue_properties(0),
        _build_info(false),
        _kernel_setValueReal(),
        _kernel_setValueState(),
        _kernel_opValueReal(),
        _kernel_opValueState(),
        _kernel_reduceReal(),
        _kernel_reduceState(),
        _kernel_copyHLineReal(),
        _kernel_copyVLineReal(),
        _kernel_copyHLineState(),
        _kernel_copyVLineState(),
        _kernel_copyPointReal(),
        _kernel_copyPointState(),
        _config_filename("config_NVIDIA_GPU.csv"),
        _hash_ranges(),
        _programsMap(),
#if defined _WIN32 || defined __CYGWIN__
        _datadir(".\\")
#else
        _datadir("./")
#endif
    {
    }


    inline Grid::Grid(Unsigned x_num, Unsigned y_num, Real length, Real x_coo, Real y_coo,
        const Options& options) :
        _cagrid(),
        _cagrid_short(),
        // former static variables.
        initcl(false),
        _warp(32),
        _context(),
        _platforms(),
        _platforms_num(),
        _devices(),
        _platform_name(),
        _device_type(CL_DEVICE_TYPE_GPU),
        _devices_num(0),
        _device_fission(false),
        _device_cu(0),
        _queue(),
        _kernels_program(),
        _building_options(),
        _queue_properties(0),
        _build_info(false),
        _kernel_setValueReal(),
        _kernel_setValueState(),
        _kernel_opValueReal(),
        _kernel_opValueState(),
        _kernel_reduceReal(),
        _kernel_reduceState(),
        _kernel_copyHLineReal(),
        _kernel_copyVLineReal(),
        _kernel_copyHLineState(),
        _kernel_copyVLineState(),
        _kernel_copyPointReal(),
        _kernel_copyPointState(),
        _config_filename("config_NVIDIA_GPU.csv"),
        _hash_ranges(),
        _programsMap(),
#if defined _WIN32 || defined __CYGWIN__
        _datadir(".\\")
#else
        _datadir("./")
#endif
    {
        // Manage the options
        manageOptions(options);

        // Read the configuration file with extra configuration parameters.
        readConfigCSV(_config_filename);
        //if(!_config_filename.empty() && readConfigCSV(_config_filename)!=0)
        //{
        //  throw std::runtime_error(std::string("Error opening the Config file: ")+_config_filename);
        //}

        //! And initialise the OpenCl
        initOpenCL();

        // This object initialise also the values needed by the
        // cell/edge/vertex buffers. These values are used to create these
        // buffers. 

        // Set the dimensions of the Grid.
        _cagrid.x_size = x_num;
        _cagrid.y_size = y_num;

        // Set the cell lenght and area.
        _cagrid.length = length;
        _cagrid.area = length * length;

        // Set the coordinates.
        _cagrid.x_coo = x_coo;
        _cagrid.y_coo = y_coo;

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

        // However in the left side of the X dimension instead to have
        // caLevel more cells the buffer has cb_x_offset more cells
        // (warp). This is to have the get_global_id(0) alligned when
        // accessing element 0.

        //!!!!  TEST TO SEE WHICH ONE GIVE BETTER PERFORMANCE !!!

        //_cagrid.cb_x_offset = _cagrid.cb_border;
        _cagrid.cb_x_offset = std::max(_warp, _cagrid.cb_border);

        _cagrid.cb_x_size = _cagrid.x_size + _cagrid.cb_border + _cagrid.cb_x_offset;
        _cagrid.cb_y_size = _cagrid.y_size + _cagrid.cb_border * 2;

        // Set the stride used by a cell buffer in the x dimension.
        _cagrid.cb_stride = computeStride(_cagrid.cb_x_size, _warp);

        // Set the dimensions of an EdgeBuffer.  The north/south buffer
        // has caLevels*2 border in the Y direction and no border in the X
        // direction. The west/east buffer has caLevels*2 border in the X
        // direction and no border in the Y direction.

        // Furthermore, the north/south buffer need one more space in the
        // Y direction and none in the X, while the west/east buffer need
        // one more space in the X direction and none in the Y.

        _cagrid.eb_ns_y_border = caLevels;
        _cagrid.eb_we_x_border = caLevels;

        // However in side of the X dimension of the west/east buffer
        // instead to have caLevel more edges the buffer has
        // eb_we_x_offset more edges (warp). This is to have the
        // get_global_id(0) alligned when accessing element 0.

        //!!!!  TEST TO SEE WHICH ONE GIVE BETTER PERFORMANCE !!!

        //_cagrid.eb_we_x_offset = _cagrid.eb_we_x_border;
        _cagrid.eb_we_x_offset = std::max(_warp, _cagrid.eb_we_x_border);

        _cagrid.eb_ns_x_size = _cagrid.x_size;
        _cagrid.eb_ns_y_size = _cagrid.y_size + _cagrid.eb_ns_y_border * 2 + 1; // Notice the +1.

        _cagrid.eb_we_x_size = _cagrid.x_size + _cagrid.eb_we_x_border + _cagrid.eb_we_x_offset + 1;
        _cagrid.eb_we_y_size = _cagrid.y_size;

        // Set the stride used by the sub-buffers of a edge buffer in the
        // x dimension.
        _cagrid.eb_ns_stride = computeStride(_cagrid.eb_ns_x_size, _warp);
        _cagrid.eb_we_stride = computeStride(_cagrid.eb_we_x_size, _warp);

        // Set the starting point for the two sub-buffers in the main buffer.
        // ATTENTION if you change this value ... the caapi.cl code must be updated since
        // eb_ns_start is not used.
        _cagrid.eb_ns_start = 0; // The north/South is first.
        _cagrid.eb_we_start = _cagrid.eb_ns_stride * _cagrid.eb_ns_y_size;

        // Populate the short version.
        initGridShort(_cagrid, _cagrid_short);
    }


    inline Grid::Grid(const std::string& datadir, const std::string& mainid, const std::string& subid,
        const Options& options) :
        _cagrid(),
        _cagrid_short(),
        // former static variables.
        initcl(false),
        _warp(32),
        _context(),
        _platforms(),
        _platforms_num(),
        _devices(),
        _platform_name(),
        _device_type(CL_DEVICE_TYPE_GPU),
        _devices_num(0),
        _device_fission(false),
        _device_cu(0),
        _queue(),
        _kernels_program(),
        _building_options(),
        _queue_properties(0),
        _build_info(false),
        _kernel_setValueReal(),
        _kernel_setValueState(),
        _kernel_opValueReal(),
        _kernel_opValueState(),
        _kernel_reduceReal(),
        _kernel_reduceState(),
        _kernel_copyHLineReal(),
        _kernel_copyVLineReal(),
        _kernel_copyHLineState(),
        _kernel_copyVLineState(),
        _kernel_copyPointReal(),
        _kernel_copyPointState(),
        _config_filename("config_NVIDIA_GPU.csv"),
        _hash_ranges(),
        _programsMap(),
        _datadir(datadir)
    {
        // Manage the options
        manageOptions(options);

        // Read the configuration file with extra configuration parameters.
        readConfigCSV(_config_filename);

        //! And initialise the OpenCl
        initOpenCL();

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

        unsigned int warp = 32;
        file.read(reinterpret_cast<char*>(&warp), sizeof(unsigned int));
        _warp = warp;

        // Read the file in a go!
        file.read(reinterpret_cast<char*>(&_cagrid), sizeof(_caGrid));
        bool ret = file.good() && (sizeof(_caGrid) == file.gcount()) && (file.peek() == EOF);

        if (!ret)
            throw std::runtime_error(std::string("Error loading data from Grid file: ") + filename);

        // Close the file.
        file.close();

        // Populate the short version.
        initGridShort(_cagrid, _cagrid_short);
    }


    inline Grid::~Grid()
    {
        if (_queue())
            _queue.finish();
    }


    inline Options Grid::options()
    {
        Options options;

        // Add the optional arguments. The tag start from higher number
        // for safety reason.
        //CA::Unsigned na = 2000;

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

        // Loop through the platforms and print the information.    
        out << "OpenCL Platform    : " << std::endl;
        out << "       Requested   : " << _platform_name << std::endl;
        out << "       Name        : " << _platforms[_platforms_num].getInfo<CL_PLATFORM_NAME>() << "" << std::endl;
        out << "       Vendor      : " << _platforms[_platforms_num].getInfo<CL_PLATFORM_VENDOR>() << std::endl;
        out << "       Version     : " << _platforms[_platforms_num].getInfo<CL_PLATFORM_VERSION>() << std::endl;
        out << "       Warp Size   : " << _warp << std::endl;
        out << "OpenCL Device      : " << std::endl;
        out << "       Name        : " << _devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
        out << "       Device Num  : " << _devices_num << std::endl;
        out << "       ComputeUnits: " << _devices[0].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS >() << std::endl;
        out << "       Fission     : " << _device_fission << std::endl;
        out << "       Device Type : ";
        switch (_device_type)
        {
        case CL_DEVICE_TYPE_GPU: out << "GPU" << std::endl;  break;
        case CL_DEVICE_TYPE_CPU: out << "CPU" << std::endl;  break;
        default: out << "UNKNOWN" << std::endl;
        }
        out << "       Mem         : " << _devices[0].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
        out << "       Max Mem Buff: " << _devices[0].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;

        // Show extra configuration options.
        if (!_config_filename.empty())
        {
            out << "Configuration file : " << _config_filename << std::endl;

            typedef std::map<std::string, cl::NDRange>::iterator it_type;
            for (it_type iterator = _hash_ranges.begin(); iterator != _hash_ranges.end(); ++iterator)
            {
                const size_t* range = iterator->second;
                out << "CA Function Range  : " << iterator->first << " (" << range[0] << "," << range[1] << ")" << std::endl;
            }
        }
        else
        {
            out << "Configuration file : NO CONFIGURATION FILE" << std::endl;
        }

        /*
        // Compute the best value for a warp.
        // Loop through the devices and take the maximum warp suggested.
        for(size_t i=0; i<_devices.size(); ++i)
        {
          _caUnsigned n = _kernel_setValueReal.getWorkGroupInfo<CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE>
        (_devices[i]);
          out<<"       WARP HINT   : "<<n<<std::endl;
        }
        */

        out << std::endl;
    }


    inline void Grid::printInfo(FILE* rptFile)
    {
        if (!rptFile)
            return;

        fprintf(rptFile, "CA API Version     : %d\n", caVersion);
        fprintf(rptFile, "       Impl Name   : %s\n", caImplName);
        fprintf(rptFile, "       Impl Version: %d\n", caImplVersion);
        fprintf(rptFile, "Grid               : \n");
        fprintf(rptFile, "       xNum        : %d\n", xNum());
        fprintf(rptFile, "       yNum        : %d\n", yNum());
        fprintf(rptFile, "       length      : %f\n", length());
        fprintf(rptFile, "       xCoo        : %f\n", xCoo());
        fprintf(rptFile, "       yCoo        : %f\n", yCoo());

        // Loop through the platforms and print the information.
        fprintf(rptFile, "OpenCL Platform    : \n");
        fprintf(rptFile, "       Requested   : %s\n", _platform_name.c_str());
        fprintf(rptFile, "       Name        : %s\n", _platforms[_platforms_num].getInfo<CL_PLATFORM_NAME>().c_str());
        fprintf(rptFile, "       Vendor      : %s\n", _platforms[_platforms_num].getInfo<CL_PLATFORM_VENDOR>().c_str());
        fprintf(rptFile, "       Version     : %s\n", _platforms[_platforms_num].getInfo<CL_PLATFORM_VERSION>().c_str());
        fprintf(rptFile, "       Warp Size   : %d\n", _warp);
        fprintf(rptFile, "OpenCL Device      : \n");
        fprintf(rptFile, "       Name        : %s\n", _devices[0].getInfo<CL_DEVICE_NAME>().c_str());
        fprintf(rptFile, "       Device Num  : %d\n", _devices_num);
        fprintf(rptFile, "       ComputeUnits: %d\n", _devices[0].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());
        fprintf(rptFile, "       Fission     : %d\n", _device_fission ? 1 : 0);
        fprintf(rptFile, "       Device Type : ");
        switch (_device_type)
        {
        case CL_DEVICE_TYPE_GPU: fprintf(rptFile, "GPU\n");  break;
        case CL_DEVICE_TYPE_CPU: fprintf(rptFile, "CPU\n");  break;
        default: fprintf(rptFile, "UNKNOWN\n");
        }
        fprintf(rptFile, "       Mem         : %lu\n", _devices[0].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>());
        fprintf(rptFile, "       Max Mem Buff: %lu\n", _devices[0].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>());

        // Show extra configuration options.
        if (!_config_filename.empty())
        {
            fprintf(rptFile, "Configuration file : %s\n", _config_filename.c_str());

            typedef std::map<std::string, cl::NDRange>::iterator it_type;
            for (it_type iterator = _hash_ranges.begin(); iterator != _hash_ranges.end(); ++iterator)
            {
                const size_t* range = iterator->second;
                fprintf(rptFile, "CA Function Range  : %s (%d,%d)\n", iterator->first.c_str(), range[0], range[1]);
            }
        }
        else
        {
            fprintf(rptFile, "Configuration file : NO CONFIGURATION FILE\n");
        }
    }


    inline void Grid::setCAPrint(bool)
    {
    }


    inline void Grid::setDataDir(const std::string& str)
    {
        _datadir = str;
    }


    inline std::string Grid::dataDir() const
    {
        return _datadir;
    }


    inline const _caGrid Grid::caGrid() const
    {
        return _cagrid;
    }


    inline const _caGrid_short Grid::caGridShort() const
    {
        return _cagrid_short;
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

        // Write the warp value.
        unsigned int warp = _warp;
        file.write(reinterpret_cast<char*>(&warp), sizeof(unsigned int));

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


    inline const cl::Context& Grid::context() const
    {
        return _context;
    }


    inline const std::vector<cl::Device>& Grid::devices() const
    {
        return _devices;
    }


    inline const cl::CommandQueue& Grid::queue() const
    {
        return _queue;
    }


    inline const cl::Program& Grid::kernelsProgram() const
    {
        return _kernels_program;
    }


    template<typename Func>
    inline const cl::Program& Grid::getProgram(Func& f)
    {
        // Get the program from the function (third element)
        //cl::Program& program = f().third;
        std::string functionName = f().first;

        if (_programsMap.count(functionName))
        {
            // program exists, therefore return the program
            return _programsMap[functionName];
        }
        // program does NOT exist, therefore build it and store, it before returning it.
        _programsMap[functionName] = cl::Program();

        // return the address
        cl::Program& program = _programsMap[functionName];

        // If the program is null, this is the first time the CA function
        // is called. The specific program needs to be build.
        if (!program())
        {
            // Find the possible NDRange set up from a configuration file.
            f().fourth = _hash_ranges[f().first];

            // Build the CA Function kernels with the caapi2D.cl code which
            // is used by all the CA Function kernels.

            // Create a single strings with all the code (caapi2D.cl + CA Function)
            std::string strsource(caapi2D().second + f().second);

            // Create the source object
            cl::Program::Sources sources(1, std::make_pair(strsource.c_str(), strsource.size() + 1));

            // Create the program and build it    
            try
            {
                //Create and build the program
                program = cl::Program(_context, sources);
                program.build(_devices, _building_options.c_str());
            }
            catch (cl::Error err)
            {
                std::cerr << std::endl << "Error CA Function: " << f().first << ".ca" << std::endl << std::endl;
                //std::cerr<<"Source:"<<std::endl<<std::endl;
                //std::cerr<<program.getInfo<CL_PROGRAM_SOURCE>()<<std::endl<<std::endl;

                for (size_t i = 0; i < _devices.size(); ++i)
                {
                    std::cerr << "Build log device " << i << ":" << std::endl << std::endl;
                    std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_devices[i]) << std::endl;
                }
                throw;
            }

            // Show the Build info      
            if (_build_info)
            {
                // Erase ~/.nv/ComputeCache
                std::cerr << "--------- Start Build Info ----" << std::endl;
                for (size_t i = 0; i < _devices.size(); ++i)
                {
                    std::cerr << "Build log device " << i << ":" << std::endl << std::endl;
                    std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_devices[i]) << std::endl;
                }
                std::cerr << "--------- End Build Info ----" << std::endl;
            }
        }

        _programsMap[functionName] = program;

        return program;
    }


    inline Unsigned Grid::warp() const
    {
        return _warp;
    }


    inline void Grid::fill1DBuff(Real value, cl::Buffer& tmp_buff, _caUnsigned start, _caUnsigned stop,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the number of global work item to be the same number of
        // elements in the buffer but alligned to warp/wavefront. Set also
        // the offset. 
        cl::NDRange offset(start);
        cl::NDRange global(computeStride(stop - start, _warp));

        // Set the arguments of the kernel.
        _kernel_setValueReal.setArg(0, tmp_buff);
        _kernel_setValueReal.setArg(1, start);
        _kernel_setValueReal.setArg(2, stop);
        _kernel_setValueReal.setArg(3, value);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel that initialise the buffer to 0 and save the event.
        _queue.enqueueNDRangeKernel(_kernel_setValueReal, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel that initialise the buffer to 0 and no event.
        _queue.enqueueNDRangeKernel(_kernel_setValueReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::fill1DBuff(State value, cl::Buffer& tmp_buff, _caUnsigned start, _caUnsigned stop,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the number of global work item to be the same number of
        // elements in the buffer but alligned to warp/wavefront. Set also
        // the offset. 
        cl::NDRange offset(start);
        cl::NDRange global(computeStride(stop - start, _warp));

        // Set the arguments of the kernel.
        _kernel_setValueState.setArg(0, tmp_buff);
        _kernel_setValueState.setArg(1, start);
        _kernel_setValueState.setArg(2, stop);
        _kernel_setValueState.setArg(3, value);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel that initialise the buffer to 0 and save the event.
        _queue.enqueueNDRangeKernel(_kernel_setValueState, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel that initialise the buffer to 0 and no event.
        _queue.enqueueNDRangeKernel(_kernel_setValueState, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::op2DBuff(Real value, cl::Buffer& tmp_buff,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        CA::Bdr::Operator op,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
        // given box. The area contains the vairous horizontal lines that
        // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the horizontal line from where the value are
        // copyed.
        _kernel_opValueReal.setArg(0, tmp_buff);
        _kernel_opValueReal.setArg(1, value);
        _kernel_opValueReal.setArg(2, x_stride);
        _kernel_opValueReal.setArg(3, (_caInt)op);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_opValueReal, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_opValueReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::op2DBuff(State value, cl::Buffer& tmp_buff,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        CA::Bdr::Operator op,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
        // given box. The area contains the vairous horizontal lines that
        // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the horizontal line from where the value are
        // copyed.
        _kernel_opValueState.setArg(0, tmp_buff);
        _kernel_opValueState.setArg(1, value);
        _kernel_opValueState.setArg(2, x_stride);
        _kernel_opValueState.setArg(3, (_caInt)op);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_opValueState, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_opValueState, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::seq2DBuff(Real& value, const cl::Buffer& tmp_buff,
        _caUnsigned x_offset, _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        CA::Seq::Operator op,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Compute worg group size.
        _caUnsigned global_size = computeStride(x_num, _warp);
        _caUnsigned local_size = _warp;

        // Set the 1D NDrange for the global workspace and offset from the
        // given box. 
        cl::NDRange offset(x_start);
        cl::NDRange global(global_size);
        cl::NDRange local(local_size);

        // Create the temporary buffer with the results.
        size_t res_num = global_size / local_size;
        size_t res_size = res_num * sizeof(Real);
        cl::Buffer res_buff(_context, CL_MEM_READ_WRITE, res_size);

        // Set the starting and ending point on the Y dimension.
        _caUnsigned src_start = y_start * x_stride;
        _caUnsigned src_stop = (y_num*x_stride) + src_start;

        // Set the arguments of the kernel. 
        _kernel_reduceReal.setArg(0, res_buff);
        _kernel_reduceReal.setArg(1, tmp_buff);
        _kernel_reduceReal.setArg(2, local_size * sizeof(Real), 0);
        _kernel_reduceReal.setArg(3, x_offset);
        _kernel_reduceReal.setArg(4, x_num + x_start);
        _kernel_reduceReal.setArg(5, src_start);
        _kernel_reduceReal.setArg(6, src_stop);
        _kernel_reduceReal.setArg(7, x_stride);
        _kernel_reduceReal.setArg(8, (_caInt)op);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel and save the event.
        cl::Event event;
        _queue.enqueueNDRangeKernel(_kernel_reduceReal, offset, global, local, wait_events, &event);

        // Wait for the kernel to finish.
        event.wait();
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_reduceReal, offset, global, local, NULL, NULL);
#endif

        // Create a temporary memory buffer with the results.
        std::vector<_caReal> res_mem(res_num);

        // Read the buffer with the results.  
        _queue.enqueueReadBuffer(res_buff, CL_TRUE, 0, res_size, &res_mem[0]);

        // Perform a sequential operation on the buffer.
        switch (op)
        {
        case Seq::Add:
            value = 0;
            for (size_t i = 0; i < res_mem.size(); i++)
                value += res_mem[i];
            break;
        case Seq::Mul:
            value = 1;
            for (size_t i = 0; i < res_mem.size(); i++)
                value *= res_mem[i];
            break;
        case Seq::Min:
            value = std::numeric_limits<Real>::max();
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::min(res_mem[i], static_cast<_caReal>(value));
            break;
        case Seq::MinAbs:
            value = std::numeric_limits<Real>::max();
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::min(res_mem[i], std::abs(static_cast<_caReal>(value)));
            break;
        case Seq::Max:
            value = std::numeric_limits<Real>::min();
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::max(res_mem[i], static_cast<_caReal>(value));
            break;
        case Seq::MaxAbs:
            value = 0;
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::max(res_mem[i], std::abs(static_cast<_caReal>(value)));
            break;
        }
#ifdef  CA_OCL_USE_EVENTS 
        // Set the event to wait.
        if (e)
            (*e) = event;
#else
        // do nothing? seems right here.
#endif
    }


    inline void Grid::seq2DBuff(State& value, const cl::Buffer& tmp_buff,
        _caUnsigned x_offset, _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        CA::Seq::Operator op,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Compute worg group size.
        _caUnsigned global_size = computeStride(x_num, _warp);
        _caUnsigned local_size = _warp;

        // Set the 1D NDrange for the global workspace and offset from the
        // given box. 
        cl::NDRange offset(x_start);
        cl::NDRange global(global_size);
        cl::NDRange local(local_size);

        // Create the temporary buffer with the results.
        size_t res_num = global_size / local_size;
        size_t res_size = res_num * sizeof(State);
        cl::Buffer res_buff(_context, CL_MEM_READ_WRITE, res_size);

        // Set the starting and ending point on the Y dimension.
        _caUnsigned src_start = y_start * x_stride;
        _caUnsigned src_stop = (y_num*x_stride) + src_start;

        // Set the arguments of the kernel. 
        _kernel_reduceState.setArg(0, res_buff);
        _kernel_reduceState.setArg(1, tmp_buff);
        _kernel_reduceState.setArg(2, local_size * sizeof(State), 0);
        _kernel_reduceState.setArg(3, x_offset);
        _kernel_reduceState.setArg(4, x_num + x_start);
        _kernel_reduceState.setArg(5, src_start);
        _kernel_reduceState.setArg(6, src_stop);
        _kernel_reduceState.setArg(7, x_stride);
        _kernel_reduceState.setArg(8, (_caInt)op);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel and save the event.
        cl::Event event;
        _queue.enqueueNDRangeKernel(_kernel_reduceState, offset, global, local, wait_events, &event);
        // Wait for the kernel to finish.
        event.wait();
#else
        _queue.enqueueNDRangeKernel(_kernel_reduceState, offset, global, local, NULL, NULL);
#endif

        // Create a temporary memory buffer with the results.
        std::vector<_caState> res_mem(res_num);

        // Read the buffer with the results.  
        _queue.enqueueReadBuffer(res_buff, CL_TRUE, 0, res_size, &res_mem[0]);

        // Perform a sequential operation on the buffer.
        switch (op)
        {
        case Seq::Add:
            value = 0;
            for (size_t i = 0; i < res_mem.size(); i++)
                value += res_mem[i];
            break;
        case Seq::Mul:
            value = 1;
            for (size_t i = 0; i < res_mem.size(); i++)
                value *= res_mem[i];
            break;
        case Seq::Min:
            value = std::numeric_limits<State>::max();
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::min(res_mem[i], static_cast<_caState>(value));
            break;
        case Seq::MinAbs:
            value = std::numeric_limits<State>::max();
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::min(res_mem[i], std::abs(static_cast<_caState>(value)));
            break;
        case Seq::Max:
            value = std::numeric_limits<State>::min();
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::max(res_mem[i], static_cast<_caState>(value));
            break;
        case Seq::MaxAbs:
            value = 0;
            for (size_t i = 0; i < res_mem.size(); i++)
                value = std::max(res_mem[i], std::abs(static_cast<_caState>(value)));
            break;
        }

#ifdef  CA_OCL_USE_EVENTS 
        // Set the event to wait.
        if (e)
            (*e) = event;
#else
        // do nothing? seems right here.
#endif
    }


    inline void Grid::copy2DBuffHLine(Real value, cl::Buffer& tmp_buff,
        _caUnsigned l_start,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
        // given box. The area contains the vairous horizontal lines that
        // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the horizontal line from where the value are
        // copyed.
        _kernel_copyHLineReal.setArg(0, tmp_buff);
        _kernel_copyHLineReal.setArg(1, l_start);
        _kernel_copyHLineReal.setArg(2, x_stride);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyHLineReal, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyHLineReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::copy2DBuffVLine(Real value, cl::Buffer& tmp_buff,
        _caUnsigned l_start,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
        // given box. The area contains the vairous vertical lines that
        // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the vertical line from where the value are
        // copyed.
        _kernel_copyVLineReal.setArg(0, tmp_buff);
        _kernel_copyVLineReal.setArg(1, l_start);
        _kernel_copyVLineReal.setArg(2, x_stride);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyVLineReal, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyVLineReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::copy2DBuffHLine(State value, cl::Buffer& tmp_buff,
        _caUnsigned l_start,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
        // given box. The area contains the vairous horizontal lines that
        // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the horizontal line from where the value are
        // copyed.
        _kernel_copyHLineState.setArg(0, tmp_buff);
        _kernel_copyHLineState.setArg(1, l_start);
        _kernel_copyHLineState.setArg(2, x_stride);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyHLineState, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyHLineState, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::copy2DBuffVLine(State value, cl::Buffer& tmp_buff,
        _caUnsigned l_start,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
        // given box. The area contains the vairous vertical lines that
        // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the vertical line from where the value are
        // copyed.
        _kernel_copyVLineState.setArg(0, tmp_buff);
        _kernel_copyVLineState.setArg(1, l_start);
        _kernel_copyVLineState.setArg(2, x_stride);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyVLineState, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyVLineState, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::copy2DBuffPoint(Real value, cl::Buffer& tmp_buff,
        _caUnsigned p_x, _caUnsigned p_y,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
       // given box. The area contains the vairous vertical lines that
       // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the vertical line from where the value are
        // copyed.
        _kernel_copyPointReal.setArg(0, tmp_buff);
        _kernel_copyPointReal.setArg(1, p_x);
        _kernel_copyPointReal.setArg(2, p_y);
        _kernel_copyPointReal.setArg(3, x_stride);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyPointReal, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyPointReal, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::copy2DBuffPoint(State value, cl::Buffer& tmp_buff,
        _caUnsigned p_x, _caUnsigned p_y,
        _caUnsigned x_stride,
        _caUnsigned x_start, _caUnsigned x_num,
        _caUnsigned y_start, _caUnsigned y_num,
        std::vector<cl::Event>* wait_events, cl::Event* e)
    {
        // Set the NDrange for the global workspace and offset from the
       // given box. The area contains the vairous vertical lines that
       // receive the value to be shifted.
        cl::NDRange offset(x_start, y_start);
        cl::NDRange global(x_num, y_num);

        // Set the arguments of the kernel. The given parameters
        // indetify the vertical line from where the value are
        // copyed.
        _kernel_copyPointState.setArg(0, tmp_buff);
        _kernel_copyPointState.setArg(1, p_x);
        _kernel_copyPointState.setArg(2, p_y);
        _kernel_copyPointState.setArg(3, x_stride);

#ifdef  CA_OCL_USE_EVENTS 
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyPointState, offset, global, cl::NDRange(), wait_events, e);
#else
        // Lunch the kernel 
        _queue.enqueueNDRangeKernel(_kernel_copyPointState, offset, global, cl::NDRange(), NULL, NULL);
#endif
    }


    inline void Grid::manageOptions(const Options& options)
    {
        for (Options::const_iterator i = options.begin(); i != options.end(); ++i)
        {
            if ((*i)->name == "device-type")
            {
                if (compareCaseInsensitive((*i)->value, "GPU"))
                    _device_type = CL_DEVICE_TYPE_GPU;
                if (compareCaseInsensitive((*i)->value, "CPU"))
                    _device_type = CL_DEVICE_TYPE_CPU;
            }

            if ((*i)->name == "platform-name")
            {
                _platform_name = CA::trimToken((*i)->value);
            }

            if ((*i)->name == "device-number")
            {
                if (!fromString(_devices_num, (*i)->value))
                    _devices_num = 0;
            }

            if ((*i)->name == "warp-size")
            {
                if (!fromString(_warp, (*i)->value))
                    _warp = 32;
            }

            if ((*i)->name == "out-of-order")
            {
                _queue_properties |= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
            }

            if ((*i)->name == "build-info")
            {
                _build_info = true;
            }

            if ((*i)->name == "config-file")
            {
                _config_filename = (*i)->value;
            }
        }
    }

    inline int Grid::readConfigCSV(const std::string filename)
    {
        // Read config file.
        std::ifstream ifile(filename.c_str());

        if (!ifile) {
            std::cout << "No config file found [" << filename << "], using default settings..." << std::endl;

            //NOTE might be better to use the automatic seach

            // Add the function with range default settings
            _hash_ranges["outflowWCA2Dv1"] = cl::NDRange(32, 4);
            _hash_ranges["outflowWCA2Dv2"] = cl::NDRange(32, 4);
            _hash_ranges["waterdepthWCA2Dv1"] = cl::NDRange(32, 4);
            _hash_ranges["waterdepthWCA2Dv2"] = cl::NDRange(32, 4);
            _hash_ranges["velocityWCA2Dv1"] = cl::NDRange(32, 4);
            _hash_ranges["velocityWCA2Dv2"] = cl::NDRange(32, 4);
            _hash_ranges["updatePEAKC"] = cl::NDRange(32, 4);
            _hash_ranges["updatePEAKE"] = cl::NDRange(32, 4);
            _hash_ranges["addRain"] = cl::NDRange(32, 4);
            _hash_ranges["outflowWCA2Dv3"] = cl::NDRange(32, 4);
            _hash_ranges["velocityWCA2Dv3"] = cl::NDRange(32, 4);
            _hash_ranges["outflowDiffusive"] = cl::NDRange(32, 4);
            _hash_ranges["outflowInertial"] = cl::NDRange(32, 4);
            _hash_ranges["waterdepth"] = cl::NDRange(32, 4);
            _hash_ranges["velocityDiffusive"] = cl::NDRange(32, 4);
            _hash_ranges["velocityInertial"] = cl::NDRange(32, 4);
            _hash_ranges["infiltration"] = cl::NDRange(32, 4);
            _hash_ranges["outflowWCA2Dv1GEO"] = cl::NDRange(32, 4);
            _hash_ranges["waterdepthWCA2Dv1GEO"] = cl::NDRange(32, 4);
            _hash_ranges["velocityWCA2Dv1GEO"] = cl::NDRange(32, 4);
            _hash_ranges["infiltrationGEO"] = cl::NDRange(32, 4);
            _hash_ranges["setBoundaryEle"] = cl::NDRange(32, 4);

            _device_type = CL_DEVICE_TYPE_GPU;

            _warp = 32;
            _devices_num = 0;

            return 1;
        }

        // Parse the file line by line until the end of file 
        // and retrieve the tokens of each line.
        while (!ifile.eof())
        {
            // If true the token was identified;
            bool found_tok = false;

            std::vector<std::string> tokens(CA::getLineTokens(ifile, ','));

            // If the tokens vector is empty we reached the eof or an
            // empty line... continue.
            if (tokens.empty())
                continue;

            if (CA::compareCaseInsensitive("CA Function Range", tokens[0], true))
            {
                found_tok = true;

                // Read the name of the function
                std::string str;
                CA_GRID_READ_TOKEN(found_tok, str, tokens[1], tokens[0]);

                // Create the NDRange as empty and read the rest of the token
                // to add the range values.
                std::vector<Unsigned> values(3, 1);

                for (size_t i = 2, v = 0; i < tokens.size() && v < 2; ++i, ++v)
                {
                    Unsigned value = 0;
                    CA_GRID_READ_TOKEN(found_tok, value, tokens[i], tokens[0]);
                    values[v] = value;
                }

                // Add the function with range
                _hash_ranges[CA::trimToken(str)] = cl::NDRange(values[0], values[1]);
            }

            if (CA::compareCaseInsensitive("Warp Size", tokens[0], true))
                CA_GRID_READ_TOKEN(found_tok, _warp, tokens[1], tokens[0]);

            if (CA::compareCaseInsensitive("Out Of Order", tokens[0], true))
            {
                std::string str = CA::trimToken(tokens[1]);
                bool value;
                CA_GRID_READ_TOKEN(found_tok, value, str, tokens[0]);
                if (value)
                    _queue_properties |= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
            }

            if (CA::compareCaseInsensitive("Device Type", tokens[0], true))
            {
                found_tok = true;
                std::string str = CA::trimToken(tokens[1]);
                if (compareCaseInsensitive(str, "GPU"))
                    _device_type = CL_DEVICE_TYPE_GPU;
                if (compareCaseInsensitive(str, "CPU"))
                    _device_type = CL_DEVICE_TYPE_CPU;
            }

            if (CA::compareCaseInsensitive("Platform Name", tokens[0], true))
            {
                std::string str = CA::trimToken(tokens[1]);
                CA_GRID_READ_TOKEN(found_tok, _platform_name, str, tokens[0]);
            }

            if (CA::compareCaseInsensitive("Device Num", tokens[0], true))
            {
                CA_GRID_READ_TOKEN(found_tok, _devices_num, tokens[1], tokens[0]);
            }

            if (CA::compareCaseInsensitive("Device CU", tokens[0], true))
                CA_GRID_READ_TOKEN(found_tok, _device_cu, tokens[1], tokens[0]);

            // If the token was not identified stop!
            if (!found_tok)
            {
                std::cerr << "Element '" << CA::trimToken(tokens[0]) << "' not identified" << std::endl; \
                    return 1;
            }
        }

        return 0;
    }

    inline void Grid::initOpenCL()
    {
        if (initcl == false)
        {
            std::cout << "Getting platform etc details...." << std::endl;
            // Retrive the OpenCL platforms information.
            cl::Platform::get(&_platforms);

            // This is the string of the OpenCL version taken from the platform.
            std::string version;

            if (_platform_name.empty())
            {
                // If there was no specific request of a OpenCL platform.
                // Loop through the platforms and until a device of the chosen
                // type is found.
                std::vector<cl::Platform>::iterator iter = _platforms.begin();
                while (_devices.empty() && iter != _platforms.end())
                {
                    try
                    {
                        iter->getDevices(_device_type, &_devices);
                        version = iter->getInfo<CL_PLATFORM_VERSION>();
                        break;
                    }
                    catch (cl::Error err)
                    {
                    }
                    ++iter;
                }

                // Memorize which platform is going to be used.
                _platforms_num = std::distance(_platforms.begin(), iter);
            }
            else
            {
                // There was a specific request of OpenCL platform.
                // Loop through the platforms and until the one chosen is found.
                std::vector<cl::Platform>::iterator iter = _platforms.begin();
                while (iter != _platforms.end())
                {
                    std::string p_name(iter->getInfo<CL_PLATFORM_NAME>());
                    if (compareCaseInsensitive(_platform_name, p_name, true))
                        break;
                    ++iter;
                }

                // Memorize which platform is going to be used.
                _platforms_num = std::distance(_platforms.begin(), iter);

                // Check if the platform was definitily found.
                if (_platforms_num >= _platforms.size())
                    throw std::runtime_error(std::string("OpenCL platform of the chosen type not found: ") + _platform_name);

                // Get the device of the given type
                try
                {
                    _platforms[_platforms_num].getDevices(_device_type, &_devices);
                    version = iter->getInfo<CL_PLATFORM_VERSION>();
                }
                catch (cl::Error err)
                {
                }
            }

            // If the devices vector is empty there are no device of the
            // chosen type in this machine. Throw an error.
            if (_devices.empty())
            {
                std::string d_str;
                switch (_device_type)
                {
                case CL_DEVICE_TYPE_GPU: d_str = "GPU";  break;
                case CL_DEVICE_TYPE_CPU: d_str = "CPU";  break;
                default: d_str = "UNKNOWN";
                }
                throw std::runtime_error(std::string("OpenCL devices of the chosen type not found: ") + d_str);
            }

            // Check if the device number is in the list of devices.
            if (_devices_num >= _devices.size())
            {
                std::string d_str;
                toString(d_str, _devices_num);
                throw std::runtime_error(std::string("OpenCL device number chosen  not found: ") + d_str);
            }

            // NOW need to make the list of devices to contain only the
            // selected one.
            cl::Device tmp_device = _devices[_devices_num];
            _devices.clear();
            _devices.push_back(tmp_device);

            // Retrieve the major an minor number.
            int major = 0;
            int minor = 0;

            fromString(major, version.substr(7, 1));
            fromString(minor, version.substr(9, 1));

            // Check that the version is at leat 1.1
            if ((major == 1 && minor < 1))
                throw std::runtime_error("OpenCL device need to be at least version 1.1.");

            // Perform the device fission if requested. At the moment this
            // work only on CPU.
            int cu = _devices[0].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS >();
            if (_device_type == CL_DEVICE_TYPE_CPU && _device_cu > 0 && _device_cu < cu)
            {
#if defined(CL_VERSION_1_2)
                if ((major == 1 && minor == 2))
                {
                    const cl_device_partition_property sub_properties[] =
                    {
                      CL_DEVICE_PARTITION_EQUALLY,
                      _device_cu,0
                    };

                    // Get the subdevice
                    std::vector<cl::Device> sub_devices;
                    _devices[0].createSubDevices(sub_properties, &sub_devices);

                    // Assign the first sub device as the new device.
                    if (!sub_devices.empty())
                    {
                        _devices[0] = sub_devices[0];
                        // Fission is on baby!
                        _device_fission = true;
                    }
                }
#endif	
                if (!_device_fission)
                {
                    // If the major and minor number is 1.1 then use device extension
                    // Check if the device fission exstension exist.
                    if (_devices[0].getInfo<CL_DEVICE_EXTENSIONS>().find("cl_ext_device_fission") == std::string::npos)
                        throw std::runtime_error("OpenCL device do not support fission");

                    const cl_device_partition_property_ext sub_properties[] =
                    {
                      CL_DEVICE_PARTITION_EQUALLY_EXT,
                      _device_cu,
                      CL_PROPERTIES_LIST_END_EXT,
                      static_cast<cl_device_partition_property_ext>(0)
                    };

                    // Get the subdevice
                    std::vector<cl::Device> sub_devices;
                    _devices[0].createSubDevices(sub_properties, &sub_devices);

                    // Assign the first sub device as the new device.
                    if (!sub_devices.empty())
                    {
                        _devices[0] = sub_devices[0];
                        // Fission is on baby!
                        _device_fission = true;
                    }
                }
            }

            try
            {
                // Create the context of this Grid.
                _context = cl::Context(_devices);
            }
            catch (cl::Error e)
            {
                //std::cerr << "GRID create contex for the device error: " << e.what() << "(" << e.err() << ")" <<std::endl;
                std::cout << "GRID create contex for the device error: " << e.what() << "(" << e.err() << ")" << std::endl;
                throw e;
            }

            // Create the queue from the first device available.
            _queue = cl::CommandQueue(_context, _devices[0], _queue_properties);

            // Build the helper kernels with the caapi2D.cl code which is used
            // by all the internal kernels.

            // Create a single strings with all the code (caapi2D.cl + kernels.cl)
            std::string strsource(caapi2D().second + kernels().second);

            // Create the source object
            cl::Program::Sources	sources(1, std::make_pair(strsource.c_str(), strsource.size() + 1));

            // Create the building options
            // Add  real precision options.
            _building_options += " -D CA_REAL_FLOAT=0 -D CA_REAL_DOUBLE=1";
#if     CA_REAL_PRECISION == CA_REAL_DOUBLE
            _building_options += " -D CA_REAL_PRECISION=1";
#else
            _building_options += " -D CA_REAL_PRECISION=0";
            _building_options += " -cl-single-precision-constant";
#endif
            // Add  constant/global options.
            _building_options += " -D CA_OCL_CONSTANT=0 -D CA_OCL_GLOBAL=1";
#if     CA_OCL_TABLE == 1
            _building_options += " -D CA_OCL_TABLE=1";
#else
            _building_options += " -D CA_OCL_TABLE=0";
#endif

#if     CA_OCL_NATIVE == 1
            _building_options += " -D CA_OCL_NATIVE=1";
#else
            _building_options += " -D CA_OCL_NATIVE=0";
#endif

#ifdef CA2D_GPU
            _building_options += " -D CA2D_GPU=1";

#endif
            // Prepare the build info
            if (_build_info)
            {
                // Get the platform name
                std::string plat_name = _platforms[_platforms_num].getInfo<CL_PLATFORM_NAME>();

                // If is a GPU and NVIDIA, add the building options to have
                // more information.
                if (_device_type == CL_DEVICE_TYPE_GPU && plat_name.find("NVIDIA") != std::string::npos)
                {
                    _building_options += " -cl-nv-verbose";
                }
            }

            // Create the program and build it    
            try
            {
                _kernels_program = cl::Program(_context, sources);
                _kernels_program.build(_devices, _building_options.c_str());
            }
            catch (cl::Error err)
            {
                std::cerr << "OpenCL Program/Kernel Error!!!" << std::endl;
                std::cerr << "Source:" << std::endl << std::endl;
                std::cout << _kernels_program.getInfo<CL_PROGRAM_SOURCE>() << std::endl << std::endl;

                for (size_t i = 0; i < _devices.size(); ++i)
                {
                    std::cerr << "Build log device " << i << ":" << std::endl << std::endl;
                    std::cerr << _kernels_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_devices[i]) << std::endl;
                }
                throw;
            }

            try {
                // Retrieve the internal kernels.
                _kernel_setValueReal = cl::Kernel(_kernels_program, "setValueReal");
                _kernel_setValueState = cl::Kernel(_kernels_program, "setValueState");
                _kernel_opValueReal = cl::Kernel(_kernels_program, "opValueReal");
                _kernel_opValueState = cl::Kernel(_kernels_program, "opValueState");
                _kernel_reduceReal = cl::Kernel(_kernels_program, "reduceReal");
                _kernel_reduceState = cl::Kernel(_kernels_program, "reduceState");
                _kernel_copyHLineReal = cl::Kernel(_kernels_program, "copyHLineReal");
                _kernel_copyVLineReal = cl::Kernel(_kernels_program, "copyVLineReal");
                _kernel_copyHLineState = cl::Kernel(_kernels_program, "copyHLineState");
                _kernel_copyVLineState = cl::Kernel(_kernels_program, "copyVLineState");
                _kernel_copyPointReal = cl::Kernel(_kernels_program, "copyPointReal");
                _kernel_copyPointState = cl::Kernel(_kernels_program, "copyPointState");
            }
            catch (cl::Error e) {
                std::cout << "GRID creating internal kernals error: " << e.what() << "(" << e.err() << ")" << std::endl;
            }

            // Show the Build info      
            if (_build_info)
            {
                std::cerr << "--------- Start Build Info ----" << std::endl;
                for (size_t i = 0; i < _devices.size(); ++i)
                {
                    std::cerr << "Build log device " << i << ":" << std::endl << std::endl;
                    std::cerr << _kernels_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_devices[i]) << std::endl;
                }
                std::cerr << "--------- End Build Info ----" << std::endl;
            }

            initcl = true;
        } // End check initcl.
    }
}


#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER


#endif  // _CA_GRID_HPP_
