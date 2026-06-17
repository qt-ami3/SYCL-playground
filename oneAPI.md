To actually get icpx you have to install the Intel oneAPI compiler outside of portage (it's a binary toolkit, not in any of your repos including guru):

1. Get the Intel oneAPI Base Toolkit (or the standalone DPC++/C++ Compiler) from Intel's distribution — the offlinen installer or their package repo. It lands in /opt/intel/oneapi.

2. Source the env before use:
  
  source /opt/intel/oneapi/setvars.sh
  # or just the compiler:
  source /opt/intel/oneapi/compiler/latest/env/vars.sh
  
3. Then icpx --version works.

4. compiling

icpx -fsycl -o outputFile fileToCompile.cpp
