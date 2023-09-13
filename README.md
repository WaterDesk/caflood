# caflood

It is a full CAFlood application which integrates the caddies-flood application into the caddies-api framework.

The original source code:

- caddies-api: https://git.exeter.ac.uk/caddies/caddies-api
- caddies-flood application: https://git.exeter.ac.uk/caddies/caddies-caflood

## compiling & building (windows only)

- launch `cmake-gui.exe` through Visual Studion Developer Command Prompt window
  - Open `Developer Command Prompt for VS2017`
  - go to the CMake bin directory, e.g. run `cd /d C:\"Program Files\CMake\bin"`
  - run `cmake-gui.exe`
- configure projects by different CAAPI implementations

  - simple
    - specify the source code directory, e.g. `E:/Code/caflood/Source`
    - specify the build directory, e.g. `E:/Code/caflood/build/simple`
    - make sure the entry value 'CAAPI_SPECIFIC_IMPL_DIR' to be `E:/Code/caflood/Source/impls/square-cell/vn-neighbours/1-levels/simple`
  - openmp
    - specify the source code directory, e.g. `E:/Code/caflood/Source`
    - specify the build directory, e.g. `E:/Code/caflood/build/simple`
    - make sure the entry value `CAAPI_SPECIFIC_IMPL_DIR` to be `E:/Code/caflood/Source/impls/square-cell/vn-neighbours/1-levels/openmp`
  - opencl
    - specify the source code directory, e.g. `E:/Code/caflood/Source`
    - specify the build directory, e.g. `E:/Code/caflood/build/simple`
    - make sure the entry value `CAAPI_SPECIFIC_IMPL_DIR` to be the opencl implementation directory, e.g. `E:/Code/caflood/Source/impls/square-cell/vn-neighbours/1-levels/opencl`
    - make sure the entry value `OPENCL_LIBRARIES` to be OpenCL.lib file, e.g. `C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.2/lib/x64/OpenCL.lib`
    - make sure the entry value `_OPENCL_CPP_INCLUDE_DIRS` to be the OpenCL include directory, e.g. `C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.2/include`

- reference: [CADDIES_Build_instructions.docx](https://engineering.exeter.ac.uk/media/universityofexeter/emps/research/cws/downloads/caddies/CADDIES_Build_instructions.docx)
