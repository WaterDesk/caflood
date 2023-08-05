/*

Copyright (c) 2016 - Centre for Water Systems,
                     University of Exeter
                   - EAWAG - Swiss Federal Institute of Water Science
                           and Technology

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
//! \author Michael J Gibson, University of Exeter, 
//! contact: m.j.gibson [at] exeter.ac.uk 
//! \date 2016-04


#include"Common.hpp"


namespace CA {


    //! Return a list of implementation specific Options  
    inline Options options()
    {
        return Options();
    }

}

#endif  // _CA_OPTIONS_HPP_
