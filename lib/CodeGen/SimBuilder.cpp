//===--- SimBuilder.cpp - Builds an AST for a set of equations -----------===//
//
// Copyright 2008 Bobby Powers, portions Chris Lattner (LLVM tutorial)
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
// This takes a set of variables (and their corresponding equations)
// and creates an AST representation with a SimModule as the root node.
//
//===---------------------------------------------------------------------===//

#include "SimBuilder.h"
#include "../AST/SimAST.h"
#include "../AST/EulerAST.h"
#include "../AST/VariableAST.h"
#include "../AST/LookupAST.h"
#include "../AST/General.h"

#include "PythonPrintModule.h"
#include "InterpreterModule.h"
#include "AS3PrintModule.h"

#include <cstdio>
using std::pair;
using std::string;
using std::vector;
using std::map;

using OpenSim::Variable;
using OpenSim::ExprAST;
using OpenSim::NumberExprAST;
using OpenSim::EulerAST;
using OpenSim::UnaryExprAST;


OpenSim::SimBuilder::SimBuilder(std::map<std::string, Variable *> variables)
{
  // save the variables we're passed.
  vars = variables;
    
  // Install standard binary operators.
  // 1 is lowest precedence.
  BinopPrecedence['='] = 2;
  BinopPrecedence['<'] = 10;
  BinopPrecedence['>'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;
  BinopPrecedence['/'] = 40;
  BinopPrecedence['^'] = 60;// highest.

  // creates AST from variable definitions.
  InitializeModule();
}



OpenSim::SimBuilder::~SimBuilder()
{
}



int 
OpenSim::SimBuilder::Parse(WalkType ourWalk, FILE *output_file)
{
  ASTConsumer *consumer = NULL;
  
  switch (ourWalk)
  {
    case walk_Python:
      consumer = new PythonPrintModule();
      break;
    case walk_AS3:
      consumer = new AS3PrintModule();
      break;
    case walk_IR:
      fprintf(stdout, "Error: Sorry, JIT is disabled.\n");
      return -2;
      //consumer = new CodeGenModule();
      break;
    case walk_Interpret:
      consumer = new InterpreterModule();
      break;
    default:
      break;
  }
  
  // if we were able to create a consumer, have it 
  // eat the AST and spit something out.
  if (consumer) 
  {
    consumer->Consume(root, output_file);
    delete consumer;
    
    return 0;
  }
  
  return -1;
}



void 
OpenSim::SimBuilder::InitializeModule()
{
  for (map<string, Variable *>::iterator itr = vars.begin(); 
       itr != vars.end(); itr++) 
  {
    topLevelVars.push_back(itr->second);
  }

  // now build ASTs until our top level variables are done.
  // its a while loop, becuase 1 or more topLevelVars will be
  // recursively parsed with the call to ProcessVar.  Any references
  // to variables in the equation will be resolved and if the 
  // variable hasn't been parsed yet, it will be then and 
  // removed from this vector.
  while (topLevelVars.size() > 0)
  {
    Variable *var = topLevelVars.back();
    topLevelVars.pop_back();

    ProcessVar(var);
  }
  
  
  // these are the root AST nodes.
  EulerAST *integrate = new EulerAST(body);
  root = new SimAST(integrate, varASTs);
}



bool 
OpenSim::SimBuilder::getNextToken()
{
  if (toks.size() == 0) return false;

  CurTok = toks.back();
  toks.pop_back();

  return true;
}



void 
OpenSim::SimBuilder::PushTokens()
{
  // we want to preserve all the tokens, which includes
  // CurTok, so we push it back on the toks vector before
  // pushing that onto our token stack
  toks.push_back(CurTok);
  stack.push_back(toks);
  
  var_stack.push_back(CurVar);
}



void 
OpenSim::SimBuilder::PopTokens()
{
  // we don't care about the current vector at toks, so 
  // we just put back the back of the stack and prime 
  // CurTok through getNextToken()
  toks = stack.back();
  stack.pop_back();
  getNextToken();
  
  CurVar = var_stack.back();
  var_stack.pop_back();
}



ExprAST *
OpenSim::SimBuilder::ParseBinOpRHS(int ExprPrec, ExprAST *LHS) 
{
  // If this is a binop, find its precedence.
  while (true) 
  {
    int TokPrec = GetTokPrecedence();
    
    // If this is a binop that binds at least as tightly as the current binop,
    // consume it, otherwise we are done.
    if (TokPrec < ExprPrec) return LHS;
    
    // Okay, we know this is a binop.
    char BinOp = CurTok.Op;
    getNextToken();  // eat binop
    
    // Parse the primary expression after the binary operator.
    ExprAST *RHS = ParseUnary();
    if (!RHS) return 0;
    
    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) 
    {
      RHS = ParseBinOpRHS(TokPrec+1, RHS);
      if (RHS == 0) return 0;
    }

    // Merge LHS/RHS.
    LHS = new BinaryExprAST(BinOp, LHS, RHS);
  }
}



ExprAST *
OpenSim::SimBuilder::ParseUnary() 
{
  // If the current token is not an operator, it must be a primary expr.
  if (CurTok.Type != tok_operator)
    return ParsePrimary();
  
  // If this is a unary operator, read it.
  char cur_op = CurTok.Op;
  getNextToken();
  
  if (cur_op == '[')
  {
    return ParseTable();
  }
  
  if (ExprAST *Operand = ParseUnary())
    return new UnaryExprAST(cur_op, Operand);
  return 0;
}



ExprAST *
OpenSim::SimBuilder::ParseTable()
{
  if (toks.size() == CurVar->EquationTokens().size()-2)
  {
    // valid lookup value, start parsing it.
    vector< pair<double, double> > tuples;
    
    // for well formed entries, should have n entries with the format
    // (index,value)
    
    while (CurTok.Type == tok_operator && CurTok.Op == '(') 
    {
      double x, y;
      
      // get the index of the tuple
      getNextToken();
      if (CurTok.Type != tok_number)
      {
        fprintf(stderr, "Error: Expecting a number in lookup, not '%s'\n",
                CurTok.Identifier.c_str());
        break;
      }
      
      x = CurTok.NumVal;
      
      // get the comma in the tuple
      getNextToken();
      if (CurTok.Type != tok_operator || CurTok.Op != ',')
      {
        fprintf(stderr, "Error: Expecting a comma in lookup, not '%s'\n",
                CurTok.Identifier.c_str());
        break;
      }
      
      // get the index of the tuple
      getNextToken();
      if (CurTok.Type != tok_number)
      {
        fprintf(stderr, "Error: Expecting a number in lookup, not '%s'\n",
                CurTok.Identifier.c_str());
        break;
      }
      
      y = CurTok.NumVal;
      
      // get the closing parenthesis
      getNextToken();
      if (CurTok.Type != tok_operator || CurTok.Op != ')')
      {
        fprintf(stderr, "Error: Expecting a ')' in lookup, not '%s'\n",
                CurTok.Identifier.c_str());
        break;
      }
      
      // add our parsed data to the list
      tuples.push_back(pair<double, double>(x, y));
      
      // FIXME: remove debug info
      //fprintf(stdout, "Info: parsed the tuple (%f, %f)\n", x, y);
      
      // get the comma in the tuple
      getNextToken();
      if (CurTok.Type != tok_operator || CurTok.Op != ',')
      {
        // its not an error if its the closing bracket
        if (CurTok.Type != tok_operator || CurTok.Op != ']')
          fprintf(stderr, "Error: Expecting a comma in lookup, not '%s'\n",
                  CurTok.Identifier.c_str());
        
        break;
      }
      
      // eat the ','
      getNextToken();
    }
    
    return new LookupAST(CurVar, tuples);
  }
  
  // shouldn't reach here.
  fprintf(stderr, "Error: '[' in a weird and undefined place.\n");
  return 0;
}



ExprAST *
OpenSim::SimBuilder::ParsePrimary() 
{
  // we check what type of primary identifier we could have,
  // but currently only support identifiers and numbers.
  switch (CurTok.Type) 
  {
    case tok_identifier: 
      return ParseIdentifierExpr();
    case tok_number:     
      return ParseNumberExpr();
    default:
      return 0;
  }
}



bool 
OpenSim::SimBuilder::IsUnparsedTL(std::string IdName)
{
  // this is somewhat expensive, but we check to see if there is a
  // variable with the specified name in topLevelVars.
  for (vector<Variable *>::iterator itr = topLevelVars.begin();
       itr != topLevelVars.end(); ++itr)
  {
    if (IdName == (*itr)->Name()) return true;
  }

  return false;
}



ExprAST *
OpenSim::SimBuilder::ParseIdentifierExpr() 
{
  std::string IdName = CurTok.Identifier;
  bool table_function = false;
  char close_op = ')';
  
  // eat identifier.
  getNextToken();  
  
  // its a simple variable reference if we don't have brackets
  if (CurTok.Op != '(' && CurTok.Op != '[') 
  {
    return ParseVarRefExpr(IdName);
  }
  
  if (CurTok.Op == '[') 
  {
    table_function = true;
    close_op = ']';
  }
  
  std::vector<ExprAST*> Args;
  
  // okay, we have a call in parenthesis, eat the opening bracket
  getNextToken();
  while (CurTok.Op != close_op) 
  {
    ExprAST *Arg = ParseExpression();
    if (!Arg) return 0;
    Args.push_back(Arg);
    
    if (CurTok.Op == close_op) break;
    
    if (CurTok.Op != ',')
    {
      fprintf(stdout, "Error: expected ')' while parsing %c.", CurTok.Op);
      return 0;
    }
    getNextToken();
  }
  
  // Eat the closing bracket
  getNextToken();
  
  // build "stock + change*dt"
  if (IdName == "INTEG")
  {
    if (Args.size() != 2)
    {
      fprintf(stdout, "Error: integrals take only 2 arguments.\n");
      return 0;
    }
    
    CurVarInitial = Args[1];
    
    ExprAST *stock_call = new VarRefAST(CurVar->Name());
    ExprAST *dt = new VarRefAST("OS_timestep");
    ExprAST *change = new BinaryExprAST('*', Args[0], dt);
    
    return new BinaryExprAST('+', stock_call, change);
  }
  
  if (table_function)
  {
    if (Args.size() != 1)
    {
      fprintf(stderr, "Error: Table functions take 1 argument.\n");
      return 0;
    }
    
    return new LookupRefAST(IdName, Args[0]);
  }
  
  fprintf(stderr, "Info: Call to function '%s'\n", IdName.c_str());
  // if we get here, we must have a function call.
  return new FunctionRefAST(IdName, Args);
}



ExprAST *
OpenSim::SimBuilder::ParseVarRefExpr(std::string IdName)
{
  // FIXME: dirty hack to get time references working...
  if (IdName == "time") return new VarRefAST("time");
  
  // FIXME: this is where error checking code goez
  Variable *requestedVar = vars[IdName];
  
  if ((requestedVar->Type() == var_aux) && IsUnparsedTL(IdName))
  {
    for (vector<Variable *>::iterator itr = topLevelVars.begin();
         itr != topLevelVars.end(); ++itr)
    {
      if (IdName == (*itr)->Name())
      {
        topLevelVars.erase(itr);
        break;
      }
    }
    
    PushTokens();
      ProcessVar(requestedVar);
    PopTokens();
  }
  
  return new VarRefAST(requestedVar->Name()); 
}



ExprAST *
OpenSim::SimBuilder::ParseNumberExpr() 
{
  ExprAST *Result = new NumberExprAST(CurTok.NumVal);
  getNextToken(); // consume the number

  return Result;
}



ExprAST *
OpenSim::SimBuilder::ParseExpression() 
{
  ExprAST *LHS = ParseUnary();
  if (!LHS) return 0;
  
  return ParseBinOpRHS(0, LHS);
}



bool 
OpenSim::SimBuilder::ProcessVar(Variable *var)
{
  toks = var->EquationTokens();
  CurVar = var;
  
  if (toks.size() == 0)
  {
    fprintf(stdout, "Warning: variable '%s' has empty equation field\n", 
            var->Name().c_str());
    return NULL;
  }

  // prime CurToken
  getNextToken();
  ExprAST *val = ParseExpression();
  
  // this is our variable
  VariableAST *newNode = new VariableAST(var, val);
  
  // all stocks go through processVar, and before we add them we need
  // to set their initial values AST nodes.
  if (var->Type() == var_stock)
  {
    newNode->SetInitial(CurVarInitial);
    
    if (CurVarInitial == 0)
    {
      fprintf(stdout, "Warning: stock '%s' has empty initial value field\n", 
              var->Name().c_str());
      return NULL;
    }
    
    CurVarInitial = 0;
  }
  
  if (!(newNode->Data()->Type() == var_const) 
      && !(newNode->Data()->Type() == var_lookup))
    body.push_back(newNode);
  varASTs[var->Name()] = newNode;

  return val ? true : false;
}



int 
OpenSim::SimBuilder::GetTokPrecedence() 
{
  if (!(CurTok.Type == tok_operator)) return -1;

  // Make sure it's a declared binop.
  int TokPrec = BinopPrecedence[CurTok.Op];
  
  // less than 0 means that its not in the map
  if (TokPrec <= 0) return -1;

  return TokPrec;
}
