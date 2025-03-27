#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct {
	long double e1, e2;
} vector_t;

typedef struct {
	long double x, y;
} point_t;

vector_t vector_init(long double e1, long double e2);
vector_t vector_vadd(vector_t v1, vector_t v2);
bool ldependent(vector_t v1, vector_t v2);
long double vector_det(vector_t v1, vector_t v2);
long double vector_dot_product(vector_t v1, vector_t v2);
long double vector_norm(vector_t v1);
vector_t vector_min(vector_t v1, vector_t v2);
vector_t vector_max(vector_t v1, vector_t v2);
vector_t vector_smul(long double, vector_t v);
vector_t vector_proj(vector_t v1, vector_t v2);
vector_t* M_inverse(vector_t v1, vector_t v2);

#endif
