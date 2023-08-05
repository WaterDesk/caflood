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

#ifndef _CA_POINTLIST_HPP_
#define _CA_POINTLIST_HPP_


//! \file PointList.hpp
//! Contains the class that contains a list of region Points. It
//! can be used to identify a series of points in the CA grid.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-09


#include"BaseTypes.hpp"
#include"Point.hpp"
#include"Box.hpp"
#include<vector>


namespace CA {

    //! A PointList contains a list of points. It can be used to identify a
    //! series of points of the CA grid.

    //! \attention The list is ordered by insertion order and it can
    //! contains be two similar points.

    //! \warning The position (0,0) in the Grid correspond to the
    //! top-left corner of the grid.

    class PointList
    {
    private:
        //! Identifies the container of Points.
        typedef std::vector<Point> Points;

    public:

        //! Indentifies the constant interator.
        typedef Points::const_iterator ConstIter;

        //! Create a PointList
        PointList();

        //! Create a Pointlist from a single point.
        PointList(const Point& p);

        //! Destroy the PointList.
        ~PointList();

        // --- METHODS --- 

        //! Return the extent of the pointlist, i.e the largest box that
        //! contain all the point in the list.
        Box extent() const;

        //! Add a point into the list. The list should not contains two
        //! equal points. 
        void add(const Point& p);

        //! Clear all the Points in the list.
        void clear();

        //! Return the iterator to the first point in the list.
        ConstIter begin() const;

        //! Return the iterator to the end of the list.
        ConstIter end() const;

        //! The number of points in the list.
        Unsigned size() const;

        //! Return the given element, \attention This does not check if it
        //! is in range.
        Point operator[](Unsigned n) const;

    private:
        //! The list of points.
        Points  _points;

        //! The extent of the PointList, i.e the largest box that contains
        //! all the  points in the list.
        Box    _extent;
    };


    /// ----- Inline implementation ----- ///


    inline PointList::PointList() :
        _points(), _extent(Box::Empty())
    {
    }


    inline PointList::PointList(const Point& p) :
        _points(), _extent(p, p)
    {
        _points.push_back(p);
    }


    inline PointList::~PointList()
    {
    }


    inline Box PointList::extent() const
    {
        return _extent;
    }


    inline void PointList::add(const Point& p)
    {
        // Insert the point.
        _points.push_back(p);

        // Increse the extent.
        _extent.include(p);
    }


    inline void PointList::clear()
    {
        _points.clear();
    }


    inline PointList::ConstIter PointList::begin() const
    {
        return _points.begin();
    }


    inline PointList::ConstIter PointList::end() const
    {
        return _points.end();
    }


    inline Unsigned PointList::size() const
    {
        return _points.size();
    }


    inline Point PointList::operator[](Unsigned n) const
    {
        return _points[n];
    }

} // CA

#endif  // _CA_POINTLIST_HPP_
