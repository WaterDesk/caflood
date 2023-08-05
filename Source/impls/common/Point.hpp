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

#ifndef _CA_POINT_HPP_
#define _CA_POINT_HPP_


//! \file Point.hpp
//! Contains the class that identifies a point in the CA grid.  This
//! is different from coordinate.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-09


#include"BaseTypes.hpp"
#include"Coo.hpp"


namespace CA {

    // Forward declaration
    class Grid;


    //! Identifies a cell point in the CA grid (X,Y).  A point contains
    //! the respective coordinate value inside. However, this cannot be
    //! directly set (the Coo object will be empty), it need to be set
    //! by using the specific setCoo and Point::create methods with the
    //! grid object.

    //! \attention This differs from coordinate.  \warning The point
    //! (0,0) in the Grid correspond to the top-left corner of the
    //! grid. This is different than coordinate.

    class Point
    {
    public:

        //default constructor, sets to zero's
        Point();

        //! Create a point in the grid.
        //! \param x         The X position.
        //! \param y         The Y position.
        Point(Unsigned x, Unsigned y);


        //! Destroy the point.
        ~Point();


        //! Return the X position.
        Unsigned x() const;


        //! Return the Y position.
        Unsigned y() const;


        //! Return the coordinate. \attention This object could be empty
        //! if it was not set by the setCoo method or the point object was
        //! created by the Point::create method.
        Coo coo() const;


        //! Set the X position.
        //! \attention this action set the coo object to empty.
        void setX(Unsigned x);


        //! Set the Y position.
        //! \attention this action set the coo object to empty.
        void setY(Unsigned y);


        //! Set the coordinate of the given point depending
        //! where the point is on the grid.
        void setCoo(const Grid& grid) const;


        //! Static method that create a point depending on given
        //! coordinate of the grid.
        static Point create(const Grid& grid, Real x_coo, Real y_coo);


        //! Return true if the given point is equal, 
        //! i.e this (X,Y) = given (X,Y)
        bool operator==(const Point& src) const;


        //! Return true if the given point is smaller, 
        //! i.e this (X,Y) > given (X,Y)
        bool operator>(const Point& src) const;


        //! Return true if the given point is smaller or equal, 
        //! i.e this (X,Y) >= given (X,Y)
        bool operator>=(const Point& src) const;


        //! Return true if the given point is larger, 
        //! i.e this (X,Y) < given (X,Y)
        bool operator<(const Point& src) const;


        //! Return true if the given point is larger or equal, 
        //! i.e this (X,Y) > given (X,Y)
        bool operator<=(const Point& src) const;


    private:

        //! X and Y position.
        Unsigned _x;
        Unsigned _y;

        //! Respective coordinates.
        mutable Coo _coo;
    };


    /// ----- Inline implementation ----- ///
    inline Point::Point() :
        _x(0), _y(0), _coo()
    {
    }


    inline Point::Point(Unsigned x, Unsigned y) :
        _x(x), _y(y), _coo()
    {
    }


    inline Point::~Point()
    {
    }


    inline Unsigned Point::x() const
    {
        return _x;
    }


    inline Unsigned Point::y() const
    {
        return _y;
    }


    inline Coo Point::coo() const
    {
        return _coo;
    }


    inline void Point::setX(Unsigned x)
    {
        _x = x;
        _coo.clear();
    }


    inline void Point::setY(Unsigned y)
    {
        _y = y;
        _coo.clear();
    }


    inline bool Point::operator==(const Point& src) const
    {
        return (_x == src._x) && (_y == src._y);
    }


    inline bool Point::operator>(const Point& src) const
    {
        return (_x > src._x) && (_y > src._y);
    }


    inline bool Point::operator>=(const Point& src) const
    {
        return (_x >= src._x) && (_y >= src._y);
    }


    inline bool Point::operator<(const Point& src) const
    {
        return (_x < src._x) && (_y < src._y);
    }


    inline bool Point::operator<=(const Point& src) const
    {
        return (_x <= src._x) && (_y <= src._y);
    }

} // CA

#endif  // _CA_POINT_HPP_
