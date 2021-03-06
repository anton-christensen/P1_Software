#ifndef _VECTOR_
#define _VECTOR_

#include <math.h>

typedef struct _vector {
	double x,y,z;
} Vector;

typedef enum { X_AXIS, Y_AXIS, Z_AXIS } VectorAxis;

Vector vector_add(Vector v1, Vector v2);
Vector vector_subtract(Vector v1, Vector v2);
Vector vector_scale(Vector v, double s);
double vector_dot(Vector v1, Vector v2);
double vector_norm(Vector v);
double vector_distance(Vector v1, Vector v2);
Vector vector_normalize(Vector v);
double vector_angle_between(Vector v1, Vector v2);
Vector vector_cross(Vector v1, Vector v2);
Vector vector_rotate_around_z(Vector v, double angle);
Vector vector_rotate_around_x(Vector v, double angle);
Vector vector_rotate_around_xz(Vector v, double horizontal, double vertical);
double vector_get_component(Vector v, VectorAxis axis);
void   vector_set_component(Vector *v, VectorAxis axis, double value);


#endif