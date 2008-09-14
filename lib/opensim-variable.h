//===--- opensim-variable.h - Base class for representing sim variables ----===//
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
// This class represents the AST nodes of Variables.
//
//===---------------------------------------------------------------------===//


#ifndef __OPENSIM_VARIABLE_H__
#define __OPENSIM_VARIABLE_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


enum _var_type
{
  var_stock = 1,
  var_aux = 2,
  var_lookup = 3,
  var_const = 4,
  
  var_undef = -1
};

typedef enum _var_type var_type;


enum _token_type 
{
  tok_eof = -1,
  
  // commands
  tok_def = -2, tok_extern = -3,
  
  // primary
  tok_identifier = -4, tok_number = -5,
  
  // control
  tok_if = -6, tok_then = -7, tok_else = -8,
  tok_for = -9, tok_in = -10,
  
  // operators
  tok_operator = -11,
  
  
  // var definition
  tok_var = -13
};

typedef enum _token_type token_type;


typedef struct _equ_token equ_token;

struct _equ_token
{
  token_type  type;
  gchar      *identifier;
  double      num_val;
  char        op;
};


/*
 * Type macros.
 */
#define OPENSIM_TYPE_VARIABLE            (opensim_variable_get_type())
#define OPENSIM_VARIABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENSIM_TYPE_VARIABLE, OpensimVariable))
#define OPENSIM_VARIABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), OPENSIM_TYPE_VARIABLE, OpensimVariableClass))
#define OPENSIM_IS_VARIABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), OPENSIM_TYPE_VARIABLE))
#define OPENSIM_IS_VARIABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), OPENSIM_TYPE_VARIABLE))
#define OPENSIM_VARIABLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), OPENSIM_TYPE_VARIABLE, OpensimVariableClass))


typedef struct _OpensimVariable        OpensimVariable;
typedef struct _OpensimVariableClass   OpensimVariableClass;
typedef struct _OpensimVariablePrivate OpensimVariablePrivate;


struct _OpensimVariable
{
  GObject parent_instance;
   
  /* instance members */
  OpensimVariablePrivate *priv;
};

struct _OpensimVariableClass
{
  GObjectClass   parent_class;
  
  const GArray * (* get_tokens) (OpensimVariable *variable);
};

/* used by OPENSIM_TYPE_VARIABLE */
GType opensim_variable_get_type();

/*
 * Method definitions.
 */

/* public */
const GArray *opensim_variable_get_tokens(OpensimVariable *variable);


G_END_DECLS

#endif /* __OPENSIM_VARIABLE_H__ */
