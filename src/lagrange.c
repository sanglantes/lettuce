#include <math.h>
#include <stdio.h>

#include "vector.h"
	
void lagrange_reduction(vector_t* v1, vector_t* v2) {
	vector_t minv = vector_min(*v1, *v2);	
	vector_t maxv = vector_max(*v1, *v2);

	while (vector_norm(minv) < vector_norm(maxv)) {
		vector_t qv = vector_smul(1/(vector_norm(minv) * vector_norm(minv)), minv);
		long double q = round(vector_dot_product(maxv, qv));

		vector_t r = vector_vadd(maxv, vector_smul(-q, minv));
		maxv = minv;
		minv = r;
	}

	*v1 = minv;
	*v2 = maxv;


}
