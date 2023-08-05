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

//! \file Grid.cpp
//! Contains the implementation and static variables of the square
//! grid Opencl implementation.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk,
//! \author Mike Gibson, University of Exeter,
//! contact: mg278 [at] exeter.ac.uk
//! \date 2012-07

#include"ca2D.hpp"


namespace CA {

    /// ----- Static variables ----- ///


      /*


      // instansiate these in the constructors

    bool         Grid::initcl(false);
    _caUnsigned Grid::_warp = 32;
    cl::Context Grid::_context;
    std::vector<cl::Platform>  Grid::_platforms;
    _caUnsigned  Grid::_platforms_num;
    std::vector<cl::Device> Grid::_devices;
    std::string Grid::_platform_name;
    cl_device_type Grid::_device_type = CL_DEVICE_TYPE_GPU;
    int Grid::_devices_num=0;
    bool Grid::_device_fission=false;
    cl_device_partition_property_ext Grid::_device_cu=0;
    cl::CommandQueue Grid::_queue;
    cl::Program Grid::_kernels_program;
    std::string Grid::_building_options;
    cl_command_queue_properties Grid::_queue_properties=0;
    bool       Grid::_build_info(false);
    cl::Kernel Grid::_kernel_setValueReal;
    cl::Kernel Grid::_kernel_setValueState;
    cl::Kernel Grid::_kernel_opValueReal;
    cl::Kernel Grid::_kernel_opValueState;
    cl::Kernel Grid::_kernel_reduceReal;
    cl::Kernel Grid::_kernel_reduceState;
    cl::Kernel Grid::_kernel_copyHLineReal;
    cl::Kernel Grid::_kernel_copyVLineReal;
    cl::Kernel Grid::_kernel_copyHLineState;
    cl::Kernel Grid::_kernel_copyVLineState;
    cl::Kernel Grid::_kernel_copyPointReal;
    cl::Kernel Grid::_kernel_copyPointState;
    std::string Grid::_config_filename("");
    std::map<std::string,cl::NDRange> Grid::_hash_ranges;
    */

}
