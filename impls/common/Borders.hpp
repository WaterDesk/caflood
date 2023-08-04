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

#ifndef _CA_BORDER_HPP_
#define _CA_BORDER_HPP_


//! \file Border.hpp
//! Contains the class that identifies a single or multiple segments in
//! the borders of the grid.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"BaseTypes.hpp"
#include<limits>
#include<vector>


namespace CA {
  
  //! Enum which identifies the four borders of a square regolar grid.
  enum Border
  {
    Right  = 1,
    Top    = 2,
    Left   = 3,
    Bottom = 4    
  };

  //! Enum which identifies the four corners of a square regolar grid.
  enum Corner
  {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
  };
 

  //! Global variable which identify that a segment include all the borders.
  const Unsigned caBStart = 0;
  const Unsigned caBStop  = std::numeric_limits<Unsigned>::max();
  

  //! Border class Identifies a single or multiple segments of the
  //! borders of the grid. Furthemore, it can identify corners of
  //! the border.  These segments and corners can then be used to perform a
  //! border action such as set a value, wrap values, etc.

  //! A segment is identified by:
  //! 1) the border which can be the top, bottom, righ or left border

  //! 2) the starting and end point which can have value from 0 to the
  //! number of cells in the border. 

  //! In order to indentify a segment that take all the border,
  //! caBStart and caBStop values can be used.

  //! \attention A segment cannot include a corner of a border. A special
  //! method is needed to add a corner in the border.
  
  //! \warning The position (0,0) in the Grid correspond to the
  //! top-left corner of the grid.

  class Borders
  {

  public:

    //! Identifies a segment in a border.
    struct Segment
    {
      Border type;		// One of the fourn border.
      Unsigned     start;		// The starting position.
      Unsigned     stop;		// The stopping position.

      Segment(Border t, Unsigned b, Unsigned e):type(t), start(b), stop(e){}
      ~Segment(){}
    };
    
    //! Define the type of a container of Segments.
    typedef std::vector<Segment> SegmentsList;

    //! Define the type of a container of Corner.
    typedef std::vector<Corner>  CornersList;


    //! Create a empty borders
    Borders();


    //! Destroy the borders.
    ~Borders();


    //! Return the number of segments in the borders.
    int numSegments() const;


    //! Return the number of corner in the borders.
    int numCorners() const;


    //! Return the given segment of the border.
    const Segment& segment(int num) const;

    
    //! Return the given corner of the border.
    Corner  corner(int num) const;
    

    //! Add a segment into the borders.

    //! \attention If start and stop are not passed, the default is to
    //! create a segment a big as the border.
    //! \param border One of the four border.
    //! \param start    The starting point.
    //! \param stop     The stoppping point.
    void addSegment(Border border, Unsigned start = caBStart, Unsigned stop = caBStop);

    //! Add a corner into the borders.
    void addCorner(Corner c);


  private:

    //! List of segments in the borders.
    SegmentsList _segments;
    
    //! List of corner in the borders.
    CornersList  _corners;
  };


  /// ----- Inline implementation ----- ///
  

  inline Borders::Borders():
    _segments(), _corners()
  {    
  }


  inline Borders::~Borders()
  {
  }



  inline int Borders::numSegments() const
  {
    return static_cast<int>(_segments.size());
  }


  inline int Borders::numCorners() const
  {
    return static_cast<int>(_corners.size());
  }



  inline const Borders::Segment& Borders::segment(int num) const
  {
    return _segments[num];
  }

    

  inline Corner Borders::corner(int num) const
  {
    return _corners[num];
  }
    


  inline void Borders::addSegment(Border border, Unsigned start, Unsigned stop)
  {
    _segments.push_back(Segment(border,start,stop));
  }


  inline void Borders::addCorner(Corner c)
  {
    _corners.push_back(c);
  }


} // CA



#endif	// _CA_BORDERS_HPP_
