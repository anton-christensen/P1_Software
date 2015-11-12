#ifndef _RAYTRACER_
#define _RAYTRACER_

#include "scene.h"
#include "camera.h"
#include "image.h"
#include "ray.h"

typedef struct _intersection {
  Vector surface_normal;
  Material material;
  Pixel color;
  Triangle *triangle;
  double t;
} Intersection;

Image* raytracer_render(Scene *scene, Camera *camera);
Pixel raytracer_trace(Ray ray, Scene *scene);
int raytracer_scene_intersection(Ray ray, Scene *scene, Intersection **intersection);

#endif