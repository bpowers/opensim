//===--- IOVenText.h - Vensim text file IO -------------------------------===//
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
// This inputs model data from Vensim text-format models.
//
//===---------------------------------------------------------------------===//


#include <cstdio>
#include <cstdlib>

#include "IOVenText.h"
using OpenSim::TokenType;

OpenSim::IOVenText::IOVenText(std::string filePath)
{
  venFile = fopen(filePath.c_str(), "r");
  
  if (venFile != NULL)
  {
    // first strip out the '{UTF-8}' line.  In the future, we can support
    // UTF-8 ;)
    
    int c = getc(venFile);
    while (c != '\n' && c != EOF)
    {
      c = getc(venFile);
    }
    
    // here is where we want to process the file...
    
    fclose(venFile);
  }
}



OpenSim::IOVenText::~IOVenText() {}



OpenSim::TokenType
OpenSim::IOVenText::get_token()
{
  int lastChar = ' ';
  
  // skip whitespace
  while (isspace(lastChar)) lastChar = getc(venFile);
  
  // only identifiers start with characters
  if (isalpha(lastChar))
  {
    cur_tok.Type = tok_identifier;
    
    // build the string /[a-zA-Z][a-zA-Z0-9_]* /
    // (I'm rusty with regexs, but I think thats right)
    cur_tok.Identifier = lastChar;
    while (isalnum((lastChar = getc(venFile))) || (lastChar == '_'))
      cur_tok.Identifier += lastChar;
  }
  else if (isdigit(lastChar) || lastChar == '.')
  {
    // we've got a number, but I'm pretty sure we 
    // don't catch negative numbers.
    
    cur_tok.Type = tok_number;
    
    // build the string like we did for identifiers.
    cur_tok.Identifier = lastChar;
    while (isdigit((lastChar = getc(venFile))) || lastChar == '.')
      cur_tok.Identifier += lastChar;
    
    // convert it to a floating point value.
    // *** FIXME: error checking, please.
    cur_tok.NumVal = strtod(cur_tok.Identifier.c_str(), 0);
  }
  else
  {
    cur_tok.Type = tok_operator;
    cur_tok.Op = lastChar;
    cur_tok.Identifier = "";
    
    // prime lastChar
    // ** FIXME - this could cause problems if the string ends
    // on an operator?
    lastChar = getc(venFile);
  }
  
  return cur_tok.Type;
}




std::map<std::string, OpenSim::Variable *> 
OpenSim::IOVenText::Variables()
{
  return vars;
}
