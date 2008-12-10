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

#ifndef __OPENSIM_GENERATOR_H__
#define __OPENSIM_GENERATOR_H__

#include <glib.h>
#include <glib-object.h>
#include <glib/gprintf.h>

// openSim stuff
#include "../globals.h"
#include "../opensim-simulator.h"
#include "../opensim-variable.h"

G_BEGIN_DECLS

/*
 * Type macros.
 */
#define OPENSIM_TYPE_GENERATOR            (opensim_simulator_get_type())
#define OPENSIM_GENERATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), \
                                           OPENSIM_TYPE_GENERATOR, \
                                           OpensimGenerator))
#define OPENSIM_GENERATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), \
                                           OPENSIM_TYPE_GENERATOR, \
                                           OpensimGeneratorClass))
#define OPENSIM_IS_GENERATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
                                           OPENSIM_TYPE_GENERATOR))
#define OPENSIM_IS_GENERATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), \
                                           OPENSIM_TYPE_GENERATOR))
#define OPENSIM_GENERATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), \
                                           OPENSIM_TYPE_GENERATOR, \
                                           OpensimGeneratorClass))


typedef struct _OpensimGenerator        OpensimGenerator;
typedef struct _OpensimGeneratorClass   OpensimGeneratorClass;
typedef struct _OpensimGeneratorPrivate OpensimGeneratorPrivate;


struct _OpensimGenerator
{
  GObject parent_instance;
  
  /*
   * Properties:
   * valid_model - get
   * errors      - get
   *
   */
  
  /* instance members */
  OpensimGeneratorPrivate *priv;
};

struct _OpensimGeneratorClass
{
  GObjectClass parent_class;
  
  int               (* rebase) (OpensimGenerator *generator,
                                GHashTable       *variables);
  int               (* update) (OpensimGenerator *generator);
  int               (* parse)  (OpensimGenerator *generator,
                                int               our_walk,
                                FILE             *output_file);
};


/* used by OPENSIM_TYPE_SIMULATOR */
GType generator ();

int
opensim_generator_rebase (OpensimGenerator *generator, 
                          GHashTable *variables);

int 
opensim_generator_update (OpensimGenerator *generator);

int 
opensim_generator_parse  (OpensimGenerator *generator, 
                          int our_walk,
                          FILE *output_file);


G_END_DECLS

#endif // __OPENSIM_GENERATOR_H__
