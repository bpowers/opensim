//===--- SimBuilder.h - Builds an AST for a set of equations ---*- C++ -*-===//
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
// This takes a set of variables (and their corresponding equations)
// and creates an AST representation with a SimModule as the root node.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_SIMBUILDER_H
#define OSIM_SIMBUILDER_H

// openSim stuff
#include "../globals.h"
#include "../model-simulator.h"
#include "../model-variable.h"


namespace OpenSim
{
  class ExprAST;
  class VariableAST;
  class SimAST;
  class CodeGenModule;
  class PythonPrintModule;
  class InterpreterModule;
  
  
  /// SimBuilder - main class for building and interacting with models
  /// and their ASTs.  Currently SimBuilder supports building and walking
  /// model ASTs from file, but not modifying or saving them.
  ///
  class SimBuilder
  {
    /// A map of all of the defined variables, keyed by name.
    std::map<std::string, ModelVariable *> vars;
    
    /// A map of all of the defined variable AST nodes, keyed by name.
    std::map<std::string, OpenSim::VariableAST *> varASTs;
    
    /// A vector of all of the top level variable AST nodes, in the
    /// order in which they need to be solved, back to front.
    std::vector<OpenSim::VariableAST *> body;
    
    /// A vector of all of the top level variable AST nodes, in the
    /// order in which they need to be solved, back to front.
    std::vector<OpenSim::VariableAST *> initial;
    
    /// Pointer to the root of the constructed model AST.
    OpenSim::SimAST *root;
    
    /// Internal method to process the variables and construct an AST.
    void InitializeModule();
    bool _valid_model;
    int _errors;
    
    
    /// Checks to see if we've started parsing the variable with 
    /// this ID name yet.
    bool IsUnparsedTL(std::string IdName);
    std::vector<ModelVariable *> topLevelVars;
    
    std::map<char, int> BinopPrecedence;
    
    // variable equation token handling
    int toks_index;
    equ_token CurTok;
    ModelVariable *CurVar;
    
    OpenSim::ExprAST *CurVarInitial;
    
    std::vector<int> index_stack;
    std::vector<ModelVariable *> var_stack;
    
    void PushTokens();
    void PopTokens();
    bool getNextToken();
    int GetTokPrecedence();
    
    // expression parsing
    bool ProcessVar(ModelVariable *var);
    OpenSim::ExprAST *ParseBinOpRHS(int ExprPrec, OpenSim::ExprAST *LHS);
    OpenSim::ExprAST *ParsePrimary();
    OpenSim::ExprAST *ParseUnary();
    OpenSim::ExprAST *ParseTable();
    OpenSim::ExprAST *ParseNumberExpr();
    OpenSim::ExprAST *ParseExpression();
    OpenSim::ExprAST *ParseIdentifierExpr();
    OpenSim::ExprAST *ParseVarRefExpr(std::string IdName);
    
  public:
    SimBuilder(std::map<std::string, ModelVariable *> variables);
    ~SimBuilder();
    
    int Update();
    int Parse(sim_output ourWalk, FILE *output_file);
  };
}

#endif // OSIM_SIMBUILDER_H
