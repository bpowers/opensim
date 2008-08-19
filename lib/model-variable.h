//===--- model-variable.h - Base class for representing sim variables ----===//
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


#ifndef __MODEL_VARIABLE_H__
#define __MODEL_VARIABLE_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS

enum varType
{
  var_stock = 1,
  var_aux = 2,
  var_lookup = 3,
  var_const = 4,
  
  var_undef = -1
};


/*
 * Type macros.
 */
#define MODEL_TYPE_VARIABLE            (model_variable_get_type())
#define MODEL_VARIABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MODEL_TYPE_VARIABLE, ModelVariable))
#define MODEL_VARIABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MODEL_TYPE_VARIABLE, ModelVariableClass))
#define MODEL_IS_VARIABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MODEL_TYPE_VARIABLE))
#define MODEL_IS_VARIABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MODEL_TYPE_VARIABLE))
#define MODEL_VARIABLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MODEL_TYPE_VARIABLE, ModelVariableClass))


typedef struct _ModelVariable        ModelVariable;
typedef struct _ModelVariableClass   ModelVariableClass;
typedef struct _ModelVariablePrivate ModelVariablePrivate;


struct _ModelVariable
{
  GObject parent_instance;
   
  /* instance members */
  ModelVariablePrivate *priv;
};

struct _ModelVariableClass
{
  GObjectClass parent_class;
  
  /* class members */
};

/* used by MODEL_TYPE_VARIABLE */
GType model_variable_get_type();

/*
 * Method definitions.
 */

/* public */
//int model_variable_load(ModelVariable *variable, gchar *model_path);
//int model_variable_save(ModelVariable *variable);

//int model_variable_get_variables(ModelVariable *variable);


G_END_DECLS

#endif /* __MODEL_VARIABLE_H__ */
