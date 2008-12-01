/*===--- ast.h - AST-related declarations ---------------------------------===
 *
 * Copyright 2008 Bobby Powers
 *
 * This file is part of OpenSim.
 * 
 * OpenSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OpenSim.  If not, see <http: *www.gnu.org/licenses/>.
 *
 *===----------------------------------------------------------------------===
 *
 * This header contains all of the structures and public functions related 
 * to opensim ASTs used to represent models
 *
 *===----------------------------------------------------------------------===
 */

#ifndef __OPENSIM_AST_H__
#define __OPENSIM_AST_H__

#include <glib.h>


// there is prolly a safer way to do this...
#define cast_node(type, node) (type *) node


typedef struct _ast_context ast_context;
struct _ast_context
{
  GHash *     var_hash;
  GHash *     val_hash;
  ast_scope  *ast;
  
  bool      (* codegen) (ast_context ctx);
}


typedef enum _opcode opcode;
enum _opcode 
{
  op_plus,
  op_minus,
  op_not,
  op_subscript,
  op_mult,
  op_div,
  op_exp,
  op_lt,
  op_gt,
  op_le,
  op_ge,
  op_eq,
  op_ne,
  op_and,
  op_or,
  op_assign
}


typedef enum _node_type node_type;
enum _node_type 
{
  node_none = 0,
  node_list,
  node_scope,
  node_binary, // expr node
  node_unary,
  node_if,
  node_loop,
  node_const,  // leaf node
  node_call,
  node_iden    // leaf node
}


// for use as flags, thats why we bitshift
typedef enum _attribute attribute;
enum _attribute
{
  attr_model   = 1 << 1,
  attr_sketch  = 1 << 2,
  attr_initial = 1 << 3,
  attr_scalar  = 1 << 4,
  attr_array   = 1 << 5,
  attr_stock   = 1 << 6,
  attr_value   = 1 << 7,
  attr_integ   = 1 << 8,
  attr_euler   = 1 << 9,
  attr_rk2     = 1 << 10,
  attr_rk4     = 1 << 11,
  attr_rk4auto = 1 << 12,
  attr_do      = 1 << 13,
  attr_for     = 1 << 14
}


// used to box return values of expressions
typedef enum _var_kind var_kind;
enum _var_kind
{
  kind_real,
  kind_bool
}


typedef struct _type type;
struct _type
{
  var_kind kind;
  
  union
  {
    gbool  boolean;
    double real;
  }
}



typedef struct _ast_node        ast_node;
typedef struct _ast_list        ast_list;
typedef struct _ast_scope       ast_scope;
typedef struct _ast_expr        ast_expr;
typedef struct _ast_leaf        ast_leaf;
typedef struct _ast_call        ast_call;
typedef struct _ast_if          ast_if;
typedef struct _ast_unary       ast_unary;
typedef struct _ast_loop        ast_loop;


struct _ast_node 
{
  guint64           type;
  attributes        attrs;
  
  gchar *           identifier;
}


struct _ast_list
{
  guint64           info;
  guint64           attrs;
  
  gchar *           identifier;
  GList *           nodes;
}


struct _ast_scope
{
  guint64           info;
  guint64           attrs;
  
  gchar *           identifier;
  ast_list *        nodes;
  
  union 
  {
    OpensimSimulator *sim;
    // in the future will put sketch here
  }
}


struct _ast_expr
{
  guint64             info;
  guint64             attrs;
  
  gchar *             identifier;
  
  opcode              op;
  ast_expr *          lvar;
  ast_expr *          rvar;
}


struct _ast_leaf
{
  guint64             info;
  guint64             attrs;
  
  gchar *             identifier;
  
  union 
  {
    double            val_real; // for normal real numbers
    gbool             val_bool; // for boolean logic
    OpensimVariable * val_var;  // for var identifiers
  }
}


struct _ast_call
{
  guint64             info;
  guint64             attrs;
  
  gchar *             identifier;
  
  GList *             val_args; // for function calls
}


struct _ast_if
{
  guint64             info;
  guint64             attrs;
  
  gchar *             identifier;
  
  ast_expr *          cond_statement;
  ast_expr *          then_statement;
  ast_expr *          else_statement;
}


struct _ast_unary
{
  guint64             info;
  guint64             attrs;
  
  gchar *             identifier;
  
  opcode              op;
  ast_node *          lvar;
}


struct _ast_loop
{
  guint64             info;
  guint64             attrs;
  
  gchar *             identifier;
  
  GList *             loop_args;
  ast_list *          body;
}



#endif /* __OPENSIM_AST_H__ */
