//===--- model-ioxml.h - XML file IO -------------------------------------===//
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
// This inputs model data in XML form from a file and creates a vector
// of variables.
// TODO: implement saving through consuming the AST.
//
//===---------------------------------------------------------------------===//

#ifndef __MODEL_IOXML_H__
#define __MODEL_IOXML_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


/*
 * Type macros.
 */
#define MODEL_TYPE_IOXML            (model_ioxml_get_type())
#define MODEL_IOXML(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MODEL_TYPE_IOXML, ModelIOxml))
#define MODEL_IOXML_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MODEL_TYPE_IOXML, ModelIOxmlClass))
#define MODEL_IS_IOXML(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MODEL_TYPE_IOXML))
#define MODEL_IS_IOXML_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MODEL_TYPE_IOXML))
#define MODEL_IOXML_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MODEL_TYPE_IOXML, ModelIOxmlClass))


typedef struct _ModelIOxml        ModelIOxml;
typedef struct _ModelIOxmlClass   ModelIOxmlClass;
typedef struct _ModelIOxmlPrivate ModelIOxmlPrivate;


struct _ModelIOxml
{
  GObject parent_instance;
   
  /* instance members */
  ModelIOxmlPrivate *priv;
};

struct _ModelIOxmlClass
{
  GObjectClass parent_class;
  
  /* class members */
};

/* used by MODEL_TYPE_IOXML */
GType model_ioxml_get_type();

/*
 * Method definitions.
 */

/* public */
int model_ioxml_load(ModelIOxml *ioxml, gchar *model_path);
int model_ioxml_save(ModelIOxml *ioxml);
//int model_ioxml_save(ModelIOxml *ioxml, gboolean partial);

int model_ioxml_new_variable(ModelIOxml *ioxml, gchar *var_name, 
                                 gpointer var_pointer);
int model_ioxml_get_variable(ModelIOxml *ioxml, gchar *var_name, 
                                 gpointer var_pointer);
int model_ioxml_remove_variable(ModelIOxml *ioxml, 
                                    gchar *var_name);

int model_ioxml_run(ModelIOxml *ioxml);


G_END_DECLS

#endif /* __MODEL_IOXML_H__ */
