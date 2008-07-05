//===--- globals.h - global definitions and includes  ----------*- C++ -*-===//
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
// This header includes a bunch of global definitions and includes, 
// including platform specific things like including windows.h
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_GLOBALS_H
#define OSIM_GLOBALS_H

// I don't think this is really necessary right now, we don't use any
// Windows functions or headers, but it might have its own ifdefs in there.
#ifdef _WIN32
#include <windows.h>
#define WIN_DLL __declspec(dllexport)
#else
#define WIN_DLL 
#endif 

#include <cstdio>

#include <string>
#include <map>
#include <vector>

/*
// llvm code generation
#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Module.h"
#include "llvm/ModuleProvider.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/LLVMBuilder.h"
#include "llvm/CodeGen/MachineCodeEmitter.h"
*/

#endif // OSIM_GLOBALS_H
