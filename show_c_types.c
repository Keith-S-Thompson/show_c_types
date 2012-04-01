/*
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This program uses a collection of very ugly macros to determine the
 * characteristics of predefined C types.  The results are printed to
 * stdout in JSON format.
 *
 * Author: Keith Thompson <Keith.S.Thompson@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <time.h>

/*
 * Some very old compilers (VAXC, for example) might not support some
 * of the following types:
 *     signed char
 *     long long
 *     unsigned long long
 *     long double
 * We assume that we can detect this by checking the appropriate macros
 * in <limits.h> and <float.h>.  We further assume that unsigned long long
 * exists if and only if long long exists.
 *
 * We also assume that the value of __STDC_VERSION__ determines whether
 * bool is supported.
 */

#ifdef SCHAR_MAX
#define SIGNED_CHAR_EXISTS
#endif
#ifdef LLONG_MAX
#define LONG_LONG_EXISTS
#endif
#ifdef LDBL_MAX
#define LONG_DOUBLE_EXISTS
#endif
#if __STDC_VERSION__ >= 199901L
#define BOOL_EXISTS
#include <stdbool.h>
#define INTMAX_T_EXISTS
#define UINTMAX_T_EXISTS
#include <stdint.h>
#endif

#ifndef BOOL_EXISTS
typedef enum { false, true } bool;
#endif

#define IS_SIGNED(type)  ((type)-1 < (type)0)

#define ALIGNOF(type) ((int)(offsetof(struct {char c; type t;}, t)))

#define MAX_SIZE 128

static bool integer_sizes[MAX_SIZE + 1] = { false };
static bool huge_integer = false;
static bool float_sizes[MAX_SIZE + 1] = { false };
static bool huge_float = false;

static float       f_one            =   1.0F;
static float       f_minus_sixteen  = -16.0F;
static float       f_one_million    = 1.0e6F;
static double      d_one            =   1.0;
static double      d_minus_sixteen  = -16.0;
static double      d_one_million    = 1.0e6;
#ifdef LONG_DOUBLE_EXISTS
static long double ld_one           =   1.0L;
static long double ld_minus_sixteen = -16.0L;
static long double ld_one_million   = 1.0e6L;
#endif

enum small_enum { se_zero, se_one, se_two };
enum small_signed_enum { sse_minus_one = -1, sse_zero, sse_one };

#define SHOW_INTEGER_TYPE(type, endianness)                           \
    do {                                                              \
        const int size = sizeof(type) * CHAR_BIT;                     \
        const int align = ALIGNOF(type) * CHAR_BIT;                   \
        puts("    {");                                                \
        printf("        \"name\" : \"%s\",\n", #type);                \
        printf("        \"size\" : %d,\n", size);                     \
        printf("        \"signedness\" : \"%s\",\n",                  \
               (IS_SIGNED(type) ? "signed" : "unsigned"));            \
        if (endianness != NULL) {                                     \
            printf("        \"endianness\" : \"%s\",\n", endianness); \
        }                                                             \
        printf("        \"alignment\" : %d\n", align);                \
        if (size <= MAX_SIZE) {                                       \
            integer_sizes[size] = true;                               \
        }                                                             \
        else {                                                        \
            huge_integer = true;                                      \
        }                                                             \
        puts("    },");                                               \
    } while(0)

#define SHOW_FLOATING_TYPE(type, mant_dig, min_exp, max_exp, one, minus_sixteen, one_million) \
    do {                                                         \
        const int size = sizeof(type) * CHAR_BIT;                \
        const int align = ALIGNOF(type) * CHAR_BIT;              \
        const char *looks_like = floating_looks_like             \
            ( hex_image(&one, sizeof one),                       \
              hex_image(&minus_sixteen, sizeof minus_sixteen),   \
              hex_image(&one_million, sizeof one_million) );     \
        puts("    {");                                           \
        printf("        \"name\" : \"%s\",\n", #type);           \
        printf("        \"size\" : %d,\n", size);                \
        printf("        \"alignment\" : %d,\n", align);          \
        printf("        \"mantissa_bits\" : %d,\n", mant_dig);   \
        printf("        \"min_exp\" : %d,\n", min_exp);          \
        printf("        \"max_exp\" : %d,\n", max_exp);          \
        printf("        \"looks_like\" : \"%s\"\n", looks_like); \
        if (size <= MAX_SIZE) {                                  \
            float_sizes[size] = true;                            \
        }                                                        \
        else {                                                   \
            huge_float = true;                                   \
        }                                                        \
        puts("    },");                                          \
    } while(0)

#define SHOW_RAW_TYPE_WITH_NAME(type, name)            \
    do {                                               \
        const int size = sizeof(type) * CHAR_BIT;      \
        const int align = ALIGNOF(type) * CHAR_BIT;    \
        puts("    {");                                 \
        printf("        \"name\" : \"%s\",\n", name);  \
        printf("        \"size\" : %d,\n", size);      \
        printf("        \"alignment\" : %d\n", align); \
        printf("    },\n");                            \
    } while(0)

#define SHOW_RAW_TYPE(type) SHOW_RAW_TYPE_WITH_NAME(type, #type)

#define declare_endianness_function(the_type, func_name) \
char *                                   \
func_name(void)                          \
{                                        \
    unsigned char arr[sizeof(the_type)]; \
                                         \
    if (sizeof(the_type) == 1) {         \
        return NULL;                     \
    }                                    \
                                         \
    memset(arr, 0, sizeof arr);          \
    arr[0] = 1;                          \
    if (*((the_type*)arr) == 1) {        \
        return "little-endian";          \
    }                                    \
                                         \
    memset(arr, 0, sizeof arr);          \
    arr[sizeof arr - 1] = 1;             \
    if (*((the_type*)arr) == 1) {        \
        return "big-endian";             \
    }                                    \
                                         \
    return NULL;                         \
}

#ifdef BOOL_EXISTS
declare_endianness_function(bool,               bool_endianness)
#endif

declare_endianness_function(enum small_enum,    small_enum_endianness)
declare_endianness_function(enum small_signed_enum, small_signed_enum_endianness)

declare_endianness_function(char,               char_endianness)
#ifdef SIGNED_CHAR_EXISTS
declare_endianness_function(signed char,        signed_char_endianness)
#endif
declare_endianness_function(unsigned char,      unsigned_char_endianness)

declare_endianness_function(short,              short_endianness)
declare_endianness_function(unsigned short,     unsigned_short_endianness)

declare_endianness_function(int,                int_endianness)
declare_endianness_function(unsigned,           unsigned_endianness)

declare_endianness_function(long,               long_endianness)
declare_endianness_function(unsigned long,      unsigned_long_endianness)

#ifdef LONG_LONG_EXISTS
declare_endianness_function(long long,          long_long_endianness)
declare_endianness_function(unsigned long long, unsigned_long_long_endianness)
#endif

#ifdef INTMAX_T_EXISTS
declare_endianness_function(intmax_t,           intmax_t_endianness)
declare_endianness_function(uintmax_t,          uintmax_t_endianness)
#endif

declare_endianness_function(ptrdiff_t,          ptrdiff_t_endianness)
declare_endianness_function(size_t,             size_t_endianness)
declare_endianness_function(wchar_t,            wchar_t_endianness)
declare_endianness_function(time_t,             time_t_endianness)
declare_endianness_function(clock_t,            clock_t_endianness)

typedef void(*simple_func_ptr)(void);
typedef double(*complex_func_ptr)(int*,char**);

static char *system_configuration(void) {
    FILE *f;
    static char result[256];

    remove("config.txt");
    system("sh ./config.guess > config.txt");

    f = fopen("config.txt", "r");
    if (f == NULL) {
        return "unknown";
    }
    if (fgets(result, sizeof result, f) == NULL) {
        fclose(f);
        return "unknown";
    }
    fclose(f);
    {
        char *const newline = strchr(result, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
    }
    return result;
} /* system_configuration */

static void check_size(char *kind, bool sizes[], int size)
{
    if (! sizes[size]) {
        puts("    {");
        printf("        \"comment\" : \"There is no %d-bit %s type\"\n", size, kind);
        puts("    },");
    }
} /* check_size */

static char *hex_image(void *base, size_t size)
{
    /*
     * Note: This function returns a pointer to a malloc()ed buffer.
     * It leaks memory if the caller doesn't free() the buffer.  For this
     * small program, the memory leak should be inconsequential.
     */
    static char *hex = "0123456789abcdef";
    size_t i;
    unsigned char *data = base;
    char *result = malloc(2 * size + 1);
    int out_index = 0;

    if (result == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    for (i = 0; i < size; i ++) {
        result[out_index++] = hex[data[i] / 16];
        result[out_index++] = hex[data[i] % 16];
    }

    result[out_index] = '\0';

    return result;
} /* hex_image */

static int bits_of(int x)
{
    int result = 0;

    while (x) {
        x >>= 1;
        result ++;
    }
    return result;
} /* bits_of */

static char *floating_looks_like(char *one, char *minus_sixteen, char *one_million)
{
#if 0
    printf("          1.0 = %s\n", one);
    printf("        -16.0 = %s\n", minus_sixteen);
    printf("        1.0e6 = %s\n", one_million);
#endif

    if (strcmp(one,           "3f800000") == 0 &&
        strcmp(minus_sixteen, "c1800000") == 0 &&
        strcmp(one_million,   "49742400") == 0)
    {
        return "32-bit IEEE, big-endian";
    }

    else if (strcmp(one,           "0000803f") == 0 &&
             strcmp(minus_sixteen, "000080c1") == 0 &&
             strcmp(one_million,   "00247449") == 0)
    {
        return "32-bit IEEE, little-endian";
    }

    else if (strcmp(one,      "3ff0000000000000") == 0 &&
        strcmp(minus_sixteen, "c030000000000000") == 0 &&
        strcmp(one_million,   "412e848000000000") == 0)
    {
        return "64-bit IEEE, big-endian";
    }

    else if (strcmp(one,      "000000000000f03f") == 0 &&
        strcmp(minus_sixteen, "00000000000030c0") == 0 &&
        strcmp(one_million,   "0000000080842e41") == 0)
    {
        return "64-bit IEEE, little-endian";
    }

    else if (strcmp(one,      "3fff0000000000000000000000000000") == 0 &&
        strcmp(minus_sixteen, "c0030000000000000000000000000000") == 0 &&
        strcmp(one_million,   "4012e848000000000000000000000000") == 0)
    {
        return "SPARC/HPPA-style 128-bit IEEE, big-endian";
    }

    else if (strcmp(one,      "3ff00000000000000000000000000000") == 0 &&
        strcmp(minus_sixteen, "c0300000000000008000000000000000") == 0 &&
        strcmp(one_million,   "412e8480000000000000000000000000") == 0)
    {
        return "SGI-style 128-bit IEEE, big-endian";
    }

    else if (strcmp(one,      "0000000000000080ff3f0000") == 0 &&
        strcmp(minus_sixteen, "000000000000008003c00000") == 0 &&
        strcmp(one_million,   "00000000000024f412400000") == 0)
    {
        return "96-bit IEEE, little-endian";
    }

    else if (strcmp(one,      "4001800000000000") == 0 &&
        strcmp(minus_sixteen, "c005800000000000") == 0 &&
        strcmp(one_million,   "4014f42400000000") == 0)
    {
        return "64-bit Cray";
    }

    else if (strcmp(one,      "40018000000000000000000000000000") == 0 &&
        strcmp(minus_sixteen, "c0058000000000000000000000000000") == 0 &&
        strcmp(one_million,   "4014f424000000000000000000000000") == 0)
    {
        return "128-bit Cray";
    }

    else {
        return "Unrecognized floating-point format";
    }
} /* floating_looks_like */

int main(void)
{
    puts("[");
    puts("    {");
    printf("    \"config\" : \"%s\"\n", system_configuration());
    puts("    },");

#ifdef BOOL_EXISTS
    SHOW_INTEGER_TYPE(bool, bool_endianness());
#endif

    SHOW_INTEGER_TYPE(enum small_enum, small_enum_endianness());
    SHOW_INTEGER_TYPE(enum small_signed_enum, small_signed_enum_endianness());

    SHOW_INTEGER_TYPE(char, char_endianness());
#ifdef SIGNED_CHAR_EXISTS
    SHOW_INTEGER_TYPE(signed char, signed_char_endianness());
#endif
    SHOW_INTEGER_TYPE(unsigned char, unsigned_char_endianness());

    SHOW_INTEGER_TYPE(short, short_endianness());
    SHOW_INTEGER_TYPE(unsigned short, unsigned_short_endianness());

    SHOW_INTEGER_TYPE(int, int_endianness());
    SHOW_INTEGER_TYPE(unsigned, unsigned_endianness());

    SHOW_INTEGER_TYPE(long, long_endianness());
    SHOW_INTEGER_TYPE(unsigned long, unsigned_long_endianness());

#ifdef LONG_LONG_EXISTS
    SHOW_INTEGER_TYPE(long long, long_long_endianness());
    SHOW_INTEGER_TYPE(unsigned long long, unsigned_long_long_endianness());
#endif

#ifdef INTMAX_T_EXISTS
    SHOW_INTEGER_TYPE(intmax_t, intmax_t_endianness());
    SHOW_INTEGER_TYPE(uintmax_t, uintmax_t_endianness());
#endif

    SHOW_FLOATING_TYPE(float,       FLT_MANT_DIG,  FLT_MIN_EXP,  FLT_MAX_EXP,
                                    f_one, f_minus_sixteen, f_one_million);
    SHOW_FLOATING_TYPE(double,      DBL_MANT_DIG,  DBL_MIN_EXP,  DBL_MAX_EXP,
                                    d_one, d_minus_sixteen, d_one_million);
#ifdef LONG_DOUBLE_EXISTS
    SHOW_FLOATING_TYPE(long double, LDBL_MANT_DIG, LDBL_MIN_EXP, LDBL_MAX_EXP,
                                    ld_one, ld_minus_sixteen, ld_one_million);
#endif

    SHOW_INTEGER_TYPE(ptrdiff_t, ptrdiff_t_endianness());
    SHOW_INTEGER_TYPE(size_t, size_t_endianness());
    SHOW_INTEGER_TYPE(wchar_t, wchar_t_endianness());

    SHOW_INTEGER_TYPE(time_t, time_t_endianness());
    SHOW_INTEGER_TYPE(clock_t, clock_t_endianness());
    SHOW_RAW_TYPE(struct tm);

    SHOW_RAW_TYPE(void*);
    SHOW_RAW_TYPE(char*);
    SHOW_RAW_TYPE(int*);
    SHOW_RAW_TYPE(double*);
    SHOW_RAW_TYPE(struct{int x;}*);
    SHOW_RAW_TYPE_WITH_NAME(simple_func_ptr, "void(*)(void)");
    SHOW_RAW_TYPE_WITH_NAME(complex_func_ptr, "double(*)(int*,char**)");

    check_size("integer", integer_sizes, 8);
    check_size("integer", integer_sizes, 16);
    check_size("integer", integer_sizes, 32);
    check_size("integer", integer_sizes, 64);

    if (huge_integer) {
        puts("    {");
        printf("        \"comment\" : \"There is at least one integer type bigger than %d bits\"\n",
               MAX_SIZE);
        puts("    },");
    }

    check_size("floating-point", float_sizes, 32);
    check_size("floating-point", float_sizes, 64);
    check_size("floating-point", float_sizes, 128);

    if (huge_float) {
        puts("    {");
        printf("       \"comment\" : \"There is at least one float type bigger than %d bits\"\n",
               MAX_SIZE);
        puts("    },");
    }

    puts("    {");
    puts("        \"comment\" : \"done\"\n");
    puts("    }");

    puts("]");

    exit(EXIT_SUCCESS);
} /* main */
