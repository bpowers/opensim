/* File : example.i */
%module simulator

%{
#include "../../Simulator.h"
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"

/* Let's just grab the original header file here */
%include "../../Simulator.h"

