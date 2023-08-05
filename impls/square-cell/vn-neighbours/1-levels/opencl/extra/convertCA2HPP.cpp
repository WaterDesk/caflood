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

//! \file convertCA2HPP.cpp
//! Convert a .ca file which contains a CA function into an .hpp file
//! which has a string that contain the CA function. This tring can be
//! used by OpenCL to build a CA function kernel.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-12



#include<iostream>
#include<string>
#include<cstdlib>
#include<fstream>
#include"Arguments.hpp"


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

    // Add the necessary arguments into the argument list;
    args.add(0, "CA-function-name", "The name of the CA function", "");
    args.add(1, "CA-input-file", "The .ca  file containing the CA function to read", "");
    args.add(2, "HPP-output-file", "The .hpp file containing the string with the CA function to write", "");

    // Add options arguments

    // Parse the argument list, if the parse fail (it returns false) then 
    // exit!
    if (args.parse(argc, argv, std::cout) == false)
    {
#if defined _WIN32 || defined __CYGWIN__   
        std::cout << "Usege: /help options to show help" << std::endl;
        std::cout << "Press 'Return' to continue" << std::endl;
        std::cin.get();
#else
        std::cout << "Usege: -help options to show help" << std::endl;
#endif
        return EXIT_FAILURE;
    }

    // Variables usedin arguments.
    std::string name;
    std::string inputfile;
    std::string outputfile;

    // Cycle throught the agument that were activated
    for (CA::Arguments::List::const_iterator i = args.active().begin(); i != args.active().end(); ++i)
    {
        // Execute the action depending on the index of the argument.
        switch ((*i)->tag)
        {
        case 0:         // Name
            name = (*i)->value;
            break;
        case 1:         // CA-input
            inputfile = (*i)->value;
            break;
        case 2:         // HPP-output
            outputfile = (*i)->value;
            break;

        default:        // Show help and exit;
            args.help(std::cout, true);
            return EXIT_SUCCESS;
        }
    }

    // Open files
    std::ifstream ifile(inputfile.c_str());
    std::ofstream ofile(outputfile.c_str());

    // Check if the files were open.
    if (!ifile)
    {
        std::cerr << "Error opening input file: " << inputfile << std::endl;
        return EXIT_FAILURE;
    }

    if (!ofile)
    {
        std::cerr << "Error opening output file: " << outputfile << std::endl;
        return EXIT_FAILURE;
    }

    // Start write file.
    // Make some preprocessor checks
    ofile << "#ifndef __" << name << "_ca__" << std::endl;
    ofile << "#define __" << name << "_ca__" << std::endl;

    // The header is composed of an inline function (with the same name
    // of the CA function) which return a CA::quadro where:
    // the fist element is a string with the name of the function;
    // the second is a string with the code;
    // the third is the OpenCL program;
    // the fourth is the eventual workgroup size.
    // The last two elements are not constant.

    // Create the inline function.
    ofile << "inline CA::quadro<const std::string,const std::string,cl::Program,cl::NDRange>& "
        << name << "() {" << std::endl;

    // Create the static string with the code of the opencl. The code is
    // added in hex.
    ofile << "  static const char code[] = {" << std::endl << "    ";

    // Add the CA function code in hex.
    size_t idx = 0;
    char   charvalue;

    while (ifile.get(charvalue))
    {
        ofile << "0x" << std::hex << static_cast<int>(charvalue) << ", ";
        idx++;
        if (idx % 15 == 0)
        {
            ofile << std::endl << "    ";
        }
    }

    // Add the "\n" to the string.
    ofile << "0x00";

    // Close the string
    ofile << std::endl;
    ofile << "  };" << std::endl;

    // Create the static cl::Program object.
    ofile << "  static cl::Program program;" << std::endl;

    // Create the static cl::NDRange object.
    ofile << "  static cl::NDRange range( cl::NullRange );" << std::endl;

    // Create the static quadro object.
    ofile << "  static CA::quadro<const std::string,const std::string,cl::Program,cl::NDRange> res(\""
        << name << "\",code,program,range);" << std::endl;

    // Return the pair object.
    ofile << "  return res;" << std::endl;

    // Close the inline function.
    ofile << "};" << std::endl;

    // Close the preprocessor check.
    ofile << "#endif //__" << name << "_ca__" << std::endl;

    // Close files.
    ifile.close();
    ofile.close();

    // Yay everithing worked.
    return EXIT_SUCCESS;
}
