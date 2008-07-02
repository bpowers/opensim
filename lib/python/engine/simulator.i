//===--- simulator.i - SWIG Interface file for Python engine -------------===//
//
// Copyright 2008 Bobby Powers
//
// This file is part of OpenSim.
// 
// OpenSim is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// OpenSim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
//
//===---------------------------------------------------------------------===//
//
// This file defines the interface for the Python wrapper of the 
// simulation engine.
//
//===---------------------------------------------------------------------===//

%module simulator

%{
#include "../../Simulator.h"
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"

/* Let's just grab the original header file here */
%include "../../Simulator.h"

