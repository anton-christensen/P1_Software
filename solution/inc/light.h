#ifndef _LIGHT_
#define _LIGHT_

#include "vector.h"
#include "pixel.h"

typedef struct _pointlight {
  Vector position;
  Pixel color;
  double intensity;
} PointLight;

#endif