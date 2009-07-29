//===--- token.cc - token definitions for Objective OpenSim -------------===//
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

#ifndef OPENSIM_TOKEN_H
#define OPENSIM_TOKEN_H

#include "opensim/c_runtime.h"
#include <iostream>
#include <string>


namespace opensim {

typedef uint32_t TagKind;

struct Tag {
  static const TagKind And      = 256,
                       Else     = 257,
                       Eq       = 258,
                       False    = 259,
                       Id       = 260,
                       If       = 261,
                       Index    = 262,
                       Le       = 263,
                       Minus    = 264,
                       Ne       = 265,
                       Num      = 266,
                       Or       = 267,
                       True     = 268,
                       TypeName = 269,
                       Class    = 270;
};

typedef uint16_t TokKind;

struct TokType {
  static const TokKind Tok      = 512,
                       Word     = 513,
                       Number   = 514;
};

struct SourceLoc {
  uint16_t line;
  uint16_t pos;

  SourceLoc() {};
  SourceLoc(uint16_t l, uint16_t p) : line(l), pos(p) {};
};

struct Token
{
  SourceLoc start;
  SourceLoc end;
  std::string file;

  TagKind tag;
  std::string iden;
  real_t value;

  TokKind kind;

  Token(uint32_t t, std::string f, SourceLoc s, SourceLoc e) {
    kind = TokType::Tok;
    tag = (TagKind)t;
    file = f;
    start = s;
    end = e;
  }
  Token(std::string lexeme, uint32_t t, std::string f,
       SourceLoc s, SourceLoc e) {
    kind = TokType::Word;
    tag = (TagKind)t;
    iden = lexeme;
    file = f;
    start = s;
    end = e;
  }
  Token(std::string lexeme, real_t val, std::string f,
         SourceLoc s, SourceLoc e) {
    kind = TokType::Number;
    tag = Tag::Num;
    iden = lexeme;
    value = val;
    file = f;
    start = s;
    end = e;
  }
  Token() {}
  void dump();
  const char *getTokenKindAsString();
};

}


#endif // OPENSIM_TOKEN_H

