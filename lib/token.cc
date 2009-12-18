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

using namespace opensim;


// needed to make GCC behave, as noted here:
// http://hellewell.homeip.net/phillip/blogs/index.php?entry=entry060314-000000
const TagKind Tag::And;
const TagKind Tag::Else;
const TagKind Tag::Eq;
const TagKind Tag::False;
const TagKind Tag::Id;
const TagKind Tag::If;
const TagKind Tag::Index;
const TagKind Tag::Le;
const TagKind Tag::Minus;
const TagKind Tag::Ne;
const TagKind Tag::Num;
const TagKind Tag::Or;
const TagKind Tag::True;
const TagKind Tag::TypeName;
const TagKind Tag::Class;

const TokKind TokType::Tok;
const TokKind TokType::Word;
const TokKind TokType::Number;


const char *Token::getTokenKindAsString() {
  switch (kind) {
  case TokType::Tok:
    return "Token";
  case TokType::Word:
    return "Word";
  case TokType::Number:
    return "Number";
  }
}

void Token::dump() {

  switch (kind) {
  case TokType::Tok:
    fprintf(stderr, "%s %d:%d-%d:  \tToken (%3d):  '%c'\n", file.c_str(),
            start.line, start.pos, end.pos, tag, tag);
    break;
  case TokType::Word:
    fprintf(stderr, "%s %d:%d-%d:  \tWord (%3d):   '%s'\n", file.c_str(),
            start.line, start.pos, end.pos, tag, iden.c_str());
    break;
  case TokType::Number:
    fprintf(stderr, "%s %d:%d-%d:  \tNumber (%3d): '%f'\n", file.c_str(),
            start.line, start.pos, end.pos, tag, value);
    break;
  }
}

