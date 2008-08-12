//===--- Simulator.cpp - Base class for interacting with models ----------===//
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
// This class represents models at a high level, suitible for inclusion
// in projects as part of a library.
// TODO: implement features for dynamically changing models.
//
//===---------------------------------------------------------------------===//

#include "model-simulator.h"

G_BEGIN_DECLS

#define MODEL_SIMULATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_SIMULATOR, ModelSimulatorPrivate))


struct _ModelSimulatorPrivate
{
  int hsize;
};


// C++ equivolent of static 
namespace 
{
  extern "C" void
  model_simulator_class_init(ModelSimulatorClass *kclass)
  {
    g_type_class_add_private(kclass, sizeof (ModelSimulatorPrivate));

    GObjectClass *gobject_class = G_OBJECT_CLASS(kclass);
  }



  extern "C" void
  model_simulator_init(ModelSimulator *self)
  {
    ModelSimulatorPrivate *priv;
    
    self->priv = priv = MODEL_SIMULATOR_GET_PRIVATE(self);
  }
}

G_END_DECLS

