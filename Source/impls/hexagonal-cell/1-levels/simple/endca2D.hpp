/*

Copyright (c) 2016 Centre for Water Systems,
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
//! \author Luís de Sousa, EAWAG,
//! contact: luis.desousa [at] eawag.ch
//! \date 2016-05

#include"Grid.hpp"
#include"../../../common/Coo.hpp"
#include"../../../common/Point.hpp"


namespace CA {


  inline void Point::setCoo(const Grid& grid) const
  {
    // Retrieve x and y coordinate (attention Y is inverted).
    _coo.setX( static_cast<_caReal>( grid.xCoo() + (_x + 0.5)* grid.length() ) );
    _coo.setY( static_cast<_caReal>( grid.yCoo() + ((grid.yNum() - _y - 0.5) * grid.length()) ) );
  }
  
  
  inline Point Point::create(const Grid& grid, Real x_coo, Real y_coo)
  {
	  // windows doesn't like the use of the array inside the map for some reason, adapting code
	//std::map<Real, std::array<Unsigned, 2> > centroids;
	  std::map<Real, Point> centroids;
	Coo param(x_coo, y_coo);
	Unsigned x,y;
    Unsigned x_principal = static_cast<Unsigned>( std::floor( (x_coo - grid.xCoo())/(grid.length()*3/2) ) );
    Unsigned y_principal = static_cast<Unsigned>( std::floor( (y_coo - grid.yCoo())/grid.distance() ) );

    Coo c1(x_principal * (grid.length()*3/2), y_principal * grid.distance());
    if(!(x_principal % 2)) c1.setY(c1.y() + grid.distance() / 2);
    //centroids[c1.distanceTo(param)] = {x_principal, y_principal};
	centroids[c1.distanceTo(param)] = Point( x_principal, y_principal );

    Coo c2(x_principal * (grid.length()*3/2), (y_principal - 1) * grid.distance());
    if(!(x_principal % 2)) c2.setY(c2.y() + grid.distance() / 2);
    //centroids[c2.distanceTo(param)] = {x_principal, y_principal - 1};
	centroids[c2.distanceTo(param)] = Point( x_principal, y_principal - 1 );

    Coo c3((x_principal - 1) * (grid.length()*3/2), (y_principal - 1) * grid.distance());
	if(!((x_principal - 1) % 2)) c3.setY(c3.y() + grid.distance() / 2);
	//centroids[c3.distanceTo(param)] = {x_principal - 1, y_principal - 1};
	centroids[c3.distanceTo(param)] = Point( x_principal - 1, y_principal - 1 );

	Coo c4((x_principal - 1) * (grid.length()*3/2), y_principal * grid.distance());
	if(!((x_principal - 1) % 2)) c4.setY(c4.y() + grid.distance() / 2);
	//centroids[c4.distanceTo(param)] = {x_principal - 1, y_principal};
	centroids[c4.distanceTo(param)] = Point( x_principal - 1, y_principal );

	//x = centroids.begin()->second[0];
	x = centroids.begin()->second.x();
	//y = centroids.begin()->second[1];
	y = centroids.begin()->second.y();

    /// Y is inverted
    /// Y cannot be zero!
    y = std::max(grid.yNum() - y - 1, static_cast<Unsigned>(1));
    /// X cannot be zero!
    x = std::max(x, static_cast<Unsigned>(1));

    Point p(x,y);
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
    Point tl( Point::create(grid,top_left_x_coo    , top_left_y_coo    ) );
    Point br( Point::create(grid,bottom_right_x_coo, bottom_right_y_coo) );

    return Box(tl,br);
  }



} // CA

#endif  // _CA_ENDCA2D_HPP_	
