//===--- lex.cc - lexer for OpenSim mark 2 ------------------------------===//
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
// This file contains data structures and functions for scanning equations
// and creating tokens from the input
//
//===--------------------------------------------------------------------===//


#include "opensim/lex.h"
#include "opensim/token.h"

#include <ctype.h>
#include <assert.h>

#include <cstdio>

using namespace opensim;
using std::exception;

// I imagine that it would be nice for some Europeans to have decimal
// represented as ',', but time will tell...
#define DECIMAL '.'


static inline bool isNumberStart(uint32_t character) {
  return isdigit(character) || character == DECIMAL;
}

static inline bool isNumberBody(uint32_t character) {
  return isdigit(character) || character == DECIMAL;
}


static inline bool isIdentifierStart(uint32_t character) {
  return isalpha(character) || character == '_';
}

static inline bool isIdentifierBody(uint32_t character) {
  return isalnum(character) || character == '_';
}


Scanner::Scanner(std::string fileName,
                 const char *fileStart,
                 const char *fileEnd) : _fileName(fileName),
					_fileStart(fileStart),
					_fileEnd(fileEnd) {

  _pos = fileStart;
  _peek = *_pos;
  _line = 1;

  _reservedWords = new llvm::StringMap<uint32_t>(16);

  reserve("if", Tag::If);
  reserve("class", Tag::Class);
}


Scanner::~Scanner() {

  delete _reservedWords;
}


inline bool Scanner::getChar() {

  if (_pos < _fileEnd)
    _peek = *++_pos;

  // XXX: do we really want this implicit cast to bool?
  return _peek;
}


inline bool Scanner::getChar(const char c) {

  getChar();
  return c == (char)_peek;
}


void Scanner::reserve(std::string lexeme, uint32_t t) {

  (*_reservedWords)[lexeme] = t;
}


inline uint32_t Scanner::getReserved(std::string lexeme) {

  return (*_reservedWords)[lexeme];
}


inline void Scanner::skipWhitespace() {

  // skip any whitespace we might encounter, but keep track of our
  // line and where the curent line begins
  do {
    if (_peek == '\n') {
      ++_line;
      _lineStart = _pos;
    }
    // finally, break when we don't have anymore whitespace
    if (!isspace(_peek)) break;
  } while (getChar());
}


Token *Scanner::getToken() {

  skipWhitespace();

  // if we've reached the end of the input, peek is null
  if (!_peek)
    return NULL;

  // keep track of the start of the token, relative to the start of
  // the current line
  uint32_t start = _pos - _lineStart;
  SourceLoc startLoc = SourceLoc(_line, start);

  // XXX: additional two-char tokens like '==' should be matched here
  switch (_peek) {
  case '=':
    if (getChar('=')) {
      // eat the second '=', since we matched
      getChar();
      return new Token("==", Tag::Eq, _fileName,
                       startLoc, SourceLoc(_line, start+2));
    } else
      return new Token('=', _fileName,
                       startLoc, SourceLoc(_line, start+2));
  default:
    break;
  }

  // match numbers first, which either begin with a digit or a decimal
  if (isNumberStart(_peek))
    return lexNumber(startLoc);

  if (isIdentifierStart(_peek))
    return lexIdentifier(startLoc);

  // if we haven't matched by here, its a simple one character token
  Token *tok = new Token(_peek, _fileName,
                         SourceLoc(_line, start),
                         SourceLoc(_line, start+1));
  getChar();

  return tok;
}


inline Token *Scanner::lexIdentifier(SourceLoc startLoc) {

  std::string s;
  do {
    s += _peek;
    getChar();
  } while (isalnum(_peek) || _peek == '_' || _peek == '.');

  // check if its a reserved word
  uint32_t t = getReserved(s);
  if (!t)
    t = Tag::Id;

  return new Token(s, t, _fileName, startLoc, SourceLoc(startLoc.line,
                                                       _pos-_lineStart));
}


inline Token *Scanner::lexNumber(SourceLoc startLoc) {

  const char *startPos = _pos;
  char *end;
  real_t num = strtod(_pos, &end);

  // by advancing to one before the end of the number and calling
  // getChar(), we set peek to be whatever the char after the number
  // is.  Since getChar is inlined, its not really a performance
  // problem.
  _pos = end - 1;
  getChar();

  // finally, return our new number token.
  return new Token(std::string(startPos, _pos-startPos), num, _fileName,
                   startLoc, SourceLoc(startLoc.line, _pos-_lineStart));
}

