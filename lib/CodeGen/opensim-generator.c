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

#include <stdio.h>

#include "opensim-generator.h"

#include "../opensim-simulator.h"
#include "../opensim-variable.h"


#define OPENSIM_GENERATOR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENSIM_TYPE_GENERATOR, \
                                            OpensimGeneratorPrivate))


static gpointer opensim_generator_parent_class = NULL;
static void opensim_generator_init          (OpensimGenerator *self);
static void opensim_generator_class_init    (OpensimGeneratorClass *klass);
static void opensim_generator_dispose       (GObject *gobject);
static void opensim_generator_finalize      (GObject *gobject);
       void opensim_generator_get_property  (GObject      *object,
                                             guint         property_id,
                                             GValue       *value,
                                             GParamSpec   *pspec);
       void opensim_generator_set_property  (GObject      *object,
                                             guint         property_id,
                                             const GValue *value,
                                             GParamSpec   *pspec);

static int opensim_generator_default_rebase (OpensimGenerator *generator, 
                                             GHashTable *variables);
static int opensim_generator_default_update (OpensimGenerator *generator);
static int opensim_generator_default_parse  (OpensimGenerator *generator, 
                                             int our_walk,
                                             FILE *output_file);

static gboolean process_var                 (OpensimGenerator *generator, 
                                             OpensimVariable *var);
static int build_bytecode ();
static int get_tok_precedence (int op_char);

struct _OpensimGeneratorPrivate
{
  int              errors;
  gboolean         valid_model;
  
  GHashTable      *top_level_vars;
  
  GHashTable      *vars;
  GByteArray      *bytecode;
  
  OpensimVariable *cur_var;
  int              tok_index;
};



GType 
opensim_generator_get_type ()
{
  static GType g_define_type_id = 0; 
  if (G_UNLIKELY(g_define_type_id == 0)) 
  { 
    static const GTypeInfo g_define_type_info = { 
      sizeof (OpensimGeneratorClass), 
      (GBaseInitFunc) NULL, 
      (GBaseFinalizeFunc) NULL, 
      (GClassInitFunc) opensim_generator_class_init, 
      (GClassFinalizeFunc) NULL, 
      NULL,   // class_data 
      sizeof (OpensimGenerator), 
      0,      // n_preallocs 
      (GInstanceInitFunc) opensim_generator_init, 
    }; 
    g_define_type_id = g_type_register_static(G_TYPE_OBJECT, 
                                              "OpensimGeneratorType", 
                                              &g_define_type_info, 
                                              (GTypeFlags) 0); 
  } 
  return g_define_type_id; 
}



void
opensim_generator_set_property(GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  OpensimGenerator *generator = OPENSIM_GENERATOR(object);
  OpensimGeneratorPrivate *self = generator->priv;
  
  switch (property_id)
  {
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}



void
opensim_generator_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  OpensimGenerator *generator = OPENSIM_GENERATOR(object);
  OpensimGeneratorPrivate *self = generator->priv;
  
  switch (property_id)
  {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
opensim_generator_class_init(OpensimGeneratorClass *klass)
{
  opensim_generator_parent_class = g_type_class_peek_parent(klass);
  
  g_type_class_add_private(klass, sizeof (OpensimGeneratorPrivate));
  
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *opensim_param_spec;
  
  gobject_class->set_property = opensim_generator_set_property;
  gobject_class->get_property = opensim_generator_get_property;
  gobject_class->dispose      = opensim_generator_dispose;
  gobject_class->finalize     = opensim_generator_finalize;
  
  klass->rebase               = opensim_generator_default_rebase;
  klass->update               = opensim_generator_default_update;
  klass->parse                = opensim_generator_default_parse;  
}



static void
opensim_generator_init(OpensimGenerator *generator)
{
  generator->priv = OPENSIM_GENERATOR_GET_PRIVATE(generator);
  OpensimGeneratorPrivate *self = generator->priv;
  
  self->valid_model    = FALSE;
  self->errors         = 0;
  
  self->top_level_vars = NULL;
  self->vars           = NULL;
  self->bytecode       = NULL;
  
  self->cur_var        = NULL;
  self->tok_index      = 0;
}



static void
opensim_generator_dispose(GObject *gobject)
{
  //OpensimVariable *self = OPENSIM_VARIABLE(gobject);
  
  /* 
   * In dispose, you are supposed to free all typesecifier before 'IOVenText'
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */
  
  /* dispose might be called multiple times, so we must guard against
   * calling g_object_unref() on an invalid GObject.
   */
  
  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_generator_parent_class)->dispose(gobject);
}



static void
opensim_generator_finalize(GObject *gobject)
{
  OpensimGenerator *generator = OPENSIM_GENERATOR(gobject);
  OpensimGeneratorPrivate *self = generator->priv;
  
  /* free any memory (and strings we've allocated */
  if (self->bytecode) g_byte_array_free (self->bytecode, TRUE);
  
  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_generator_parent_class)->finalize(gobject);
}



int
opensim_generator_rebase (OpensimGenerator *generator,
                          GHashTable       *variables)
{
  return OPENSIM_GENERATOR_GET_CLASS (generator)->rebase (generator,
                                                          variables);
}



static int
opensim_generator_default_rebase (OpensimGenerator *generator,
                                  GHashTable       *variables)
{
  /* first replace hash table, then call update */
  
  return 0;
}



int
opensim_generator_update (OpensimGenerator *generator)
{
  return OPENSIM_GENERATOR_GET_CLASS (generator)->update (generator);
}



static int
opensim_generator_default_update (OpensimGenerator *generator)
{
  /* delete generated data structures and then initialize module */

  return 0;
}



int
opensim_generator_parse (OpensimGenerator *generator,
                         int               our_walk,
                         FILE             *output_file)
{
  return OPENSIM_GENERATOR_GET_CLASS (generator)->parse (generator,
                                                         our_walk,
                                                         output_file);
}



static int
opensim_generator_default_parse (OpensimGenerator *generator,
                                 int               our_walk,
                                 FILE             *output_file)
{
  fprintf (stderr, "OMG.\n");

  OpensimGeneratorPrivate *self = generator->priv;
  
  if (!self->valid_model)
  {
    fprintf (stderr, "Error: model not valid; not running.\n");
    return -1;
  }
  
  switch (our_walk)
  {
  case sim_emit_Python:
    
    break;
    
  case sim_emit_AS3:
    
    break;
    
  case sim_emit_IR:
    fprintf (stderr, "Error: Sorry, JIT is disabled.\n");
    return -2;
    break;
    
  case sim_emit_Output:
    
    break;
    
  default:
    fprintf (stderr, "Error: unrecognized internal walk_type.\n");
    break;
  }
  
  return 0;
}



static int initialize_module (OpensimGenerator *generator)
{
  OpensimGeneratorPrivate *self = generator->priv;

  // these should be cleared by now, but lets just make sure
  if (self->top_level_vars)
  {
    fprintf (stderr, "Error: top_level_vars exist before initialization\n");
    return -1;
  }

  self->top_level_vars = g_hash_table_new (g_str_hash, g_str_equal);

  GHashTableIter iter;
  gpointer key, value;

  // add all of the Variables to our top_level_vars hash
  g_hash_table_iter_init (&iter, self->vars);
  while (g_hash_table_iter_next (&iter, &key, &value)) 
  {
    g_hash_table_insert (self->top_level_vars, key, value);
  }

  // construct our bytecode for each variable
  g_hash_table_iter_init (&iter, self->top_level_vars);
  while (g_hash_table_iter_next (&iter, &key, &value))
  {
    OpensimVariable *var = OPENSIM_VARIABLE (value);

    // slowly eat away at the table
    g_hash_table_iter_remove (&iter);

    process_var (generator, var);
  }

  return -1;
}
/*
void 
OpenSim::SimBuilder::InitializeModule()
{
  for (map<string, OpensimVariable *>::iterator itr = vars.begin(); 
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
    OpensimVariable *var = topLevelVars.back();
    topLevelVars.pop_back();

    ProcessVar(var);
  }
  
  if (_errors)
  {
    _valid_model = false;
    fprintf(stderr, "opensim: The model has %d errors.\n", _errors);
  }
  else
    _valid_model = true;
  
  // these are the root AST nodes.
  EulerAST *integrate = new EulerAST(body);
  root = new SimAST(integrate, initial, varASTs);
}



bool 
OpenSim::SimBuilder::getNextToken()
{
  const GArray *toks = opensim_variable_get_tokens(CurVar);

  if (toks_index >= toks->len) return false;

  //gchar    *var_name = NULL;
  //g_object_get(G_OBJECT(CurVar), "name", &var_name, NULL);
                              
  equ_token tok = g_array_index(toks, equ_token, toks_index);
  
  //fprintf(stdout, "  *tok '%s' ('%c' '%d') '%s' (%f)\n", 
  //        var_name, tok.op, tok.type, 
  //        tok.identifier, tok.num_val);
  
  //g_free(var_name);
  
  CurTok = tok;
  ++toks_index;

  return true;
}



void 
OpenSim::SimBuilder::PushTokens()
{
  // we want to preserve all the tokens, which includes
  // CurTok, so we push it back on the toks vector before
  // pushing that onto our token stack
  index_stack.push_back(--toks_index);
  
  var_stack.push_back(CurVar);
}



void 
OpenSim::SimBuilder::PopTokens()
{
  // we don't care about the current vector at toks, so 
  // we just put back the back of the stack and prime 
  // CurTok through getNextToken()
  toks_index = index_stack.back();
  index_stack.pop_back();
  
  CurVar = var_stack.back();
  var_stack.pop_back();
  
  getNextToken();
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
    char BinOp = CurTok.op;
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
  if (CurTok.type != tok_operator)
    return ParsePrimary();
  
  // If this is a unary operator, read it.
  char cur_op = CurTok.op;
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
  // honestly, i forget why this needs to be 2...
  if (toks_index == 2)
  {
    // valid lookup value, start parsing it.
    vector< pair<double, double> > tuples;
    
    // for well formed entries, should have n entries with the format
    // (index,value)
    
    while (CurTok.type == tok_operator && CurTok.op == '(') 
    {
      double x, y;
      
      // get the index of the tuple
      getNextToken();
      if (CurTok.type != tok_number)
      {
        fprintf(stderr, "Error: Expecting a number in lookup, not '%s'\n",
                CurTok.identifier);
        _errors++;
        
        break;
      }
      
      x = CurTok.num_val;
      
      // get the comma in the tuple
      getNextToken();
      if (CurTok.type != tok_operator || CurTok.op != ',')
      {
        fprintf(stderr, "Error: Expecting a comma in lookup, not '%s'\n",
                CurTok.identifier);
        _errors++;
        
        break;
      }
      
      // get the index of the tuple
      getNextToken();
      if (CurTok.type != tok_number)
      {
        fprintf(stderr, "Error: Expecting a number in lookup, not '%s'\n",
                CurTok.identifier);
        _errors++;
        
        break;
      }
      
      y = CurTok.num_val;
      
      // get the closing parenthesis
      getNextToken();
      if (CurTok.type != tok_operator || CurTok.op != ')')
      {
        fprintf(stderr, "Error: Expecting a ')' in lookup, not '%s'\n",
                CurTok.identifier);
        _errors++;
        
        break;
      }
      
      // add our parsed data to the list
      tuples.push_back(pair<double, double>(x, y));
      
      // FIXME: remove debug info
      //fprintf(stdout, "Info: parsed the tuple (%f, %f)\n", x, y);
      
      // get the comma in the tuple
      getNextToken();
      if (CurTok.type != tok_operator || CurTok.op != ',')
      {
        // its not an error if its the closing bracket
        if (CurTok.type != tok_operator || CurTok.op != ']')
        {
          fprintf(stderr, "Error: Expecting a comma in lookup, not '%s'\n",
                  CurTok.identifier);
          _errors++;
        }
        
        break;
      }
      
      // eat the ','
      getNextToken();
    }
    
    return new LookupAST(CurVar, tuples);
  }
  
  gchar *v_name = NULL;
  
  g_object_get(G_OBJECT(CurVar), "name", &v_name, NULL);
  // shouldn't reach here.
  fprintf(stderr, 
          "Error: '[' in a weird and undefined place for %s (%d).\n", 
          v_name, toks_index);

  g_free(v_name);
  return 0;
}



ExprAST *
OpenSim::SimBuilder::ParsePrimary() 
{
  // we check what type of primary identifier we could have,
  // but currently only support identifiers and numbers.
  switch (CurTok.type) 
  {
    case tok_identifier: 
      return ParseIdentifierExpr();
    case tok_number:     
      return ParseNumberExpr();
    default:
      //fprintf(stderr, "Warning: unknown top level token of type '%d'\n", 
      //        CurTok.type);
      return 0;
  }
}



bool 
OpenSim::SimBuilder::IsUnparsedTL(std::string IdName)
{
  bool ret = false;
  // this is somewhat expensive, but we check to see if there is a
  // variable with the specified name in topLevelVars.
  for (vector<OpensimVariable *>::iterator itr = topLevelVars.begin();
       itr != topLevelVars.end(); ++itr)
  {
    gchar *name = NULL;
    
    g_object_get(G_OBJECT(*itr), "name", &name, NULL);
    if (!g_strcmp0(IdName.c_str(), name))
      ret = true;
    g_free(name);
  }

  return ret;
}



ExprAST *
OpenSim::SimBuilder::ParseIdentifierExpr() 
{
  std::string IdName = CurTok.identifier;
  bool table_function = false;
  char close_op = ')';
  
  // eat identifier.
  getNextToken();  
  
  // its a simple variable reference if we don't have brackets
  if (CurTok.op != '(' && CurTok.op != '[') 
  {
    return ParseVarRefExpr(IdName);
  }
  
  if (CurTok.op == '[') 
  {
    table_function = true;
    close_op = ']';
  }
  
  std::vector<ExprAST*> Args;
  
  // okay, we have a call in parenthesis, eat the opening bracket
  getNextToken();
  while (CurTok.op != close_op) 
  {
    ExprAST *Arg = ParseExpression();
    if (!Arg) return 0;
    Args.push_back(Arg);
    
    if (CurTok.op == close_op) break;
    
    if (CurTok.op != ',')
    {
      fprintf(stdout, "Error: expected ')' while parsing %c.\n", CurTok.op);
      _errors++;
      
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
      _errors++;
      
      return 0;
    }
    
    CurVarInitial = Args[1];
    
    
    gchar *name = NULL;
    g_object_get(G_OBJECT(CurVar), "name", &name, NULL);  
    ExprAST *stock_call = new VarRefAST(name);
    g_free(name);
    
    ExprAST *dt = new VarRefAST("time_step");
    ExprAST *change = new BinaryExprAST('*', Args[0], dt);
    
    return new BinaryExprAST('+', stock_call, change);
  }
  
  if (table_function)
  {
    if (Args.size() != 1)
    {
      fprintf(stderr, "Error: Table functions take 1 argument.\n");
      _errors++;
      
      return 0;
    }
    
    return new LookupRefAST(IdName, Args[0]);
  }
  
  // if we get here, we must have a function call.
  return new FunctionRefAST(IdName, Args);
}



ExprAST *
OpenSim::SimBuilder::ParseVarRefExpr(std::string IdName)
{
  if (IdName == "time") return new VarRefAST("time");
  
  if (vars.find(IdName) == vars.end())
  {
    fprintf(stderr, "Error: Reference to undefined variable '%s'\n", 
            IdName.c_str());
    _errors++;
    
    return NULL;
  }
  
  OpensimVariable *requestedVar = vars[IdName];
  
  var_type v_type = var_undef;
  g_object_get(G_OBJECT(requestedVar), "type", &v_type,  NULL);
  
  if ((v_type != var_stock) && IsUnparsedTL(IdName))
  {
    for (vector<OpensimVariable *>::iterator itr = topLevelVars.begin();
         itr != topLevelVars.end(); ++itr)
    {
      gchar *name = NULL;
      
      g_object_get(G_OBJECT(*itr), "name", &name, NULL);
      if (!g_strcmp0(IdName.c_str(), name))
      {
        g_free(name);
        topLevelVars.erase(itr);
        break;
      }
      
      g_free(name);
    }
    
    PushTokens();
      ProcessVar(requestedVar);
    PopTokens();
  }
  
  VarRefAST *ret = new VarRefAST(IdName); 
  
  return ret;
}



ExprAST *
OpenSim::SimBuilder::ParseNumberExpr() 
{
  ExprAST *Result = new NumberExprAST(CurTok.num_val);
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

*/

static gboolean 
process_var (OpensimGenerator *generator, OpensimVariable *var)
{
  OpensimGeneratorPrivate *self = generator->priv;
  
  const GArray *toks = opensim_variable_get_tokens (var);
  self->tok_index = 0;
  
  self->cur_var = var;
  
  var_type  var_t = var_undef;
  gchar    *var_name = NULL;
  g_object_get (G_OBJECT (var), "type", &var_t, 
                                "name", &var_name, NULL);
  
  if (toks->len == 0)
  {
    fprintf (stderr, "Error: variable '%s' has empty equation field\n", 
             var_name);
    self->errors++;
    
    g_free (var_name);
    return FALSE;
  }

  fprintf (stdout, "processing: %s\n", var_name);
  /*
  // prime CurToken
  get_next_token ();
  ExprAST *val = parse_expression ();
  
  // this is our variable
  VariableAST *newNode = new VariableAST(var, val);
  
  // all stocks go through processVar, and before we add them we need
  // to set their initial values AST nodes.
  if (var_t == var_stock)
  {
    newNode->SetInitial(CurVarInitial);
    
    if (CurVarInitial == 0)
    {
      fprintf(stderr, "Error: stock '%s' has empty initial value field\n", 
              var_name);
      _errors++;
      
      g_free(var_name);
      return false;
    }
    
    CurVarInitial = 0;
  }
  
  if (var_t == var_aux || var_t == var_stock)
    body.push_back(newNode);
  
  if (var_t == var_const || var_t == var_lookup || var_t == var_stock)
    initial.push_back(newNode);
  
  varASTs[var_name] = newNode;
   
  */
  g_free (var_name);

  return FALSE;//val ? TRUE : FALSE;
}


static int 
get_tok_precedence (int op_char) 
{
  // standard mathmatical operators.
  
  switch (op_char) 
  {
  case '=':
    return 2;
  case '<':
    return 10;
  case '>':
    return 10;
  case '+':
    return 20;
  case '-':
    return 20;
  case '*':
    return 40;
  case '/':
    return 40;
  case '^':
    return 60;
  default:
    return -1;
  }
}
