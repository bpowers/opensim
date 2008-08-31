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

#ifndef __OPENSIM_IOXML_H__
#define __OPENSIM_IOXML_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


/*
 * Type macros.
 */
#define OPENSIM_TYPE_IOXML            (opensim_ioxml_get_type())
#define OPENSIM_IOXML(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENSIM_TYPE_IOXML, OpensimIOxml))
#define OPENSIM_IOXML_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), OPENSIM_TYPE_IOXML, OpensimIOxmlClass))
#define OPENSIM_IS_IOXML(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), OPENSIM_TYPE_IOXML))
#define OPENSIM_IS_IOXML_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), OPENSIM_TYPE_IOXML))
#define OPENSIM_IOXML_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), OPENSIM_TYPE_IOXML, OpensimIOxmlClass))


typedef struct _OpensimIOxml        OpensimIOxml;
typedef struct _OpensimIOxmlClass   OpensimIOxmlClass;
typedef struct _OpensimIOxmlPrivate OpensimIOxmlPrivate;


struct _OpensimIOxml
{
  GObject parent_instance;
   
  /* instance members */
  OpensimIOxmlPrivate *priv;
};

struct _OpensimIOxmlClass
{
  GObjectClass parent_class;
  
  int          (* load)          (OpensimIOxml *ioxml, gchar *path);
  int          (* save)          (OpensimIOxml *ioxml);
  GArray *     (* get_variables) (OpensimIOxml *ioxml);
};

/* used by OPENSIM_TYPE_IOXML */
GType opensim_ioxml_get_type();

/*
 * Method definitions.
 */

/* public */
int opensim_ioxml_load(OpensimIOxml *ioxml, gchar *model_path);
int opensim_ioxml_save(OpensimIOxml *ioxml);

GArray *opensim_ioxml_get_variables(OpensimIOxml *ioxml);


G_END_DECLS

#endif /* __OPENSIM_IOXML_H__ */
