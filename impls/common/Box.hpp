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

#ifndef _CA_BOX_HPP_
#define _CA_BOX_HPP_


//! \file Box.hpp
//! Contains the class that identifies a rectangular box in the CA
//! grid. It contains the starting cell position and the number of
//! cells in the X and Y directions.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"BaseTypes.hpp"
#include"Point.hpp"
#include<limits>


namespace CA {


  // Forward declaration
  class Grid;
  

  //! A rectangular box which identifies a region on the CA grid. It
  //! contains the (X,Y) starting cell position and the number of
  //! cells in the X and Y directions. This differs from coordinate.

  //! A box contains the top left and bottom right points. The
  //! coordinate values of these points cannot be directly set (the
  //! Coo object will be empty), it need to be set by using the
  //! specific setCoo and Box::create methods with the grid object.
  
  //! \warning The position (0,0) in the Grid correspond to the
  //! top-left corner of the grid.

  class Box
  {

  public:

    //! Create a rectangular box.
    //! \param x         The starting X position to the top left corner of the
    //!                  rectangular box. It can be the offset of a
    //!                  region of the grid to be accessed.  This
    //!                  wouldbe zero to start from the left side.
    //! \param x         The starting Y position to the top left corner of the
    //!                  rectangular box. It can be the offset of a
    //!                  region of the grid to be accessed.  This
    //!                  wouldbe zero to start from the left side.
    //! \param w         The width of the recatangular box. This is the number
    //!                  of cells in the X dimension of the region of
    //!                  the grid to be accessed.
    //! \param h         The height of the recatangular box. This is the number
    //!                  of cells in the Y dimension of the region of
    //!                  the grid to be accessed.
    //! \param e         The efficiency of the box (number of domain points / total points).
    Box(Unsigned x, Unsigned y, Unsigned w, Unsigned h, Real e = 0.0);


    //! Create a rectangular box using top-left and bottom-right
    //! corner.  \attention the bottom-righ point is the point just
    //! outside the box, i.e. the inside() method return false.
    Box(const Point& top_left, const Point& bottom_right);


    //! Destroy the box.
    ~Box();


    // --- STATIC --- 


    //! Static class which returns an empty box. 
    static Box Empty();


    //! Static class which returns the intersection between two
    //! boxes, i.e. the overlay.
    static Box Intersect(const Box& a, const Box& b);


    //! Static class which returns the union between two
    //! boxesm, i.e. the common extent between the two boxes.
    static Box Union(const Box& a, const Box& b);


    // --- METHODS --- 
    

    //! Return the starting X position to the top left corner of the
    //! rectangular box.
    Unsigned x() const;
    

    //! Return the starting Y position to the top left corner of the
    //! rectangular box.
    Unsigned y() const;


    //! Return the width of the rectangular box. This is the number of
    //! cells in the X directions.
    Unsigned w() const;


    //! Return the height of the rectangular box. This is the number of
    //! cells in the Y directions.
    Unsigned h() const;


    //! Return the efficiency of the box.
    Real e() const;


    //! Set the starting X position to the top left corner of the
    //! rectangular box.
    //! \attention This set the coordinate values of top/left and
    //! bottom/right points to empty.
    void setX(Unsigned x);
    

    //! Set the starting Y position to the top left corner of the
    //! rectangular box.
    //! \attention This set the coordinate values of top/left and
    //! bottom/right points to empty.
    void setY(Unsigned y);


    //! Set the width of the rectangular box. This is the number of
    //! cells in the X directions.
    //! \attention This set the coordinate values of top/left and
    //! bottom/right points to empty.
    void setW(Unsigned w);


    //! Set the height of the rectangular box. This is the number of
    //! cells in the Y directions.
    //! \attention This set the coordinate values of top/left and
    //! bottom/right points to empty.
    void setH(Unsigned h);


    //! Set the efficciency.
    void setE(Real e);

    
    //! Set the coordinate of the given top/left and bottom right
    //! points depending where the box is on the grid.
    void setCoo(const Grid& grid) const;


    //! Static method that create a box depending on given
    //! coordinate of the grid.
    //! \attention This code might have a nasty bug!
    static Box create(const Grid& grid, Real top_left_x_coo, Real top_left_y_coo,
		      Real bottom_right_x_coo, Real bottom_right_y_coo);


    //! Expand the box to include the given point.
    void include(const Point& p);


    //! Clear the rectangular box, i.e no width and no height.
    void clear();


    //! Return true if the rectangular box is empty, i.e no width and
    //! no height.
    bool empty() const;


    //! Return the size of the rectangular box, i.e. the number of
    //! cell inside the region. 
    Unsigned size() const;

    
    //! Return the point to the top-left corner of the box.
    //! \attention, this point is inside the box.
    //! \warning The coordinate values of the point are empty, if the
    //! Box was not set by the setCoo method or the Box object was
    //! created by the Box::create method.
    Point  topLeft() const;


    //! Return the point to the bottom-right corner of the box.
    //! \attention This point is outside the box, i.e. inside() method
    //! return false();
    //! \warning The coordinate values of the point are empty, if the
    //! Box was not set by the setCoo method or the Box object was
    //! created by the Box::create method.
    Point  bottomRight() const;

    
    //! Return the value of left side of the box;
    Unsigned left() const;

 
    //! Return the value of right side of the box;
    Unsigned right() const;


    //! Return the value of top side of the box;
    Unsigned top() const;

 
    //! Return the value of bottom side of the box;
    Unsigned bottom() const;

    
    //! Return true if the given point is inside the box (can be on
    //! the edge).
    bool inside(const Point& p) const;


    //! Return true if the given box is inside the box (can have the
    //! same edge).
    bool inside(const Box& b) const;


    //! Return true if the given point is interior in the box (cannnot
    //! be on the edge).
    bool interior(const Point& p) const;


    //! Return true if the given box is interior in the box (cannnot
    //! have the same edge).
    bool interior(const Box& b) const;


    //! Return true if the given box intersect with the box, i.e
    //! collision.
    bool intersect(const Box& b) const;


    //! Return true if the two boxes identify the same region.
    //! \attention it does not compere the efficiency.
    bool operator==(const Box& b) const;


    //! Limit the extent of the box to be inside the given box.
    void limit(const Box& b);


  private:
    
    //! Starting X and Y position to the top left corner of the
    //! rectangular box.
    Unsigned _x;
    Unsigned _y;

    //! Width and height of the recatangular box.
    Unsigned _w;
    Unsigned _h;

    //! Efficiency of the box, i.e. the number of points in the domain / total points.
    Real _e;

    //! Top left point of the box     
    //! \attention, this point is inside the box..
    Point _tl;

    //! Bottom right point of the box.
    //! \attention This point is outside the box, i.e. inside() method
    //! return false();
    Point _br;
  };


  /// ----- Inline implementation ----- ///
  

  inline Box::Box(Unsigned x, Unsigned y, Unsigned w, Unsigned h, Real e):
    _x(x), _y(y), _w(w), _h(h), _e(e), _tl(_x,_y), _br(_x+_w,_y+_h)
  {    
  }


  inline Box::Box(const Point& top_left, const Point& bottom_right):
    _x(top_left.x()), 
    _y(top_left.y()), 
    _w(bottom_right.x()-top_left.x()),
    _h(bottom_right.y()-top_left.y()),
    _e(0.0),
    _tl(top_left),
    _br(bottom_right)
  {
  }

  inline Box::~Box()
  {
  }

  
  inline Box Box::Empty()
  {
    return Box(std::numeric_limits<Unsigned>::max(),std::numeric_limits<Unsigned>::max(),0,0);
  }


  inline Box Box::Intersect(const Box& src_a, const Box& src_b)
  {
    if(!src_a.intersect(src_b))
      return Empty();

    // The intersection of two boxes can be defined in terms of their
    // spans on each axis.
    
    // . a b c d	  
    // e +-+-+-+
    // . |1|2|3|
    // f +-+-+-+
    // . |4|5|6|
    // g +-+-+-+
    // . |7|8|9|
    // h +-+-+-+
    
    // The 5 is the area of intersection.

    Unsigned b = std::max(src_a.left()  ,src_b.left());
    Unsigned c = std::min(src_a.right() ,src_b.right());
    
    Unsigned f = std::max(src_a.top()   ,src_b.top());
    Unsigned g = std::min(src_a.bottom(),src_b.bottom());
    
    Point bf(b,f); 
    Point cg(c,g); 
    
    return Box(bf,cg); // Box 5
  }
  

  inline Box Box::Union(const Box& src_a, const Box& src_b)
  {
    if(src_a.empty())
      return Box(src_b);

    if(src_b.empty())
      return Box(src_a);

    // The union of two boxes can be defined in terms of their
    // spans on each axis.
    
    // . a b c d	  
    // e +-+-+-+
    // . |1|2|3|
    // f +-+-+-+
    // . |4|5|6|
    // g +-+-+-+
    // . |7|8|9|
    // h +-+-+-+
    
    Unsigned a = std::min(src_a.left()  ,src_b.left());
    Unsigned d = std::max(src_a.right() ,src_b.right());
    
    Unsigned e = std::min(src_a.top()   ,src_b.top());
    Unsigned h = std::max(src_a.bottom(),src_b.bottom());

    Point ae(a,e); 
    Point dh(d,h); 

    return Box(ae,dh);     
  }


  inline Unsigned Box::x() const
  {
    return _x;
  }
    

  inline Unsigned Box::y() const
  {
    return _y;
  }


  inline Unsigned Box::w() const
  {
    return _w;
  }


  inline Unsigned Box::h() const
  {
    return _h;
  }


  inline Real Box::e() const
  {
    return _e;
  }


  inline void Box::setX(Unsigned x)
  {
    _x = x;
    _tl.setX(_x);
    _br.setX(_x+_w);
  }
    

  inline void Box::setY(Unsigned y)
  {
    _y = y;
    _tl.setY(_y);
    _br.setY(_y + _h);
  }


  inline void Box::setW(Unsigned w)
  {
    _w = w;
    _tl.setX(_x);
    _br.setX(_x+_w);
  }


  inline void Box::setH(Unsigned h)
  {
    _h = h;
    _tl.setY(_y);
    _br.setY(_y + _h);
  }


  inline void Box::setE(Real e)
  {
    _e = e;
  }


  inline void Box::include(const Point& p)
  {
    if(_x > p.x())
    {
      if(_x !=  std::numeric_limits<Unsigned>::max())
	setW( _w + _x - p.x() +1);
      setX( p.x() );
    }
    
    if(_y > p.y())
    {
      if( _y !=  std::numeric_limits<Unsigned>::max() )
	setH(_h + _y - p.y() +1);
      setY( p.y() );
    }

    if(_x + _w < p.x())
      setW( p.x() - _x +1);

    if(_y + _h < p.y())
      setH( p.y() - _y +1);
  }


  inline void Box::clear()
  {
    _x = std::numeric_limits<Unsigned>::max(); 
    _y = std::numeric_limits<Unsigned>::max();
    _w = 0;
    _h = 0;    
    _e = 0.0;
    _tl.setX(_x);
    _br.setX(_x + _w);
    _tl.setY(_y);
    _br.setY(_y + _h);
  }


  inline bool Box::empty() const
  {
    return ((_w==0) || (_h==0));
  }


  inline Unsigned Box::size() const
  {
    return (_w*_h);
  }


  inline Point Box::topLeft() const
  {
    return _tl;
  }
 

  inline Point Box::bottomRight() const
  {
    return _br;
  }


  inline Unsigned Box::left() const
  {
    return _x;
  }


  inline Unsigned Box::right() const
  {
    return _x+_w;
  }


  inline Unsigned Box::top() const
  {
    return _y;
  }


  inline Unsigned Box::bottom() const
  {
    return _y+_h;
  }


  inline bool Box::inside(const Point& p) const
  {
    return ((p.x() >= left() && p.x() < right()) && (p.y() >= top() && p.y() < bottom()));
  }


  inline bool Box::inside(const Box& src) const
  {
    return ((src.topLeft() >= topLeft()) && (src.bottomRight() <= bottomRight()));
  }


  inline bool Box::interior(const Point& p) const
  {
    // notice the minux one!
    return ((p.x()> left() && p.x() < right()-1) && (p.y() > top() && p.y() < bottom()-1));
  }


  inline bool Box::interior(const Box& src) const
  {
    return ((src.topLeft() > topLeft()) && (src.bottomRight() < bottomRight()));
  }


  inline bool Box::intersect(const Box& src) const
  {
    return !( 
	     ( src.left()   >= right()  ) ||
	     ( src.right()  <= left()   ) ||
	     ( src.top()    >= bottom() ) ||
	     ( src.bottom() <= top()    )
	    ); 	    
  }


  inline bool Box::operator==(const Box& src) const
  {
    return ( 
	    ( src.x() == x()  ) &&
	    ( src.y() == y()  ) &&
	    ( src.w() == w()  ) &&
	    ( src.h() == h()  )); 	    
  }


  inline void Box::limit(const Box& b)
  {
    Unsigned brx = _x + _w;
    Unsigned bry = _y + _h;
    
    brx = std::min(brx,b._x+b._w);
    bry = std::min(bry,b._y+b._h);
    
    setX( std::max(_x,b._x) );
    setY( std::max(_y,b._y) );

    setW( brx - _x);
    setH( bry - _y);
  }


} // CA



#endif	// _CA_BOX_HPP_
