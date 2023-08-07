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

#ifndef _CA_BOXLIST_HPP_
#define _CA_BOXLIST_HPP_


//! \file BoxList.hpp
//! Contains the class that contains a list of rectangular Boxes. It
//! can be used to identify a non rectangular region of the CA grid.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-09


#include"BaseTypes.hpp"
#include"Box.hpp"
#include<list>


namespace CA {

    //! A BoxList contains a list of boxes. It can be used to identify a
    //! non rectangular region of the CA grid.

    //! \attention The list should not contains two boxes that
    //! overlap (intersect). 

    //! \warning The position (0,0) in the Grid correspond to the
    //! top-left corner of the grid.

    class BoxList
    {
    private:
        //! Identifies the container of Boxes.
        typedef std::list<Box> Boxes;

        //! Identifies the interator.
        typedef Boxes::iterator Iter;

    public:
        //! Identifies the constant interator.
        typedef Boxes::const_iterator ConstIter;

        //! Create a BoxList
        BoxList();

        //! Create a Boxlist from a single box.
        BoxList(const Box& box);

        //! Destroy the BoxList.
        ~BoxList();

        // --- METHODS --- 

        //! Return the extent of the boxlist, i.e. the largest box that
        //! contain all the boxes in the list.
        Box extent() const;

        //! Add a box into the list. The list should not contain two
        //! boxes that overlap (intersect). Thus when a box is added, the
        //! method checks if it is overlapping with an existing box. If
        //! there is overlap between two boxes, the algorithm substitute
        //! the two boxes with smaller non overlapping one.
        void add(const Box& box);

        //! Clear all the element in the box list.
        void clear();

        //! Return the iterator to the first box in the list.
        ConstIter begin() const;

        //! Return the iterator to the end of the list.
        ConstIter end() const;

        //! Return the iterator to the first box in the list.
        Iter begin();

        //! Return the iterator to the end of the list.
        Iter end();

        //! The number of boxes in the list.
        Unsigned size() const;

    private:
        //! The list of boxes.
        Boxes  _boxes;

        //! The extent of the BoxList, i.e. the largest box that contains
        //! all the boxes in the list.
        Box    _extent;
    };


    /// ----- Inline implementation ----- ///


    inline BoxList::BoxList() :
        _boxes(), _extent(Box::Empty())
    {
    }


    inline BoxList::BoxList(const Box& box) :
        _boxes(1, box), _extent(box)
    {
    }


    inline BoxList::~BoxList()
    {
    }


    inline Box BoxList::extent() const
    {
        return _extent;
    }


    inline void BoxList::add(const Box& src)
    {
        // If the width or height is zero then there is nothing to add
        if (src.empty())
            return;

        // NOTe: caution is required, I'm not sure this logic is complete. it's ambitious

        // Create a list of boxes to add.
        Boxes toadd;

        // Add the given box to the list.
        toadd.push_back(src);

        // Cycle through all the boxes in the existing list.
        for (Boxes::iterator iorg = _boxes.begin(); iorg != _boxes.end(); ++iorg)
        {
            // Cycle through all the boxes to add.
            for (Boxes::iterator iadd = toadd.begin(); iadd != toadd.end() && !toadd.empty();)
            {
                // if box to add is completely within an existing box, then remove it from the list to add, and move on
                if ((*iorg).Union((*iadd), (*iorg)) == (*iorg))
                {
                    iadd = toadd.erase(iadd);
                    continue;
                }

                // Check if they intersect.
                if ((*iorg).intersect((*iadd)))
                {
                    // The boxes can be defined in terms of their spans on each
                    // axis - It is possible find the interesting points on each
                    // axis where a box starts or ends. This gives the
                    // possibility of 9 possible sub-boxes of which between 7 to
                    // 9 might exist. The boxes 2, 4, 5, 6 and 8 always exist.

                    // . a b c d
                    // e +-+-+-+
                    // . |1|2|3|
                    // f +-+-+-+
                    // . |4|5|6|
                    // g +-+-+-+
                    // . |7|8|9|
                    // h +-+-+-+

                    // The 5 is the area of intersection.

                    Unsigned a = std::min((*iorg).left(), (*iadd).left());
                    Unsigned b = std::max((*iorg).left(), (*iadd).left());
                    Unsigned c = std::min((*iorg).right(), (*iadd).right());
                    Unsigned d = std::max((*iorg).right(), (*iadd).right());

                    Unsigned e = std::min((*iorg).top(), (*iadd).top());
                    Unsigned f = std::max((*iorg).top(), (*iadd).top());
                    Unsigned g = std::min((*iorg).bottom(), (*iadd).bottom());
                    Unsigned h = std::max((*iorg).bottom(), (*iadd).bottom());

                    Point ae(a, e); Point be(b, e);  Point ce(c, e); Point de(d, e);
                    Point af(a, f); Point bf(b, f);  Point cf(c, f); Point df(d, f);
                    Point ag(a, g); Point bg(b, g);  Point cg(c, g); Point dg(d, g);
                    Point ah(a, h); Point bh(b, h);  Point ch(c, h); Point dh(d, h);


                    // We substitute the two boxes doing these steps:
                    // (1) Add boxes 2,4,6, and 8 to the list of boxes to add.
                    // (2) Check if sub-boxes 1,3,7,9 are inside iadd or iorg.
                    //     and if it is the case add them to the list of boxes to add.
                    // (3) Remove the iadd box from the list of boxes to add.
                    // (4) Reduce the iorg box to be of the size of the intersect area (5). 

                    // Step (1)
                    toadd.push_back(Box(be, cf)); // Box 2
                    toadd.push_back(Box(af, bg)); // Box 4
                    toadd.push_back(Box(cf, dg)); // Box 6
                    toadd.push_back(Box(bg, ch)); // Box 8

                    // Step (2)
                    if ((*iorg).inside(ae) || (*iadd).inside(ae))
                        toadd.push_back(Box(ae, bf)); // Box 1

                    if ((*iorg).inside(ce) || (*iadd).inside(ce))
                        toadd.push_back(Box(ce, df)); // Box 3

                    if ((*iorg).inside(ag) || (*iadd).inside(ag))
                        toadd.push_back(Box(ag, bh)); // Box 7

                    if ((*iorg).inside(cg) || (*iadd).inside(cg))
                        toadd.push_back(Box(cg, dh)); // Box 9

                    // Step (3)
                    iadd = toadd.erase(iadd);

                    // Step (4)
                    (*iorg).setX(b);
                    (*iorg).setY(f);
                    (*iorg).setW(c - b);
                    (*iorg).setH(g - f);
                }
                else
                {
                    // If does not intersect do nothing.
                    ++iadd;
                }
            }
        }

        // Now all the boxes in the toadd list shoud be guarantee not to
        // overlapping. Thus add the new boxes into the BoxList.
        _boxes.insert(_boxes.end(), toadd.begin(), toadd.end());

        // Ok the box is added, update the extent. 
        _extent = Box::Union(_extent, src);
    }


    inline void BoxList::clear()
    {
        _boxes.clear();
    }


    inline BoxList::ConstIter BoxList::begin() const
    {
        return _boxes.begin();
    }


    inline BoxList::ConstIter BoxList::end() const
    {
        return _boxes.end();
    }


    inline BoxList::Iter BoxList::begin()
    {
        return _boxes.begin();
    }


    inline BoxList::Iter BoxList::end()
    {
        return _boxes.end();
    }


    inline Unsigned BoxList::size() const
    {
        return _boxes.size();
    }

} // CA

#endif  // _CA_BOXLIST_HPP_
