/* File : example.i */
%module simulator

%{
#include "../Simulator.h"
#include "../Variable.h"
%}

%include "std_string.i"

/* Let's just grab the original header file here */
%include "../Simulator.h"

