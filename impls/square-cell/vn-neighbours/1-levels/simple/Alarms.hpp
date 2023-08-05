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

#ifndef _CA_ALARMS_HPP_
#define _CA_ALARMS_HPP_


//! \file Alarms.hpp
//! Define a set of alarms object.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"Grid.hpp"
#include<limits>
#include<iostream>
#include<cstdlib>
#include"caapi2D.hpp"


namespace CA {


    //! Define a set of alarms. Alarms can be activated (only) inside a
    //! CA function, however they cannot be de-activated of checked
    //! inside the CA function but only in the main file.

    //! \attention The CA_GLOB_ALARMS used in the CA function can be
    //! different from this class. 

    class Alarms : public CA::Uncopyable
    {
    public:

        //! Create the set of alarms. It is possible to have implementation
        //! specific options set by using the options list. \attention Do
        //! not destroy the grid before destroying this buffer.
        //! The alarms are all deactivated.
        //! \param grid    The Grid
        //! \param num     The number of alarms.
        //! \param options The list of implementation specific options.
        Alarms(Grid& grid, Unsigned num, const Options& options = Options());

        //! Destroy the alarms.
        virtual ~Alarms();

        //! Return the specific options about the Alarm object and this
        //! implementation.
        static Options options();

        //! Deactivate all the alarms in the set.
        void deactivateAll();

        //! Deactivate the specific alarm.
        void deactivate(Unsigned n);

        //! Set alarms states after deactivation. \attention. This method
        //! must be called after the deactivation of an alarm.
        void set();

        //! Get alarms states before checking the
        //! activation. \attention. This method must be called before the
        //! checking of an alarm.
        void get();

        //! Check if a specific alarm was activated.
        bool isActivated(Unsigned n) const;

        //! Check if all the alarms in the set were activated.
        bool areAllActivated() const;

        //! Return the Alarms as an array where each index is true (~0) if
        //! the respective alarm is activated.
        const char* isActivatedArray() const;

        // ---------  Implementation dependent --------------

        // Convert the set of allarms into the CA_GLOB_ALARMS used in the CA function
        operator char*() { return &_alarms[0]; }

    private:

        //! The set of alarms which is a vector of char.
        std::vector<char> _alarms;
    };


    /// ----- Inline implementation ----- ///


    inline Alarms::Alarms(Grid& grid, Unsigned num, const Options& options) :
        _alarms(num)
    {
        // GRID, FLAGS and OPTIONS are not used, it is just there for conformity.
    }


    inline Alarms::~Alarms()
    {
    }


    inline Options Alarms::options()
    {
        Options options;

        // Add the optional arguments. The tag start from higher number
        // for safety reason.
        CA::Unsigned na = 5000;

        // Example
        //options.push_back(new Arguments::Arg(na++,"option-name", "Option desctiption","value",true, true, false));

        return options;
    }


    inline void Alarms::deactivateAll()
    {
        for (Unsigned i = 0; i < _alarms.size(); ++i)
        {
            _alarms[i] = 0;
        }
    }


    inline void Alarms::deactivate(Unsigned n)
    {
        _alarms[n] = 0;
    }


    inline void Alarms::set()
    {
        // do nothing.
    }


    inline void Alarms::get()
    {
        // do nothing.
    }


    inline bool Alarms::isActivated(Unsigned n) const
    {
        return (_alarms[n] > 0);
    }


    inline bool Alarms::areAllActivated() const
    {
        for (Unsigned i = 0; i < _alarms.size(); ++i)
        {
            if (_alarms[i] == 0)
                return false;
        }
        return true;
    }

    inline const char* Alarms::isActivatedArray() const
    {
        return &_alarms[0];
    }
}

#endif  // _CA_ALARMS_HPP_
