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

//! \file CADDIES2D.cpp
//! Perform the CADDIES2D flood modelling 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2013-03


#include"ca2D.hpp"
#include"Masks.hpp"
#include"ArgsData.hpp"
#include"Arguments.hpp"
#include"Setup.hpp"
#include"Rain.hpp"
#include"Inflow.hpp"
#include"WaterLevel.hpp"
#include"TimePlot.hpp"
#include"RasterGrid.hpp"
#include"TSPlot.hpp"
#include<ctime>

typedef void(*SetRunStatusCallbackFuncPtr)(void* owner, const std::string& status);
void(*SetRunStatusCallbackFunc)(void* owner, const std::string& status);
void*      SetRunStatusCallbackOwner;  // The owner pointer which call the callback function

void setRunStatusCallback(SetRunStatusCallbackFuncPtr funcPtr, void* owner)
{
    SetRunStatusCallbackFunc = funcPtr;
    SetRunStatusCallbackOwner = owner;
}


typedef bool(*SetForceStoppedCallbackFuncPtr)(void* owner);
bool(*ForceStoppedCallbackFunc)(void* owner);
void*      ForceStoppedCallbackOwner;  // The owner pointer which call the callback function

void setForceStoppedCallback(SetForceStoppedCallbackFuncPtr funcPtr, void* owner)
{
    ForceStoppedCallbackFunc = funcPtr;
    ForceStoppedCallbackOwner = owner;
}


// WCA2Dv2 model base on:
// CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
//
// Paper name : A WEIGHTED CELLULAR AUTOMATA 2D 
//              INUNDATION MODEL FOR RAPID FLOOD ANALYSIS 
// Authors    : M. Guidolin, A. S. Chen, B. Ghimire, E. Keedwell, 
//              S. Djordjević, D.A. Savić 
//
// CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


/* Round a number to the given digits. */
inline CA::Real fround(CA::Real n, unsigned d)
{
    return std::floor(n * std::pow(static_cast<CA::Real>(10.0), static_cast<CA::Real>(d))
        + static_cast<CA::Real>(.5))
        / std::pow(static_cast<CA::Real>(10.0), static_cast<CA::Real>(d));
}


/* Extend a domain */
inline CA::Box extendBox(CA::Box extent, CA::Box& fullbox, CA::Unsigned lines)
{
    // Check that the new box is not too big by using full.
    if (extent.x() > fullbox.x() + lines)
        extent.setX(extent.x() - lines);
    else
        extent.setX(fullbox.x());
    if (extent.y() > fullbox.y() + lines)
        extent.setY(extent.y() - lines);
    else
        extent.setY(fullbox.y());
    extent.setW(std::min((extent.w() + lines * 2), fullbox.w()));
    extent.setH(std::min((extent.h() + lines * 2), fullbox.h()));

    return extent;
}


//! Output to console the information about the simulation.
void outputConsole(CA::Unsigned iter, CA::Unsigned oiter, CA::Real t, CA::Real dt,
    CA::Real avgodt, CA::Real minodt, CA::Real maxodt,
    CA::Real vamax, CA::Real upstr_elv, const CA::BoxList& domain,
    const Setup& setup)
{
    std::cout << "-----" << std::endl;

    // additional output requested by CIRP
    CA::Real percentage = t / ((CA::Real)(setup.time_end - setup.time_start));
    percentage *= 100.0;

    std::stringstream percentageProcessStream;
    percentageProcessStream.precision(2);
    percentageProcessStream << std::fixed << percentage;
    std::cout << "Progress (%): " << percentageProcessStream.str() << std::endl;

    std::cout << "Total iterations = " << iter << " Simulation time (MIN) = " << t / 60
        << " Last DT = " << dt << std::endl;
    std::cout << "Last iterations  = " << oiter << " Average DT =" << avgodt / oiter
        << " Min DT = " << minodt << " Max DT = " << maxodt << std::endl;
    std::cout << "UPSTRELV = " << upstr_elv << std::endl;
    std::cout << "VAMAX    = " << vamax << std::endl;
    if (setup.expand_domain)
    {
        CA::Box B(domain.extent());
        std::cout << "DOMAIN   = (" << B.x() << "," << B.y() << "):(" << B.w() << "," << B.h() << ")" << std::endl;
    }
    std::cout << "-----" << std::endl;
}


//! Compute the next time step as a fraction of the period time step.
//! Check that dt is between min and max.
void computeDT(CA::Real& dt, CA::Unsigned& dtfrac, CA::Real dtn1, const Setup& setup)
{
    CA::Unsigned dtfracmax = static_cast<CA::Unsigned>(setup.time_maxdt / setup.time_mindt);

    // Find the fraction of time_maxdt second that is just less than dtn1.
    // If dt is smaller than dtn1 we need to decrease the
    // fraction and stop as soon as the result is higher than dtn1.
    // If dt is bigger than dtn1 we need to increase the
    // fraction and stop as soon as the result is lower than dtn1.
    if (dt <= dtn1)
    {
        for (; dtfrac >= 1; dtfrac--)
        {
            CA::Real tmpdt = setup.time_maxdt / static_cast<CA::Real>(dtfrac);
            if (tmpdt >= dtn1)
                break;
        }
        if (dtfrac == 1)
            dt = setup.time_maxdt;
        else
        {
            dtfrac += 1;
            dt = setup.time_maxdt / static_cast<CA::Real>(dtfrac);
        }
    }
    else
    {
        for (; dtfrac <= dtfracmax; dtfrac++)
        {
            CA::Real tmpdt = setup.time_maxdt / static_cast<CA::Real>(dtfrac);
            if (tmpdt <= dtn1)
                break;
        }
        dt = setup.time_maxdt / static_cast<CA::Real>(dtfrac);
    }

    // Check that dt is between min and max.
    dt = std::min(std::max(dt, setup.time_mindt), setup.time_maxdt);
}


// -------------------------//
// Include the CA 2D functions //
// -------------------------//
#include CA_2D_INCLUDE(outflowWCA2Dv1)
#include CA_2D_INCLUDE(waterdepthWCA2Dv1)
#include CA_2D_INCLUDE(velocityWCA2Dv1)
#include CA_2D_INCLUDE(infiltration)

#include CA_2D_INCLUDE(setBoundaryEle)
#include CA_2D_INCLUDE(removeUpstr)
#include CA_2D_INCLUDE(updatePEAKC)
#include CA_2D_INCLUDE(updatePEAKE)

#include CA_2D_INCLUDE(outflowWCA2Dv2)
#include CA_2D_INCLUDE(waterdepth)
#include CA_2D_INCLUDE(velocityDiffusive)

int CADDIES2D(const ArgsData& ad, const Setup& setup, const CA::AsciiGrid<CA::Real>& eg,
    const std::vector<RainEvent>& res, const std::vector<WLEvent>& wles,
    const std::vector<IEvent>& ies,
    const std::vector<TimePlot>& tps, const std::vector<RasterGrid>& rgs)
{
    // Check the model.
    switch (setup.model_type)
    {
    case MODEL::WCA2Dv1:
    case MODEL::WCA2Dv2:
        // OK!
        break;
    default:
        std::cerr << "Error the simulation does not support the model: " << setup.model_type << std::endl;
        return 1;
    }

    if (setup.output_console)
    {
        time_t t = time(0);   // get time now
        struct tm * now = localtime(&t);
        std::cout << "Simulation : " << setup.sim_name << std::endl;
        std::cout << "Model      : " << setup.model_type << std::endl;
        std::cout << "Date Start : " << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << '-' << now->tm_mday
            << " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << std::endl;
        std::cout << "------------------------------------------" << std::endl;
    }

    // ---- Timer ----

    // Get starting time.
    CA::Clock total_timer;

    // ---- CA GRID ----

    // Load the CA Grid from the DataDir. 
    // ATTENTION this should have an extra set of cells in each
    // direction. The internal implementation could be different than a
    // square regular grid.
    CA::Grid  GRID(ad.data_dir, setup.preproc_name + "_Grid", "0", ad.args.active(), 9999);

    if (setup.output_console)
        std::cout << "Loaded Grid data" << std::endl;

    // Print the Grid Information.
    if (setup.output_console)
    {
        std::cout << "-----------------" << std::endl;
        GRID.printInfo(std::cout);
        std::cout << "-----------------" << std::endl;
    }

    // Set if to print debug information on CA function.
    GRID.setCAPrint(false);

    // Create the full (extended) computational domain of CA grid. 
    CA::BoxList  fulldomain;
    CA::Box      fullbox = GRID.box();
    fulldomain.add(fullbox);

    // Create a borders object that contains all the borders and
    // corners of the grid.
    CA::Borders borders;

    // Create the real computational domain of CA grid, i.e. the
    // original DEM size not the extended one.
    CA::BoxList  realdomain;
    CA::Box      realbox(GRID.box().x() + 1, GRID.box().y() + 1, GRID.box().w() - 2, GRID.box().h() - 2);
    realdomain.add(realbox);

    // Create the computational domain, i.e. the area where the water is
    // present and moving.
    CA::BoxList  compdomain;

    // Create the computational domain used for sequential
    // computation. SequentialOp does not work with multiple boxes.
    CA::BoxList  seqdomain(fullbox);

    // -- INITIALISE ELEVATION ---

    // Create the elevation cell buffer.
    // It contains a "real" value in each cell of the grid.
    CA::CellBuffReal  ELV(GRID);

    // Set the border of the elevation buffer to be no data. 
    ELV.bordersValue(borders, eg.nodata);

    // Se the default value of the elevation to be nodata.
    ELV.fill(fulldomain, eg.nodata);

    // Load the data not from the DEM file but from the pre-processed
    // file.
    if (!ELV.loadData(setup.preproc_name + "_ELV", "0"))
    {
        std::cerr << "Error while loading the Elevation pre-processed file" << std::endl;
        return 1;
    }

    // Highest elevation
    CA::Real     high_elv = 90000;

    // Find highest elevation
    ELV.sequentialOp(fulldomain, high_elv, CA::Seq::Max);

    if (setup.output_console)
    {
        std::cout << "Loaded Elevation data" << std::endl;
        std::cout << "Highest elevation = " << high_elv << std::endl;
    }
    // ---- CELL BUFFERS ----

    // Create the water depth cell buffer.
    CA::CellBuffReal WD(GRID);

    // Create the MASK cell buffer. The mask is useful to check which
    // cell has data and nodata and which cell has neighbourhood with
    // data.
    CA::CellBuffState MASK(GRID);

    // Create the velocity cell buffer with the speed of the velocity
    CA::CellBuffReal V(GRID);

    // Create the velocity cell buffer with the angle of the velocity
    CA::CellBuffReal A(GRID);

    // If the we are using the WCA2Dv2 model create the buffer with the
    // dt value.
    cpp11::shared_ptr<CA::CellBuffReal> PDT;

    // Allocate the buffer for the time step
    // Only WCA2Dv1 does not need this.
    if (setup.model_type != MODEL::WCA2Dv1)
    {
        PDT.reset(new CA::CellBuffReal(GRID));
        (*PDT).fill(fulldomain, setup.time_updatedt);
    }

    // ---- EDGES BUFFERS ----

    // Create the outflow edge buffer(s). Many models use a double buffer
    // technique to save time.
    CA::EdgeBuffReal OUTF1(GRID);
    CA::EdgeBuffReal OUTF2(GRID);

    CA::EdgeBuffReal *POUTF1 = &OUTF1;
    CA::EdgeBuffReal *POUTF2 = &OUTF2;

    // Allocate the buffer for total flow for an edge for an update step.
    // Only WCA2Dv1 does need this.
    cpp11::shared_ptr<CA::EdgeBuffReal> PTOT;
    if (setup.model_type == MODEL::WCA2Dv1)
    {
        PTOT.reset(new CA::EdgeBuffReal(GRID));
        (*PTOT).clear();
    }


    // ---- ALARMS ----

    // Create the alarm(s) checked during the outflux computation.
    // Alarm 1: indicates when there is going to be an outflux outside of the computational domain.
    CA::Alarms  OUTFALARMS(GRID, 1);

    // Create the alarm(s) checked during the velocity computation
    // Alarm 1: indicates when there is still water movement over the elevation threshold.
    CA::Alarms  VELALARMS(GRID, 1);


    // ---- SCALAR VALUES ----

    CA::Unsigned iter = 0;              // The actual iteration number.
    CA::Real     t = setup.time_start;  // The actual time in seconds
    CA::Real     dt = setup.time_maxdt; // Starting delta time.
    CA::Real     dtn1 = 0.0;
    CA::Real     nodata = eg.nodata;

    // The simulation time when the events will not produce any further water.
    CA::Real     t_end_events = setup.time_start;

    // The level of water that can be ignored.
    CA::Real     ignore_wd = setup.ignore_wd;

    // The water difference between cell that can be ignored.
    CA::Real     tol_delwl = setup.tolerance;

    // The minimum level of water that is used for computing the
    // velocity. 
    CA::Real     tol_va = std::min(setup.ignore_wd, static_cast<CA::Real>(0.001));

    // The tolerance of the slope used to compute the dt. The slope
    // between two points must be higher than this value.
    // The tolerance is passed in percentile.
    CA::Real     tol_slope = setup.tol_slope / 100;

    // This is the period of when the velocity is computed.
    CA::Real     period_time_dt = setup.time_updatedt;
    CA::Real     time_dt = t + period_time_dt;          // The next simulation time when to update dt.
    CA::Unsigned iter_dt = 0;                  // The number of iteration before next update dt.

    // This is the time from the start of an update dt.
    CA::Real     start_updatedt = 0.0;

    // This is the previous update step dt.
    CA::Real     previous_dt = dt;

    // The fraction of time_maxdt second used to compute the next the dt
    CA::Unsigned dtfrac = 1;

    // The parameter of the time step.
    CA::Real     alpha = setup.time_alpha;

    // The inverse of the roughness
    CA::Real     irough = 1 / setup.roughness_global;

    CA::Unsigned oiter = 0;                 // Output step num of iteration.
    CA::Real     minodt = setup.time_maxdt; // Output step minimum dt.
    CA::Real     maxodt = 0.0;              // Output step maximum dt.
    CA::Real     avgodt = 0.0;              // Average dt;
    CA::Real     time_output = t + setup.output_period; // The time of the next output.

    CA::Real     rain_volume = 0.0;
    CA::Real     inflow_volume = 0.0;
    CA::Real     wd_volume = 0.0;
    CA::Real     inf_volume = 0.0;

    // Maximum velocity.
    CA::Real     vamax = 0.0;

    // Possible dt for WCA2Dv2 model. This is used to compute the
    // possible dt create by an event and created by the outflow.
    CA::Real     possible_dt;

    // Upstream elevation value. This value is the elevation where the
    // water "probably cannot reach anymore".
    CA::Real     upstr_elv = high_elv;

    // The potential velocity that an event has created. This should be
    // used to create multiple loop of FLUX calculation.
    CA::Real     potential_va;

    // If true the outflow computation will check the box cell for a
    // possible expanding domain.
    bool         checkbox = setup.expand_domain;

    // Variable which indicates if the PEAK values need to be updated.
    // The PEAK values can be updated on every update step or on every
    // steps.
    bool UpdatePEAK = false;

    // Variable which indicates if the raster has been saved in the last
    // iteration.
    bool RGwritten = false;

    // If true perform the infiltration step.
    bool useInfiltration = false;

    // The amount of water to remove during an update step.
    CA::Real inf_updatedt = 0.0;

    // -- CREATE FULL MASK ---

    CA::createCellMask(fulldomain, GRID, ELV, MASK, nodata);

    //***********************//
    //* BOUNDARY DISCUSSION *//
    //***********************//

    // The MASK is used to check if a cell with nodata has a neighbour
    // with data (Bit 31 set to true in the MASK). This kind of cells
    // are called boundary cells and they are ignored by the computation
    // (not visited), i.e. the rain is not added into them, the outflow
    // are not computed. However, these boundary cells are used as
    // neighbour cell thus the boundary cell can have inflow (outflow
    // from the data cell). Thus the elevation value in these boundary
    // cell is changed with the Boundary Elevation value. If this value
    // is very high the global boundary is a CLOSED one, if it is VERY
    // negative the global boundary is an OPEN one.

    // ATTENTION The water that finish in the boundary cell (open
    // boundary case) is not removed (it stays in the WD buffer). 

    // Set the boundary cell elevation to the given boundary_elv value.
    CA::Execute::function(fulldomain, setBoundaryEle, GRID, ELV, MASK, setup.boundary_elv);

    //CA_DUMP_BUFF(ELV,0);  

    // ---- INFILTRATION ----

   // Check if the infiltration computation is needed.
    useInfiltration = (setup.infrate_global > 0);

    // Transform from meter to mm and from hour to update_dt
    inf_updatedt = (setup.infrate_global * 0.001) * (period_time_dt / 3600.0);

    if (setup.output_console)
    {
        std::cout << "--------------------------------------------------------" << std::endl;
        if (useInfiltration)
        {
            std::cout << "Infiltration computation     : yes" << std::endl;
            std::cout << "Amount during an update step : " << inf_updatedt << std::endl;
            std::cout << "Attention                    : Beta code" << std::endl;
        }
        else
            std::cout << "Infiltration computation     : no" << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl;
    }

    // ---- INIT WATER LEVEL EVENT ----

    // Initialise the object that manage the water level events.
    WaterLevelManager wl_manager(GRID, wles);

    // Add the area with water level in the computational domain.
    wl_manager.addDomain(compdomain);

    // Get the elevation information.
    wl_manager.getElevation(ELV);

    // Analyse the area where a water level event will happen. WD
    // is used as temporary buffer.
    if (setup.check_vols)
        wl_manager.analyseArea(WD, MASK, fulldomain);

    // ---- INIT RAIN EVENT ----

    // Initialise the object that manage the rain.
    RainManager rain_manager(GRID, res);

    // Add the area with rain in the computational domain.
    rain_manager.addDomain(compdomain);

    // Analyse the area where it will rain to use for volume checking. WD
    // is used as temporary buffer.
    if (setup.check_vols)
        rain_manager.analyseArea(WD, MASK, fulldomain);

    // ---- INIT INFLOW EVENT ----

    // Initialise the object that manage the inflow.
    InflowManager inflow_manager(GRID, ies);

    // Add the area with the inflow in the computational domain.
    inflow_manager.addDomain(compdomain);

    // Analyse the area where it will inflow to use for volume
    // checking. WD is used as temporary buffer.
    if (setup.check_vols)
        inflow_manager.analyseArea(WD, MASK, fulldomain);

    // ---- INIT TIME PLOTS AND RASTER GRID ----

    std::string basefilename = ad.output_dir + ad.sdir + setup.short_name;

    // Initialise the object that manages the time plots.
    TPManager tp_manager(GRID, ELV, tps, basefilename, setup.timeplot_files);

    // Initialise the object that manages the time plots.
    RGManager rg_manager(GRID, rgs, basefilename, setup.rastergrid_files);

    // List of raster grid data
    std::vector<RGData> rgdatas(rgs.size());

    // Peak buffers
    RGPeak rgpeak;

    for (size_t i = 0; i < rgs.size(); ++i)
    {
        std::string filename = ad.output_dir + ad.sdir + setup.short_name + "_" + setup.rastergrid_files[i];
        initRGData(filename, GRID, nodata, rgs[i], rgdatas[i], rgpeak);
    }

    // Time Step plot manager
    TSPlot tsplot(basefilename + "_ts.csv", setup.ts_plot);

    // -- INITIALISE ---

    // Clear the buffer to zero (borders included).
    OUTF1.clear();
    OUTF2.clear();
    A.clear();
    V.clear();
    WD.clear();

    // If there is not request to expand domain. Set the computational and extend domain to full domain.
    if (!setup.expand_domain)
    {
        compdomain.clear();
        compdomain.add(fullbox);
    }
    else
    {
        // Set the sequential domain to the compdomain.
        seqdomain = compdomain;
    }


    // -- CALCULATE POSSIBLE INITIAL DT ---

    // Find the possible velocity caused by the events.
    potential_va = 0.0;
    potential_va = std::max(potential_va, rain_manager.potentialVA(t, period_time_dt));
    potential_va = std::max(potential_va, inflow_manager.potentialVA(t, period_time_dt));
    potential_va = std::max(potential_va, wl_manager.potentialVA(t, period_time_dt));

    switch (setup.model_type)
    {
    case MODEL::WCA2Dv1:
    case MODEL::WCA2Dv2:
        // Compute the possible next time step using the critical velocity equations.
        // Use the smaller between dx/dy
        // I Don't like using alpha. But at the moment this is the
        // simplest way to find the potential impact of events.
        dtn1 = std::min(setup.time_maxdt, alpha*GRID.length() / potential_va);
        break;
    }

    // Compute the next time step as a fraction for the period time step.
    // Check that dt is between min and max.
    computeDT(dt, dtfrac, dtn1, setup);

    // Update the number of iterations before the next update dt.
    iter_dt = floor(setup.time_updatedt / dt + 0.5);

    // -- PREPARE THE EVENTS MANAGERS WITH THE NEW DT ---

    // Get the amount of events that would happen in each area for each dt
    // for the next period.
    rain_manager.prepare(t, period_time_dt, dt);
    inflow_manager.prepare(t, period_time_dt, dt);
    wl_manager.prepare(t, period_time_dt, dt);

    // -- PREMATURE END --

    // get the time when the events will not add any further water.
    t_end_events = std::max(t_end_events, rain_manager.endTime());
    t_end_events = std::max(t_end_events, inflow_manager.endTime());
    t_end_events = std::max(t_end_events, wl_manager.endTime());

    if (setup.output_console)
    {
        std::cout << "The events will end at " << t_end_events << " (s) simulation time" << std::endl;
        std::cout << "------------------------------------------" << std::endl;
    }

    if (setup.output_console && setup.output_computation)
    {
        std::cout << "-----------------" << std::endl;
        std::cout << "Initialisation time taken (s) = " << total_timer.millisecond() / 1000.0 << std::endl;
        std::cout << "-----------------" << std::endl;
    }
    if (setup.output_console)
    {
        std::cout << "Start main loop" << std::endl;
        std::cout << "-----------------" << std::endl;
    }

    // ------------------------- MAIN LOOP -------------------------------
    while (iter < setup.time_maxiters && t < setup.time_end)
    {
        // Set this to false. This will be set to the right value during an
        // update step or before the update itself.
        UpdatePEAK = false;

        // The raster has not been written this iteration.
        RGwritten = false;

        // If there is the request to expand the domain.
        // Deactivate Box alarm(s) and set them.
        if (setup.expand_domain)
        {
            OUTFALARMS.deactivateAll();
            OUTFALARMS.set();
        }

        // --- CONSOLE OUTPUT ---

        // Check if it is time to output to console.
        if (setup.output_console && t >= time_output)
        {
            outputConsole(iter, oiter, t, dt, avgodt, minodt, maxodt, vamax, upstr_elv, compdomain, setup);

            oiter = 0;
            avgodt = 0.0;
            minodt = setup.time_maxdt;  // Output step minimum dt.
            maxodt = 0.0;               // Output step maximum dt.

            // Compute the next output time.
            time_output += setup.output_period;

            if (setup.check_vols == true)
            {
                // Compute the total volume of water that is in the water
                // depth (included the boundary cell).
                WD.sequentialOp(fulldomain, wd_volume, CA::Seq::Add);
                wd_volume *= GRID.area();

                std::cout << "Volume check:" << std::endl;
                std::cout << "RAIN = " << rain_volume << " INFLOW = " << inflow_volume << " INFILT = " << -inf_volume
                    << " WD = " << wd_volume << std::endl;
                std::cout << "-----------------" << std::endl;
            }

            if (setup.output_console && setup.output_computation)
            {
                std::cout << "Partial run time taken (s) = " << total_timer.millisecond() / 1000.0 << std::endl;
                std::cout << "-----------------" << std::endl;
            }
        }

        // --- SIMULATION TIME ---

        // Set the new time step.
        t += dt;

        // Round the time step to be of 0.01 second precision and then
        // check if it is a multiple of 60 (with a 0.01 precision). If it
        // is the case we need to reset the time of the simulation. If we
        // don't do this the floating point error will creep into the time
        // of simulation.
        CA::Real tround = fround(t, 2);
        if (std::fmod(tround, period_time_dt) < static_cast<CA::Real>(0.01))
        {
            t = tround;
        }

        // Compute output step information.
        avgodt += dt;

        if (dt > maxodt) maxodt = dt;
        if (dt < minodt) minodt = dt;

        // --- COMPUTE OUTFLUX ---
        switch (setup.model_type)
        {
        case MODEL::WCA2Dv1:
            // Clear the outflow buffer to zero (borders included).
            OUTF1.clear();

            // Compute outflow using WCA2Dv1.
            // Check if there is an outflow in the border of the box.
            CA::Execute::function(compdomain, outflowWCA2Dv1, GRID, OUTF1, ELV, WD, MASK, OUTFALARMS,
                ignore_wd, tol_delwl, dt, irough);

            break;
        case MODEL::WCA2Dv2:
            // Compute outflow using WCA2Dv2.
            // This save a division operation for each cell.
            CA::Real ratio_dt = dt / previous_dt;
            CA::Execute::function(compdomain, outflowWCA2Dv2, GRID, (*POUTF1), (*POUTF2),
                ELV, WD, MASK, OUTFALARMS, ignore_wd, tol_delwl, dt, ratio_dt, irough);

            break;
        }

        // If there is a request to expand the domain.
        // Get the alarms states.
        if (setup.expand_domain)
        {
            OUTFALARMS.get();

            // Check if the box alarm is active, that mean there is some
            // outflux on the border of the computational domain.
            if (OUTFALARMS.isActivated(0))
            {
                // Set the computational domain to be the extend version and
                // create the new extended one.
                CA::Box extent(compdomain.extent());
                compdomain.clear();
                compdomain.add(extendBox(extent, fullbox, 1));
            }
        }

        // --- UPDATE WL AND WD ---
        switch (setup.model_type)
        {
        case MODEL::WCA2Dv1:
            // Update the water depth with the outflux and store the total
            // amount of outflux for the WCA2Dv1 model. 
            CA::Execute::function(compdomain, waterdepthWCA2Dv1, GRID, WD, OUTF1, (*PTOT), MASK, dt, period_time_dt);
            break;

        case MODEL::WCA2Dv2:
            // Generic water depth, use OUTF1, erase OUTF2.
            CA::Execute::function(compdomain, waterdepth, GRID, WD, (*POUTF1), (*POUTF2), MASK, dt);

            // Swap the double buffer
            // Now POUTF1 is zeroed while POUTF2 contains the previous flux.
            std::swap(POUTF1, POUTF2);
            break;
        }

        start_updatedt += dt;

        // --- EXTRA LATERAL EVENT(s) ---

        // Add the eventual rain events.
        rain_manager.add(WD, MASK, t, dt);

        // Add the eventual inflow events.
        inflow_manager.add(WD, MASK, t, dt);

        // Add the eventual water level events.
        wl_manager.add(WD, ELV, MASK, t, dt);

        // --- COMPUTE NEXT DT, I.E. PERIOD STEP ---

        // Update previous dt
        previous_dt = dt;

        // Check if the dt need to be re-computed.
        if (t >= time_dt || --iter_dt == 0)
        {
            // Reset the start of updatedt.
            start_updatedt = 0.0;

            // Compute the Inflitration if needed. It uses the CellBuffer A (with
            // velocity angle) as temporary buffer where to store the volume
            // of water removed.
            if (useInfiltration)
            {
                // Clear the angle.
                A.clear();

                CA::Execute::function(fulldomain, infiltration, GRID, WD, MASK, A, inf_updatedt);

                if (setup.check_vols)
                {
                    // Retrieve the volume removed through infiltration
                    CA::Real vol = 0;
                    A.sequentialOp(seqdomain, vol, CA::Seq::Add);
                    inf_volume += vol;
                }
            }

            if (setup.ignore_upstream)
            {
                // Deactivate the alarms checked during the velocity
                // calculation.
                VELALARMS.deactivateAll();
                VELALARMS.set();
            }

            // Make sure there are not any rounding errors.
            t = time_dt;

            // Plot the time steps if requested.
            tsplot.output(t, dt);

            // The peak value need to be updated
            UpdatePEAK = true;

            // Update the total volume from the events for the last period.
            rain_volume += rain_manager.volume();
            inflow_volume += inflow_manager.volume();
            // NO water level at the moment.

            // --- UPDATE VA ---

            switch (setup.model_type)
            {
            case MODEL::WCA2Dv1:
                // Clear the Velocity and angle.
                V.clear();
                A.clear();

                // Compute the velocity using the total outflux.
                // Attention the tolerance is different here. 
                // Check if there is water movement over the upstream elevation threshold.
                CA::Execute::function(compdomain, velocityWCA2Dv1, GRID, V, A, WD, ELV, (*PTOT), MASK, VELALARMS,
                    tol_va, period_time_dt, irough, upstr_elv);

                // CLear the total outflux.
                (*PTOT).clear();
                break;

            case MODEL::WCA2Dv2:
                // Clear the Velocity and angle.
                V.clear();
                A.clear();

                // Compute the velocity using the last outflux (OUTF2)
                // Compute dt using Hunter formula
                CA::Execute::function(compdomain, velocityDiffusive, GRID, V, A, (*PDT),
                    WD, ELV, (*POUTF2), MASK, VELALARMS,
                    tol_va, tol_slope, dt, irough, upstr_elv);
                break;
            }

            // Retrieve the maximum velocity 
            V.sequentialOp(compdomain, vamax, CA::Seq::MaxAbs);

            // Find the maximum velocity
            CA::Real grid_max_va = vamax;

            // Find the possible velocity caused by the events.
            potential_va = 0.0;
            potential_va = std::max(potential_va, rain_manager.potentialVA(t, period_time_dt));
            potential_va = std::max(potential_va, inflow_manager.potentialVA(t, period_time_dt));
            potential_va = std::max(potential_va, wl_manager.potentialVA(t, period_time_dt));

            switch (setup.model_type)
            {
            case MODEL::WCA2Dv1:
                // Compute the possible next dt from the grid velocity and from
                // the potential velocity for an event.
                // Use the minimum between dx and dy.
                dtn1 = setup.time_maxdt;
                dtn1 = std::min(dtn1, alpha*GRID.length() / potential_va);
                dtn1 = std::min(dtn1, alpha*GRID.length() / grid_max_va);
                break;

            case MODEL::WCA2Dv2:
                dtn1 = setup.time_maxdt;
                // Retrieve the possible dt using the WCA2Dv2 diffusive formula.
                // This is very similar to the LISFLOOD-FP diffusive formula.
                (*PDT).sequentialOp(seqdomain, possible_dt, CA::Seq::Min);

                // I Don't like using alpha. But at the moment this is the
                // simplest way to find the potential impact of events.
                dtn1 = std::min(dtn1, alpha*GRID.length() / potential_va);

                // Furthermore we are using alpha to keep a minimum time step
                // depending on the velocity like in WCA2Dv1
                dtn1 = std::min(dtn1, alpha*GRID.length() / grid_max_va);

                //std::cerr<<"@@ PDT = "<<possible_dt <<" NDT = "<<dtn1<<" ST = "<<t/60<<" @@"<<std::endl;

                // Use the possible dt and the dtn1 to find the time step.
                dtn1 = std::min(dtn1, possible_dt);

                // Reset the PDT.
                (*PDT).fill(fulldomain, setup.time_updatedt);
                break;
            }


            // Compute the next time step as a fraction for the period time step.
            // Check that dt is between min and max.
            computeDT(dt, dtfrac, dtn1, setup);

            // Update the number of iterations before the next update dt.
            iter_dt = floor(setup.time_updatedt / dt + 0.5);

            // When the dt need to be recomputed.
            time_dt += period_time_dt;

            // Prepare the Events manager for the next update.
            rain_manager.prepare(t, period_time_dt, dt);
            inflow_manager.prepare(t, period_time_dt, dt);
            wl_manager.prepare(t, period_time_dt, dt);

            if (setup.ignore_upstream)
            {
                // Check the ALARMS computed during the velocity step.
                VELALARMS.get();

                // If the alarms one is not active, then there was not any cell
                // which had the water level over the upstream elevation
                // threshold and some flux at the same time. So we can remove
                // the cell from the computation and then lower the upstream
                // threshold.
                // ATTENTION This action is performed only when all the events
                // finished to add water to the domain.
                if (!VELALARMS.isActivated(0) && t > t_end_events)
                {
                    CA::Execute::function(fulldomain, removeUpstr, GRID, MASK, ELV, upstr_elv);
                    upstr_elv -= setup.upstream_reduction;
                }
            }
        } // COMPUTE NEXT DT.

        // ------- OUTPUTS --------!!!!!!!!!!!!!!!

        // Output time plots.
        tp_manager.output(t, iter, WD, V, setup.output_console);

        // Check if we need to update the peak at every time step.
        if (setup.update_peak_dt)
            UpdatePEAK = true;

        // Update the peak
        if (UpdatePEAK)
            rg_manager.updatePeak(compdomain, WD, V, MASK);

        // Output raster grid. Keep track if the raster has been written.
        RGwritten = rg_manager.output(t, WD, V, A, setup.short_name, setup.output_console,
            (iter >= setup.time_maxiters - 1 || t >= setup.time_end));

        // ---- END OF ITERATION ----

        // Increase time step (and output time step)
        iter++;
        oiter++;
    }

    // --- END OF MAIN LOOP ---

    // --- OUTPUT PEAK & FINAL ---

    // Check if the raster has not been written in the last
    // iteration. If not, we need to make sure that we save the PEAK and
    // the FINAL extend (if requested).
    if (!RGwritten)
    {
        // Make sure to output the last peak value.
        rg_manager.updatePeak(compdomain, WD, V, MASK);
        rg_manager.output(t, WD, V, A, setup.short_name, setup.output_console, true);
        rg_manager.outputPeak(t, WD, V, setup.short_name, setup.output_console);
    }

    // --- CONSOLE OUTPUT ---

    // Check if it is time to output to console.
    if (setup.output_console && t >= time_output)
    {
        outputConsole(iter, oiter, t, dt, avgodt, minodt, maxodt, vamax, upstr_elv, compdomain, setup);

        if (setup.check_vols == true)
        {
            // Compute the total volume of water that is in the water
            // depth (included the boundary cell).
            WD.sequentialOp(fulldomain, wd_volume, CA::Seq::Add);
            wd_volume *= GRID.area();

            std::cout << "Volume check:" << std::endl;
            std::cout << "RAIN = " << rain_volume << " INFLOW = " << inflow_volume << " INFILT = " << -inf_volume
                << " WD = " << wd_volume << std::endl;
            std::cout << "-----------------" << std::endl;
        }
    }

    // ---- TIME OUTPUT ----

    if (setup.output_console && setup.output_computation)
    {
        std::cout << "-----------------" << std::endl;
        std::cout << "Total run time taken (s) = " << total_timer.millisecond() / 1000.0 << std::endl;
        std::cout << "-----------------" << std::endl;
    }

    if (setup.output_console)
    {
        time_t t = time(0);   // get time now
        struct tm * now = localtime(&t);
        std::cout << "Simulation : " << setup.sim_name << std::endl;
        std::cout << "Model      : " << setup.model_type << std::endl;
        std::cout << "Date End   : " << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << '-' << now->tm_mday
            << " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << std::endl;
        std::cout << "------------------------------------------" << std::endl;
    }

    return 0;
}


void setRunStatus(std::string status)
{
    if (SetRunStatusCallbackFunc != nullptr && SetRunStatusCallbackOwner != nullptr)
        SetRunStatusCallbackFunc(SetRunStatusCallbackOwner, status);
}


bool isForceStopped()
{
    if (ForceStoppedCallbackFunc != nullptr && ForceStoppedCallbackOwner != nullptr)
        return ForceStoppedCallbackFunc(ForceStoppedCallbackOwner);

    return false;
}


void outputConsole_2(CA::Unsigned iter, CA::Unsigned oiter, CA::Real t, CA::Real dt,
    CA::Real avgodt, CA::Real minodt, CA::Real maxodt,
    CA::Real vamax, CA::Real upstr_elv, const CA::BoxList& domain,
    const Setup& setup,
    FILE* rptFile)
{
    if (rptFile == nullptr)
        return;

    fprintf(rptFile, "-----\n");

    // additional output requested by CIRP
    CA::Real percentage = t / ((CA::Real)(setup.time_end - setup.time_start));
    percentage *= 100.0;

    fprintf(rptFile, "Progress (%%): %0.2f\n", percentage);

    fprintf(rptFile, "Total iterations = %d Simulation time (MIN) = %f Last DT = %f\n", iter, t / 60, dt);
    fprintf(rptFile, "Last iterations  = %d Average DT =%f Min DT = %f Max DT = %f\n", oiter, avgodt /oiter, minodt, maxodt);
    fprintf(rptFile, "UPSTRELV = %f\n", upstr_elv);
    fprintf(rptFile, "VAMAX    = %f\n", vamax);
    if (setup.expand_domain)
    {
        CA::Box B(domain.extent());
        fprintf(rptFile, "DOMAIN   = (%d,%d):(%d,%d)\n", B.x(), B.y(), B.w(), B.h());
    }
    fprintf(rptFile, "-----\n");
}


int CADDIES2D_2(const std::string& data_dir, const Setup& setup, const CA::AsciiGrid<CA::Real>& eg,
    const std::vector<RainEvent>& res, const std::vector<WLEvent>& wles,
    const std::vector<IEvent>& ies,
    const std::vector<RasterGrid>& rgs,
    int platform_index,
    FILE* rptFile)
{
    // Check the model.
    switch (setup.model_type)
    {
    case MODEL::WCA2Dv1:
    case MODEL::WCA2Dv2:
        // OK!
        break;
    default:
        std::cerr << "Error the simulation does not support the model: " << setup.model_type << std::endl;
        return 1;
    }

    if (rptFile)
    {
        time_t t = time(0);   // get time now
        struct tm * now = localtime(&t);
        fprintf(rptFile, "Simulation : %s\n", setup.sim_name.c_str());
        fprintf(rptFile, "Model      : WCA2Dv2\n");
        fprintf(rptFile, "Date Start : %d-%d-%d %d:%d:%d\n", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        fprintf(rptFile, "------------------------------------------\n");
    }

    setRunStatus("Loading grid data ...");

    // ---- Timer ----

    // Get starting time.
    CA::Clock total_timer;

    // ---- CA GRID ----

    // Load the CA Grid from the DataDir. 
    // ATTENTION this should have an extra set of cells in each
    // direction. The internal implementation could be different than a
    // square regular grid.
    CA::Arguments args;
    CA::Grid  GRID(data_dir, setup.preproc_name + "_Grid", "0", args.active(), platform_index);

    if (rptFile)
        fprintf(rptFile, "Loaded Grid data\n");

    // Print the Grid Information.
    if (rptFile)
    {
        fprintf(rptFile, "-----------------\n");
        GRID.printInfo(rptFile);
        fprintf(rptFile, "-----------------\n");
    }

    // Set if to print debug information on CA function.
    GRID.setCAPrint(false);

    // Create the full (extended) computational domain of CA grid. 
    CA::BoxList  fulldomain;
    CA::Box      fullbox = GRID.box();
    fulldomain.add(fullbox);

    // Create a borders object that contains all the borders and
    // corners of the grid.
    CA::Borders borders;

    // Create the real computational domain of CA grid, i.e. the
    // original DEM size not the extended one.
    CA::BoxList  realdomain;
    CA::Box      realbox(GRID.box().x() + 1, GRID.box().y() + 1, GRID.box().w() - 2, GRID.box().h() - 2);
    realdomain.add(realbox);

    // Create the computational domain, i.e. the area where the water is
    // present and moving.
    CA::BoxList  compdomain;

    // Create the computational domain used for sequential
    // computation. SequentialOp does not work with multiple boxes.
    CA::BoxList  seqdomain(fullbox);

    // -- INITIALISE ELEVATION ---

    // Create the elevation cell buffer.
    // It contains a "real" value in each cell of the grid.
    CA::CellBuffReal  ELV(GRID);

    // Set the border of the elevation buffer to be no data. 
    ELV.bordersValue(borders, eg.nodata);

    // Se the default value of the elevation to be nodata.
    ELV.fill(fulldomain, eg.nodata);

    // Load the data not from the DEM file but from the pre-processed
    // file.
    if (!ELV.loadData(setup.preproc_name + "_ELV", "0"))
    {
        std::cerr << "Error while loading the Elevation pre-processed file" << std::endl;
        return 1;
    }

    // Highest elevation
    CA::Real     high_elv = 90000;

    // Find highest elevation
    ELV.sequentialOp(fulldomain, high_elv, CA::Seq::Max);

    if (rptFile)
    {
        fprintf(rptFile, "Loaded Elevation data\n");
        fprintf(rptFile, "Highest elevation = %f\n", high_elv);
    }
    // ---- CELL BUFFERS ----

    // Create the water depth cell buffer.
    CA::CellBuffReal WD(GRID);

    // Create the MASK cell buffer. The mask is useful to check which
    // cell has data and nodata and which cell has neighbourhood with
    // data.
    CA::CellBuffState MASK(GRID);

    // Create the velocity cell buffer with the speed of the velocity
    CA::CellBuffReal V(GRID);

    // Create the velocity cell buffer with the angle of the velocity
    CA::CellBuffReal A(GRID);

    // If the we are using the WCA2Dv2 model create the buffer with the
    // dt value.
    cpp11::shared_ptr<CA::CellBuffReal> PDT;

    // Allocate the buffer for the time step
    // Only WCA2Dv1 does not need this.
    if (setup.model_type != MODEL::WCA2Dv1)
    {
        PDT.reset(new CA::CellBuffReal(GRID));
        (*PDT).fill(fulldomain, setup.time_updatedt);
    }

    // ---- EDGES BUFFERS ----

    // Create the outflow edge buffer(s). Many models use a double buffer
    // technique to save time.
    CA::EdgeBuffReal OUTF1(GRID);
    CA::EdgeBuffReal OUTF2(GRID);

    CA::EdgeBuffReal *POUTF1 = &OUTF1;
    CA::EdgeBuffReal *POUTF2 = &OUTF2;

    // Allocate the buffer for total flow for an edge for an update step.
    // Only WCA2Dv1 does need this.
    cpp11::shared_ptr<CA::EdgeBuffReal> PTOT;
    if (setup.model_type == MODEL::WCA2Dv1)
    {
        PTOT.reset(new CA::EdgeBuffReal(GRID));
        (*PTOT).clear();
    }


    // ---- ALARMS ----

    // Create the alarm(s) checked during the outflux computation.
    // Alarm 1: indicates when there is going to be an outflux outside of the computational domain.
    CA::Alarms  OUTFALARMS(GRID, 1);

    // Create the alarm(s) checked during the velocity computation
    // Alarm 1: indicates when there is still water movement over the elevation threshold.
    CA::Alarms  VELALARMS(GRID, 1);


    // ---- SCALAR VALUES ----

    CA::Unsigned iter = 0;              // The actual iteration number.
    CA::Real     t = setup.time_start;  // The actual time in seconds
    CA::Real     dt = std::min(setup.time_maxdt, setup.output_period); // Starting delta time.
    CA::Real     dtn1 = 0.0;
    CA::Real     nodata = eg.nodata;

    // The simulation time when the events will not produce any further water.
    CA::Real     t_end_events = setup.time_start;

    // The level of water that can be ignored.
    CA::Real     ignore_wd = setup.ignore_wd;

    // The water difference between cell that can be ignored.
    CA::Real     tol_delwl = setup.tolerance;

    // The minimum level of water that is used for computing the
    // velocity. 
    CA::Real     tol_va = std::min(setup.ignore_wd, static_cast<CA::Real>(0.001));

    // The tolerance of the slope used to compute the dt. The slope
    // between two points must be higher than this value.
    // The tolerance is passed in percentile.
    CA::Real     tol_slope = setup.tol_slope / 100;

    // This is the period of when the velocity is computed.
    CA::Real     period_time_dt = setup.time_updatedt;
    CA::Real     time_dt = t + period_time_dt;          // The next simulation time when to update dt.
    CA::Unsigned iter_dt = 0;                  // The number of iteration before next update dt.

    // This is the time from the start of an update dt.
    CA::Real     start_updatedt = 0.0;

    // This is the previous update step dt.
    CA::Real     previous_dt = dt;

    // The fraction of time_maxdt second used to compute the next the dt
    CA::Unsigned dtfrac = 1;

    // The parameter of the time step.
    CA::Real     alpha = setup.time_alpha;

    // The inverse of the roughness
    CA::Real     irough = 1 / setup.roughness_global;

    CA::Unsigned oiter = 0;                 // Output step num of iteration.
    CA::Real     minodt = setup.time_maxdt; // Output step minimum dt.
    CA::Real     maxodt = 0.0;              // Output step maximum dt.
    CA::Real     avgodt = 0.0;              // Average dt;
    CA::Real     time_output = t + setup.output_period; // The time of the next output.

    CA::Real     rain_volume = 0.0;
    CA::Real     inflow_volume = 0.0;
    CA::Real     wd_volume = 0.0;
    CA::Real     inf_volume = 0.0;

    // Maximum velocity.
    CA::Real     vamax = 0.0;

    // Possible dt for WCA2Dv2 model. This is used to compute the
    // possible dt create by an event and created by the outflow.
    CA::Real     possible_dt = setup.time_maxdt;

    // Upstream elevation value. This value is the elevation where the
    // water "probably cannot reach anymore".
    CA::Real     upstr_elv = high_elv;

    // The potential velocity that an event has created. This should be
    // used to create multiple loop of FLUX calculation.
    CA::Real     potential_va;

    // If true the outflow computation will check the box cell for a
    // possible expanding domain.
    bool         checkbox = setup.expand_domain;

    // Variable which indicates if the PEAK values need to be updated.
    // The PEAK values can be updated on every update step or on every
    // steps.
    bool UpdatePEAK = false;

    // Variable which indicates if the raster has been saved in the last
    // iteration.
    bool RGwritten = false;

    // If true perform the infiltration step.
    bool useInfiltration = false;

    // The amount of water to remove during an update step.
    CA::Real inf_updatedt = 0.0;

    // -- CREATE FULL MASK ---

    CA::createCellMask(fulldomain, GRID, ELV, MASK, nodata);

    //***********************//
    //* BOUNDARY DISCUSSION *//
    //***********************//

    // The MASK is used to check if a cell with nodata has a neighbour
    // with data (Bit 31 set to true in the MASK). This kind of cells
    // are called boundary cells and they are ignored by the computation
    // (not visited), i.e. the rain is not added into them, the outflow
    // are not computed. However, these boundary cells are used as
    // neighbour cell thus the boundary cell can have inflow (outflow
    // from the data cell). Thus the elevation value in these boundary
    // cell is changed with the Boundary Elevation value. If this value
    // is very high the global boundary is a CLOSED one, if it is VERY
    // negative the global boundary is an OPEN one.

    // ATTENTION The water that finish in the boundary cell (open
    // boundary case) is not removed (it stays in the WD buffer). 

    // Set the boundary cell elevation to the given boundary_elv value.
    CA::Execute::function(fulldomain, setBoundaryEle, GRID, ELV, MASK, setup.boundary_elv);

    //CA_DUMP_BUFF(ELV,0);  

    // ---- INFILTRATION ----

   // Check if the infiltration computation is needed.
    useInfiltration = (setup.infrate_global > 0);

    // Transform from meter to mm and from hour to update_dt
    inf_updatedt = (setup.infrate_global * 0.001) * (period_time_dt / 3600.0);

    if (rptFile)
    {
        fprintf(rptFile, "--------------------------------------------------------\n");
        if (useInfiltration)
        {
            fprintf(rptFile, "Infiltration computation     : yes\n");
            fprintf(rptFile, "Amount during an update step : %f\n", inf_updatedt);
        }
        else
            fprintf(rptFile, "Infiltration computation     : no\n");
        fprintf(rptFile, "--------------------------------------------------------\n");
    }

    // ---- INIT WATER LEVEL EVENT ----

    // Initialise the object that manage the water level events.
    WaterLevelManager wl_manager(GRID, wles);

    // Add the area with water level in the computational domain.
    wl_manager.addDomain(compdomain);

    // Get the elevation information.
    wl_manager.getElevation(ELV);

    // Analyse the area where a water level event will happen. WD
    // is used as temporary buffer.
    if (setup.check_vols)
        wl_manager.analyseArea(WD, MASK, fulldomain);

    // ---- INIT RAIN EVENT ----

    // Initialise the object that manage the rain.
    RainManager rain_manager(GRID, res);

    // Add the area with rain in the computational domain.
    rain_manager.addDomain(compdomain);

    // Analyse the area where it will rain to use for volume checking. WD
    // is used as temporary buffer.
    if (setup.check_vols)
        rain_manager.analyseArea(WD, MASK, fulldomain);

    // ---- INIT INFLOW EVENT ----

    // Initialise the object that manage the inflow.
    InflowManager inflow_manager(GRID, ies);

    // Add the area with the inflow in the computational domain.
    inflow_manager.addDomain(compdomain);

    // Analyse the area where it will inflow to use for volume
    // checking. WD is used as temporary buffer.
    //if (setup.check_vols)
        inflow_manager.analyseArea(WD, MASK, fulldomain);

    // ---- INIT TIME PLOTS AND RASTER GRID ----

    std::string basefilename = data_dir + setup.short_name;

    // Initialise the object that manages the time plots.
    RGManager rg_manager(GRID, rgs, basefilename, setup.rastergrid_files);

    // List of raster grid data
    std::vector<RGData> rgdatas(rgs.size());

    // Peak buffers
    RGPeak rgpeak;

    for (size_t i = 0; i < rgs.size(); ++i)
    {
        std::string filename = data_dir + setup.short_name + "_" + setup.rastergrid_files[i];
        initRGData(filename, GRID, nodata, rgs[i], rgdatas[i], rgpeak);
    }

    // -- INITIALISE ---

    // Clear the buffer to zero (borders included).
    OUTF1.clear();
    OUTF2.clear();
    A.clear();
    V.clear();
    WD.clear();

    // If there is not request to expand domain. Set the computational and extend domain to full domain.
    if (!setup.expand_domain)
    {
        compdomain.clear();
        compdomain.add(fullbox);
    }
    else
    {
        // Set the sequential domain to the compdomain.
        seqdomain = compdomain;
    }


    // -- CALCULATE POSSIBLE INITIAL DT ---

    // Find the possible velocity caused by the events.
    potential_va = 0.0;
    potential_va = std::max(potential_va, rain_manager.potentialVA(t, period_time_dt));
    potential_va = std::max(potential_va, inflow_manager.potentialVA(t, period_time_dt));
    potential_va = std::max(potential_va, wl_manager.potentialVA(t, period_time_dt));

    switch (setup.model_type)
    {
    case MODEL::WCA2Dv1:
    case MODEL::WCA2Dv2:
        // Compute the possible next time step using the critical velocity equations.
        // Use the smaller between dx/dy
        // I Don't like using alpha. But at the moment this is the
        // simplest way to find the potential impact of events.
        dtn1 = std::min(setup.time_maxdt, alpha*GRID.length() / potential_va);
        break;
    }

    // Compute the next time step as a fraction for the period time step.
    // Check that dt is between min and max.
    computeDT(dt, dtfrac, dtn1, setup);

    // Update the number of iterations before the next update dt.
    iter_dt = floor(setup.time_updatedt / dt + 0.5);

    // -- PREPARE THE EVENTS MANAGERS WITH THE NEW DT ---

    // Get the amount of events that would happen in each area for each dt
    // for the next period.
    rain_manager.prepare(t, period_time_dt, dt);
    inflow_manager.prepare(t, period_time_dt, dt);
    wl_manager.prepare(t, period_time_dt, dt);

    // -- PREMATURE END --

    // get the time when the events will not add any further water.
    t_end_events = std::max(t_end_events, rain_manager.endTime());
    t_end_events = std::max(t_end_events, inflow_manager.endTime());
    t_end_events = std::max(t_end_events, wl_manager.endTime());

    if (rptFile)
    {
        fprintf(rptFile, "The events will end at %f (s) simulation time\n", t_end_events);
        fprintf(rptFile, "------------------------------------------\n");
    }

    if (rptFile)
    {
        fprintf(rptFile, "-----------------\n");
        fprintf(rptFile, "Initialisation time taken (s) = %f\n", total_timer.millisecond() / 1000.0);
        fprintf(rptFile, "-----------------\n");
    }
    if (rptFile)
    {
        fprintf(rptFile, "Start main loop\n");
        fprintf(rptFile, "-----------------\n");
    }

    std::stringstream startStatus;
    startStatus << "Run flood simulation at 0:00/"
        << ((int)setup.time_end / 3600) << ":" << std::setw(2) << std::setfill('0') << ((int)setup.time_end / 60 % 60);
    setRunStatus(startStatus.str());

    // ------------------------- MAIN LOOP -------------------------------
    while (iter < setup.time_maxiters && t < setup.time_end)
    {      
        CA::Real previous_possible_dt = possible_dt;

        // force stop by callback
        if (isForceStopped()) break;

        // Set this to false. This will be set to the right value during an
        // update step or before the update itself.
        UpdatePEAK = false;

        // The raster has not been written this iteration.
        RGwritten = false;

        // If there is the request to expand the domain.
        // Deactivate Box alarm(s) and set them.
        if (setup.expand_domain)
        {
            OUTFALARMS.deactivateAll();
            OUTFALARMS.set();
        }

        // --- CONSOLE OUTPUT ---
        if (t >= time_output)
        {
            if (rptFile)
                outputConsole_2(iter, oiter, t, dt, avgodt, minodt, maxodt, vamax, upstr_elv, compdomain, setup, rptFile);
            oiter = 0;
            avgodt = 0.0;
            minodt = setup.time_maxdt;  // Output step minimum dt.
            maxodt = 0.0;               // Output step maximum dt.

            std::stringstream status;
            status << "Run flood simulation at "
                << ((int)time_output / 3600) << ":" << std::setw(2) << std::setfill('0') << ((int)time_output / 60 % 60)
                << "/"
                << ((int)setup.time_end / 3600) << ":" << std::setw(2) << std::setfill('0') << ((int)setup.time_end / 60 % 60);
            setRunStatus(status.str());

            // Compute the next output time.
            time_output += setup.output_period;

            if (setup.check_vols == true && rptFile)
            {
                // Compute the total volume of water that is in the water
                // depth (included the boundary cell).
                WD.sequentialOp(fulldomain, wd_volume, CA::Seq::Add);
                wd_volume *= GRID.area();

                fprintf(rptFile, "Volume check:\n");
                fprintf(rptFile, "RAIN = %f INFLOW = %f INFILT = %f WD = %f\n", rain_volume, inflow_volume, -inf_volume, wd_volume);
                fprintf(rptFile, "-----------------\n");
            }

            if (rptFile)
            {
                fprintf(rptFile, "Partial run time taken (s) = %f\n", total_timer.millisecond() / 1000.0);
                fprintf(rptFile, "-----------------\n");
            }
        }

        // --- SIMULATION TIME ---

        // Set the new time step.
        t += dt;

        // Round the time step to be of 0.01 second precision and then
        // check if it is a multiple of 60 (with a 0.01 precision). If it
        // is the case we need to reset the time of the simulation. If we
        // don't do this the floating point error will creep into the time
        // of simulation.
        CA::Real tround = fround(t, 2);
        if (std::fmod(tround, period_time_dt) < static_cast<CA::Real>(0.01))
        {
            t = tround;
        }

        // Compute output step information.
        avgodt += dt;

        if (dt > maxodt) maxodt = dt;
        if (dt < minodt) minodt = dt;

        // --- COMPUTE OUTFLUX ---
        switch (setup.model_type)
        {
        case MODEL::WCA2Dv1:
            // Clear the outflow buffer to zero (borders included).
            OUTF1.clear();

            // Compute outflow using WCA2Dv1.
            // Check if there is an outflow in the border of the box.
            CA::Execute::function(compdomain, outflowWCA2Dv1, GRID, OUTF1, ELV, WD, MASK, OUTFALARMS,
                ignore_wd, tol_delwl, dt, irough);

            break;
        case MODEL::WCA2Dv2:
            // Compute outflow using WCA2Dv2.
            // This save a division operation for each cell.
            CA::Real ratio_dt = dt / previous_dt;
            CA::Execute::function(compdomain, outflowWCA2Dv2, GRID, (*POUTF1), (*POUTF2),
                ELV, WD, MASK, OUTFALARMS, ignore_wd, tol_delwl, dt, ratio_dt, irough);

            break;
        }

        // If there is a request to expand the domain.
        // Get the alarms states.
        if (setup.expand_domain)
        {
            OUTFALARMS.get();

            // Check if the box alarm is active, that mean there is some
            // outflux on the border of the computational domain.
            if (OUTFALARMS.isActivated(0))
            {
                // Set the computational domain to be the extend version and
                // create the new extended one.
                CA::Box extent(compdomain.extent());
                compdomain.clear();
                compdomain.add(extendBox(extent, fullbox, 1));
            }
        }

        // --- UPDATE WL AND WD ---
        switch (setup.model_type)
        {
        case MODEL::WCA2Dv1:
            // Update the water depth with the outflux and store the total
            // amount of outflux for the WCA2Dv1 model. 
            CA::Execute::function(compdomain, waterdepthWCA2Dv1, GRID, WD, OUTF1, (*PTOT), MASK, dt, period_time_dt);
            break;

        case MODEL::WCA2Dv2:
            // Generic water depth, use OUTF1, erase OUTF2.
            CA::Execute::function(compdomain, waterdepth, GRID, WD, (*POUTF1), (*POUTF2), MASK, dt);

            // Swap the double buffer
            // Now POUTF1 is zeroed while POUTF2 contains the previous flux.
            std::swap(POUTF1, POUTF2);
            break;
        }

        start_updatedt += dt;

        // --- EXTRA LATERAL EVENT(s) ---

        // Add the eventual rain events.
        rain_manager.add(WD, MASK, t, dt);

        // Add the eventual inflow events.
        inflow_manager.add(WD, MASK, t, dt);

        // Add the eventual water level events.
        wl_manager.add(WD, ELV, MASK, t, dt);

        // --- COMPUTE NEXT DT, I.E. PERIOD STEP ---

        // Update previous dt
        previous_dt = dt;

        // Check if the dt need to be re-computed.
        if (t >= time_dt || --iter_dt == 0)
        {
            // Reset the start of updatedt.
            start_updatedt = 0.0;

            // Compute the Inflitration if needed. It uses the CellBuffer A (with
            // velocity angle) as temporary buffer where to store the volume
            // of water removed.
            if (useInfiltration)
            {
                // Clear the angle.
                A.clear();

                CA::Execute::function(fulldomain, infiltration, GRID, WD, MASK, A, inf_updatedt);

                if (setup.check_vols)
                {
                    // Retrieve the volume removed through infiltration
                    CA::Real vol = 0;
                    A.sequentialOp(seqdomain, vol, CA::Seq::Add);
                    inf_volume += vol;
                }
            }

            if (setup.ignore_upstream)
            {
                // Deactivate the alarms checked during the velocity
                // calculation.
                VELALARMS.deactivateAll();
                VELALARMS.set();
            }

            // Make sure there are not any rounding errors.
            t = time_dt;

            // The peak value need to be updated
            UpdatePEAK = true;

            // Update the total volume from the events for the last period.
            rain_volume += rain_manager.volume();
            inflow_volume += inflow_manager.volume();
            // NO water level at the moment.

            // --- UPDATE VA ---

            switch (setup.model_type)
            {
            case MODEL::WCA2Dv1:
                // Clear the Velocity and angle.
                V.clear();
                A.clear();

                // Compute the velocity using the total outflux.
                // Attention the tolerance is different here. 
                // Check if there is water movement over the upstream elevation threshold.
                CA::Execute::function(compdomain, velocityWCA2Dv1, GRID, V, A, WD, ELV, (*PTOT), MASK, VELALARMS,
                    tol_va, period_time_dt, irough, upstr_elv);

                // CLear the total outflux.
                (*PTOT).clear();
                break;

            case MODEL::WCA2Dv2:
                // Clear the Velocity and angle.
                V.clear();
                A.clear();

                // Compute the velocity using the last outflux (OUTF2)
                // Compute dt using Hunter formula
                CA::Execute::function(compdomain, velocityDiffusive, GRID, V, A, (*PDT),
                    WD, ELV, (*POUTF2), MASK, VELALARMS,
                    tol_va, tol_slope, dt, irough, upstr_elv);
                break;
            }

            // Retrieve the maximum velocity 
            V.sequentialOp(compdomain, vamax, CA::Seq::MaxAbs);

            // Find the maximum velocity
            CA::Real grid_max_va = vamax;

            // Find the possible velocity caused by the events.
            potential_va = 0.0;
            potential_va = std::max(potential_va, rain_manager.potentialVA(t, period_time_dt));
            potential_va = std::max(potential_va, inflow_manager.potentialVA(t, period_time_dt));
            potential_va = std::max(potential_va, wl_manager.potentialVA(t, period_time_dt));

            switch (setup.model_type)
            {
            case MODEL::WCA2Dv1:
                // Compute the possible next dt from the grid velocity and from
                // the potential velocity for an event.
                // Use the minimum between dx and dy.
                dtn1 = setup.time_maxdt;
                dtn1 = std::min(dtn1, alpha*GRID.length() / potential_va);
                dtn1 = std::min(dtn1, alpha*GRID.length() / grid_max_va);
                break;

            case MODEL::WCA2Dv2:
                dtn1 = setup.time_maxdt;
                // Retrieve the possible dt using the WCA2Dv2 diffusive formula.
                // This is very similar to the LISFLOOD-FP diffusive formula.
                (*PDT).sequentialOp(seqdomain, possible_dt, CA::Seq::Min);

                if (possible_dt < setup.time_mindt)
                {
                    if (previous_possible_dt * alpha > possible_dt)
                        possible_dt = previous_possible_dt;
                }

                // I Don't like using alpha. But at the moment this is the
                // simplest way to find the potential impact of events.
                dtn1 = std::min(dtn1, alpha*GRID.length() / potential_va);

                // Furthermore we are using alpha to keep a minimum time step
                // depending on the velocity like in WCA2Dv1
                dtn1 = std::min(dtn1, alpha*GRID.length() / grid_max_va);

                //std::cerr<<"@@ PDT = "<<possible_dt <<" NDT = "<<dtn1<<" ST = "<<t/60<<" @@"<<std::endl;

                // Use the possible dt and the dtn1 to find the time step.
                dtn1 = std::min(dtn1, possible_dt);

                // Reset the PDT.
                (*PDT).fill(fulldomain, setup.time_updatedt);
                break;
            }


            // Compute the next time step as a fraction for the period time step.
            // Check that dt is between min and max.
            computeDT(dt, dtfrac, dtn1, setup);

            // Update the number of iterations before the next update dt.
            iter_dt = floor(setup.time_updatedt / dt + 0.5);

            // When the dt need to be recomputed.
            time_dt += period_time_dt;

            // Prepare the Events manager for the next update.
            rain_manager.prepare(t, period_time_dt, dt);
            inflow_manager.prepare(t, period_time_dt, dt);
            wl_manager.prepare(t, period_time_dt, dt);

            if (setup.ignore_upstream)
            {
                // Check the ALARMS computed during the velocity step.
                VELALARMS.get();

                // If the alarms one is not active, then there was not any cell
                // which had the water level over the upstream elevation
                // threshold and some flux at the same time. So we can remove
                // the cell from the computation and then lower the upstream
                // threshold.
                // ATTENTION This action is performed only when all the events
                // finished to add water to the domain.
                if (!VELALARMS.isActivated(0) && t > t_end_events)
                {
                    CA::Execute::function(fulldomain, removeUpstr, GRID, MASK, ELV, upstr_elv);
                    upstr_elv -= setup.upstream_reduction;
                }
            }
        } // COMPUTE NEXT DT.

        // ------- OUTPUTS --------!!!!!!!!!!!!!!!

        // Check if we need to update the peak at every time step.
        if (setup.update_peak_dt)
            UpdatePEAK = true;

        // Update the peak
        if (UpdatePEAK)
            rg_manager.updatePeak(compdomain, WD, V, MASK);

        // Output raster grid. Keep track if the raster has been written.
        RGwritten = rg_manager.output(t, WD, V, A, setup.short_name, setup.output_console,
            (iter >= setup.time_maxiters - 1 || t >= setup.time_end));

        // ---- END OF ITERATION ----

        // Increase time step (and output time step)
        iter++;
        oiter++;
    }

    // --- END OF MAIN LOOP ---

    // --- OUTPUT PEAK & FINAL ---

    // Check if the raster has not been written in the last
    // iteration. If not, we need to make sure that we save the PEAK and
    // the FINAL extend (if requested).
    if (!RGwritten)
    {
        // Make sure to output the last peak value.
        rg_manager.updatePeak(compdomain, WD, V, MASK);
        rg_manager.output(t, WD, V, A, setup.short_name, setup.output_console, true);
        rg_manager.outputPeak(t, WD, V, setup.short_name, setup.output_console);
    }

    // --- CONSOLE OUTPUT ---

    // Check if it is time to output to console.
    if (rptFile && t >= time_output)
    {
        outputConsole_2(iter, oiter, t, dt, avgodt, minodt, maxodt, vamax, upstr_elv, compdomain, setup, rptFile);

        if (setup.check_vols == true)
        {
            // Compute the total volume of water that is in the water
            // depth (included the boundary cell).
            WD.sequentialOp(fulldomain, wd_volume, CA::Seq::Add);
            wd_volume *= GRID.area();

            fprintf(rptFile, "Volume check:\n");
            fprintf(rptFile, "RAIN = %f INFLOW = %f INFILT = %f WD = \n", rain_volume, inflow_volume, -inf_volume, wd_volume);
            fprintf(rptFile, "-----------------\n");
        }
    }

    // ---- TIME OUTPUT ----

    if (rptFile)
    {
        fprintf(rptFile, "-----------------\n");
        fprintf(rptFile, "Total run time taken (s) = %f\n", total_timer.millisecond() / 1000.0);
        fprintf(rptFile, "-----------------\n");
    }

    if (rptFile)
    {
        time_t t = time(0);   // get time now
        struct tm * now = localtime(&t);
        fprintf(rptFile, "Simulation : %s\n", setup.sim_name.c_str());
        fprintf(rptFile, "Date End   : %d-%d-%d %d:%d:%d\n", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        fprintf(rptFile, "------------------------------------------\n");
    }

    return 0;
}
