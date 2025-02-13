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

//! \file Inflow.cpp
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2013-03


#include"ca2D.hpp"
#include"ArgsData.hpp"
#include"Inflow.hpp"
#include"Utilities.hpp"
#include<iostream>
#include<fstream>

// -------------------------//
// Include the CA 2D functions //
// -------------------------//
#include CA_2D_INCLUDE(computeArea)
#include CA_2D_INCLUDE(addInflow)


int initIEventFromCSV(const std::string& filename, IEvent& ie)
{
    ie.u = 0.0;
    ie.n = 0.01;

    std::ifstream ifile(filename.c_str());

    if (!ifile)
    {
        std::cerr << "Error opening CSV file: " << filename << std::endl;
        return 1;
    }

    // Parse the file line by line until the end of file 
    // and retrieve the tokens of each line.
    while (!ifile.eof())
    {

        // If true the token was identified;
        bool found_tok = false;

        std::vector<std::string> tokens(CA::getLineTokens(ifile, ','));

        // If the tokens vector is empty we reached the eof or an
        // empty line... continue.
        if (tokens.empty())
            continue;

        if (CA::compareCaseInsensitive("Event Name", tokens[0], true))
        {
            std::string str;
            READ_TOKEN(found_tok, str, tokens[1], tokens[0]);

            ie.name = CA::trimToken(str, " \t\r");
        }

        if (CA::compareCaseInsensitive("Inflow", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                CA::Real value;
                READ_TOKEN(found_tok, value, tokens[i], tokens[0]);

                ie.ins.push_back(value);
            }
        }

        if (CA::compareCaseInsensitive("Time", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                CA::Real value;
                READ_TOKEN(found_tok, value, tokens[i], tokens[0]);

                ie.times.push_back(value);
            }
        }

        if (CA::compareCaseInsensitive("Area", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                CA::Real value;
                READ_TOKEN(found_tok, value, tokens[i], tokens[0]);

                ie.area.push_back(value);
            }
        }

        if (CA::compareCaseInsensitive("Zone", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                CA::Real value;
                READ_TOKEN(found_tok, value, tokens[i], tokens[0]);

                ie.zone.push_back(value);
            }
        }

        if (CA::compareCaseInsensitive("Analytical Solution U", tokens[0], true))
        {
            found_tok = true;
            READ_TOKEN(found_tok, ie.u, tokens[1], tokens[0]);
        }

        if (CA::compareCaseInsensitive("Analytical Solution N", tokens[0], true))
        {
            found_tok = true;
            READ_TOKEN(found_tok, ie.n, tokens[1], tokens[0]);
        }

        // If the token was not identified stop!
        if (!found_tok)
        {
            std::cerr << "Element '" << CA::trimToken(tokens[0]) << "' not identified" << std::endl; \
                return 1;
        }
    }

    return 0;
}


InflowManager::InflowManager(CA::Grid&  GRID, const std::vector<IEvent>& ies) :
    _grid(GRID),
    _ies(ies),
    _datas(ies.size())
{
    for (size_t i = 0; i < _ies.size(); ++i)
    {
        initData(_ies[i], _datas[i]);
    }
}


InflowManager::~InflowManager()
{
}


//! Add the computational domain of the inflow events into the given domain.
void InflowManager::addDomain(CA::BoxList& compdomain)
{
    for (size_t i = 0; i < _datas.size(); ++i)
    {
        compdomain.add(_datas[i].box_area);
    }
}


void InflowManager::analyseArea(CA::CellBuffReal& TMP, CA::CellBuffState& MASK, CA::BoxList&  domain)
{
    for (size_t i = 0; i < _datas.size(); ++i)
    {
        TMP.fill(domain, 0.0);
        CA::Execute::function(_datas[i].box_area, computeArea, _grid, TMP, MASK);
        TMP.sequentialOp(_datas[i].box_area, _datas[i].grid_area, CA::Seq::Add);
    }
}


void InflowManager::prepare(CA::Real t, CA::Real period_time_dt, CA::Real next_dt)
{
    // Loop through the inflow event(s).
    for (size_t i = 0; i < _ies.size(); ++i)
    {
        // Compute the difference of inflow between the expected and the
        // added. This inflow should be added/subtracted as one off when it
        // reaches high value.
        _datas[i].one_off_inflow = (_datas[i].expected_inflow - _datas[i].total_inflow);

        // Set the volume to zero.
        _datas[i].volume = 0.0;

        size_t index = _datas[i].index;

        // If the index is larger than the available ins/time, do
        // nothing.
        if (index >= _ies[i].ins.size())
            continue;

        // Compute the total volume for the period time.
        CA::Real volume = 0;
        if (index != _ies[i].ins.size() - 1)
        {
            CA::Real y0 = _ies[i].ins[index];
            CA::Real y1 = _ies[i].ins[index + 1];
            CA::Real x0 = _ies[i].times[index];
            CA::Real x1 = _ies[i].times[index + 1];
            CA::Real t0 = t;
            CA::Real t1 = t + period_time_dt;
            CA::Real yt0 = y0 + (y1 - y0) * ((t0 - x0) / (x1 - x0));
            CA::Real yt1 = y0 + (y1 - y0) * ((t1 - x0) / (x1 - x0));
            volume = 0.5*(t1 - t0)*(yt1 - yt0) + (t1 - t0)*(yt0);
        }

        _datas[i].volume = volume;

        // The expected amount of inflow for the next period.
        _datas[i].expected_inflow = volume;

        // Reset the total amount of inflow for the next period.
        _datas[i].total_inflow = 0.0;

    }
}


CA::Real InflowManager::volume()
{
    CA::Real inflow_volume = 0.0;

    // Loop through the inflow event(s).
    for (size_t i = 0; i < _ies.size(); ++i)
        inflow_volume += _datas[i].volume;

    return inflow_volume;
}


void InflowManager::add(CA::CellBuffReal& WD, CA::CellBuffState& MASK, CA::Real t, CA::Real dt)
{
    // Loop through the inflow event(s).
    for (size_t i = 0; i < _ies.size(); ++i)
    {
        // ANALYTICAl SOLUTION!
        // If the value of U is set (differ from zero). Then the
        // analytical solution is compute with C=0
        if (_ies[i].u != 0)
        {
            CA::Real pn = std::pow(_ies[i].n, 2);
            CA::Real pu = std::pow(_ies[i].u, 3);
            CA::Real level_now = std::pow((7.0 / 3.0)*(0 - pn * pu*(-_ies[i].u*(t))), (3.0 / 7.0));
            CA::Real level_prev = std::pow((7.0 / 3.0)*(0 - pn * pu*(-_ies[i].u*(t - dt))), (3.0 / 7.0));
            CA::Real volume = _ies[i].u*((level_now + level_prev) / 2)*_grid.length()*dt; // per cell!!!

            CA::Execute::function(_datas[i].box_area, addInflow, _grid, WD, MASK, volume);
            continue;
        }

        size_t index = _datas[i].index;

        // If the index is larger than the available ins/time, do
        // nothing.
        if (index >= _ies[i].ins.size())
            continue;

        // Compute the inflow volume at specific time using
        // interpolation. Check if the index is the last available
        // one, then there is no inflow.
        CA::Real volume = 0;
        if (index != _ies[i].ins.size() - 1)
        {
            CA::Real y0 = _ies[i].ins[index];
            CA::Real y1 = _ies[i].ins[index + 1];
            CA::Real x0 = _ies[i].times[index];
            CA::Real x1 = _ies[i].times[index + 1];
            CA::Real t0 = t - dt;
            CA::Real t1 = t;
            CA::Real yt0 = y0 + (y1 - y0) * ((t0 - x0) / (x1 - x0));
            CA::Real yt1 = y0 + (y1 - y0) * ((t1 - x0) / (x1 - x0));
            volume = 0.5*(t1 - t0)*(yt1 - yt0) + (t1 - t0)*(yt0);
        }

        // Increase the amount of inflow added into the period.
        // It needs to be added before the correction.
        _datas[i].total_inflow += volume;

        // Correct the volume
        volume = static_cast<double>(volume) + _datas[i].one_off_inflow;
        _datas[i].one_off_inflow = 0.0; // Reset the one of inflow.


        // ATTENTION The volume is the total volume, it needs to be
        // divided by the number of cells that are going to receive the
        // inflow. 
        if (_datas[i].grid_area == 0.0)
            continue;
        volume = volume / (_datas[i].grid_area / (_grid.area()));

        // Add (or subtract) the given volume into the water depth of the
        // given area.
        // Do not add it if it is zero.
        if (volume >= SMALL_INFLOW)
        {
            CA::Execute::function(_datas[i].box_area, addInflow, _grid, WD, MASK, volume);
        }

        // Check if the simulation time now is equal or higher than the
        // time of the NEXT index.
        if (index < _ies[i].times.size() - 1 && t >= _ies[i].times[index + 1])
            index++;

        // Update index.
        _datas[i].index = index;
    }
}


CA::Real InflowManager::potentialVA(CA::Real t, CA::Real period_time_dt)
{
    CA::Real potential_va = 0.0;

    // Compute the total amount of volume that is going to be added into
    // each cell for the time period. Calculate the extra amount of
    // water depth that would be added in a second. Compute the possible velocity
    // using the critical velocity.
    for (size_t i = 0; i < _ies.size(); ++i)
    {
        size_t index = _datas[i].index;

        // If the index is larger than the available ins/time, do
        // nothing.
        if (index >= _ies[i].ins.size())
            continue;

        // Compute the total volume for the period time.
        CA::Real volume = 0;
        if (index != _ies[i].ins.size() - 1)
        {
            CA::Real y0 = _ies[i].ins[index];
            CA::Real y1 = _ies[i].ins[index + 1];
            CA::Real x0 = _ies[i].times[index];
            CA::Real x1 = _ies[i].times[index + 1];
            CA::Real t0 = t;
            CA::Real t1 = t + period_time_dt;
            CA::Real yt0 = y0 + (y1 - y0) * ((t0 - x0) / (x1 - x0));
            CA::Real yt1 = y0 + (y1 - y0) * ((t1 - x0) / (x1 - x0));
            volume = 0.5*(t1 - t0)*(yt1 - yt0) + (t1 - t0)*(yt0);
        }

        // ATTENTION The volume is the total volume, to get the volume per
        // cell it needs to be divided by the number of cells that are
        // going to receive the inflow. Then it needs to be divided by the
        // cell area.
        if (_datas[i].grid_area == 0.0)
            continue;
        CA::Real wd = volume / (_datas[i].grid_area*period_time_dt);

        // Compute the potential velocity.
        potential_va = std::max(potential_va, std::sqrt(wd*static_cast<CA::Real>(9.81)));
    }

    // ATTENTION! This does not need to be precise but just give a rough estimation

    return potential_va;
}


CA::Real InflowManager::endTime()
{
    CA::Real t_end = 0.0;

    // Loop through the inflow event(s).
    for (size_t i = 0; i < _ies.size(); ++i)
    {
        // Loop through the time steps.
        for (size_t j = 1; j < _ies[i].times.size(); j++)
        {
            // Check if there is no inflow at this time but there was at
            // previous time updated t_end.
            if (_ies[i].ins[j - 1] > 0.0 && _ies[i].ins[j] == 0)
                t_end = std::max(t_end, _ies[i].times[j]);
        }
    }

    return t_end;
}


int InflowManager::initData(const IEvent& ie, Data& data)
{
    data.index = 0;

    // If the area vector does not contain four values, then the box
    // stay empty. NO INFLOW.
    if (ie.area.size() == 4)
    {
        // Compute the given box area.
        data.box_area = CA::Box::create(_grid, ie.area[0], ie.area[1], ie.area[2], ie.area[3]);
    }

    // This is new. The area could have been identified as a zone, i.e. x,y,w,h/
    if (ie.zone.size() == 4)
    {
        // Compute the given box area from the zone.
        CA::Point     tl(CA::Point::create(_grid, ie.zone[0], ie.zone[1]));
        CA::Unsigned  w = static_cast<CA::Unsigned>(std::ceil(ie.zone[2] / _grid.length()));
        CA::Unsigned  h = static_cast<CA::Unsigned>(std::ceil(ie.zone[3] / _grid.length()));

        data.box_area = CA::Box(tl.x(), tl.y(), w, h);
    }

    data.grid_area = 0.0;
    data.volume = 0.0;

    return 0;
}
