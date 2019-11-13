
# SugarConvTape

[![Build Status](https://travis-ci.com/Tom1975/SugarConvTape.svg?branch=master)](https://travis-ci.com/Tom1975/SugarConvTape)
[![Build status](https://ci.appveyor.com/api/projects/status/7960cjorphh603as/branch/master?svg=true)](https://ci.appveyor.com/project/Tom1975/sugarconvtape/branch/master)

Convert any Amstrad CPC dump file tape format to other format

Usage
=====
Usage : SugarConvTape source [destination] [-o=outputformat] [-r] [-f=filter]

    -o=outputformat : Select output format. Can take the following values: 
        wav : WAV format 
        cdt_drb : CDT with DRB 
        cdt_csw : CDT with CSW inside 
        csw11 : CSW 1.1 
        csw20 : CSW 2.0 
    If this parameter is not used, default output format is WAV 
 
    destination : output file. If source file is a directory, destination is used as an output directory 
 
    -r : If the source file is a directory, convert recursively the given directory. 
    -f=filter : If the source file is a directory, set a filter for the files to convert. 

Compilation
=
This tool can be compiled on various system.
Known working targets are :
 - Windows : Both x64 and x86
 - Ubuntu 

The tool use CMake as makefile generator
Also, a modern compiler is required as it use some C++17 features 
