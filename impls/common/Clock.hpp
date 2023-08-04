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

//! \file Clock.hpp
//! Contain the class Clock which can be used to calculate the elapsed
//! time of a computation in at least one microsecond precision.
//! \author Michele Guidolin, University of Exeter,
//! Mike Gibson, University of Exeter,
//! contact: m.guidolin [at] exeter.ac.uk,
//! mg278@ex.ac.uk
//! \date 2012-01



#ifndef _CA_CLOCK_HPP_
#define _CA_CLOCK_HPP_



#if defined _WIN32 
	#ifndef NOMINMAX
		#define NOMINMAX          //!< This macro stop a name clash.		
	#endif

	#include <windows.h>
#else
	#include <sys/time.h>
#endif


namespace CA{


  // The Clock class is used to calculate the elpased time of a
  // computation. The precison should be at least one microsecond.

  class Clock
  {    
  public:
        
    //! Construct the object and start the clock.
    Clock();

 
    //! Destructor. 
    ~Clock(){}


    //! Return the elapsed time in millisecond.
    double millisecond() const;
    
   
  private:    

    //! Contain the starting time of the clock.
#if defined _WIN32 
    LARGE_INTEGER  _start;
#else
    struct timeval _start;    
#endif
  };


  // --- INLINE IMPLEMENTATION -- //  
  

  inline Clock::Clock()
  {
    // retrieve the starting time.
#if defined _WIN32 
    ::QueryPerformanceCounter(&_start);
#else
    gettimeofday(&_start, (struct timezone *) NULL);
#endif    
  }


  inline double Clock::millisecond() const
  {
    // retrieve the stop time.
#if defined _WIN32 
    LARGE_INTEGER  _stop;
    LARGE_INTEGER  _freq;
    ::QueryPerformanceFrequency(&_freq);
    ::QueryPerformanceCounter(&_stop); 
    
    return (static_cast<double>((_stop.QuadPart - _start.QuadPart)) / 
	    static_cast<double>(_freq.QuadPart))*1000.0;
#else
    struct timeval _stop;    
    gettimeofday(&_stop, (struct timezone *) NULL);

    struct timeval temp;
    if ((_stop.tv_usec-_start.tv_usec)<0)
    {
      temp.tv_sec  = _stop.tv_sec - _start.tv_sec - 1;
      temp.tv_usec = 1000000+_stop.tv_usec-_start.tv_usec;    
    }
    else
    {
      temp.tv_sec  = _stop.tv_sec  - _start.tv_sec;
      temp.tv_usec = _stop.tv_usec - _start.tv_usec;
    }
    
    return static_cast<double>(temp.tv_sec)* 1000.0 + static_cast<double>(temp.tv_usec) * 1.0e-3;
#endif    
    
  }

}


#endif // _CA_CLOCK_HPP_	  
