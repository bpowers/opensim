/*
 *  Variable.cpp
 *  OpenSim
 *
 *  Created by Bobby Powers on 3/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <algorithm>
#include "Variable.h"

// openSim stuff
#include "globals.h"
using std::string;
using std::vector;


void 
OpenSim::Variable::SetEquation(std::string newEquation) 
{
  if (equation != newEquation) tokens.clear();
  
  equation = newEquation;
  type = var_undef;
}



std::vector<OpenSim::EquToken> 
OpenSim::Variable::EquationTokens()
{
  // lazy tokenization.
  if ((equation != "") && (tokens.size() == 0)) tokenize();
  
  return tokens;
}



OpenSim::varType 
OpenSim::Variable::Type()
{
  // lazy tokenization.
  if ((equation != "") && (tokens.size() == 0)) tokenize();
  
  return type;
}



int
OpenSim::Variable::tokenize()
{
  if (type == var_undef) type = var_aux;
  
  
  // if the equation is empty return an error
  if (equation.length() == 0) return -1;
  
  // keep track of where we are in the equation
  // and prime our character buffer
  string::size_type charPos = 0;
  char lastChar = equation[charPos++];
  
  // now we process the equation string one character at a time,
  // testing to see if it fits into any of the patterns we recognize.
  while (charPos <= equation.length())
  {
    EquToken newTok;
    
    // skip whitespace
    while (isspace(lastChar)) lastChar = equation[charPos++];
    
    // only identifiers start with characters
    if (isalpha(lastChar))
    {
      newTok.Type = tok_identifier;
      
      // build the string /[a-zA-Z][a-zA-Z0-9_]*/
      // (I'm rusty with regexs, but I think thats right)
      newTok.Identifier = lastChar;
      while (isalnum((lastChar = equation[charPos++])) || (lastChar == '_'))
        newTok.Identifier += lastChar;
      
      if ((tokens.size() == 0) && (newTok.Identifier == "INTEG"))
        type = var_stock;
      
      tokens.push_back(newTok);
      continue;
    }
    else if (isdigit(lastChar) || lastChar == '.')
    {
      // we've got a number, but I'm pretty sure we 
      // don't catch negative numbers.
      
      newTok.Type = tok_number;
      
      // build the string like we did for identifiers.
      newTok.Identifier = lastChar;
      while (isdigit((lastChar = equation[charPos++])) || lastChar == '.')
        newTok.Identifier += lastChar;
      
      // convert it to a floating point value.
      // *** FIXME: error checking, please.
      newTok.NumVal = strtod(newTok.Identifier.c_str(), 0);
      
      tokens.push_back(newTok);
      continue;
    }
    else
    {
      newTok.Type = tok_operator;
      newTok.Op = lastChar;
      newTok.Identifier = "";
      
      // prime lastChar
      // ** FIXME - this could cause problems if the string ends
      // on an operator?
      lastChar = equation[charPos++];
      
      tokens.push_back(newTok);
      continue;
    }
  }
  
  std::reverse(tokens.begin(), tokens.end());
  
  if ((tokens.size() == 1) && (tokens[0].Type == tok_number)) 
  {
    type = var_const;
  }
  
  
  return 0;
}

