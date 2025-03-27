#include <math.h>
#include "vector.h"
#include <stdlib.h>

/* vector_t babai_cvp(vector_t v1, vector_t v2, vector_t t) {
  vector_t* gso_basis = gso(v1, v2);
  
  long double l0 = vector_dot_product(t, gso_basis[1]) / vector_dot_product(gso_basis[1],
                                      gso_basis[1]);
  vector_t y0 = vector_smul(roundl(l0), v2);
  vector_t w1 = vector_vadd(vector_vadd(t, vector_smul(-(l0 - roundl(l0)),
                            gso_basis[1])), vector_smul(-roundl(l0), v2));

  long double l1 = vector_dot_product(w1, gso_basis[0]) / vector_dot_product(gso_basis[0], gso_basis[0]);
  vector_t y1 = vector_smul(roundl(l1), v1);
  vector_t w2 = vector_vadd(vector_vadd(w1, vector_smul(-(l1 - roundl(l1)),
                            gso_basis[0])), vector_smul(-roundl(l1), v1));

  return vector_vadd(y0, y1);
} */


  // Based on Theorem 6.34 (Babai's Closest Vertex Algorithm), 
  // An Introduction to Mathematical Cryptography by J. Hoffstein, J. Pipher, and J. Silverman
  // page 380
vector_t babai2_cvp(vector_t v1, vector_t v2, vector_t t) {
    vector_t* Minv = M_inverse(v1, v2);
    if (!Minv) {
        vector_t zero = {0.0L, 0.0L};
        return zero;
    }

    long double alpha1 = Minv[0].e1 * t.e1 + Minv[0].e2 * t.e2;
    long double alpha2 = Minv[1].e1 * t.e1 + Minv[1].e2 * t.e2;

    long double a1 = roundl(alpha1);
    long double a2 = roundl(alpha2);

    vector_t approx = vector_vadd(vector_smul(a1, v1),
                                  vector_smul(a2, v2));

    free(Minv);
    return approx;
}
