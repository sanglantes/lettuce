#include <stdlib.h>
#include "vector.h"

vector_t* gso(vector_t v1, vector_t v2, double long delta) {
  vector_t* nbasis = malloc(sizeof(vector_t) * 2);
  
  vector_t u2 = vector_vadd(v2, vector_smul(-1, vector_proj(v1, v2)));

  nbasis[0] = vector_smul(1/vector_norm(v1), v1); // Normalize
  nbasis[1] = (vector_t){nbasis[0].e2, -nbasis[0].e1}; // Rotation trick

  return nbasis;
}
