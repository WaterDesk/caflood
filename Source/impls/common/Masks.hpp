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

#ifndef _CA_MASKS_HPP_
#define _CA_MASKS_HPP_

//! \file Masks.hpp

//! Contain various methods that can be used to create masks fomr cell
//! and edge buffers. 
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2012-07


#include"ca2D.hpp"

// -------------------------//
// Include the CA 2D functions //
// -------------------------//
#include CA_2D_INCLUDE(caFuncCreateCellMask)
#include CA_2D_INCLUDE(caFuncCreateSimplerCellMask)


namespace CA {

    //! Special method that modifies the given state cell buffer to be a
    //! Mask with various information about the given real cell buffer
    //! using nodata.  

    //! The mask is divided in different bits which represents: 
    //! bit 0 : true  (1) if the main cell has data, 
    //!         false (0) if the main cell has nodata.
    //! bit 1 : true  (1) if the neighbour cell 1 has data,
    //!         false (0) if the neighbour cell 1 has nodata.
    //! bit 2 : true  (1) if the neighbour cell 2 has data,
    //!         false (0) if the neighbour cell 2 has nodata.
    //! bit 3 : true  (1) if the neighbour cell 3 has data,
    //!         false (0) if the neighbour cell 3 has nodata.
    //! .....
    //! bit 30: true  (1) if the main cell has data and a neighbour cell with nodata,
    //!         false (0) if the main cell has nodata or all the cell in the neighbourhood has data.
    //! bit 31: true  (1) if the main cell has nodata and a neighbour cell with data,
    //!         false (0) if the main cell has data or all the cell in the neighbourhood has nodata.

    //! \param bl     Identifies the region of the grid where to compute the mask.
    //! \param g      The grid
    //! \param src    The real cell buffer from where the mask is computed.
    //! \param mask   The output mask.
    //! \param nodata The value of the no-data in the real cell buffer.
    inline void createCellMask(const BoxList& bl, Grid& g, CellBuffReal& src, CellBuffState& mask, Real nodata)
    {
        CA::Execute::function(bl, caFuncCreateCellMask, g, src, mask, nodata);
    }


    //! Special method that modifies the given state cell buffer to be a
    //! simpler Mask (data or nodata) with various information about
    //! the given real cell buffer using nodata.

    //! The mask can have only two values
    //! value : 1  if the main cell has data, 
    //!         0  if the main cell has nodata.

    //! \param bl     Identifies the region of the grid where to compute the mask.
    //! \param g      The grid
    //! \param src    The real cell buffer from where the mask is computed.
    //! \param mask   The output mask.
    //! \param nodata The value of the no-data in the real cell buffer.
    inline void createSimplerCellMask(const BoxList& bl, Grid& g, CellBuffReal& src, CellBuffState& mask,
        Real nodata)
    {
        CA::Execute::function(bl, caFuncCreateSimplerCellMask, g, src, mask, nodata);
    }


    //! Read the value stored in the selected bits from the given value.
    inline State readBitsState(State value, int start, int stop)
    {
        State ret = 0;

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
    inline State writeBitsState(State value, State buffer, int start, int stop)
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

} // Namespace CA

#endif // _CA_MASKS_HPP_
