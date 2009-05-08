
#include <cstdio>
#include "run.h"

using namespace opensim;

int
main (int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s model_file\n", argv[0]);
    return 0;
  }

  Runtime model;

  model.parse(argv[1]);

  model.simulate();

  return 0;
}
