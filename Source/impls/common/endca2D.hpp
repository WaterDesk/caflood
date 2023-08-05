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

#ifndef _CA_ENDCA2D_HPP_
#define _CA_ENDCA2D_HPP_

//! \file endca2D.hpp

//! This file contains the various implementations of methods and
//! classes that need all the elements defined by ca2D.hpp, such as
//! Grid,CellBuffReal, etc..
//! \ettention This file should be included at the end of a ca2D.hpp
//! implementation.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk,
//! \date 2012-08


#include"Grid.hpp"
#include"Point.hpp"


namespace CA {

    inline void Point::setCoo(const Grid& grid) const
    {
        // Retrieve x and y coordinate (attention Y is inverted).
        _coo.setX(static_cast<_caReal>(grid.xCoo() + (_x + 0.5)* grid.length()));
        _coo.setY(static_cast<_caReal>(grid.yCoo() + ((grid.yNum() - _y - 0.5) * grid.length())));
    }


    inline Point Point::create(const Grid& grid, Real x_coo, Real y_coo)
    {
        Unsigned x = static_cast<Unsigned>(std::floor((x_coo - grid.xCoo()) / grid.length()));
        Unsigned y = static_cast<Unsigned>(std::floor((y_coo - grid.yCoo()) / grid.length()));

        /// Y is inverted
        /// Y cannot be zero!
        y = std::max(grid.yNum() - y - 1, static_cast<Unsigned>(1));
        /// X cannot be zero!
        x = std::max(x, static_cast<Unsigned>(1));

        Point p(x, y);
        p._coo.setX(x_coo);
        p._coo.setY(y_coo);

        return p;
    }


    inline void Box::setCoo(const Grid& grid) const
    {
        _tl.setCoo(grid);
        _br.setCoo(grid);
    }

    inline Box Box::create(const Grid& grid, Real top_left_x_coo, Real top_left_y_coo,
        Real bottom_right_x_coo, Real bottom_right_y_coo)
    {
        Point tl(Point::create(grid, top_left_x_coo, top_left_y_coo));
        Point br(Point::create(grid, bottom_right_x_coo, bottom_right_y_coo));

        return Box(tl, br);
    }

} // CA

#endif  // _CA_ENDCA2D_HPP_
