//===--- mac_driver.c - OpenSim mac application driver -------------------===//
//
// Copyright 2008 Bobby Powers, portions copyright Free Software 
//   Foundation, Inc.
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
// This file contains the driver to start our python mac application
//
//===---------------------------------------------------------------------===//

#import <Python/Python.h>
#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
  NSBundle *mainBundle = [NSBundle mainBundle];
  NSString *resourcePath = [mainBundle resourcePath];
  NSArray *pythonPathArray = [NSArray arrayWithObjects: resourcePath, [resourcePath stringByAppendingPathComponent:@"PyObjC"], nil];
  NSArray *libPathArray = [NSArray arrayWithObjects: @"$LD_LIBRARY_PATH", resourcePath, [resourcePath stringByAppendingPathComponent:@"lib"], nil];

  setenv ("PYTHONPATH", [[pythonPathArray componentsJoinedByString:@":"] UTF8String], 1);
  setenv ("LD_LIBRARY_PATH", [[libPathArray componentsJoinedByString:@":"] UTF8String], 1);

  NSArray *possibleMainExtensions = [NSArray arrayWithObjects: @"py", @"pyc", @"pyo", nil];
  NSString *mainFilePath = nil;

  for (NSString *possibleMainExtension in possibleMainExtensions) 
  {
    mainFilePath = [mainBundle pathForResource: @"opensim-gtk" ofType: possibleMainExtension];
    if ( mainFilePath != nil ) break;
  }
    
  if ( !mainFilePath ) 
  {
    [NSException raise: NSInternalInconsistencyException format: @"%s:%d main() Failed to find the Main.{py,pyc,pyo} file in the application wrapper's Resources directory.", __FILE__, __LINE__];
  }
    
  Py_SetProgramName("/usr/bin/python");
  Py_Initialize();
  PySys_SetArgv(argc, (char **)argv);
    
  const char *mainFilePathPtr = [mainFilePath UTF8String];
  FILE *mainFile = fopen(mainFilePathPtr, "r");
  int result = PyRun_SimpleFile(mainFile, (char *)[[mainFilePath lastPathComponent] UTF8String]);
    
  if ( result != 0 )
    [NSException raise: NSInternalInconsistencyException
                 format: @"%s:%d main() PyRun_SimpleFile failed with file '%@'.  See console for errors.", __FILE__, __LINE__, mainFilePath];
    
  [pool drain];
    
  return result;
}
