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

struct tag {
  static const uint32_t And      = 256,
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
                        Integral = 269;
};

struct Token
{
  uint32_t line;
  uint16_t start;
  uint16_t end;
  std::string file;

  uint32_t tag;
  std::string iden;

  Token(uint32_t t, std::string f, uint32_t l,
        uint16_t s, uint16_t e) : tag(t) {
    file = f;
    line = l;
    start = s;
    end = e;
  }
  Token() {}
  virtual void dump();
};


struct Word: public Token
{
  Word(std::string lexeme, uint32_t t, std::string f,
       uint32_t l, uint16_t s, uint16_t e) {
    tag = t;
    iden = lexeme;
    file = f;
    line = l;
    start = s;
    end = e;
  }
  Word(std::string lexeme, uint32_t t) {
    tag = t;
    iden = lexeme;
  }
  virtual void dump();
};


struct Number: public Token
{
  real_t value;

  Number(std::string lexeme, real_t val, std::string f,
         uint32_t l, uint16_t s, uint16_t e) {
    tag = tag::Num;
    iden = lexeme;
    value = val;
    file = f;
    line = l;
    start = s;
    end = e;
  }
  virtual void dump();
};

}


#endif // OPENSIM_TOKEN_H

