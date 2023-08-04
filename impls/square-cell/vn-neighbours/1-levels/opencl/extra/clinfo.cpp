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

//! \file clinfo.cpp
//! retrieve OpenCL information
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2014-01

#include<iostream>
#include<string>
#include<cstdlib>
#include<fstream>
#include"Arguments.hpp"


#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS
#include<CL/cl.hpp>


const int app_ver = 100;

//! Print the version info to std output.
inline void version()
{
 std::cout<<"Copyright 2013 Centre for Water Systems, University of Exeter"<<std::endl;
 std::cout<<"clinfo version "<<app_ver<<std::endl;
}


int main(int argc, char* argv[])
{
  // Create the arguments list and define the prefix which identifies
  // the arguments that are optional. This prefix differs between
  // windows and unix.
#if defined _WIN32 || defined __CYGWIN__   
  CA::Arguments args("/");
#else
  CA::Arguments args("-");
#endif

  // The number of argument.
  CA::Unsigned na = 0;
  
  args.add(na++,"help",      "Display the help and exit.", "",  true, false, true);
  args.add(na++,"version",   "Show the version of the code.", "",  true, false, true);
  args.add(na++,"test",      "Simple test that check if the executable work.", "",  true, false, true);
  
  // Parse the argument list, if the parse fail (it returns false) then 
  // exit!
  if(args.parse(argc,argv,std::cout) == false)
  {
#if defined _WIN32 || defined __CYGWIN__   
    std::cout<<"Usege: /help options to show help"<<std::endl;
    std::cout<<"Press 'Return' to continue"<<std::endl;
    std::cin.get();
#else
    std::cout<<"Usege: -help options to show help"<<std::endl;
#endif
    return EXIT_FAILURE;
  }
  
  // Variables usedin arguments.
  bool info = true;

  // Cycle throught the agument that were activated
  for(CA::Arguments::List::const_iterator i = args.active().begin(); i != args.active().end(); ++i)
  {
    if((*i)->name == "help")
    {
      args.help(std::cout,true);
      return -1;
    }
    if((*i)->name == "version")
    {
      version();
      return EXIT_SUCCESS;
    }
    
    if((*i)->name == "test")
    {
      std::cout<<app_ver<<std::endl;
      return EXIT_SUCCESS;
    }

  }

  // Retrive the OpenCL platforms information.
  static std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
      
  //! The OpenCL devices of a specific type available.
  static std::vector< std::vector<cl::Device> > devices(platforms.size());

  for(CA::Unsigned p=0; p<platforms.size(); ++p)
  {
    platforms[p].getDevices(CL_DEVICE_TYPE_ALL, &devices[p]);	
  }
  
  // Print the global information about OpenCL
  if(info)
  {
    std::cout<<"OpenCL Platforms    : "<<platforms.size()<<std::endl;
    for(CA::Unsigned p=0; p<platforms.size(); ++p)
    {
      // Loop through the platforms and print the information.    
      std::cout<<"  Platform ("<<p<<")      : "<<std::endl;
      std::cout<<"    Name            : "<<platforms[p].getInfo<CL_PLATFORM_NAME>()<<""<<std::endl;
      std::cout<<"    Vendor          : "<<platforms[p].getInfo<CL_PLATFORM_VENDOR>()<<std::endl;
      std::cout<<"    Version         : "<<platforms[p].getInfo<CL_PLATFORM_VERSION>()<<std::endl;    
      std::cout<<"    Profile         : "<<platforms[p].getInfo<CL_PLATFORM_PROFILE>()<<std::endl;    
      std::cout<<"    OpenCL Devices  : "<<devices[p].size()<<std::endl;    
      for(CA::Unsigned d=0; d<devices[p].size(); ++d)
      {
	std::cout<<"    Device   ("<<d<<")    : "<<std::endl;
	std::cout<<"      Name          : "<<devices[p][d].getInfo<CL_DEVICE_NAME>()<<std::endl;    
	cl_device_type device_type = devices[p][d].getInfo<CL_DEVICE_TYPE>();
	std::cout<<"      Type          : ";
	switch(device_type)
	{
	case CL_DEVICE_TYPE_GPU:
	  std::cout<<"GPU"<<std::endl;
	  break;
	case CL_DEVICE_TYPE_CPU:
	  std::cout<<"CPU"<<std::endl;
	  break;	  
	default:
	  std::cout<<"UNKNOWN"<<std::endl;
	}

	CA::Unsigned  max_mem = devices[p][d].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>()/1048576;
	CA::Unsigned  glo_mem = devices[p][d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()/1048576;
	std::cout<<"      ComputeUnits  : "<<devices[p][d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS >() << std::endl;
	std::cout<<"      Mem           : "<<glo_mem<<" MB"<<std::endl;
	std::cout<<"      Max Mem Buff  : "<<max_mem<<" MB"<<std::endl;
	std::string ext = devices[p][d].getInfo<CL_DEVICE_EXTENSIONS>();
	if (ext.find("cl_ext_device_fission") == std::string::npos) 	  
	  std::cout<<"      Fission       : Not supported"<<std::endl;
	else
	  std::cout<<"      Fission       : Supported"<<std::endl;
	if (ext.find("cl_khr_fp64") == std::string::npos && ext.find("cl_amd_fp64") == std::string::npos )
	  std::cout<<"      Double        : Not supported"<<std::endl;
	else
	  std::cout<<"      Double        : Supported"<<std::endl;

      }
    }    
  }

  // Yay everithing worked.
  return EXIT_SUCCESS;
}
