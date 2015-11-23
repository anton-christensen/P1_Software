#include "raytracer.h"

Image* raytracer_render(Scene* scene, Camera *camera) {
  int x, y;
  Image *image;
  Ray ray;

  image = new_image(camera->width, camera->height);
  printf("hej");
  for(x = 0; x < camera->width; x++) {
    for(y = 0; y < camera->height; y++) {
      // beregn ray
      ray = calculate_ray(x, y, camera);

      // trace ray
      image->pixels[x][y] = raytracer_trace(ray, scene);
    }
    printf("%.1f\n", ((double)x + 1) / camera->width * 100);
  }

  return image;
}

Ray calculate_ray(int x, int y, Camera *camera){
  Vector direction;
  direction = vector_scale(camera->forward, camera->distance);
  direction = vector_add(direction, vector_scale(camera->up, -y + camera->height / 2.0));
  direction = vector_add(direction, vector_scale(camera->right, x - camera->width / 2.0));
  return create_ray(camera->position, direction);
}

Pixel raytracer_trace(Ray ray, Scene *scene) {
  Intersection *intersection;
  Pixel pixel = {0.05, 0.05, 0.05};
  if( raytracer_scene_intersection(ray, scene, &intersection) ) {
    pixel = raytracer_phong(intersection, scene);
  }
  return pixel;
}

int raytracer_scene_intersection(Ray ray, Scene *scene, Intersection **intersection) {
  int i;
  double lowest_t = -1;
  Intersection* temporary_intersection = malloc(sizeof(Intersection));

  for(i = 0; i < scene->n_objects; i++) {
    if(raytracer_object_intersection(ray, scene->objects[i], &temporary_intersection)) {
      if(temporary_intersection->t < lowest_t || lowest_t == -1) {
        *intersection = temporary_intersection;
        lowest_t = temporary_intersection->t;
      }
    }
  }
  return lowest_t > 0;
}

int raytracer_object_intersection(Ray ray, Object *object, Intersection **intersection) {
  double lowest_t;
  Intersection *nearest_intersection, *temporary_intersection;
  int i;
  
  nearest_intersection = new_intersection();
  temporary_intersection = new_intersection();
  nearest_intersection->t = -1;

  for(i = 0; i < object->n_triangles; i++) {
    if(raytracer_triangle_intersection(ray, &(object->triangles[i]), &temporary_intersection)) {
      if(temporary_intersection->t < nearest_intersection->t || nearest_intersection->t == -1) {
        *nearest_intersection = *temporary_intersection;
        nearest_intersection->triangle = &(object->triangles[i]);
        nearest_intersection->color = object->color;
        nearest_intersection->material = object->material;
      }
    }
  }
  *intersection = nearest_intersection;
  return nearest_intersection->t > 0;
}

int raytracer_triangle_intersection(Ray ray, Triangle *triangle, Intersection **intersection) {
  double denominator, time;
  Vector v01, v02, v10, v12, v20, v21, triangle_normal;
  time = -1;

  v01 = vector_normalize(vector_subtract(triangle->verticies[1]->position, 
                                triangle->verticies[0]->position));
  v12 = vector_normalize(vector_subtract(triangle->verticies[2]->position, 
                                triangle->verticies[1]->position));
  v20 = vector_normalize(vector_subtract(triangle->verticies[0]->position, 
                                triangle->verticies[2]->position));
  triangle_normal = vector_normalize(vector_cross(v01, v12));
  
  denominator = vector_dot(ray.direction, triangle_normal);
  
  if(denominator == 0)
    return 0;

  time = vector_dot(vector_subtract(triangle->verticies[0]->position, 
                     ray.initial_point), triangle_normal) / denominator;

  // If triangle on front of camera: check if point is inside triangle
  if(time > 0) {
    Vector p = ray_get_point(ray, time);
    Vector v0p = vector_subtract(p, triangle->verticies[0]->position);
    Vector v1p = vector_subtract(p, triangle->verticies[1]->position);
    Vector v2p = vector_subtract(p, triangle->verticies[2]->position);
    if( vector_dot(triangle_normal, vector_cross(v01, v0p)) >= 0 && 
        vector_dot(triangle_normal, vector_cross(v12, v1p)) >= 0 &&
        vector_dot(triangle_normal, vector_cross(v20, v2p)) >= 0 ) {
      (*intersection)->t = time;
      (*intersection)->ray = ray;
      if(vector_dot(ray.direction, triangle_normal) > 0)
        triangle_normal = vector_scale(triangle_normal, -1);
      (*intersection)->normal = triangle_normal;
      return 1;
    }
  }
  
  return 0;
}

Pixel raytracer_phong(Intersection *intersection, Scene *scene) {
  int i, j;
  double m_a, m_l, m_s, m_sp, m_sm;
  Vector vN, vI, vR, vU, intersection_point;
  Pixel pA, pS, pC, pI, ambient, diffuse, specular;
  
  /* initialize */
  m_a = intersection->material.ambient_coefficient;
  m_l = intersection->material.diffuse_coefficient;
  m_s = intersection->material.specular_coefficient;
  m_sp = intersection->material.material_smoothness;
  m_sm = intersection->material.material_metalness;
  vN = intersection->normal;
  pC = intersection->color;
  pA = scene->ambient_intensity;
  diffuse = create_pixel(0.0, 0.0, 0.0);
  specular = create_pixel(0.0, 0.0, 0.0);
  
  /* ambient light = m_a * pC * pA */
  ambient = pixel_multiply(pixel_scale(pC, m_a), pA);
  
  intersection_point = ray_get_point(intersection->ray, intersection->t);
  vU = vector_scale(intersection->ray.direction, 1.0);
  pS = pixel_add(pixel_scale(pC, m_sm), pixel_scale(create_pixel(1.0,1.0,1.0),(1-m_sm)));

  for(i=0; i<scene->n_lights; i++){
    pI = scene->lights[i]->intensity;
    vI = vector_normalize(vector_subtract(scene->lights[i]->position, 
                          intersection_point));
    vR = vector_normalize(vector_add(vector_scale(vI, -1), 
                          vector_scale(vN, vector_dot(vI, vN) * 2)));
    
    /* diffuse light =  m_l * MAX(vI * vN, 0) * pC * pI*/
    diffuse = pixel_add(diffuse, pixel_multiply(pixel_scale(pC, 
                        m_l * MAX(vector_dot(vI, vN), 0)), pI));
    
    /* specular light = m_s * MAX(-vR * vU, 0) ^ m_sp * pI * pS */
    specular = pixel_add(specular, pixel_multiply(pS, pixel_scale(pI, 
                         m_s * pow(MAX(-vector_dot(vR, vU), 0), m_sp))));
  }   
  
  /* return ambient + diffuse + specular */
  return pixel_add(ambient, pixel_add(diffuse, specular));
}

Intersection *new_intersection(void){
  return (Intersection*)malloc(sizeof(Intersection));
}