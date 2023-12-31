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

//! \file TimePlot.cpp
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2013-03


#include"ca2D.hpp"
#include"ArgsData.hpp"
#include"TimePlot.hpp"
#include"Utilities.hpp"
#include<iostream>
#include<fstream>


// Initialise the TimePlot structure using a CSV file.
int initTimePlotFromCSV(const std::string& filename, TimePlot& tp)
{
    std::ifstream ifile(filename.c_str());

    if (!ifile)
    {
        std::cerr << "Error opening CSV file: " << filename << std::endl;
        return 1;
    }

    // Get the input file name.
    tp.filename = filename;

    // Parse the file line by line until the end of file 
    // and retrieve the tokens of each line.
    while (!ifile.eof())
    {
        // If true the token was identified;
        bool found_tok = false;

        std::vector<std::string> tokens(CA::getLineTokens(ifile, ','));

        // If the tokens vector is empty, we reached the eof or an
        // empty line... continue.
        if (tokens.empty())
            continue;

        if (CA::compareCaseInsensitive("Time Plot Name", tokens[0], true))
        {
            std::string str;
            READ_TOKEN(found_tok, str, tokens[1], tokens[0]);

            tp.name = CA::trimToken(str);
        }

        if (CA::compareCaseInsensitive("Physical Variable", tokens[0], true))
            READ_TOKEN(found_tok, tp.pv, tokens[1], tokens[0]);

        if (CA::compareCaseInsensitive("Points Name", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                std::string str;
                READ_TOKEN(found_tok, str, tokens[i], tokens[0]);

                tp.pnames.push_back(CA::trimToken(str));
            }
        }

        if (CA::compareCaseInsensitive("Points X Coo", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                CA::Real value;
                READ_TOKEN(found_tok, value, tokens[i], tokens[0]);

                tp.xcoos.push_back(value);
            }
        }

        if (CA::compareCaseInsensitive("Points Y Coo", tokens[0], true))
        {
            found_tok = true;
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                CA::Real value;
                READ_TOKEN(found_tok, value, tokens[i], tokens[0]);

                tp.ycoos.push_back(value);
            }
        }

        if (CA::compareCaseInsensitive("Period", tokens[0], true))
            READ_TOKEN(found_tok, tp.period, tokens[1], tokens[0]);

        // If the token was not identified stop!
        if (!found_tok)
        {
            std::cerr << "Element '" << CA::trimToken(tokens[0]) << "' not identified" << std::endl; \
                return 1;
        }
    }

    return 0;
}


TPManager::TPManager(CA::Grid&  GRID, CA::CellBuffReal&  ELV,
    const std::vector<TimePlot>& tps,
    const std::string& base, std::vector<std::string> names) :
    _grid(GRID),
    _elv(ELV),
    _tps(tps),
    _datas(tps.size())
{
    for (size_t i = 0; i < _tps.size(); ++i)
    {
        std::string filename(base + "_" + names[i]);
        initData(filename, _tps[i], _datas[i]);
    }
}


TPManager::~TPManager()
{
}


void TPManager::output(CA::Real t, CA::Unsigned iter, CA::CellBuffReal& WD, CA::CellBuffReal& V, bool output)
{
    // This variable is used to indicate if the output to console
    // happen in the case of time plot.
    bool outputed = false;

    // Loop through the time plot data
    for (size_t i = 0; i < _datas.size(); ++i)
    {
        // Check if it is time to plot and the file is good.
        if (t >= _datas[i].time_next && _datas[i].file->good())
        {
            if (!outputed && output)
            {
                std::cout << "Update Time Plot  (MIN " << t / 60 << "): ";
                outputed = true;
            }

            switch (_tps[i].pv)
            {
            case PV::VEL:
            {
                if (output)
                    std::cout << " VEL";

                // Retrieve the speed
                V.retrievePoints(_datas[i].pl, &(_datas[i].pvals[0]), _datas[i].pl.size());

                (*_datas[i].file) << iter << ", " << t / 60.0 << ", ";
                // Write the speed
                for (CA::Unsigned p = 0; p < _datas[i].pl.size(); p++)
                {
                    (*_datas[i].file) << _datas[i].pvals[p] << ", ";
                }
                (*_datas[i].file) << std::endl;
            }
            break;
            case PV::WL:
            {
                if (output)
                    std::cout << " WL";

                // Retrieve the water depth
                WD.retrievePoints(_datas[i].pl, &(_datas[i].pvals[0]), _datas[i].pl.size());

                (*_datas[i].file) << iter << ", " << t / 60.0 << ", ";
                // Write the water level by adding the previously saved elevation.
                for (CA::Unsigned p = 0; p < _datas[i].pl.size(); p++)
                {
                    (*_datas[i].file) << _datas[i].pelvs[p] + _datas[i].pvals[p] << ", ";
                }
                (*_datas[i].file) << std::endl;
            }
            break;
            case PV::WD:
            {
                if (output)
                    std::cout << " WD";

                WD.retrievePoints(_datas[i].pl, &(_datas[i].pvals[0]), _datas[i].pl.size());

                (*_datas[i].file) << iter << ", " << t / 60.0 << ", ";
                for (CA::Unsigned p = 0; p < _datas[i].pl.size(); p++)
                {
                    (*_datas[i].file) << _datas[i].pvals[p] << ", ";
                }
                (*_datas[i].file) << std::endl;
            }
            break;
            default:
                break;
            }

            // Update the next time to plot.
            _datas[i].time_next += _tps[i].period;
        }
    }

    if (outputed && output)
        std::cout << std::endl;
}


int TPManager::initData(const std::string& filename, const TimePlot& tp, Data& tpdata)
{
    // Create file
    tpdata.filename = filename;
    tpdata.file.reset(new std::ofstream(filename.c_str()));

    if (!tpdata.file->good())
        return 1;

    // Set manipulators
    tpdata.file->setf(std::ios::fixed, std::ios::floatfield);
    tpdata.file->precision(6);

    // Write the header
    (*tpdata.file) << "Iter, Time (min), ";

    // Write point name.
    switch (tp.pv)
    {
    case PV::WD:
    case PV::WL:
    case PV::VEL:
        // Only once for water depth and water level.
        for (size_t p = 0; p < tp.pnames.size(); p++)
        {
            (*tpdata.file) << tp.pnames[p] << ", ";
        }
        break;
    default:
        break;
    }
    (*tpdata.file) << std::endl;

    // Loop through coordinates
    for (size_t p = 0; p < tp.pnames.size(); p++)
    {
        tpdata.pl.add(CA::Point::create(_grid, tp.xcoos[p], tp.ycoos[p]));
    }

    // Create buffer where to store the point data.
    tpdata.pvals.resize(tpdata.pl.size());

    // If we need to save the water level.
    if (tp.pv == PV::WL)
    {
        // Create buffer where to store the elevation data.
        tpdata.pelvs.resize(tpdata.pl.size());
        // Retrieve the levation data.
        _elv.retrievePoints(tpdata.pl, &(tpdata.pelvs[0]), tpdata.pl.size());
    }

    if (tp.period > 0.0)
        tpdata.time_next = tp.period;
    else
        tpdata.time_next = std::numeric_limits<CA::Real>::max();

    return 0;
}
