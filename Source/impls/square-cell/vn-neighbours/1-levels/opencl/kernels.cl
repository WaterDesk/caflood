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

//! \file kernels.cl
//! Contains the utility/helper kernels which are
//! used internally by the Cell/Edge/Vertex buffers.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-12


//! Enum which identifies the possible operators that can be used on
//! setting a border value.
enum BdrOperator
{
    BdrEqual,
    BdrAdd,
    BdrSub,
    BdrMul,
    BdrDiv,
};


//! Enum which identifies the possible operators that can be used on
//! a sequential operation.
enum SeqOperator
{
    SeqAdd,
    SeqMul,
    SeqMin,
    SeqMinAbs,
    SeqMax,
    SeqMaxAbs
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// ATTENTION!!!
//
// This kernels will access the buffer in one of three way:
//
// 1) X = get_global_id(0), Y= get_global_id(1) represents the cells (X,Y) of a Grid buffer (no border)
//
// 2) X = get_global_id(0), Y= get_global_id(1) represents a location in a 2D buffer (with border)
//
// 3) X = get_global_id(0) represents a location in a 1D buffer (with border)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


//! Set the area of the given real one dimensional buffer to the given
//! real value. 
__kernel void setValueReal(__global _caReal* buff, _caUnsigned start, _caUnsigned stop, _caReal value)
{
    size_t tid = get_global_id(0);
    if (tid >= start && tid < stop)
        buff[tid] = value;
}


//! Set the area of the given state one dimensional buffer to the
//! given state value.
__kernel void setValueState(__global _caState* buff, _caUnsigned start, _caUnsigned stop, _caState value)
{
    size_t tid = get_global_id(0);
    if (tid >= start && tid < stop)
        buff[tid] = value;
}


//! Perform the given operation into the given NDRange region of the given 2D buffer.
__kernel void opValueReal(__global _caReal* buff, _caReal value, _caUnsigned x_stride, int op)
{
    // Get the buffer value 
    _caReal result = buff[get_global_id(1)*x_stride + get_global_id(0)];

    switch (op)
    {
    case BdrEqual:
        result = value;
        break;

    case BdrAdd:
        result += value;
        break;

    case BdrSub:
        result -= value;
        break;

    case BdrMul:
        result *= value;
        break;

    case BdrDiv:
        result /= value;
        break;
    }

    // Write the result into the 2D buffer
    buff[get_global_id(1)*x_stride + get_global_id(0)] = result;
}


//! Perform the given operation into the given NDRange region of the given 2D buffer.
__kernel void opValueState(__global _caState* buff, _caState value, _caUnsigned x_stride, int op)
{
    // Get the buffer value 
    _caState result = buff[get_global_id(1)*x_stride + get_global_id(0)];

    switch (op)
    {
    case BdrEqual:
        result = value;
        break;

    case BdrAdd:
        result += value;
        break;

    case BdrSub:
        result -= value;
        break;

    case BdrMul:
        result *= value;
        break;

    case BdrDiv:
        result /= value;
        break;
    }

    // Write the result into the 2D buffer
    buff[get_global_id(1)*x_stride + get_global_id(0)] = result;
}


//! Perform the given commutative reduction into the given NDRange
//! region of the given 1D buffer. 
__kernel void reduceReal(__global _caReal* dst, __global _caReal* src, __local _caReal* tmp,
    _caUnsigned x_offset, _caUnsigned x_size,
    _caUnsigned src_start, _caUnsigned src_stop, _caUnsigned src_stride, int op)
{
    // Get the neutral value for the given operation.
    _caReal neutral;

    switch (op)
    {
    case SeqAdd: neutral = 0.0;  break;
    case SeqMul: neutral = 1.0;  break;
    case SeqMin: neutral = INFINITY;  break;
    case SeqMinAbs: neutral = INFINITY;  break;
    case SeqMax: neutral = -INFINITY;  break;
    case SeqMaxAbs: neutral = 0;  break;
    }

    // Set the neutral value into the result as starting value.
    _caReal result = neutral;

    // Get the global position 
    _caUnsigned global_index = src_start + get_global_id(0);

    // Perform the operation sequentialy on the stride (Y dimension).
    while (global_index < src_stop)
    {
        // Get the lement from the buffer if it is inside the given
        // region, otherwise use the neutral value.
        _caReal element = neutral;

        if (get_global_id(0) < x_size)
            element = src[global_index + x_offset];

        // Perform the operation.
        switch (op)
        {
        case SeqAdd: result += element;  break;
        case SeqMul: result *= element;  break;
        case SeqMin: result = fmin(element, result);  break;
        case SeqMinAbs: result = fmin(fabs(element), result);  break;
        case SeqMax: result = fmax(element, result);  break;
        case SeqMaxAbs: result = fmax(fabs(element), result);  break;
        }

        // Add the stride.
        global_index += src_stride;
    }

    // Get the local position.  
    _caUnsigned local_index = get_local_id(0);

    // Save the previous result into the local position.
    tmp[local_index] = result;

    // Wait the other threads on the same work group.
    barrier(CLK_LOCAL_MEM_FENCE);

    // Perform parallel reduction X dimension.
    for (_caUnsigned offset = get_local_size(0) / 2; offset > 0; offset = offset / 2)
    {
        if (local_index < offset)
        {
            switch (op)
            {
            case SeqAdd: tmp[local_index] += tmp[local_index + offset]; break;
            case SeqMul: tmp[local_index] *= tmp[local_index + offset]; break;
            case SeqMin: tmp[local_index] = fmin(tmp[local_index + offset], tmp[local_index]);  break;
            case SeqMinAbs: tmp[local_index] = fmin(tmp[local_index + offset], tmp[local_index]);  break;
            case SeqMax: tmp[local_index] = fmax(tmp[local_index + offset], tmp[local_index]);  break;
            case SeqMaxAbs: tmp[local_index] = fmax(tmp[local_index + offset], tmp[local_index]);  break;
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (local_index == 0)
    {
        dst[get_group_id(0)] = tmp[0];
    }
}


//! Perform the given commutative reduction into the given NDRange
//! region of the given 1D buffer. 
__kernel void reduceState(__global _caState* dst, __global _caState* src, __local _caState* tmp,
    _caUnsigned x_offset, _caUnsigned x_size,
    _caUnsigned src_start, _caUnsigned src_stop, _caUnsigned src_stride, int op)
{
    // Get the neutral value for the given operation.
    _caState neutral;

    switch (op)
    {
    case SeqAdd: neutral = 0.0;  break;
    case SeqMul: neutral = 1.0;  break;
    case SeqMin: neutral = 2147483647;  break;
    case SeqMinAbs: neutral = 2147483647;  break;
    case SeqMax: neutral = -2147483648;  break;
    case SeqMaxAbs: neutral = 0;  break;
    }

    // Set the neutral value into the result as starting value.
    _caState result = neutral;

    // Get the global position 
    _caUnsigned global_index = src_start + get_global_id(0);

    // Perform the operation sequentialy on the stride (Y dimension).
    while (global_index < src_stop)
    {
        // Get the lement from the buffer if it is inside the given
        // region, otherwise use the neutral value.
        _caState element = neutral;

        if (get_global_id(0) < x_size)
            element = src[global_index + x_offset];

        // Perform the operation.
        switch (op)
        {
        case SeqAdd: result += element;  break;
        case SeqMul: result *= element;  break;
        case SeqMin: result = min(element, result);  break;
        case SeqMinAbs: result = min((_caState)abs(element), result);  break;
        case SeqMax: result = max(element, result);  break;
        case SeqMaxAbs: result = max((_caState)abs(element), result);  break;
        }

        // Add the stride.
        global_index += src_stride;
    }

    // Get the local position.  
    _caUnsigned local_index = get_local_id(0);

    // Save the previous result into the local position.
    tmp[local_index] = result;

    // Wait the other threads on the same work group.
    barrier(CLK_LOCAL_MEM_FENCE);

    // Perform parallel reduction X dimension.
    for (_caUnsigned offset = get_local_size(0) / 2; offset > 0; offset = offset / 2)
    {
        if (local_index < offset)
        {
            switch (op)
            {
            case SeqAdd: tmp[local_index] += tmp[local_index + offset]; break;
            case SeqMul: tmp[local_index] *= tmp[local_index + offset]; break;
            case SeqMin: tmp[local_index] = min(tmp[local_index + offset], tmp[local_index]);  break;
            case SeqMinAbs: tmp[local_index] = min(tmp[local_index + offset], tmp[local_index]);  break;
            case SeqMax: tmp[local_index] = max(tmp[local_index + offset], tmp[local_index]);  break;
            case SeqMaxAbs: tmp[local_index] = max(tmp[local_index + offset], tmp[local_index]);  break;
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (local_index == 0)
    {
        dst[get_group_id(0)] = tmp[0];
    }
}


//! Copy the given horizontal line of the real two dimensional buffer
//! into each horizontal line of the given NDRange area of the same
//! buffer. This can be used to shift borders values.
__kernel void copyHLineReal(__global _caReal* buff, _caUnsigned y, _caUnsigned x_stride)
{
    buff[get_global_id(1) * x_stride + get_global_id(0)] = buff[y * x_stride + get_global_id(0)];
}


//! Copy the given vertical line of the real two dimensional buffer
//! into each vertical line of the given NDRange area of the same
//! buffer. This can be used to shift borders values.
__kernel void copyVLineReal(__global _caReal* buff, _caUnsigned x, _caUnsigned x_stride)
{
    buff[get_global_id(1) * x_stride + get_global_id(0)] = buff[get_global_id(1) * x_stride + x];
}


//! Copy the given horizontal line of the state two dimensional buffer
//! into each horizontal line of the given NDRange area of the same
//! buffer. This can be used to shift borders values.
__kernel void copyHLineState(__global _caState* buff, _caUnsigned y, _caUnsigned x_stride)
{
    buff[get_global_id(1) * x_stride + get_global_id(0)] = buff[y * x_stride + get_global_id(0)];
}


//! Copy the given vertical line of the state two dimensional buffer
//! into each vertical line of the given NDRange area of the same
//! buffer. This can be used to shift borders values.
__kernel void copyVLineState(__global _caState* buff, _caUnsigned x, _caUnsigned x_stride)
{
    buff[get_global_id(1) * x_stride + get_global_id(0)] = buff[get_global_id(1) * x_stride + x];
}


//! Copy the value of the given location of the real two dimensional
//! buffer into the given NDRange area of the same buffer. This can be
//! used to shift corners
__kernel void copyPointReal(__global _caReal* buff, _caUnsigned x, _caUnsigned y, _caUnsigned x_stride)
{
    buff[get_global_id(1) * x_stride + get_global_id(0)] = buff[y * x_stride + x];
}


//! Copy the value of the given location of the state two dimensional
//! buffer into the given NDRange area of the same buffer. This can be used to
//! shift corners
__kernel void copyPointState(__global _caState* buff, _caUnsigned x, _caUnsigned y, _caUnsigned x_stride)
{
    buff[get_global_id(1) * x_stride + get_global_id(0)] = buff[y * x_stride + x];
}


//! Copy the real cell buffer into the 2D buffer.
CA_FUNCTION copyCellBuffRealTo2DBuff(CA_GRID grid, CA_CELLBUFF_REAL_IO src,
    __global _caReal* dst, _caUnsigned dst_x_size, _caUnsigned dst_y_size)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position in the 2D buffer considering that get_global
    // could have an offset.
    _caUnsigned x = get_global_id(0) - grid.bx_lx;
    _caUnsigned y = get_global_id(1) - grid.bx_ty;

    // Check that the global_id is inside the 2D buffer.
    if (y >= dst_y_size || x >= dst_x_size)
        return;

    // Get the main cell value
    CA_REAL  value = caReadCellBuffReal(grid, src, 0);

    // Write the value into the 2D buffer
    dst[y*dst_x_size + x] = value;
}


//! Copy the state cell buffer into the 2D buffer.
CA_FUNCTION copyCellBuffStateTo2DBuff(CA_GRID grid, CA_CELLBUFF_STATE_IO src,
    __global _caState* dst, _caUnsigned dst_x_size, _caUnsigned dst_y_size)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position in the 2D buffer considering that
    // get_global could have an offset.
    _caUnsigned x = get_global_id(0) - grid.bx_lx;
    _caUnsigned y = get_global_id(1) - grid.bx_ty;

    // Check that the global_id is inside the 2D buffer.
    if (y >= dst_y_size || x >= dst_x_size)
        return;

    // Get the main cell value
    CA_STATE  value = caReadCellBuffState(grid, src, 0);

    // Write the value into the 2D buffer
    dst[y*dst_x_size + x] = value;
}


//! Copy the 2D  buffer into the  real cell buffer.  
CA_FUNCTION copy2DBuffToCellBuffReal(CA_GRID grid, __global const _caReal* src,
    _caUnsigned src_x_size, _caUnsigned src_y_size,
    CA_CELLBUFF_REAL_IO dst)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position in the 2D buffer considering that get_global
    // could have an offset.
    _caUnsigned x = get_global_id(0) - grid.bx_lx;
    _caUnsigned y = get_global_id(1) - grid.bx_ty;

    // Check that the global_id is inside the 2D buffer.
    if (y >= src_y_size || x >= src_x_size)
        return;

    // Get the value from the 2D buffer.
    CA_REAL value = src[y*src_x_size + x];

    // Set the main cell to be the given value
    caWriteCellBuffReal(grid, dst, value);
}


//! Copy the 2D  buffer into the  state cell buffer.  
CA_FUNCTION copy2DBuffToCellBuffState(CA_GRID grid, __global _caState* src,
    _caUnsigned src_x_size, _caUnsigned src_y_size,
    CA_CELLBUFF_STATE_IO dst)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position in the 2D buffer considering that get_global
    // could have an offset.
    _caUnsigned x = get_global_id(0) - grid.bx_lx;
    _caUnsigned y = get_global_id(1) - grid.bx_ty;

    // Check that the global_id is inside the 2D buffer.
    if (y >= src_y_size || x >= src_x_size)
        return;

    // Get the value from the 2D buffer.
    CA_STATE value = src[y*src_x_size + x];

    // Set the main cell to be the given value
    caWriteCellBuffState(grid, dst, value);
}


//! Set the main cell of the given real buffer to the given value
CA_FUNCTION setValueCellBuffReal(CA_GRID grid, CA_CELLBUFF_REAL_IO buff, CA_GLOB_REAL_I value)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Set the main cell to be the given value
    caWriteCellBuffReal(grid, buff, value);
}


//! Set the main cell of the given real buffer to the given value
CA_FUNCTION setValueCellBuffState(CA_GRID grid, CA_CELLBUFF_STATE_IO buff, CA_GLOB_STATE_I value)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Set the main cell to be the given value
    caWriteCellBuffState(grid, buff, value);
}


//! Set the main edges of the given real buffer to the given value
CA_FUNCTION setValueEdgeBuffReal(CA_GRID grid, CA_EDGEBUFF_REAL_IO buff, CA_GLOB_REAL_I value)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Cycle through the edges.
    for (int e = 1; e <= caEdges; e++)
    {
        // Set the edge to be the given value
        caWriteEdgeBuffReal(grid, buff, e, value);
    }
}


//! Set the main edges of the given state buffer to the given value
CA_FUNCTION setValueEdgeBuffState(CA_GRID grid, CA_EDGEBUFF_STATE_IO buff, CA_GLOB_STATE_I value)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Cycle through the edges.
    for (int e = 1; e <= caEdges; e++)
    {
        // Set the edge to be the given value
        caWriteEdgeBuffState(grid, buff, e, value);
    }
}


//! Copy the given points of the real cell buffer into the 1D buffer.
CA_FUNCTION copyPLCellBuffRealTo1DBuff(CA_GRID grid, CA_CELLBUFF_REAL_IO src,
    __global _caReal* dst, _caUnsigned dst_size,
    __global _caUnsigned2* xy, _caUnsigned xy_size)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position 
    _caUnsigned x = get_global_id(0);
    _caUnsigned y = get_global_id(1);

    // Get the main cell value
    CA_REAL  value = caReadCellBuffReal(grid, src, 0);

    for (int i = 0; i < xy_size; i++)
    {
        if (xy[i].x == x && xy[i].y == y)
            dst[i] = value;
    }
}


//! Copy the given points of the state cell buffer into the 1D buffer.
CA_FUNCTION copyPLCellBuffStateTo1DBuff(CA_GRID grid, CA_CELLBUFF_STATE_IO src,
    __global _caState* dst, _caUnsigned dst_size,
    __global _caUnsigned2* xy, _caUnsigned xy_size)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position 
    _caUnsigned x = get_global_id(0);
    _caUnsigned y = get_global_id(1);

    // Get the main cell value
    CA_STATE  value = caReadCellBuffState(grid, src, 0);

    for (int i = 0; i < xy_size; i++)
    {
        if (xy[i].x == x && xy[i].y == y)
            dst[i] = value;
    }
}


//! Copy the 1D buffer into the given points of the real cell buffer.
CA_FUNCTION copy1DBuffToPLCellBuffReal(CA_GRID grid, __global _caReal* src, _caUnsigned src_size,
    CA_CELLBUFF_REAL_IO dst,
    __global _caUnsigned2* xy, _caUnsigned xy_size)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position 
    _caUnsigned x = get_global_id(0);
    _caUnsigned y = get_global_id(1);

    for (int i = 0; i < xy_size; i++)
    {
        if (xy[i].x == x && xy[i].y == y)
            caWriteCellBuffReal(grid, dst, src[i]);
    }
}


//! Copy the 1D buffer into the given points of the real cell buffer.
CA_FUNCTION copy1DBuffToPLCellBuffState(CA_GRID grid, __global _caState* src, _caUnsigned src_size,
    CA_CELLBUFF_STATE_IO dst,
    __global _caUnsigned2* xy, _caUnsigned xy_size)
{
    // Initialise the grid.
    CA_GRID_INIT(grid);

    // Find the x and y position 
    _caUnsigned x = get_global_id(0);
    _caUnsigned y = get_global_id(1);

    for (int i = 0; i < xy_size; i++)
    {
        if (xy[i].x == x && xy[i].y == y)
            caWriteCellBuffState(grid, dst, src[i]);
    }
}
