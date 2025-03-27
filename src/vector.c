#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "vector.h"

vector_t vector_init(long double e1, long double e2) {
	vector_t rop_vector;

	rop_vector.e1 = e1;
	rop_vector.e2 = e2;

	return rop_vector;
}

vector_t vector_smul(long double s, vector_t v) {
	vector_t rop_vector;

	rop_vector.e1 = v.e1 * s;
	rop_vector.e2 = v.e2 * s;

	return rop_vector;
}

vector_t vector_vadd(vector_t v1, vector_t v2) {
	vector_t rop_vector;
	
	rop_vector.e1 = v1.e1 + v2.e1;
	rop_vector.e2 = v1.e2 + v2.e2;

	return rop_vector;
}

bool ldependent(vector_t v1, vector_t v2) {
	bool r = (v1.e1 / v2.e1) == (v1.e2 / v2.e2);
	return r;
}

long double vector_norm(vector_t v) {
	return sqrtl(v.e1 * v.e1 + v.e2 * v.e2);
}

long double vector_det(vector_t v1, vector_t v2) {
	return (v1.e1 * v2.e2) - (v2.e1 * v1.e2);
}

long double vector_dot_product(vector_t v1, vector_t v2) {
	return v1.e1 * v2.e1 + v1.e2 * v2.e2;
}

vector_t vector_min(vector_t v1, vector_t v2) {
	if (vector_norm(v1) <= vector_norm(v2)) {
		return v1;
	}
	return v2;
}

vector_t vector_max(vector_t v1, vector_t v2) {
	if (vector_norm(v1) >= vector_norm(v2)) {
		return v1;
	}
	return v2;
}

vector_t vector_proj(vector_t v1, vector_t v2) {
  long double ab = vector_dot_product(v1, v2);
  long double bb = vector_dot_product(v2, v2);
  return vector_smul((ab/bb), v2);
}

vector_t* M_inverse(vector_t v1, vector_t v2) {
    long double det = vector_det(v1, v2);
    if (fabsl(det) < 1e-18) {
        fprintf(stderr, "[ERROR] Matrix is not invertible.\n");
        return NULL;
    }

    vector_t* M_inv = malloc(sizeof(vector_t) * 2);
    long double s = 1.0L / det;

    M_inv[0].e1 = s *  v2.e2;
    M_inv[0].e2 = s * -v1.e2;
    M_inv[1].e1 = s * -v2.e1;
    M_inv[1].e2 = s *  v1.e1;

    return M_inv;
}
