#ifndef _KDNODE_
#define _KDNODE_

#include <stdlib.h>

#include "object.h"
#include "vector.h"

typedef struct _KDNode {
  struct _KDNode *low, *high;
  AABB box;
  Triangle **triangles;
  int n_triangles;
} KDNode;

int kdnode_build(KDNode *root, Object **objects, int n_objects);
int kdnode_is_leaf(KDNode *node);

#endif