//===--- token.cc - token implementation for Objective OpenSim ----------===//
//
// Copyright 2009 Bobby Powers
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
//===--------------------------------------------------------------------===//
//
// This file contains the implementation of Tokens for Objective OpenSim.
//
//===--------------------------------------------------------------------===//

#include "opensim/token.h"

#include <cstdio>


void opensim::Token::dump() {
  fprintf(stderr, "%s %d:%d-%d:  \tToken (%3d):  '%c'\n", file.c_str(), line,
          start, end, tag, tag);
}


void opensim::Word::dump() {
  fprintf(stderr, "%s %d:%d-%d:  \tWord (%3d):   '%s'\n", file.c_str(), line,
          start, end, tag, iden.c_str());
}


void opensim::Number::dump() {
  fprintf(stderr, "%s %d:%d-%d:  \tNumber (%3d): '%f'\n", file.c_str(), line,
          start, end, tag, value);
}

