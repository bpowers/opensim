
#include <cstdio>
#include "run.h"

int
main (int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s model_file\n", argv[0]);
    return 0;
  }

  opensim::Runtime model;

  model.loadFile(argv[1]);

  model.simulate();

  return 0;
}
