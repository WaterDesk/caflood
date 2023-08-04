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

//! \file caapi2D.cl
//! caAPI 2D OpenCL implementation with a lattice that is structured,
//! regular, with square cells and a Von Neumann neighbourhood of 1
//! level. This file is used by the CA functions. It contains also the
//! helper kernels.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2011-12


// The OpenCL code needs 64 bit double operations.
#if     CA_REAL_PRECISION == CA_REAL_DOUBLE
#pragma OPENCL EXTENSION cl_khr_fp64 : enable 
#endif

//#if !(defined(CL_VERSION_1_1) || defined(CL_VERSION_1_2)) 
#if !(defined(__OPENCL_C_VERSION__) && __OPENCL_C_VERSION__ >= 110) 

#error "At least openCL version 1.1 needed".
#endif


//////////////////////////////////////////////////////////////////////
//
// The types, classes and methods with a "_ca" prefix are internal by
// the CA Function and should not be used outside of it.
//
// The types,  classes and methods with a "ca" prefix are public. 
//
//////////////////////////////////////////////////////////////////////


//! Define the type of real values.

#if     CA_REAL_PRECISION == CA_REAL_FLOAT
typedef float           _caReal;  
#else
typedef double          _caReal;  
#endif

//! Define the type of state values.
typedef int             _caState; 


//! Define the type of a size value.
typedef ulong           _caUnsigned; 


//! Define the type of a integer value.
typedef int             _caInt; 


//! Define the type that contains two size values.
typedef ulong2          _caUnsigned2; 


//! This is the shorted version of the _caGrid. It Contains only the
//! esential information about the grid which are used by the CA
//! functions CA functions.  \attention position (0,0) is top left
//! corner.
struct _caGrid_short
{
  //! The lenght of an edge of the cell in the X/Y dimension.
  const _caReal length;


  //! The area of a cell;
  const _caReal area;		
  

  //! The x coordinate of the top-left corner
  const _caReal x_coo_top;


  //! The y coordinate of the top-left corner
  const _caReal y_coo_top;


  //! The left x position to the top left corner of the
  //! rectangular box.
  const _caUnsigned bx_lx;
  

  //! The top y position to the top left corner of the
  //! rectangular box.
  const _caUnsigned bx_ty;


  //! The right x position to the bottom righ corner of the
  //! rectangular box.
  const _caUnsigned bx_rx;
  

  //! The bottom y position to the bottom righ corner of the
  //! rectangular box.
  const _caUnsigned bx_by;


  //! The offset in the X dimension of a cell buffer. 
  const _caUnsigned cb_x_offset;       

  
  //! The stride in the X dimension of a cell buffer. It represent the
  //! alligned X dimension which is used to improve the performance on
  //! GPU. \attention This is the X dimenesnison that the buffer is
  //! allocated.
  const _caUnsigned cb_stride;       

  
  //! The offset in the X dimension of the west/east sub-buffers of
  //! the edge buffer + starting point of the we buffer.
  const _caUnsigned eb_we_offset;       


  //! The stride in the X dimension of north/south west/east
  //! sub-buffers of a edge buffer. It represent the alligned X
  //! dimension which is used to improve the performance on
  //! GPU. \attention This is the X dimenesnison that the buffer is
  //! allocated.
  const _caUnsigned eb_ns_stride;   
  const _caUnsigned eb_we_stride;    

   
  //! This contains the index of the central cell in the "CellBuffers"
  //! considerd as 1D buffers. This value should change in each
  //! thread.
  _caUnsigned cb_index;

}; 


// ---- GLOBAL VARIABLES  ----


//! PI Value
const __constant _caReal PI = 3.141592654; 


// ---- CONSTANTS ----

#define CA2D_NEIGHBOURS 4

//! The total number of cells in the neighbourhood, i.e. the
//! number of neighbours. 
const __constant _caInt caNeighbours = 4; 


//! Number of edges of a cell. 
const __constant _caInt caEdges = 4; 


//! Number of vertices of a cell. 
const __constant _caInt caVertices = 4; 


//! Number of neighbours' levels in the neighbourhood. 
const __constant _caInt caLevels = 1; 


//! Maximum number of neighbours in a level
const __constant _caInt caMaxNeighboursLevel = 4; 


// ---- CA FUNCTION DECLARATION METHODS ----


//! \def CA_FUNCTION
//! Define a CA function. This is a prefix that is added before the
//! function name.
#define CA_FUNCTION  __kernel void 


//! Define the type of the grid variable in the CA function. 
typedef struct _caGrid_short         CA_GRID; 


//! Define the type of the read only buffer with a real value in each cell.
typedef __global const _caReal*      CA_CELLBUFF_REAL_I; 


//! Define the type of the read/write buffer with a real value in each cell.
typedef __global _caReal*            CA_CELLBUFF_REAL_IO; 


//! Define the type of the read only buffer with a state value in each cell.
typedef __global const _caState*     CA_CELLBUFF_STATE_I; 


//! Define the type of the read/write buffer with a state value in each cell.
typedef __global _caState*           CA_CELLBUFF_STATE_IO; 


//! Define the type of the read only buffer with a real value in each edge.
typedef __global const _caReal*      CA_EDGEBUFF_REAL_I; 


//! Define the type of the read/write buffer with a real value in each edge.
typedef __global _caReal*            CA_EDGEBUFF_REAL_IO; 


//! Define the type of the read only buffer with a state value in each edge.
typedef __global const _caState*     CA_EDGEBUFF_STATE_I; 


//! Define the type of the read/write buffer with a state value in each edge.
typedef  __global _caState*          CA_EDGEBUFF_STATE_IO; 


//! Define the type of a constant global variable with a real value.
typedef const _caReal                CA_GLOB_REAL_I; 


//! Define the type of a canstant global variable with a state value.
typedef const _caState               CA_GLOB_STATE_I; 


//! Define the type of a set of global alarms. Alarms can be activated
//! inside a CA function. However, they cannot be deactivate or
//! checked their status inside a CA function..
typedef __global char*               CA_ALARMS_O;


#if     CA_OCL_TABLE == CA_OCL_CONSTANT
//! Define the type of a table with real data.
typedef __constant  _caReal*         CA_TABLE_REAL_I;


//! Define the type of a table with state data.
typedef __constant  _caState*        CA_TABLE_STATE_I;
#else
//! Define the type of a table with real data.
typedef __global  _caReal*         CA_TABLE_REAL_I;


//! Define the type of a table with state data.
typedef __global  _caState*        CA_TABLE_STATE_I;
#endif

// ---- CA FUNCTION BODY  METHODS ----


//! Define the type of a real value.
typedef _caReal          CA_REAL; 


//! Define the type of a state value.
typedef _caState         CA_STATE; 


//! \def CA_GRID_INIT
//! Intialise the grid structure.
#define CA_GRID_INIT(grid)						\
  if(	(get_global_id(0) < grid.bx_lx)  ||				\
	(get_global_id(0) >= grid.bx_rx) ||				\
	(get_global_id(1) < grid.bx_ty)  ||				\
	(get_global_id(1) >= grid.bx_by) ) return;			\
  grid.cb_index = (get_global_id(1) + 1) * grid.cb_stride + (get_global_id(0) + grid.cb_x_offset);

    
//! \def CA_ARRAY_CREATE
//! Create a ca array of specific type, name and size.
#define CA_ARRAY_CREATE(grid,type,name,size)  __private type name[5] 


//! Return the X coordinate of the centroid of the cell.
inline _caReal caXCoo(CA_GRID grid, int cell_number)
{
  _caUnsigned x = get_global_id(0);
  switch(cell_number)
  {
  case 0: break;
  case 1: x +=1; break;
  case 2: break;
  case 3: x -=1; break;
  case 4: break;
  }

  return  grid.x_coo_top + (x + 0.5)* grid.length;
}


//! Return the Y coordinate of the centroid of the cell.
inline _caReal caYCoo(CA_GRID grid, int cell_number)
{
  _caUnsigned y = get_global_id(1);

  switch(cell_number)
  {
  case 0: break;
  case 1: break;
  case 2: y -=1; break;
  case 3: break;
  case 4: y +=1; break;
  }

  return  grid.y_coo_top - (y + 0.5)* grid.length;
}


//! Set in the given array, the X coordinate of the centroid all visible cells;
inline void caXCooCellArray(CA_GRID grid, _caReal xcoos[])
{
  xcoos[0]=caXCoo(grid,0);
  xcoos[1]=caXCoo(grid,1);
  xcoos[2]=caXCoo(grid,2);
  xcoos[3]=caXCoo(grid,3);
  xcoos[4]=caXCoo(grid,4);
}


//! Set in the given array, the Y coordinate of the centroid all visible cells;
inline void caYCooCellArray(CA_GRID grid, _caReal ycoos[])
{
  ycoos[0]=caYCoo(grid,0);
  ycoos[1]=caYCoo(grid,1);
  ycoos[2]=caYCoo(grid,2);
  ycoos[3]=caYCoo(grid,3);
  ycoos[4]=caYCoo(grid,4);
}


//! Return the area of the given cell number;
inline _caReal caArea(CA_GRID grid, int cell_number)
{
  return grid.area;
}


//! Set in the given array, the area of all visible cells;
inline void caAreaCellArray(CA_GRID grid, _caReal areas[])
{
  areas[0]=grid.area;
  areas[1]=grid.area;
  areas[2]=grid.area;
  areas[3]=grid.area;
  areas[4]=grid.area;
}


//! Return the angle (radian) between the centroid of the main
//! cell (number 0) and the centroid of the given cell_number. The
//! base line is the first coordinate axis (x-axis) in the
//! Cartesian plane and the angle is measure anticlockwise.
inline _caReal caAngle(CA_GRID grid, int cell_number)
{
  switch(cell_number)
  {
  case 0: return 0;
  case 1: return 0;
  case 2: return PI/2;
  case 3: return PI;
  case 4: return 3*PI/2;
  }
  return 0;
}


//! Set in the given array, the angle (radian) between the centroid of the main
//! cell (number 0) and the centroid for all visible cells.
inline void caAngleCellArray(CA_GRID grid, _caReal angles[])
{
  angles[0] = 0;
  angles[1] = 0;
  angles[2] = PI/2;
  angles[3] = PI;
  angles[4] = 3*PI/2;
}


//! Return the distance between the centroid of the main
//! cell (number 0) and the centroid of the given cell_number. 
inline _caReal caDistance(CA_GRID grid, int cell_number)
{
  return (cell_number==0) ? 0.0 : grid.length;
}


//! Set in the given array, the  distance between the centroid of the main
//! cell (number 0) and the centroid for all visible cells.
inline void caDistanceCellArray(CA_GRID grid, _caReal distances[])
{
  distances[0] = 0;
  distances[1] = grid.length;
  distances[2] = grid.length;
  distances[3] = grid.length;
  distances[4] = grid.length;
}


//! Return the lenght of the given edge of the given cell number.
inline _caReal caLength(CA_GRID grid, int cell_number, int edge_number)
{
  return grid.length;
}


//! Set the given array with the lenght of all the edges of the given cell number.
inline void caLengthEdgeArray(CA_GRID grid, int cell_number, _caReal lengths[])
{
  lengths[0] = grid.length;
  lengths[1] = grid.length;
  lengths[2] = grid.length;
  lengths[3] = grid.length;
  lengths[4] = grid.length;
}


//! Return the number of cells in a neighbours' level. This is
//! used to number all the cells in the particular level. This is
//! the cell_level_number. 
inline int caCellsLevel(CA_GRID grid, int level_number)
{
  return 4;
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
  return 2;  
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
  edges[0]=0;
  edges[1]=1;
  edges[2]=2;
  edges[3]=3;
  edges[4]=4;
}



//! Box border management, it set the bits:
//! Bit 0, True if the central cell is  in the border of the box, one neighbour cell is outside the box.
//! Bit 1, True if the neighbour cell 1 is outside the box.
//! Bit 2, True if the neighbour cell 2 is outside the box.
//! ....
inline CA_STATE caBoxStatus(CA_GRID grid)
{
  CA_STATE mask = 0; 
  mask |= (get_global_id(0) == grid.bx_rx-1)<<1; // Check neighbour cell 1.
  mask |= (get_global_id(1) == grid.bx_ty)<<2;   // Check neighbour cell 2.
  mask |= (get_global_id(0) == grid.bx_lx)<<3;   // Check neighbour cell 3.
  mask |= (get_global_id(1) == grid.bx_by-1)<<4;   // Check neighbour cell 3.
  mask |= (mask>0);			    // Check central cell.

  return mask;
}


//! Print a string
#define caPrint(grid, str)


//! Print a real value.
#define caPrintReal(grid, pre, post, value)


//! Print the real values of a ca array.
#define caPrintRealArray(grid, pre, sep, post,  values, n )


//! Print a state value.
#define caPrintState(grid, pre, post, value)


//! Print the state values of a ca array..
#define caPrintStateArray(grid, pre, sep, post,  values, n )


// ---- MATHS  METHODS ----


//! Rettun the ABS value of the given value.
inline _caReal caAbsReal(_caReal value)
{
  return fabs(value);
}


//! Set the ABS value of each elements in the array.
inline void caAbsRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
    values[i] = fabs(values[i]);
  }
} 


//! Return the minimum between two values
inline _caReal caMinReal(_caReal va, _caReal vb)
{
  return min(va,vb);
}


//! Return the minimum of the given array of values.
inline _caReal caMinRealArray(_caReal values[], int n)
{
  _caReal min = values[0];
  
  for(int i=1; i<n; i++)
  {
    if(values[i] < min )
    {
      min = values[i];
    }
  }
  return min;
}


//! Return the maximum between two values
inline _caReal caMaxReal(_caReal va, _caReal vb)
{
  return max(va,vb);
}


//! Return the maximum of the given array of values.
inline _caReal caMaxRealArray(_caReal values[], int n)
{
  _caReal max = values[0];
  
  for(int i=1; i<n; i++)
  {
    if(values[i] > max )
    {
      max = values[i];
    }
  }
  return max;
}


//! Return 1.0 for x>0.0, -1.0 for x<0.0 and 0 for x=0.0;
inline _caReal caSignReal(_caReal x)
{
  return sign(x);
}


//! Set the sign value of each elements in the array, i.e. 1.0 for
//! x>0.0, -1.0 for x<0.0 and 0 for x=0.0;
inline void caSignRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
    values[i]=sign(values[i]);
  }
} 


//! Return 0.0 for x<edge, else 1.0
inline _caReal caStepReal(_caReal x, _caReal edge)
{
  return step(edge,x);
}


//! Set the the value of each element of the array to 0.0 is the
//! original value is < of edge, else the value 1.0
inline void caStepRealArray(_caReal values[], _caReal edge, int n)
{
  for(int i=0; i<n; i++)
  {
    values[i]=step(edge,values[i]);
  }
} 



//! Compute the power function
inline _caReal caPowReal(_caReal x, _caReal y)
{
#if CA_OCL_NATIVE == 1
  return native_powr(x,y);
#else
  return pow(x,y);
#endif
}


//! Compute the power of each element in the array for the given exponent.
inline void caPowRealArray(_caReal values[], _caReal y, int n)
{
  for(int i=0; i<n; i++)
  {
#if CA_OCL_NATIVE == 1
    values[i] = native_powr(values[i],y);
#else
    values[i] = pow(values[i],y);
#endif
  }
} 


//! Compute the square function
inline _caReal caSqrtReal(_caReal x)
{
#if CA_OCL_NATIVE == 1
  return native_sqrt(x);
#else
  return sqrt(x);
#endif
}


//! Compute the square of each element in the array.
inline void caSqrtRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
#if CA_OCL_NATIVE == 1
    values[i] = native_sqrt(values[i]);
#else
    values[i] = sqrt(values[i]);
#endif
  }
} 

//! Compute the sin function in radians
inline _caReal caSinReal(_caReal x)
{
#if CA_OCL_NATIVE == 1
  return native_sin(x);
#else
  return sin(x);
#endif
}


//! Compute the sin in radians of each element in the array
inline void caSinRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
#if CA_OCL_NATIVE == 1
    values[i] = native_sin(values[i]);
#else
    values[i] = sin(values[i]);
#endif
  }
} 

//! Compute the cos function in radians
inline _caReal caCosReal(_caReal x)
{
#if CA_OCL_NATIVE == 1
  return native_cos(x);
#else
  return cos(x);
#endif
}


//! Compute the cos in radians of each element in the array
inline void caCosRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
#if CA_OCL_NATIVE == 1
    values[i] = native_cos(values[i]);
#else
    values[i] = cos(values[i]);
#endif
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
  for(int i=0; i<n; i++)
  {
    values[i] = atan(values[i]);
  }
} 

//! Compute the atan2 function in radians
inline _caReal caAtan2Real(_caReal y, _caReal x)
{
  return atan2(y,x);
}


//! Compute the atan2 in radians of each element in the array. The
//! results are stored in the first array.
inline void caAtan2RealArray(_caReal ys[], _caReal xs[], int n)
{
  for(int i=0; i<n; i++)
  {
    ys[i] = atan2(ys[i],xs[i]);
  }
} 


// Round to integer toward zero
inline _caReal caTruncReal(_caReal x)
{
  return trunc(x);
}


// Round to integer toward zero for each element in the array.
inline void caTruncRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
    values[i] = trunc(values[i]);
  }
}

// Round to nearest integer away from zero
inline _caReal caRoundReal(_caReal x)
{
  return round(x);
}


// Round to nearest integer away from zero for each element in the array.
inline void caRoundRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
    values[i] = round(values[i]);
  }
}


// Largest integral value not greater than argument
inline _caReal caFloorReal(_caReal x)
{
  return floor(x);
}


// Largest integral value not greater than argument for each element in the array.
inline void caFloorRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
    values[i] = floor(values[i]);
  }
}


// Smallest integral value not less than argument
inline _caReal caCeilReal(_caReal x)
{
  return ceil(x);
}


// Smallest integral value not less than argumen for each element in the array.
inline void caCeilRealArray(_caReal values[], int n)
{
  for(int i=0; i<n; i++)
  {
    values[i] = ceil(values[i]);
  }
}


//! Read the value stored in the selected bits from the given value.
inline _caState caReadBitsState(_caState value, int start, int stop)
{
  _caState ret   = 0;

  int b = 0;
  int s = 0;

  for(b=0, s=start; s<stop;b++,s++)
  {
    ret |= (value & ( 1 << s )) >> (s-b);
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

  for(b=0, s=start; s<stop;b++,s++)                                                                 
  {                                                                                                 
    mask |= ( 1 << s );                                                                             
    bits |= (((1<<b)&value)>>b)<<s;                                                                 
    buffer = (buffer&(~mask))^bits;
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
inline _caReal caFlowVelocity(_caReal irough,_caReal S, _caReal R, _caReal D)
{
  //return caMinReal(caSqrtReal(9.81*D),irough*caPowReal(R,(2.0/3.0))*caSqrtReal(S));

#if     CA_REAL_PRECISION == CA_REAL_FLOAT
  float2 sqrts;

  // Compute square values using native and vector computation.
  sqrts.x = 9.81*D;
  sqrts.y = S;
  sqrts = native_sqrt(sqrts);

  // Compute the power using native.
  return min(sqrts.x,irough*native_powr(fabs(R),(2.0/3.0))*sqrts.y);

#else
  return caMinReal(caSqrtReal(9.81*D),irough*caPowReal(R,(2.0/3.0))*caSqrtReal(S));
#endif
  
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
  for(int i=0; i<num;i++)
    if(((mask >> i) & 1) == 1) alarms[i] = 1;
}


// ---- CELL BUFFERS ----

//! Read the real value of the cell from the given buffer at the given cell 
inline _caReal caReadCellBuffReal(CA_GRID grid, CA_CELLBUFF_REAL_I src, int cell_number)
{  
  long off = 0;
  switch(cell_number)
  {
  case 0: break;
  case 1: off +=1; break;
  case 2: off -=grid.cb_stride; break;
  case 3: off -=1; break;
  case 4: off +=grid.cb_stride; break;
  }
  
  return src[grid.cb_index + off];
}


//! Set the given ca array with the real values of all the visible
//! cells from the given buffer.
inline void  caReadCellBuffRealCellArray(CA_GRID grid, CA_CELLBUFF_REAL_I src, _caReal values[])
{
  values[0] = src[grid.cb_index];
  values[1] = src[grid.cb_index +1];
  values[2] = src[grid.cb_index - grid.cb_stride];
  values[3] = src[grid.cb_index -1];
  values[4] = src[grid.cb_index + grid.cb_stride];
}


//! Write the given real value of the cell into the given buffer at the main cell 
void caWriteCellBuffReal(CA_GRID grid, CA_CELLBUFF_REAL_IO dst, CA_GLOB_REAL_I value)
{
  dst[grid.cb_index] = value;
}


//! Read the state value of the cell from the given buffer at the given cell 
inline _caState caReadCellBuffState(CA_GRID grid, CA_CELLBUFF_STATE_I src, int cell_number)
{
  long off = 0;
  switch(cell_number)
  {
  case 0: break;
  case 1: off +=1; break;
  case 2: off -=grid.cb_stride; break;
  case 3: off -=1; break;
  case 4: off +=grid.cb_stride; break;
  }
  
  return src[grid.cb_index + off];
}


//! Set the given ca array with the state values of all the visible
//! cells from the given buffer.
inline void  caReadCellBuffStateCellArray(CA_GRID grid, CA_CELLBUFF_STATE_I src, _caState values[])
{
  values[0] = src[grid.cb_index];
  values[1] = src[grid.cb_index +1];
  values[2] = src[grid.cb_index - grid.cb_stride];
  values[3] = src[grid.cb_index -1];
  values[4] = src[grid.cb_index + grid.cb_stride];
}


//! Write the given state value of the cell into the given buffer at the main cell 
inline void caWriteCellBuffState(CA_GRID grid, CA_CELLBUFF_STATE_IO dst, CA_GLOB_STATE_I value)
{
  dst[grid.cb_index] = value;
}


// ---- EDGE BUFFERS ----


//! Read the real value of the edge from the given buffer at the given
//! cell index and given edge number.
inline _caReal caReadEdgeBuffReal(CA_GRID grid, CA_EDGEBUFF_REAL_I src, 
				  int cell_number, int edge_number)
{
  _caUnsigned x = get_global_id(0);
  _caUnsigned y = get_global_id(1);

  switch(cell_number)
  {
  case 0: break;
  case 1: x +=1; break;
  case 2: y -=1; break;
  case 3: x -=1; break;
  case 4: y +=1; break;
  }

  _caUnsigned i_ns = (y + 1) * grid.eb_ns_stride  + x;

  _caUnsigned i_we = (y) * grid.eb_we_stride  + x + grid.eb_we_offset;
 
  switch(edge_number)
  {
  case 0: return 0.0;
  case 1: return src[i_we+1];
  case 2: return src[i_ns];
  case 3: return src[i_we];
  case 4: return src[i_ns+grid.eb_ns_stride];
  }
   
  return 0.0;
}


//! Set the given ca array with the real values of all the edges from
//! the given buffer at given cell 
inline void  caReadEdgeBuffRealEdgeArray(CA_GRID grid, CA_EDGEBUFF_REAL_I src, 
					 int cell_number, _caReal values[])
{
  _caUnsigned x = get_global_id(0);
  _caUnsigned y = get_global_id(1);

  switch(cell_number)
  {
  case 0: break;
  case 1: x +=1; break;
  case 2: y -=1; break;
  case 3: x -=1; break;
  case 4: y +=1; break;
  }

  _caUnsigned i_ns = (y + 1) * grid.eb_ns_stride + x;

  _caUnsigned i_we = (y) * grid.eb_we_stride  
    + x + grid.eb_we_offset;
 
  values[0] = 0.0;
  values[1] = src[i_we+1];
  values[2] = src[i_ns];
  values[3] = src[i_we];
  values[4] = src[i_ns+grid.eb_ns_stride];
}


//! Write the given real value into the given buffer at the main cell
//! index and given edge number.
inline void caWriteEdgeBuffReal(CA_GRID grid, CA_EDGEBUFF_REAL_IO dst, int edge_number, _caReal value)
{
  _caUnsigned i_ns = (get_global_id(1) + 1) * grid.eb_ns_stride  
    + get_global_id(0);

  _caUnsigned i_we = (get_global_id(1)) * grid.eb_we_stride  
    + get_global_id(0) + grid.eb_we_offset;
 
  switch(edge_number)
  {
  case 0: break;
  case 1: dst[i_we+1] = value; break;
  case 2: dst[i_ns] = value; break;
  case 3: dst[i_we] = value; break;
  case 4: dst[i_ns+grid.eb_ns_stride] = value; break;
  }   
}


// Miisng caReadEdgeBuffState and caReadEdgeBuffRealEdgeArray

//! Write the given state value into the given buffer at the main cell
//! index and given edge number.
inline void caWriteEdgeBuffState(CA_GRID grid, CA_EDGEBUFF_STATE_IO dst, int edge_number, _caState value)
{
  _caUnsigned i_ns = (get_global_id(1) + 1) * grid.eb_ns_stride  
    + get_global_id(0);

  _caUnsigned i_we = (get_global_id(1)) * grid.eb_we_stride  
    + get_global_id(0) + grid.eb_we_offset;
 
  switch(edge_number)
  {
  case 0: break;
  case 1: dst[i_we+1] = value; break;
  case 2: dst[i_ns] = value; break;
  case 3: dst[i_we] = value; break;
  case 4: dst[i_ns+grid.eb_ns_stride] = value; break;
  }   
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
