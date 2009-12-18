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


const char *Token::getKindAsString() {
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

  llvm::errs() << *this;
}


raw_ostream &opensim::operator<<(raw_ostream &stream, Token &tok) {

  std::string val;

  switch (tok.kind) {
  case TokType::Tok:
    val = tok.tag;
    break;
  case TokType::Word:
    val = tok.iden;
    break;
  case TokType::Number:
    char c_val[32];
    snprintf(c_val, 32, "%f", tok.value);
    val = c_val;
    break;
  }

  stream << tok.file << " " << tok.start.line << ":" << tok.start.pos
	 << "-" << tok.end.pos << ": " << tok.getKindAsString() << " ("
	 << (uint32_t)tok.tag << ") '" << val << "'\n";

  return stream;
}
