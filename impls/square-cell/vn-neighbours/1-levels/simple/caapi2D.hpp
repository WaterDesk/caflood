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

//! \file caapi2D.h caAPI 2D simple implementation with a lattice that
//! is structured, regular, with square cells and a Von Neumann
//! neighbourhood of 1 level. \attention This file contains also the
//! code for the Moore neighbourhood using the CA2D_MOORE macro.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-07


#include"Common.hpp"


#include<cstddef>
#include<cstdlib>
#include<vector>
#include<utility>
#include<cmath>
#include<iostream>


#if !defined __cplusplus
#error This CAAPI 2D implementation requires a C++ compiler
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4305)
#endif // _MSC_VER

// Problem with some C99 math functions in Visual Studio.
#if _MSC_VER <= 1600 && !defined _CA_MISSING_ROUND_
#define _CA_MISSING_ROUND_
inline double round(double d)
{
    return floor(d + 0.5);
}
inline float roundf(float d)
{
    return floorf(d + 0.5);
}
inline double trunc(double d)
{
    return (d > 0) ? floor(d) : ceil(d);
}
inline float truncf(float d)
{
    return (d > 0) ? floor(d) : ceil(d);
}
#endif

//////////////////////////////////////////////////////////////////////
//
// The types, classes and methods with a "_ca" prefix are internal by
// the CA Function and should not be used outside of it.
//
// The types,  classes and methods with a "ca" prefix are public. 
//
//////////////////////////////////////////////////////////////////////



// ********************************************* IMPLEMENTATION CODE ******************************************


// ---- TYPES ----


//! Define the type of real values.
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
typedef float          _caReal;
#elif CA_REAL_PRECISION == CA_REAL_DOUBLE
typedef double         _caReal;
#else
#error Undefined Real precision 
#endif


//! Define the type of state values.
typedef int           _caState;


//! Define the type of a size value.
typedef std::size_t   _caUnsigned;


//! Define the type of a integer value.
typedef int           _caInt;


//! Define the values of the ca grid which are used by the vairous
//! functions.
//! \attention position (0,0) is top left corner.
struct _caGrid
{
    //! The X index of the main cell.
    _caUnsigned main_x;

    //! The Y index of the main cell.
    _caUnsigned main_y;

    //! The size in the X dimension of the grid (num cells).
    _caUnsigned x_size;

    //! The size in the Y dimension of the grid (num cells).
    _caUnsigned y_size;

    //! The lenght of an edge of the cell in the X/Y dimension.
    _caReal length;

    //! The distance between the centroid of the main cell (number 0)
    //! and the centroid of the given cell in the vertical and
    //! horizontal plane.
    _caReal distance;

#ifdef CA2D_MOORE
    //! The distance between the centroid of the main cell (number 0)
    //! and the centroid of the given cell in the diagonal plane.
    _caReal distance_diag;
#endif

    //! The x coordinate of the bottom-left corner
    _caReal x_coo;

    //! The y coordinate of the bottom-left corner
    _caReal y_coo;

    //! The y coordinate of the top-left corner
    _caReal y_coo_top;

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

    //! Since the main buffer is divided in two sub buffers, this the
    //! the starting point of the two sub-buffers in the main buffer.
    _caUnsigned eb_ns_start;
    _caUnsigned eb_we_start;

#ifdef CA2D_MOORE
    //! The size of the diagonal sub-buffer of an edge buffer on
    //! the X and Y dimensions with the border edges.
    _caUnsigned eb_diag_x_size;
    _caUnsigned eb_diag_y_size;

    //! Border size in the Y direction for the diagonal sub-buffer of an
    //! edge buffer. It depends on the number of neighbours's levels
    //! chosen.
    _caUnsigned eb_diag_y_border;

    //! Border size in the X direction for the diagonal sub-buffer of
    //! an edge buffer. It depends on the number of neighbours's levels
    //! chosen.
    _caUnsigned eb_diag_x_border;

    //! Since the main buffer is divided in four sub buffers in the cse
    //! of Moore neighbourhood, this the the starting point of the two
    //! extra sub-buffers in the main buffer.
    _caUnsigned eb_nwse_start;
    _caUnsigned eb_nesw_start;

#endif

    //! If true, print works.
    bool print;
};

// ---- GLOBAL VARIABLES  ----

//! PI Value
const _caReal PI = 3.141592654;

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


#ifdef CA2D_MOORE

#define CA2D_NEIGHBOURS 8

//! The total number of cells in the neighbourhood, i.e. the
//! number of neighbours. 
const int caNeighbours = 8;

//! Number of edges of a cell. 
const int caEdges = 8;

//! Number of neighbours' levels in the neighbourhood. 
const int caLevels = 1;

//! Maximum number of neighbours in a level
const int caMaxNeighboursLevel = 8;

#else // CA2D_VN

#define CA2D_NEIGHBOURS 4

//! The total number of cells in the neighbourhood, i.e. the
//! number of neighbours. 
const int caNeighbours = 4;

//! Number of edges of a cell. 
const int caEdges = 4;

//! Number of neighbours' levels in the neighbourhood. 
const int caLevels = 1;

//! Maximum number of neighbours in a level
const int caMaxNeighboursLevel = 4;

#endif

// ---- CA FUNCTION DECLARATION METHODS ----


//! \def CA_FUNCTION
//! Define a CA function. This is a prefix that is added before the
//! function name.
#ifdef  CA_FUCNTION
#undef  CA_FUNCTION
#endif
#define CA_FUNCTION inline void

//! Define the type of the grid variable in the CA function. 
typedef const struct _caGrid&     CA_GRID;

//! Define the type of the read only buffer with a real value in each cell.
typedef const _caReal*      CA_CELLBUFF_REAL_I;

//! Define the type of the read/write buffer with a real value in each cell.
typedef _caReal*            CA_CELLBUFF_REAL_IO;

//! Define the type of the read only buffer with a state value in each cell.
typedef const _caState*     CA_CELLBUFF_STATE_I;

//! Define the type of the read/write buffer with a state value in each cell.
typedef _caState*           CA_CELLBUFF_STATE_IO;

//! Define the type of the read only buffer with a real value in each edge.
typedef const _caReal*      CA_EDGEBUFF_REAL_I;

//! Define the type of the read/write buffer with a real value in each edge.
typedef _caReal*            CA_EDGEBUFF_REAL_IO;

//! Define the type of the read only buffer with a state value in each edge.
typedef const _caState*     CA_EDGEBUFF_STATE_I;

//! Define the type of the read/write buffer with a state value in each edge.
typedef  _caState*          CA_EDGEBUFF_STATE_IO;

//! Define the type of a constant global variable with a real value.
typedef const _caReal       CA_GLOB_REAL_I;

//! Define the type of a constant global variable with a state value.
typedef const _caState      CA_GLOB_STATE_I;

//! Define the type of a set of global alarms. Alarms can be activated
//! inside a CA function. However, they cannot be deactivate or
//! checked their status inside a CA function..
typedef char*               CA_ALARMS_O;

//! Define the type of a table with real data.
typedef const  _caReal*     CA_TABLE_REAL_I;

//! Define the type of a table with state data.
typedef const  _caState*    CA_TABLE_STATE_I;

// ---- CA FUNCTION BODY  METHODS ----

//! Define the type of a real value.
typedef _caReal   CA_REAL;

//! Define the type of a state value.
typedef _caState  CA_STATE;

//! \def CA_GRID_INIT
//! Intialise the grid structure.
#ifdef  CA_GRID_INIT
#undef  CA_GRID_INIT
#endif
#define CA_GRID_INIT(grid)  


//! \def CA_ARRAY_CREATE
//! Create a ca array of specific type, name and size.
#ifdef  CA_ARRAY_CREATE
#undef  CA_ARRAY_CREATE
#endif
#define CA_ARRAY_CREATE(grid,type,name,size)  type name[size]


// The indexing in the von-Neumann and Moore neighbourhood are different.
//
//  NH - moore
// 4 3 2
// 5 0 1
// 6 7 8
//
// NH - Von Neumann
//   2
// 3 0 1
//   4
//
//

//! Return the X coordinate of the centroid of the cell.
inline _caReal caXCoo(CA_GRID grid, int cell_number)
{
    _caUnsigned x = grid.main_x;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0:               break;
    case 1: x += 1;        break;
    case 2: x += 1;        break;
    case 3:               break;
    case 4: x -= 1;       break;
    case 5: x -= 1;        break;
    case 6: x -= 1;        break;
    case 7:               break;
    case 8: x += 1;        break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0:        break;
    case 1: x += 1; break;
    case 2:        break;
    case 3: x -= 1; break;
    case 4:        break;
    }
#endif

    return  grid.x_coo + (x + 0.5)* grid.length;
}


//! Return the Y coordinate of the centroid of the cell.
inline _caReal caYCoo(CA_GRID grid, int cell_number)
{
    _caUnsigned y = grid.main_y;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0:               break;
    case 1:               break;
    case 2:        y -= 1; break;
    case 3: y -= 1;        break;
    case 4:        y -= 1; break;
    case 5:               break;
    case 6:        y += 1; break;
    case 7: y += 1;        break;
    case 8:        y += 1; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0:        break;
    case 1:        break;
    case 2: y -= 1; break;
    case 3:        break;
    case 4: y += 1; break;
    }
#endif

    return  grid.y_coo_top - (y + 0.5)* grid.length;
}


//! Set in the given array, the X coordinate of the centroid all visible cells;
inline void caXCooCellArray(CA_GRID grid, _caReal xcoos[])
{
    xcoos[0] = caXCoo(grid, 0);
    xcoos[1] = caXCoo(grid, 1);
    xcoos[2] = caXCoo(grid, 2);
    xcoos[3] = caXCoo(grid, 3);
    xcoos[4] = caXCoo(grid, 4);

#ifdef CA2D_MOORE
    xcoos[5] = caXCoo(grid, 5);
    xcoos[6] = caXCoo(grid, 6);
    xcoos[7] = caXCoo(grid, 7);
    xcoos[8] = caXCoo(grid, 8);
#endif
}


//! Set in the given array, the Y coordinate of the centroid all visible cells;
inline void caYCooCellArray(CA_GRID grid, _caReal ycoos[])
{
    ycoos[0] = caYCoo(grid, 0);
    ycoos[1] = caYCoo(grid, 1);
    ycoos[2] = caYCoo(grid, 2);
    ycoos[3] = caYCoo(grid, 3);
    ycoos[4] = caYCoo(grid, 4);

#ifdef CA2D_MOORE
    ycoos[5] = caYCoo(grid, 5);
    ycoos[6] = caYCoo(grid, 6);
    ycoos[7] = caYCoo(grid, 7);
    ycoos[8] = caYCoo(grid, 8);
#endif
}


//! Return the area of the given cell given the cell number.
inline _caReal caArea(CA_GRID grid, int cell_number)
{
    return grid.area;
}


//! Set in the given array, the area of all visible cells;
inline void caAreaCellArray(CA_GRID grid, _caReal areas[])
{
    areas[0] = grid.area;
    areas[1] = grid.area;
    areas[2] = grid.area;
    areas[3] = grid.area;
    areas[4] = grid.area;

#ifdef CA2D_MOORE
    // Adding the extra cells from the moores NH
    areas[5] = grid.area;
    areas[6] = grid.area;
    areas[7] = grid.area;
    areas[8] = grid.area;
#endif
}


//! Return the angle (radian) between the centroid of the main
//! cell (number 0) and the centroid of the given cell_number. The
//! base line is the first coordinate axis (x-axis) in the
//! Cartesian plane and the angle is measure anticlockwise.
inline _caReal caAngle(CA_GRID grid, int cell_number)
{
#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0: return 0;
    case 1: return 0;
    case 2: return PI / 4;
    case 3: return PI / 2;
    case 4: return 3 * PI / 4;
    case 5: return PI;
    case 6: return 5 * PI / 4;
    case 7: return 3 * PI / 2;
    case 8: return 7 * PI / 4;
    }
#else // CA2D_VN
    switch (cell_number)
    {
    case 0: return 0;
    case 1: return 0;
    case 2: return PI / 2;
    case 3: return PI;
    case 4: return 3 * PI / 2;
    }
#endif

    return 0;
}


//! Set in the given array, the angle (radian) between the centroid of the main
//! cell (number 0) and the centroid for all visible cells.
inline void caAngleCellArray(CA_GRID grid, _caReal angles[])
{
#ifdef CA2D_MOORE
    angles[0] = 0;
    angles[1] = 0;
    angles[2] = PI / 4;
    angles[3] = PI / 2;
    angles[4] = 3 * PI / 4;
    angles[5] = PI;
    angles[6] = 5 * PI / 4;
    angles[7] = 3 * PI / 2;
    angles[8] = 7 * PI / 4;
#else // CA2D_VN
    angles[0] = 0;
    angles[1] = 0;
    angles[2] = PI / 2;
    angles[3] = PI;
    angles[4] = 3 * PI / 2;
#endif
}


//! Return the distance between the centroid of the main
//! cell (number 0) and the centroid of the given cell_number. 
inline _caReal caDistance(CA_GRID grid, int cell_number)
{
#ifdef CA2D_MOORE
    // if cell index is zero (i.e. distance from self, is zero.
    // else if the cell index is odd then its a cell length.
    // otherwise sqrt(a^2 + b^2) 
    return (cell_number == 0) ? 0.0 : ((cell_number % 2 == 1) ? grid.distance : grid.distance_diag);
#else // CA2D_VN
    return static_cast<_caReal>((cell_number == 0) ? 0.0 : grid.distance);
#endif
}


//! Set in the given array, the  distance between the centroid of the main
//! cell (number 0) and the centroid for all visible cells.
inline void caDistanceCellArray(CA_GRID grid, _caReal distances[])
{
#ifdef CA2D_MOORE
    distances[0] = 0;
    distances[1] = grid.distance;
    distances[2] = grid.distance_diag;
    distances[3] = grid.distance;
    distances[4] = grid.distance_diag;
    distances[5] = grid.distance;
    distances[6] = grid.distance_diag;
    distances[7] = grid.distance;
    distances[8] = grid.distance_diag;
#else // CA2D_VN
    distances[0] = 0;
    distances[1] = grid.distance;
    distances[2] = grid.distance;
    distances[3] = grid.distance;
    distances[4] = grid.distance;
#endif
}


//! Return the lenght of the given edge of the given cell.
inline _caReal caLength(CA_GRID grid, int cell_number, int edge_number)
{
    // ATTENTION.
    // At the moment the lenght of vertex edge in the moore
    // neighbourhood is the same one of the normal edge.
    return grid.length;
}


//! Set the given array with the lenght of all the edges of the given cell.
inline void caLengthEdgeArray(CA_GRID grid, int cell_number, _caReal lengths[])
{
    lengths[0] = grid.length;
    lengths[1] = grid.length;
    lengths[2] = grid.length;
    lengths[3] = grid.length;
    lengths[4] = grid.length;
#ifdef CA2D_MOORE
    // ATTENTION.
    // At the moment the lenght of vertex edge in the moore
    // neighbourhood is the same one of the normal edge.
    lengths[5] = grid.length;
    lengths[6] = grid.length;
    lengths[7] = grid.length;
    lengths[8] = grid.length;
#endif
}


//! Return the number of cells in a neighbours' level. This is
//! used to number all the cells in the particular level. This is
//! the cell_level_number. 
inline int caCellsLevel(CA_GRID grid, int level_number)
{
#ifdef CA2D_MOORE
    return 8;
#else // CA2D_VN
    return 4;
#endif
}


//! Return the correspondent cell_number given the specific
//! cell_level_number.
inline int caNumberLevel(CA_GRID grid, int cell_level_number, int level_number)
{
    return cell_level_number;
}


//! Return the maximum edge_number which can be updated in a EdgeBuffer
//! without overwriting the values of the edges.
inline int caUpdateEdges(CA_GRID grid)
{
#ifdef CA2D_MOORE
    return 4;
#else // CA2D_VN
    return 2;
#endif
}


//! If the given cell_number share and edge with the main cell, it
//! returns the edge_number of the main cell, otherwise it returns 0.
inline int caShareEdge(CA_GRID grid, int cell_number)
{
    return cell_number;
}


//! Set the given array with the number of the common edges between
//! the main cell and all the visible cells.
inline void caShareEdgeCellArray(CA_GRID grid, int edges[])
{
    edges[0] = 0;
    edges[1] = 1;
    edges[2] = 2;
    edges[3] = 3;
    edges[4] = 4;
#ifdef CA2D_MOORE
    edges[5] = 5;
    edges[6] = 6;
    edges[7] = 7;
    edges[8] = 8;
#endif
}


// ---- UTILITIES  METHODS ----


//! Box border management, it set the bits:
//! Bit 0, True if the central cell is  in the border of the box, one neighbour cell is outside the box.
//! Bit 1, True if the neighbour cell 1 is outside the box.
//! Bit 2, True if the neighbour cell 2 is outside the box.
//! ....
inline CA_STATE caBoxStatus(CA_GRID grid)
{
    CA_STATE mask = 0;
    mask |= (grid.main_x == grid.bx_rx - 1) << 1;   // Check neighbour cell 1.
    mask |= (grid.main_y == grid.bx_ty) << 2;       // Check neighbour cell 2.
    mask |= (grid.main_x == grid.bx_lx) << 3;       // Check neighbour cell 3.
    mask |= (grid.main_y == grid.bx_by - 1) << 4;   // Check neighbour cell 3.
    mask |= (mask > 0);                             // Check central cell.

    return mask;
}


//! Print a string
inline void caPrint(CA_GRID grid, const char* str)
{
    if (!grid.print) return;

    std::cout << str;
}


//! Print a real value.
inline void caPrintReal(CA_GRID grid, const char* pre, const char* post, _caReal value)
{
    if (!grid.print) return;

    std::cout << pre << value << post;
}

//! Print the real values of a ca array.
inline void caPrintRealArray(CA_GRID grid, const char* pre, const char* sep, const char* post,
    _caReal values[], int n)
{
    if (!grid.print) return;

    std::cout << pre;
    for (int k = 0; k < n; k++)
    {
        std::cout << values[k] << sep;
    }
    std::cout << post;
}


//! Print a state value.
inline void caPrintState(CA_GRID grid, const char* pre, const char* post, _caState value)
{
    if (!grid.print) return;

    std::cout << pre << value << post;
}

//! Print the state values of a ca array..
inline void caPrintStateArray(CA_GRID grid, const char* pre, const char* sep, const char* post,
    _caState values[], int n)
{
    if (!grid.print) return;

    std::cout << pre;
    for (int k = 0; k < n; k++)
    {
        std::cout << values[k] << sep;
    }
    std::cout << post;
}


// ---- MATHS  METHODS ----


//! Return the ABS value of the given value.
inline _caReal caAbsReal(_caReal value)
{
    return std::abs(value);
}


//! Set the ABS value of each elements in the array.
inline void caAbsRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = std::abs(values[i]);
    }
}


//! Return the minimum between two values
inline _caReal caMinReal(_caReal va, _caReal vb)
{
    return std::min(va, vb);
}


//! Return the minimum of the given array of values.
inline _caReal caMinRealArray(_caReal values[], int n)
{
    _caReal min = values[0];

    for (int i = 1; i < n; i++)
    {
        if (values[i] < min)
        {
            min = values[i];
        }
    }
    return min;
}


//! Return the maximum between two values
inline _caReal caMaxReal(_caReal va, _caReal vb)
{
    return std::max(va, vb);
}


//! Return the maximum of the given array of values.
inline _caReal caMaxRealArray(_caReal values[], int n)
{
    _caReal max = values[0];

    for (int i = 1; i < n; i++)
    {
        if (values[i] > max)
        {
            max = values[i];
        }
    }
    return max;
}


//! Return 1.0 for x>0.0, -1.0 for x<0.0 and 0 for x=0.0;
inline _caReal caSignReal(_caReal x)
{
    if (x > 0.0) return 1.0;
    if (x < 0.0) return -1.0;
    return 0.0;
}


//! Set the sign value of each elements in the array, i.e. 1.0 for
//! x>0.0, -1.0 for x<0.0 and 0 for x=0.0;
inline void caSignRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        if (values[i] > 0.0) values[i] = 1.0;
        if (values[i] < 0.0) values[i] = -1.0;
        values[i] = 0.0;
    }
}


//! Return 0.0 for x<edge, else 1.0
inline _caReal caStepReal(_caReal x, _caReal edge)
{
    return static_cast<_caReal>((x < edge) ? 0.0 : 1.0);
}


//! Set the the value of each element of the array to 0.0 is the
//! original value is < of edge, else 1.0
inline void caStepRealArray(_caReal values[], _caReal edge, int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = static_cast<_caReal>((values[i] < edge) ? 0.0 : 1.0);
    }
}


//! Compute the power function
inline _caReal caPowReal(_caReal x, _caReal y)
{
    return std::pow(x, y);
}


//! Compute the power of each element in the array for the given exponent.
inline void caPowRealArray(_caReal values[], _caReal y, int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = std::pow(values[i], y);
    }
}


//! Compute the square function
inline _caReal caSqrtReal(_caReal x)
{
    return sqrt(x);
}


//! Compute the square of each element in the array.
inline void caSqrtRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = sqrt(values[i]);
    }
}


//! Compute the sin function in radians
inline _caReal caSinReal(_caReal x)
{
    return sin(x);
}


//! Compute the sin in radians of each element in the array
inline void caSinRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = sin(values[i]);
    }
}

//! Compute the cos function in radians
inline _caReal caCosReal(_caReal x)
{
    return cos(x);
}


//! Compute the cos in radians of each element in the array
inline void caCosRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = cos(values[i]);
    }
}

//! Compute the atan function in radians
inline _caReal caAtanReal(_caReal x)
{
    return atan(x);
}


//! Compute the atan in radians of each element in the array
inline void caAtanRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        values[i] = atan(values[i]);
    }
}

//! Compute the atan2 function in radians
inline _caReal caAtan2Real(_caReal y, _caReal x)
{
    return atan2(y, x);
}


//! Compute the atan2 in radians of each element in the array. The
//! results are stored in the first array.
inline void caAtan2RealArray(_caReal ys[], _caReal xs[], int n)
{
    for (int i = 0; i < n; i++)
    {
        ys[i] = atan2(ys[i], xs[i]);
    }
}


// Round to integer toward zero
inline _caReal caTruncReal(_caReal x)
{
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
    return truncf(x);
#else
    return trunc(x);
#endif
}


// Round to integer toward zero for each element in the array.
inline void caTruncRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
        values[i] = truncf(values[i]);
#else
        values[i] = trunc(values[i]);
#endif
    }
}


// Round to nearest integer away from zero
inline _caReal caRoundReal(_caReal x)
{
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
    return roundf(x);
#else
    return round(x);
#endif
}


// Round to nearest integer away from zero for each element in the array.
inline void caRoundRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
        values[i] = roundf(values[i]);
#else
        values[i] = round(values[i]);
#endif
    }
}


// Largest integral value not greater than argument
inline _caReal caFloorReal(_caReal x)
{
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
    return floorf(x);
#else
    return floor(x);
#endif
}


// Largest integral value not greater than argument for each element in the array.
inline void caFloorRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
        values[i] = floorf(values[i]);
#else
        values[i] = floor(values[i]);
#endif
    }
}


// Smallest integral value not less than argument
inline _caReal caCeilReal(_caReal x)
{
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
    return ceilf(x);
#else
    return ceil(x);
#endif
}


// Smallest integral value not less than argumen for each element in the array.
inline void caCeilRealArray(_caReal values[], int n)
{
    for (int i = 0; i < n; i++)
    {
#if     CA_REAL_PRECISION == CA_REAL_FLOAT
        values[i] = ceilf(values[i]);
#else
        values[i] = ceil(values[i]);
#endif
    }
}


//! Read the value stored in the selected bits from the given value.
inline _caState caReadBitsState(_caState value, int start, int stop)
{
    _caState ret = 0;

    int b = 0;
    int s = 0;

    for (b = 0, s = start; s < stop; b++, s++)
    {
        ret |= (value & (1 << s)) >> (s - b);
    }

    return ret;
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

// ---- SPECIALISED FUNCTIONS ----


//! Return the velocity computing Manning Equation and the Critical Velocity formula.
//! \param irough  the inverse of the roughness.
//! \param S the slope (delwl/dist)
//! \param R the radious
//! \param D the water depth.
//! \retunr the velocity
inline _caReal caFlowVelocity(_caReal irough, _caReal S, _caReal R, _caReal D)
{
    return caMinReal(caSqrtReal(9.81*D), irough*caPowReal(R, (2.0 / 3.0))*caSqrtReal(S));
}

// ---- CA FUNCTION BUFFERS  AND VARIABLE METHODS ----


// ---- ALARMS ----


//! Activate the specific alarm from the set of alarms.
inline void caActivateAlarm(CA_GRID grid, CA_ALARMS_O alarms, int n)
{
    alarms[n] = 1;
}

//! Activate all the alarms that have the bit set to true in the given
//! mask.
inline void caActivateAlarmMask(CA_GRID grid, CA_ALARMS_O alarms, CA_STATE mask, int num)
{
    for (int i = 0; i < num; i++)
        if (((mask >> i) & 1) == 1) alarms[i] = 1;
}


// ---- CELL BUFFERS ----


//! Read the real value of the cell from the given buffer at the given cell number.
inline _caReal caReadCellBuffReal(CA_GRID grid, CA_CELLBUFF_REAL_I src, int cell_number)
{
    _caUnsigned x_size = grid.cb_x_size;
    _caUnsigned i = (grid.main_y + grid.cb_border) * x_size + (grid.main_x + grid.cb_border);

    _caReal value = 0.0;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0: value = src[i]; break; // found missing break here....
    case 1: value = src[i + 1]; break;
    case 2: value = src[i - x_size + 1]; break;
    case 3: value = src[i - x_size]; break;
    case 4: value = src[i - x_size - 1]; break;
    case 5: value = src[i - 1]; break;
    case 6: value = src[i + x_size - 1]; break;
    case 7: value = src[i + x_size]; break;
    case 8: value = src[i + x_size + 1]; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0: value = src[i]; break;
    case 1: value = src[i + 1]; break;
    case 2: value = src[i - x_size]; break;
    case 3: value = src[i - 1]; break;
    case 4: value = src[i + x_size]; break;
    }
#endif
    return value;
}


//! Set the given ca array with the real values of all the visible
//! cells from the given buffer.
inline void  caReadCellBuffRealCellArray(CA_GRID grid, CA_CELLBUFF_REAL_I src, _caReal values[])
{
    _caUnsigned x_size = grid.cb_x_size;

    _caUnsigned i = (grid.main_y + grid.cb_border) * x_size + (grid.main_x + grid.cb_border);

#ifdef CA2D_MOORE

    values[0] = src[i];
    values[1] = src[i + 1];
    values[2] = src[i - x_size + 1];
    values[3] = src[i - x_size];
    values[4] = src[i - x_size - 1];
    values[5] = src[i - 1];
    values[6] = src[i + x_size - 1];
    values[7] = src[i + x_size];
    values[8] = src[i + x_size + 1];

#else // CA2D_VN

    values[0] = src[i];
    values[1] = src[i + 1];
    values[2] = src[i - x_size];
    values[3] = src[i - 1];
    values[4] = src[i + x_size];

#endif
}


//! Write the given real value of the cell into the given buffer at the main cell index.
inline void caWriteCellBuffReal(CA_GRID grid, CA_CELLBUFF_REAL_IO dst, _caReal value)
{
    dst[(grid.main_y + grid.cb_border) * grid.cb_x_size + (grid.main_x + grid.cb_border)] = value;
}


//! Read the state value of the cell from the given buffer at the given cell index.
inline _caState caReadCellBuffState(CA_GRID grid, CA_CELLBUFF_STATE_I src, int cell_number)
{
    _caUnsigned x_size = grid.cb_x_size;
    _caUnsigned i = (grid.main_y + grid.cb_border) * x_size + (grid.main_x + grid.cb_border);

    _caState value = 0;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0: value = src[i]; break;
    case 1: value = src[i + 1]; break;
    case 2: value = src[i - x_size + 1]; break;
    case 3: value = src[i - x_size]; break;
    case 4: value = src[i - x_size - 1]; break;
    case 5: value = src[i - 1]; break;
    case 6: value = src[i + x_size - 1]; break;
    case 7: value = src[i + x_size]; break;
    case 8: value = src[i + x_size + 1]; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0: value = src[i]; break;
    case 1: value = src[i + 1]; break;
    case 2: value = src[i - x_size]; break;
    case 3: value = src[i - 1]; break;
    case 4: value = src[i + x_size]; break;
    }
#endif
    return value;
}


//! Set the given ca array with the state values of all the visible
//! cells from the given buffer.
inline void  caReadCellBuffStateCellArray(CA_GRID grid, CA_CELLBUFF_STATE_I src, _caState values[])
{
    _caUnsigned x_size = grid.cb_x_size;

    _caUnsigned i = (grid.main_y + grid.cb_border) * x_size + (grid.main_x + grid.cb_border);

#ifdef CA2D_MOORE

    values[0] = src[i];
    values[1] = src[i + 1];
    values[2] = src[i - x_size + 1];
    values[3] = src[i - x_size];
    values[4] = src[i - x_size - 1];
    values[5] = src[i - 1];
    values[6] = src[i + x_size - 1];
    values[7] = src[i + x_size];
    values[8] = src[i + x_size + 1];

#else // CA2D_VN

    values[0] = src[i];
    values[1] = src[i + 1];
    values[2] = src[i - x_size];
    values[3] = src[i - 1];
    values[4] = src[i + x_size];

#endif
}


//! Write the given state value of the cell into the given buffer at the main cell index.
inline void caWriteCellBuffState(CA_GRID grid, CA_CELLBUFF_STATE_IO dst, _caState value)
{
    dst[(grid.main_y + grid.cb_border) * grid.cb_x_size + (grid.main_x + grid.cb_border)] = value;
}


// ---- EDGE BUFFERS ----

//! Read the real value of the edge from the given buffer at the given
//! cell index and given edge number.
inline _caReal caReadEdgeBuffReal(CA_GRID grid, CA_EDGEBUFF_REAL_I src,
    int cell_number, int edge_number)
{
    _caReal value = 0.0;

    _caUnsigned x = grid.main_x;
    _caUnsigned y = grid.main_y;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0:               break;
    case 1: x += 1;        break;
    case 2: x += 1; y -= 1; break;
    case 3: y -= 1;        break;
    case 4: x -= 1; y -= 1; break;
    case 5: x -= 1;        break;
    case 6: x -= 1; y += 1; break;
    case 7: y += 1;        break;
    case 8: x += 1; y += 1; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0:        break;
    case 1: x += 1; break;
    case 2: y -= 1; break;
    case 3: x -= 1; break;
    case 4: y += 1; break;
    }
#endif

    _caUnsigned i_ns = (y + grid.eb_ns_y_border) * grid.eb_ns_x_size
        + x + grid.eb_ns_start;

    _caUnsigned i_we = (y)* grid.eb_we_x_size
        + x + grid.eb_we_x_border + grid.eb_we_start;

#ifdef CA2D_MOORE

    _caUnsigned i_nwse = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nwse_start;

    _caUnsigned i_nesw = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nesw_start;

    switch (edge_number)
    {
    case 0: value = 0.0; break;
    case 1: value = src[i_we + 1]; break;
    case 2: value = src[i_nesw + 1]; break;
    case 3: value = src[i_ns]; break;
    case 4: value = src[i_nwse]; break;
    case 5: value = src[i_we]; break;
    case 6: value = src[i_nesw + grid.eb_diag_x_size]; break;
    case 7: value = src[i_ns + grid.eb_ns_x_size]; break;
    case 8: value = src[i_nwse + 1 + grid.eb_diag_x_size]; break;
    }
#else  // CA2D_VN
    switch (edge_number)
    {
    case 0: value = 0.0; break;
    case 1: value = src[i_we + 1]; break;
    case 2: value = src[i_ns]; break;
    case 3: value = src[i_we]; break;
    case 4: value = src[i_ns + grid.eb_ns_x_size]; break;
    }
#endif

    return value;
}


//! Set the given ca array with the real values of all the edges from
//! the given buffer at given cell index.
inline void  caReadEdgeBuffRealEdgeArray(CA_GRID grid, CA_EDGEBUFF_REAL_I src,
    int cell_number, _caReal values[])
{
    _caUnsigned x = grid.main_x;
    _caUnsigned y = grid.main_y;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0:               break;
    case 1: x += 1;        break;
    case 2: x += 1; y -= 1; break;
    case 3: y -= 1;        break;
    case 4: x -= 1; y -= 1; break;
    case 5: x -= 1;        break;
    case 6: x -= 1; y += 1; break;
    case 7: y += 1;        break;
    case 8: x += 1; y += 1; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0:        break;
    case 1: x += 1; break;
    case 2: y -= 1; break;
    case 3: x -= 1; break;
    case 4: y += 1; break;
    }
#endif

    _caUnsigned i_ns = (y + grid.eb_ns_y_border) * grid.eb_ns_x_size
        + x + grid.eb_ns_start;

    _caUnsigned i_we = (y)* grid.eb_we_x_size
        + x + grid.eb_we_x_border + grid.eb_we_start;

#ifdef CA2D_MOORE

    _caUnsigned i_nwse = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nwse_start;

    _caUnsigned i_nesw = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nesw_start;

    values[0] = 0.0;
    values[1] = src[i_we + 1];
    values[2] = src[i_nesw + 1];
    values[3] = src[i_ns];
    values[4] = src[i_nwse];
    values[5] = src[i_we];
    values[6] = src[i_nesw + grid.eb_diag_x_size];
    values[7] = src[i_ns + grid.eb_ns_x_size];
    values[8] = src[i_nwse + 1 + grid.eb_diag_x_size];

#else  // CA2D_VN

    values[0] = 0.0;
    values[1] = src[i_we + 1];
    values[2] = src[i_ns];
    values[3] = src[i_we];
    values[4] = src[i_ns + grid.eb_ns_x_size];

#endif
}


//! Write the given real value into the given buffer at the main cell
//! index and given edge number.
inline void caWriteEdgeBuffReal(CA_GRID grid, CA_EDGEBUFF_REAL_IO dst, int edge_number, _caReal value)
{
    _caUnsigned i_ns = (grid.main_y + grid.eb_ns_y_border) * grid.eb_ns_x_size
        + grid.main_x + grid.eb_ns_start;

    _caUnsigned i_we = (grid.main_y) * grid.eb_we_x_size
        + grid.main_x + grid.eb_we_x_border + grid.eb_we_start;

#ifdef CA2D_MOORE

    _caUnsigned i_nwse = (grid.main_y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (grid.main_x + grid.eb_diag_y_border) + grid.eb_nwse_start;

    _caUnsigned i_nesw = (grid.main_y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (grid.main_x + grid.eb_diag_y_border) + grid.eb_nesw_start;

    switch (edge_number)
    {
    case 0: break;
    case 1: dst[i_we + 1] = value; break;
    case 2: dst[i_nesw + 1] = value; break;
    case 3: dst[i_ns] = value; break;
    case 4: dst[i_nwse] = value; break;
    case 5: dst[i_we] = value; break;
    case 6: dst[i_nesw + grid.eb_diag_x_size] = value; break;
    case 7: dst[i_ns + grid.eb_ns_x_size] = value; break;
    case 8: dst[i_nwse + 1 + grid.eb_diag_x_size] = value; break;
    }

#else  // CA2D_VN

    switch (edge_number)
    {
    case 0: break;
    case 1: dst[i_we + 1] = value; break;
    case 2: dst[i_ns] = value; break;
    case 3: dst[i_we] = value; break;
    case 4: dst[i_ns + grid.eb_ns_x_size] = value; break;
    }

#endif
}


//! Read the state value of the edge from the given buffer at the given
//! cell index and given edge number.
inline _caState caReadEdgeBuffState(CA_GRID grid, CA_EDGEBUFF_STATE_I src,
    int cell_number, int edge_number)
{
    _caState value = 0;

    _caUnsigned x = grid.main_x;
    _caUnsigned y = grid.main_y;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0:               break;
    case 1: x += 1;        break;
    case 2: x += 1; y -= 1; break;
    case 3: y -= 1;        break;
    case 4: x -= 1; y -= 1; break;
    case 5: x -= 1;        break;
    case 6: x -= 1; y += 1; break;
    case 7: y += 1;        break;
    case 8: x += 1; y += 1; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0:        break;
    case 1: x += 1; break;
    case 2: y -= 1; break;
    case 3: x -= 1; break;
    case 4: y += 1; break;
    }
#endif

    _caUnsigned i_ns = (y + grid.eb_ns_y_border) * grid.eb_ns_x_size
        + x + grid.eb_ns_start;

    _caUnsigned i_we = (y)* grid.eb_we_x_size
        + x + grid.eb_we_x_border + grid.eb_we_start;

#ifdef CA2D_MOORE

    _caUnsigned i_nwse = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nwse_start;

    _caUnsigned i_nesw = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nesw_start;

    switch (edge_number)
    {
    case 0: value = 0.0; break;
    case 1: value = src[i_we + 1]; break;
    case 2: value = src[i_nesw + 1]; break;
    case 3: value = src[i_ns]; break;
    case 4: value = src[i_nwse]; break;
    case 5: value = src[i_we]; break;
    case 6: value = src[i_nesw + grid.eb_diag_x_size]; break;
    case 7: value = src[i_ns + grid.eb_ns_x_size]; break;
    case 8: value = src[i_nwse + 1 + grid.eb_diag_x_size]; break;
    }
#else  // CA2D_VN
    switch (edge_number)
    {
    case 0: value = 0.0; break;
    case 1: value = src[i_we + 1]; break;
    case 2: value = src[i_ns]; break;
    case 3: value = src[i_we]; break;
    case 4: value = src[i_ns + grid.eb_ns_x_size]; break;
    }
#endif

    return value;
}


//! Set the given ca array with the state values of all the edges from
//! the given buffer at given cell index.
inline void  caReadEdgeBuffStateEdgeArray(CA_GRID grid, CA_EDGEBUFF_STATE_I src,
    int cell_number, _caState values[])
{
    _caUnsigned x = grid.main_x;
    _caUnsigned y = grid.main_y;

#ifdef CA2D_MOORE
    switch (cell_number)
    {
    case 0:               break;
    case 1: x += 1;        break;
    case 2: x += 1; y -= 1; break;
    case 3: y -= 1;        break;
    case 4: x -= 1; y -= 1; break;
    case 5: x -= 1;        break;
    case 6: x -= 1; y += 1; break;
    case 7: y += 1;        break;
    case 8: x += 1; y += 1; break;
    }
#else //CA2D_VN
    switch (cell_number)
    {
    case 0:        break;
    case 1: x += 1; break;
    case 2: y -= 1; break;
    case 3: x -= 1; break;
    case 4: y += 1; break;
    }
#endif

    _caUnsigned i_ns = (y + grid.eb_ns_y_border) * grid.eb_ns_x_size
        + x + grid.eb_ns_start;

    _caUnsigned i_we = (y)* grid.eb_we_x_size
        + x + grid.eb_we_x_border + grid.eb_we_start;

#ifdef CA2D_MOORE

    _caUnsigned i_nwse = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nwse_start;

    _caUnsigned i_nesw = (y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (x + grid.eb_diag_y_border) + grid.eb_nesw_start;

    values[0] = 0.0;
    values[1] = src[i_we + 1];
    values[2] = src[i_nesw + 1];
    values[3] = src[i_ns];
    values[4] = src[i_nwse];
    values[5] = src[i_we];
    values[6] = src[i_nesw + grid.eb_diag_x_size];
    values[7] = src[i_ns + grid.eb_ns_x_size];
    values[8] = src[i_nwse + 1 + grid.eb_diag_x_size];

#else  // CA2D_VN

    values[0] = 0;
    values[1] = src[i_we + 1];
    values[2] = src[i_ns];
    values[3] = src[i_we];
    values[4] = src[i_ns + grid.eb_ns_x_size];

#endif
}


//! Write the given state value into the given buffer at the main cell
//! index and given edge number.
inline void caWriteEdgeBuffState(CA_GRID grid, CA_EDGEBUFF_STATE_IO dst, int edge_number, _caState value)
{
    _caUnsigned i_ns = (grid.main_y + grid.eb_ns_y_border) * grid.eb_ns_x_size
        + grid.main_x + grid.eb_ns_start;

    _caUnsigned i_we = (grid.main_y) * grid.eb_we_x_size
        + grid.main_x + grid.eb_we_x_border + grid.eb_we_start;

#ifdef CA2D_MOORE

    _caUnsigned i_nwse = (grid.main_y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (grid.main_x + grid.eb_diag_y_border) + grid.eb_nwse_start;

    _caUnsigned i_nesw = (grid.main_y + grid.eb_diag_y_border) * grid.eb_diag_x_size
        + (grid.main_x + grid.eb_diag_y_border) + grid.eb_nesw_start;

    switch (edge_number)
    {
    case 0: break;
    case 1: dst[i_we + 1] = value; break;
    case 2: dst[i_nesw + 1] = value; break;
    case 3: dst[i_ns] = value; break;
    case 4: dst[i_nwse] = value; break;
    case 5: dst[i_we] = value; break;
    case 6: dst[i_nesw + grid.eb_diag_x_size] = value; break;
    case 7: dst[i_ns + grid.eb_ns_x_size] = value; break;
    case 8: dst[i_nwse + 1 + grid.eb_diag_x_size] = value; break;
    }

#else  // CA2D_VN

    switch (edge_number)
    {
    case 0: break;
    case 1: dst[i_we + 1] = value; break;
    case 2: dst[i_ns] = value; break;
    case 3: dst[i_we] = value; break;
    case 4: dst[i_ns + grid.eb_ns_x_size] = value; break;
    }

#endif
}


// ---- TABLE ----


//! Read from the given real table data the values at specific position. 
//! \attention The position is not checked if it is out of range.
inline CA_REAL caReadTableReal(CA_GRID grid, CA_TABLE_REAL_I table, int i)
{
    return table[i];
}


//! Read from the given state table data the values at specific position. 
//! \attention The position is not checked if it is out of range.
inline CA_STATE caReadTableState(CA_GRID grid, CA_TABLE_STATE_I table, int i)
{
    return table[i];
}


#endif  // _CAAPI2D_HPP_
