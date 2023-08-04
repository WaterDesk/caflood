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

#ifndef _CA_OPTIONS_HPP_
#define _CA_OPTIONS_HPP_

//! \file Options.hpp 
//! Contains the list of implementation specific Options
//! \author Michele  Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2012-07


#include"Common.hpp"


namespace CA {


  //! Return a list of implementation specific Options  
  inline Options options()
  {
    Options options;

    // Add the optional arguments. The tag start from 1000 for safety
    // reason.
    CA::Unsigned na = 1000;

    options.push_back(new Arguments::Arg(na++,"config-file",
					 "Select the OpenCL config CSV file.","",true, true, false));

    options.push_back(new Arguments::Arg(na++,"device-type",
					 "Select the OpenCL device type (CPU/GPU).","GPU",true, true, false));

    options.push_back(new Arguments::Arg(na++,"device-number",
					 "Select the OpenCL device from many available.","0",true, true, false));

    options.push_back(new Arguments::Arg(na++,"platform-name",
					 "Select the OpenCL specific platform","",true, true, false));

    options.push_back(new Arguments::Arg(na++,"warp-size",
					 "Select the OpenCL warp size (32).","",true, true, false));

    options.push_back(new Arguments::Arg(na++,"out-of-order",
					 "Uses an out of order execution.","",true, false, false));

    options.push_back(new Arguments::Arg(na++,"build-info",
					 "Show in standard error the OpenCl buidl info","",true, false, false));

    return options;
  }

}

#endif// _CA_BASETYPES_HPP_
