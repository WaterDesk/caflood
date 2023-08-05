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

#ifndef _CAEXEC2D_HPP_
#define _CAEXEC2D_HPP_

//! \file caexec2D.hpp 
//! Contains the methods that execute CA functions of the OpenCL
//! implementation of the caAPI
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk 
//! \date 2012-02


#include"cabuffs2D.hpp"
#include"Alarms.hpp"


// Get rid of the annoying visual studio warning 4244 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif // _MSC_VER


namespace CA {

    //! Contains the methods which execute a CA functions.
    namespace Execute {

        // Set the given argument to the given kernel and retrieve the
        // eventual event to wait.
        template<typename A>
        inline void setKernelArg(cl::Kernel& k, cl_uint index, A& a, std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a);
        }


        // Template specialisation that set the given CellBuff as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<CellBuffReal>(cl::Kernel& k, cl_uint index, CellBuffReal& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Template specialisation that set the given CellBuff as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<CellBuffState>(cl::Kernel& k, cl_uint index, CellBuffState& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Template specialisation that set the given EdgeBuff as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<EdgeBuffReal>(cl::Kernel& k, cl_uint index, EdgeBuffReal& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Template specialisation that set the given EdgeBuff as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<EdgeBuffState>(cl::Kernel& k, cl_uint index, EdgeBuffState& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Template specialisation that set the given Alarms as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<Alarms>(cl::Kernel& k, cl_uint index, Alarms& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Template specialisation that set the given TableReal as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<TableReal>(cl::Kernel& k, cl_uint index, TableReal& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Template specialisation that set the given TableState as argument
        // to the given kernel and retrieve the eventual event to wait.
        template<>
        inline void setKernelArg<TableState>(cl::Kernel& k, cl_uint index, TableState& a,
            std::vector<cl::Event>* wait_events)
        {
            k.setArg(index, a.buffer());

#ifdef  CA_OCL_USE_EVENTS
            wait_events->push_back(a.event());
#endif
        }


        // Set the given event into the given arg.
        template<typename A>
        inline void setEventArg(cl::Event& e, A& a)
        {
        }


#ifdef  CA_OCL_USE_EVENTS
        // Template specialisation that set the given event into the 
        // given CellBuffer arg.
        template<>
        inline void setEventArg<CellBuffReal>(cl::Event& e, CellBuffReal& a)
        {
            a.setEvent(e);
        }


        // Template specialisation that set the given event into the 
        // given CellBuffer arg.
        template<>
        inline void setEventArg<CellBuffState>(cl::Event& e, CellBuffState& a)
        {
            a.setEvent(e);
        }


        // Template specialisation that set the given event into the 
        // given EdgeBuffer arg.
        template<>
        inline void setEventArg<EdgeBuffReal>(cl::Event& e, EdgeBuffReal& a)
        {
            a.setEvent(e);
        }


        // Template specialisation that set the given event into the 
        // given EdgeBuffer arg.
        template<>
        inline void setEventArg<EdgeBuffState>(cl::Event& e, EdgeBuffState& a)
        {
            a.setEvent(e);
        }


        // Template specialisation that set the given event into the 
        // given Alarms arg.
        template<>
        inline void setEventArg<Alarms>(cl::Event& e, Alarms& a)
        {
            a.setEvent(e);
        }


        // Template specialisation that set the given event into the 
        // given TableReal arg.
        template<>
        inline void setEventArg<TableReal>(cl::Event& e, TableReal& a)
        {
            a.setEvent(e);
        }


        // Template specialisation that set the given event into the 
        // given TableState arg.
        template<>
        inline void setEventArg<TableState>(cl::Event& e, TableState& a)
        {
            a.setEvent(e);
        }

#endif

        //! Retrive the kernel of the given function from the Grid using the
        //! relative compiled program.
        template<typename Func>
        inline cl::Kernel getKernel(Func f, Grid& g)
        {
            // Retrieve the program of the given function from the Grid.
            const cl::Program& program = g.getProgram(f);

            // Print Kernel info
            //const size_t* r = f().fourth;
            //std::cout<<"CA Function: "<<f().first<<"("<<r[0]<<","<<r[1]<<")"<<std::endl;

            // Retrieve the kernel.
            try
            {
                cl::Kernel kernel(cl::Kernel(program, f().first.c_str()));
                return kernel;
            }
            catch (cl::Error err)
            {
                std::cerr << std::endl << "Error CA Function: " << f().first << ".ca" << std::endl << std::endl;
                std::cerr << "Source:" << std::endl << std::endl;
                std::cerr << program.getInfo<CL_PROGRAM_SOURCE>() << std::endl << std::endl;
                exit(EXIT_FAILURE);
            }
        }


        //! Execute the Kernel of a CA functions.
        inline void execute(const BoxList& bl, cl::NDRange& range,
            Grid& g, cl::Kernel& kernel, std::vector<cl::Event>* wait_events, cl::Event* e)
        {
            // Check that the extent of the boxlist is inside the domain of
            // the grid.
            if (!g.box().inside(bl.extent()))
                return;

            // Cycle through the boxes.
            for (BoxList::ConstIter ibox = bl.begin(); ibox != bl.end(); ++ibox)
            {
                Box box(*ibox);

                // Set the value of the box into the cagrid.
                _caGrid_short cagrid_short = g.caGridShort();
                cagrid_short.bx_lx = box.x();
                cagrid_short.bx_ty = box.y();
                cagrid_short.bx_rx = box.w() + box.x();
                cagrid_short.bx_by = box.h() + box.y();

                // Set the NDrange for the global workspace and offset from the
                // given box. The size of the global workspace is a multiple of
                // range or warp.
                cl::NDRange offset;
                cl::NDRange global;
                if (range.dimensions() != 2)
                {
                    offset = cl::NDRange(box.x(), box.y());
                    global = cl::NDRange(computeStride(box.w(), g.warp()), computeStride(box.h(), g.warp()));
                }
                else
                {
                    const size_t* r = range;
                    offset = cl::NDRange(box.x(), box.y());
                    global = cl::NDRange(computeStride(box.w(), r[0]), computeStride(box.h(), r[1]));
                }
                // Set the CA argument in the kernel.
                kernel.setArg(0, cagrid_short);
                //cl::NDRange local (8,8);

            // Lunch the kernel that set a value into the given region
            // using the specific range of the CA function.
#ifdef  CA_OCL_USE_EVENTS 
                g.queue().enqueueNDRangeKernel(kernel, offset, global, range, wait_events, 0);
#else
                g.queue().enqueueNDRangeKernel(kernel, offset, global, cl::NDRange(), NULL, NULL);
#endif

                g.queue().flush();
            }


#ifdef  CA_OCL_USE_EVENTS 
            // Set the given event to wait for all the previous kernel lunch
            // events to finish.

            g.queue().enqueueMarker(e);
            g.queue().enqueueMarkerWithWaitList(e);
#endif  
        }


        template<typename Func, typename A1>
        void function(const BoxList& bl, Func& f, Grid& g, A1& a1)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);
            setEventArg(event, a1);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }


        template<typename Func, typename A1, typename A2>
        void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }


        template<typename Func, typename A1, typename A2, typename A3>
        void function(const BoxList& bl, Func &f, Grid& g, A1& a1, A2& a2, A3& a3)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4>
        void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5>
        void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }


        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////// EIGHT //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////// NINE //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////// TEN //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////// ELEVEN //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////// TWELVE //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11, A12& a12)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);
            setKernelArg(kernel, 12, a12, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
            setEventArg(event, a12);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////// THIRTEEN //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);
            setKernelArg(kernel, 12, a12, wait_events);
            setKernelArg(kernel, 13, a13, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
            setEventArg(event, a12);
            setEventArg(event, a13);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////// FOURTHEEN //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13,
            typename A14>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);
            setKernelArg(kernel, 12, a12, wait_events);
            setKernelArg(kernel, 13, a13, wait_events);
            setKernelArg(kernel, 14, a14, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
            setEventArg(event, a12);
            setEventArg(event, a13);
            setEventArg(event, a14);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////// 15 //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13,
            typename A14, typename A15>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);
            setKernelArg(kernel, 12, a12, wait_events);
            setKernelArg(kernel, 13, a13, wait_events);
            setKernelArg(kernel, 14, a14, wait_events);
            setKernelArg(kernel, 15, a15, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
            setEventArg(event, a12);
            setEventArg(event, a13);
            setEventArg(event, a14);
            setEventArg(event, a15);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////////////////////////////////////////////////////////


        ////////////////////////////////////////// 16 //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13,
            typename A14, typename A15, typename A16>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15, A16& a16)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);
            setKernelArg(kernel, 12, a12, wait_events);
            setKernelArg(kernel, 13, a13, wait_events);
            setKernelArg(kernel, 14, a14, wait_events);
            setKernelArg(kernel, 15, a15, wait_events);
            setKernelArg(kernel, 16, a16, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
            setEventArg(event, a12);
            setEventArg(event, a13);
            setEventArg(event, a14);
            setEventArg(event, a15);
            setEventArg(event, a16);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////// 17 //////////////////////////////////////////
        template<typename Func, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
            typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13,
            typename A14, typename A15, typename A16, typename A17>
            void function(const BoxList& bl, Func& f, Grid& g, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
                A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15, A16& a16, A17& a17)
        {
            std::vector<cl::Event>* wait_events = NULL;
#ifdef  CA_OCL_USE_EVENTS 
            cl::Event event;
#endif

            cl::Kernel kernel(getKernel(f, g));

            setKernelArg(kernel, 1, a1, wait_events);
            setKernelArg(kernel, 2, a2, wait_events);
            setKernelArg(kernel, 3, a3, wait_events);
            setKernelArg(kernel, 4, a4, wait_events);
            setKernelArg(kernel, 5, a5, wait_events);
            setKernelArg(kernel, 6, a6, wait_events);
            setKernelArg(kernel, 7, a7, wait_events);
            setKernelArg(kernel, 8, a8, wait_events);
            setKernelArg(kernel, 9, a9, wait_events);
            setKernelArg(kernel, 10, a10, wait_events);
            setKernelArg(kernel, 11, a11, wait_events);
            setKernelArg(kernel, 12, a12, wait_events);
            setKernelArg(kernel, 13, a13, wait_events);
            setKernelArg(kernel, 14, a14, wait_events);
            setKernelArg(kernel, 15, a15, wait_events);
            setKernelArg(kernel, 16, a16, wait_events);
            setKernelArg(kernel, 17, a17, wait_events);

#ifdef  CA_OCL_USE_EVENTS 
            execute(bl, f().fourth, g, kernel, &wait_events, &event);

            setEventArg(event, a1);
            setEventArg(event, a2);
            setEventArg(event, a3);
            setEventArg(event, a4);
            setEventArg(event, a5);
            setEventArg(event, a6);
            setEventArg(event, a7);
            setEventArg(event, a8);
            setEventArg(event, a9);
            setEventArg(event, a10);
            setEventArg(event, a11);
            setEventArg(event, a12);
            setEventArg(event, a13);
            setEventArg(event, a14);
            setEventArg(event, a15);
            setEventArg(event, a16);
            setEventArg(event, a17);
#else
            execute(bl, f().fourth, g, kernel, NULL, NULL);
#endif
        }

        ////////////////////////////////////////////////////////////////////////////////////////////

    } // Namespace Execute

} // Namespace CA


#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER


#endif  // _CAEXEC2D_HPP_
