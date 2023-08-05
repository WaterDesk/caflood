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

#ifndef _CA_COO_HPP_
#define _CA_COO_HPP_


//! \file Coo.hpp
//! Contains the class that identifies a coordinate in the CA grid.  This
//! is different from a point.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-09


#include"BaseTypes.hpp"
#include<limits>


namespace CA {

    //! Identifies a coordinate in the CA grid (X,Y).
    //! \attention This differs from a point.
    //! \warning The coordinate (0.0,0.0) in the Grid correspond to the
    //! bottom-left corner of the grid. This is different than point.
    class Coo
    {
    public:
        //! Create a coordinate in the grid.
        //! \param x         The X coordinate.
        //! \param y         The Y coordinate.
        Coo(Real x, Real y);

        //! Create an empty coordinate.
        Coo();

        //! Destroy the coordinate.
        ~Coo();

        //! Return the X coordinate.
        Real x() const;

        //! Return the Y coordinate.
        Real y() const;

        //! Set the X coordinate.
        void setX(Real x);

        //! Set the Y coordinate.
        void setY(Real y);

        //! Return true if the given coordinate is equal, 
        //! i.e this (X,Y) = given (X,Y)
        bool operator==(const Coo& src) const;

        //! Return true if the given coordinate is smaller, 
        //! i.e this (X,Y) > given (X,Y)
        bool operator>(const Coo& src) const;

        //! Return true if the given coordinate is smaller or equal, 
        //! i.e this (X,Y) >= given (X,Y)
        bool operator>=(const Coo& src) const;

        //! Return true if the given coordinate is larger, 
        //! i.e this (X,Y) < given (X,Y)
        bool operator<(const Coo& src) const;

        //! Return true if the given coordinate is larger or equal, 
        //! i.e this (X,Y) > given (X,Y)
        bool operator<=(const Coo& src) const;

        //! Returns the euclidian distance to another pair of coordinates.
        Real distanceTo(Coo c);

        //! Return true if the coordinate is empty,i.e. not in the grid.
        bool isEmty() const;

        //! Clear the coordinate values ans set them to empty.
        void clear();

    private:

        //! X and Y coordinates.
        Real _x;
        Real _y;
    };


    /// ----- Inline implementation ----- ///


    inline Coo::Coo(Real x, Real y) :
        _x(x), _y(y)
    {
    }


    inline Coo::Coo() :
        _x(std::numeric_limits<Real>::infinity()),
        _y(std::numeric_limits<Real>::infinity())
    {
    }


    inline Coo::~Coo()
    {
    }


    inline Real Coo::x() const
    {
        return _x;
    }


    inline Real Coo::y() const
    {
        return _y;
    }


    inline void Coo::setX(Real x)
    {
        _x = x;
    }


    inline void Coo::setY(Real y)
    {
        _y = y;
    }


    inline bool Coo::operator==(const Coo& src) const
    {
        return (_x == src._x) && (_y == src._y);
    }


    inline bool Coo::operator>(const Coo& src) const
    {
        return (_x > src._x) && (_y > src._y);
    }


    inline bool Coo::operator>=(const Coo& src) const
    {
        return (_x >= src._x) && (_y >= src._y);
    }


    inline bool Coo::operator<(const Coo& src) const
    {
        return (_x < src._x) && (_y < src._y);
    }


    inline bool Coo::operator<=(const Coo& src) const
    {
        return (_x <= src._x) && (_y <= src._y);
    }


    inline Real Coo::distanceTo(Coo c)
    {
        return sqrt(pow((_x - c.x()), 2) + pow((_y - c.y()), 2));
    }


    inline bool Coo::isEmty() const
    {
        return
            (_x == std::numeric_limits<Real>::infinity()) ||
            (_y == std::numeric_limits<Real>::infinity());
    }


    inline void Coo::clear()
    {
        _x = std::numeric_limits<Real>::infinity();
        _y = std::numeric_limits<Real>::infinity();
    }

} // CA

#endif  // _CA_COO_HPP_
