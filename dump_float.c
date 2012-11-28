#include <stdio.h>
#include <stdlib.h>

static char *hex_image(const void *base, size_t size) {
    /*
     * Note: This function returns a pointer to a malloc()ed buffer.
     * The caller should free() the buffer.
     */
    static char *hex = "0123456789abcdef";
    size_t i;
    const unsigned char *data = base;
    char *result = malloc(2 * size + 1);
    int out_index = 0;

    if (result == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < size; i ++) {
        result[out_index++] = hex[data[i] / 16];
        result[out_index++] = hex[data[i] % 16];
    }

    result[out_index] = '\0';

    return result;
} /* hex_image */

#define SHOW(type, name, value) printf("%-12s %7s  %s\n", type, name, hex_image(&value, sizeof value))

int main(void) {
    const static float       f_one            =   1.0F;
    const static float       f_fraction       = 1.09375F;
    const static float       f_minus_sixteen  = -16.0F;
    const static float       f_one_million    = 1.0e6F;

    const static double      d_one            =   1.0;
    const static double      d_fraction       = 1.09375;
    const static double      d_minus_sixteen  = -16.0;
    const static double      d_one_million    = 1.0e6;

#ifndef DISABLE_LONG_DOUBLE
    const static long double ld_one           =   1.0L;
    const static long double ld_fraction      = 1.09375L;
    const static long double ld_minus_sixteen = -16.0L;
    const static long double ld_one_million   = 1.0e6L;
#endif

    SHOW("float", "1.0", f_one);
    SHOW("float", "1.09375", f_fraction);
    SHOW("float", "-16.0", f_minus_sixteen);
    SHOW("float", "1.0e6", f_one_million);

    SHOW("double", "1.0", d_one);
    SHOW("double", "1.09375", d_fraction);
    SHOW("double", "-16.0", d_minus_sixteen);
    SHOW("double", "1.0e6", d_one_million);

#ifndef DISABLE_LONG_DOUBLE
    SHOW("long double", "1.0", ld_one);
    SHOW("long double", "1.09375", ld_fraction);
    SHOW("long double", "-16.0", ld_minus_sixteen);
    SHOW("long double", "1.0e6", ld_one_million);
#endif
    
    return 0;
}
