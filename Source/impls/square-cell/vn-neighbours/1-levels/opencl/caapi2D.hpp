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

#ifndef _CAAPI2D_H_
#define _CAAPI2D_H_

//! \file caapi2D.h 
//! caAPI 2D OpenCL implementation with a lattice that is structured,
//! regular, with square cells and a Von Neumann neighbourhood of 1
//! level. This file is used by the CA algorithm.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-12


#include"Common.hpp"

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS
#define USE_CL_DEVICE_FISSION
#define CL_EXT_SUFFIX__VERSION_1_1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include<CL/cl.hpp>


#if !defined __cplusplus
#error This CAAPI 2D implementation requires a C++ compiler
#endif


//////////////////////////////////////////////////////////////////////
//
// Set the file caapi2D.cl as a string.
// Set the file kernels.cl as a string.
//
// The included file must have been converted into an header by the
// convertCA2HPP command.
//////////////////////////////////////////////////////////////////////


#include"caapi2D.cl.hpp"
#include"kernels.cl.hpp"


// ---- GLOBAL VARIABLES  ----


//! PI Value
const double PI = 3.141592654;


// ---- CONSTANTS ----


// The number of the CAAPI version
const int caVersion = CAAPI_VERSION;

//! The string with the version of the caAPI implemented.
static const char* caVersionStr = CA_QUOTE_MACRO(CAAPI_VERSION);

//! The string with implementation name.
static const char* caImplName = CA_QUOTE_MACRO(CAAPI_IMPL_NAME);

//! The string with implementation short name.
static const char* caImplShortName = CA_QUOTE_MACRO(CAAPI_IMPL_SHORTNAME);

//! The number with the version of the implementation.
const int caImplVersion = CAAPI_IMPL_VERSION;

//! The string with the version of the implementation.
static const char* caImplVersionStr = CA_QUOTE_MACRO(CAAPI_IMPL_VERSION);


//! The string with the precision implemented.
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
static const char* caImplPrecision = "float";
#elif CA_REAL_PRECISION == CA_REAL_DOUBLE
static const char* caImplPrecision = "double";
#else
static const char* caImplPrecision = "unknown";
#endif

#if     CA_OCL_EVENTS == 1
#define CA_OCL_USE_EVENTS
#endif

//////////////////////////////////////////////////////////////////////
//
// Attention!!!!
//
// The following data and types should be the same on caapi2D.cl since
// they are shared between the Main (host) algorithm and the CA
// (kernel) functions.
//
//////////////////////////////////////////////////////////////////////

//! Define the type of real values.
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
typedef cl_float            _caReal;
#elif CA_REAL_PRECISION == CA_REAL_DOUBLE
typedef cl_double           _caReal;
#else
#error Undefined Real precision 
#endif


//! Define the type of state values.
typedef cl_int             _caState;

//! Define the type of a size value.
typedef cl_ulong           _caUnsigned;

//! Define the type of a integer value.
typedef cl_int             _caInt;

//! Define the type that contains two size values.
typedef cl_ulong2          _caUnsigned2;

//! Define the numebr of neighbours
#define CA2D_NEIGHBOURS 4


//! The total number of cells in the neighbourhood, i.e. the
//! number of neighbours. 
const int caNeighbours = 4;

//! Number of edges of a cell. 
const int caEdges = 4;

//! Number of vertices of a cell. 
const int caVertices = 4;

//! Number of neighbours' levels in the neighbourhood. 
const int caLevels = 1;

//! Maximum number of neighbours in a level
const int caMaxNeighboursLevel = 4;


//! Contains the information about the grid which are used by the CA
//! functions CA functions.  \attention position (0,0) is top left
//! corner.
struct _caGrid
{
    //! The size in the X dimension of the grid (num cells).
    _caUnsigned x_size;

    //! The size in the Y dimension of the grid (num cells).
    _caUnsigned y_size;

    //! The lenght of an edge of the cell in the X/Y dimension.
    _caReal length;

    //! The x coordinate of the bottom-left corner
    _caReal x_coo;

    //! The y coordinate of the bottom-left corner
    _caReal y_coo;

    //! The area of a cell;
    _caReal area;

    //! The left x position to the top left corner of the
    //! rectangular box.
    _caUnsigned bx_lx;

    //! The top y position to the top left corner of the
    //! rectangular box.
    _caUnsigned bx_ty;

    //! The right x position to the bottom righ corner of the
    //! rectangular box.
    _caUnsigned bx_rx;

    //! The bottom y position to the bottom righ corner of the
    //! rectangular box.
    _caUnsigned bx_by;

    //! The size of a cell buffer on the X dimension with the border
    //! cells.
    _caUnsigned cb_x_size;

    //! The size of a cell buffer on the Y dimension with the border
    //! cells.
    _caUnsigned cb_y_size;

    //! Border size in one side of a cell buffer. It depends on the
    //! number of neighbours's levels chosen.
    _caUnsigned cb_border;

    //! The offset in the X dimension of a cell buffer. 
    _caUnsigned cb_x_offset;

    //! The stride in the X dimension of a cell buffer. It represent the
    //! alligned X dimension which is used to improve the performance on
    //! GPU. \attention This is the X dimenesnison that the buffer is
    //! allocated.
    _caUnsigned cb_stride;

    //! The size of the north/sourth sub-buffer of an edge buffer on
    //! the X and Y dimensions with the border edges.
    _caUnsigned eb_ns_x_size;
    _caUnsigned eb_ns_y_size;

    //! The size of the west/east sub-buffer of an edge buffer on the X and
    //! Y dimensions with the border edges.
    _caUnsigned eb_we_x_size;
    _caUnsigned eb_we_y_size;

    //! Border size in the Y direction for the north/south sub-buffer
    //! of an edge buffer. It depends on the number of neighbours's
    //! levels chosen.
    _caUnsigned eb_ns_y_border;

    //! Border size in the X direction for the west/east sub-buffer of
    //! an edge buffer. It depends on the number of neighbours's levels
    //! chosen.
    _caUnsigned eb_we_x_border;

    //! The offset in the X dimension of the west/east sub-buffers of the edge buffer. 
    _caUnsigned eb_we_x_offset;

    //! The stride in the X dimension of north/south west/east
    //! sub-buffers of a edge buffer. It represent the alligned X
    //! dimension which is used to improve the performance on
    //! GPU. \attention This is the X dimenesnison that the buffer is
    //! allocated.
    _caUnsigned eb_ns_stride;
    _caUnsigned eb_we_stride;

    //! Since the main buffer is divided in two sub buffers, this the
    //! the starting point of the two sub-buffers in the main buffer.
    _caUnsigned eb_ns_start;
    _caUnsigned eb_we_start;
};


//! This is the shorted version of the _caGrid. It Contains only the
//! esential information about the grid which are used by the CA
//! functions CA functions.  \attention position (0,0) is top left
//! corner.
struct _caGrid_short
{
    //! The lenght of an edge of the cell in the X/Y dimension.
    _caReal length;

    //! The area of a cell;
    _caReal area;

    //! The x coordinate of the bottom-left corner
    _caReal x_coo_top;

    //! The y coordinate of the top-left corner
    _caReal y_coo_top;

    //! The left x position to the top left corner of the
    //! rectangular box.
    _caUnsigned bx_lx;

    //! The top y position to the top left corner of the
    //! rectangular box.
    _caUnsigned bx_ty;

    //! The right x position to the bottom righ corner of the
    //! rectangular box.
    _caUnsigned bx_rx;

    //! The bottom y position to the bottom righ corner of the
    //! rectangular box.
    _caUnsigned bx_by;

    //! The offset in the X dimension of a cell buffer. 
    _caUnsigned cb_x_offset;

    //! The stride in the X dimension of a cell buffer. It represent the
    //! alligned X dimension which is used to improve the performance on
    //! GPU. \attention This is the X dimenesnison that the buffer is
    //! allocated.
    _caUnsigned cb_stride;

    //! The offset in the X dimension of the west/east sub-buffers of
    //! the edge buffer + starting point of the we buffer.
    _caUnsigned eb_we_offset;

    //! The stride in the X dimension of north/south west/east
    //! sub-buffers of a edge buffer. It represent the alligned X
    //! dimension which is used to improve the performance on
    //! GPU. \attention This is the X dimenesnison that the buffer is
    //! allocated.
    _caUnsigned eb_ns_stride;
    _caUnsigned eb_we_stride;

    /*
    //! This are the offset of the index for the cell buffers.
    //! They are precalculated.
    _caUnsigned cb_offset[5];
    */

    //! This contains the index of the central cell in the "CellBuffers"
    //! considerd as 1D buffers. This value should change in each
    //! thread.
    _caUnsigned cb_index;
};


inline void initGridShort(const _caGrid& _cagrid, _caGrid_short& _cagrid_short)
{
    _cagrid_short.length = _cagrid.length;
    _cagrid_short.area = _cagrid.area;
    _cagrid_short.x_coo_top = _cagrid.x_coo;
    _cagrid_short.y_coo_top = _cagrid.y_coo + _cagrid.y_size*_cagrid.length;
    _cagrid_short.bx_lx = _cagrid.bx_lx;
    _cagrid_short.bx_ty = _cagrid.bx_ty;
    _cagrid_short.bx_rx = _cagrid.bx_rx;
    _cagrid_short.bx_by = _cagrid.bx_by;
    _cagrid_short.cb_x_offset = _cagrid.cb_x_offset;
    _cagrid_short.cb_stride = _cagrid.cb_stride;
    _cagrid_short.eb_we_offset = _cagrid.eb_we_x_offset + _cagrid.eb_we_start;
    _cagrid_short.eb_ns_stride = _cagrid.eb_ns_stride;
    _cagrid_short.eb_we_stride = _cagrid.eb_we_stride;

    /*
    _cagrid_short.cb_offset[0]   = 0;
    _cagrid_short.cb_offset[1]   = +1;
    _cagrid_short.cb_offset[2]   = -_cagrid.cb_stride;
    _cagrid_short.cb_offset[3]   = -1;
    _cagrid_short.cb_offset[4]   = +_cagrid.cb_stride;
    */

    _cagrid_short.cb_index = 0;
}


//////////////////////////////////////////////////////////////////////
//
// End shared
// 
//////////////////////////////////////////////////////////////////////


// ---- UTILITY FUNCTIONS ----

// Compute an alligned stride given a buffer size.
inline _caUnsigned computeStride(_caUnsigned size, _caUnsigned alignment = 32)
{
    return alignment * ((size + alignment - 1) / alignment);
}

//! Write the value to store in the selected bits of the given buffer
//! value. The given buffer value with the bits set is retunerd by the
//! method.
inline _caState caWriteBitsState(_caState value, _caState buffer, int start, int stop)
{
    int mask = 0;
    int bits = 0;
    int b = 0;
    int s = 0;

    for (b = 0, s = start; s < stop; b++, s++)
    {
        mask |= (1 << s);
        bits |= (((1 << b)&value) >> b) << s;
        buffer = (buffer&(~mask)) ^ bits;
    }
    return buffer;
}


#endif  // _CAAPI2D_HPP_
