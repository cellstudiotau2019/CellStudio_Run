#!/bin/bash

# Manually set paths for necessary compilers and tools
export PATH=./libs/gcc-13.2.0/bin:$PATH

# Set the environment variables required for the project
export OPAL_PREFIX=$PWD/libs
export OPAL_LIBDIR=$PWD/libs
export OPAL_DATADIR=$PWD/libs/share/openmpi
export PATH=$PWD/libs:$PATH
export LD_LIBRARY_PATH=$PWD/libs:$LD_LIBRARY_PATH

# Set environment variables for MPI
export CC=mpicc
export CXX=mpic++

# Run valgrind with the desired input file
# valgrind ./build/CellStudio ./build/Input/ExampleReduced_SV.xml

valgrind --log-file=valgrind_output.log ./build/CellStudio ./build/Input/ExampleReduced_SV.xml
