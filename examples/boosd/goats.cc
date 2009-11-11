//===--- goats.cc - BOOSD class test driver ------------------------------===//
//
// Copyright 2008 Bobby Powers.
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
// This file contains the command line driver for the OpenSim library, 
// allowing you to open OpenSim XML models and translate them to standard
// output in one of several languages.
//
//===---------------------------------------------------------------------===//

// project specific defines
#include "opensim/runtime.h"
#include "opensim/types.h"
using namespace opensim;
using namespace opensim::types;


int main (int argc, char *argv[]) {

  startup::init();

  try {

    Namespace *root = new Namespace();
    Model *goats = new Model("goats");

    if (!root->add(goats))
      outs() << "add failed\n";

    Model *model = dyn_cast<Model>(root->get("goats"));
    if (model)
      outs() << "worked: model '" << model->getName() << "'!\n";
    else
      outs() << "didn't\n";

  } catch (const std::string& msg) {

    outs() << argv[0] << ": " << msg << "\n";
  }
  catch (...) {

    outs() << argv[0] << ": Unexpected unknown exception occurred.\n";
  }

  startup::exit();

  return 0;
}

