#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "utils.h"


size_t strcount(char* str, char delim) {
	size_t len = strlen(str);
	size_t count = 0;
	for (size_t i = 0; i < len; i++) {
		if (str[i] == delim) { count++; }
	}

	return count;
}

char** string_to_vectorf(long double a, long double b, long double c, long double d) {
    char** result = malloc(2 * sizeof(char*));
    if (!result) return NULL;

    result[0] = malloc(200);
    result[1] = malloc(200);

    if (!result[0] || !result[1]) {
        free(result[0]);
        free(result[1]);
        free(result);
        return NULL;
    }

    sprintf(result[0], "(%.1Lf, %.1Lf)", a, b);
    sprintf(result[1], "(%.1Lf, %.1Lf)", c, d);

    return result;
}
