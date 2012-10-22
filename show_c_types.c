/*
 * Copyright (C) 2012 Keith Thompson
 */

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

#ifdef __cplusplus
#error "Please use a C compiler not a C++ compiler"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <time.h>

#define SHOW_C_TYPES_VERSION "2012-10-22"
/*
 * The version is the date in YYYY-MM-DD format.
 * If you modify this program, please update this definition.
 * If you're not the original author, please append something to
 * the version string such as "2012-10-22-yourname".
 */

/*
 * We try to use predefined macro values to determine characteristics
 * of the system.  In some cases, these tests can fail.  For example,
 * some compilers might support long long even though they don't define
 * __STDC_VERSION__ to the proper value; other very old compilers might
 * not support signed char or long double.
 *
 * See the header comment in Makefile for details.
 */

/*
 * TODO: Check for representation of negative integers
 *       (2's-complement, 1s'-complement, sign-and-magnitude).
 * TODO: Add a configure step to determine more implementation
 *       characteristics automatically.
 */

#if defined(DISABLE_SIGNED_CHAR)
#    undef SIGNED_CHAR_EXISTS
#elif defined(ENABLE_SIGNED_CHAR)
#    define SIGNED_CHAR_EXISTS
#elif defined(SCHAR_MAX)
#     define SIGNED_CHAR_EXISTS
#else
#     undef SIGNED_CHAR_EXISTS
#endif

#if defined(DISABLE_LONG_LONG)
#    undef LONG_LONG_EXISTS
#elif defined(ENABLE_LONG_LONG)
#    define LONG_LONG_EXISTS
#elif defined(LLONG_MAX)
#    define LONG_LONG_EXISTS
#else
#    undef LONG_LONG_EXISTS
#endif

#if defined(DISABLE_LONG_DOUBLE)
#    undef LONG_DOUBLE_EXISTS
#elif defined(ENABLE_LONG_DOUBLE)
#    define LONG_DOUBLE_EXISTS
#elif defined(LDBL_MAX)
#    define LONG_DOUBLE_EXISTS
#else
#    undef LONG_DOUBLE_EXISTS
#endif

#if defined(DISABLE_STDBOOL_H)
#    undef STDBOOL_H_EXISTS
#elif defined(ENABLE_STDBOOL_H)
#    define STDBOOL_H_EXISTS
#elif __STDC_VERSION__ >= 199901L
#    define STDBOOL_H_EXISTS
#else
#    undef STDBOOL_H_EXISTS
#endif

#if defined(DISABLE_STDINT_H)
#    undef STDINT_H_EXISTS
#elif defined(ENABLE_STDINT_H)
#    define STDINT_H_EXISTS
#elif __STDC_VERSION__ >= 199901L
#    define STDINT_H_EXISTS
#else
#    undef STDINT_H_EXISTS
#endif

#ifdef STDINT_H_EXISTS
#include <stdint.h>
#endif

#if defined(STDINT_H_EXISTS)
typedef intmax_t longest_signed;
typedef uintmax_t longest_unsigned;
#elif defined(LONG_LONG_EXISTS)
typedef long long longest_signed;
typedef unsigned long long longest_unsigned;
#else
typedef long longest_signed;
typedef unsigned long longest_unsigned;
#endif

#ifdef STDBOOL_H_EXISTS
#include <stdbool.h>
#else
typedef enum { false, true } bool; /* for internal use only; we don't show
                                      the characteristics of this enum type */
#endif

#define IS_SIGNED(type) ((type)-1 < (type)0)

#define ALIGNOF(type) ((int)(offsetof(struct {char c; type t;}, t)))

#define MAX_SIZE 128

static char *longest_unsigned_format;
static char *longest_signed_format;

static bool integer_sizes[MAX_SIZE + 1] = { false };
static bool huge_integer = false;
static bool float_sizes[MAX_SIZE + 1] = { false };
static bool huge_float = false;

const static float       f_one            =   1.0F;
const static float       f_minus_sixteen  = -16.0F;
const static float       f_one_million    = 1.0e6F;
const static double      d_one            =   1.0;
const static double      d_minus_sixteen  = -16.0;
const static double      d_one_million    = 1.0e6;
#ifdef LONG_DOUBLE_EXISTS
const static long double ld_one           =   1.0L;
const static long double ld_minus_sixteen = -16.0L;
const static long double ld_one_million   = 1.0e6L;
#endif

enum small_enum { se_zero, se_one, se_two };
enum small_signed_enum { sse_minus_one = -1, sse_zero, sse_one };

#define SHOW_INTEGER_TYPE(type, endianness, min, max)                 \
    do {                                                              \
        const int size = sizeof(type) * CHAR_BIT;                     \
        const int align = ALIGNOF(type) * CHAR_BIT;                   \
        puts("    {");                                                \
        printf("        \"type\" : \"%s\",\n", #type);                \
        printf("        \"size\" : %d,\n", size);                     \
        if (min != 0) {                                               \
            if (IS_SIGNED(type)) {                                    \
                printf("        \"min\" : %s,\n",                     \
                       signed_image(min));                            \
            }                                                         \
            else {                                                    \
                printf("        \"min\" : %s,\n",                     \
                       unsigned_image(min));                          \
            }                                                         \
        }                                                             \
        if (max != 0) {                                               \
            if (IS_SIGNED(type)) {                                    \
                printf("        \"max\" : %s,\n",                     \
                       signed_image(max));                            \
            }                                                         \
            else {                                                    \
                printf("        \"max\" : %s,\n",                     \
                       unsigned_image(max));                          \
            }                                                         \
        }                                                             \
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
    do {                                                             \
        const int size = sizeof(type) * CHAR_BIT;                    \
        const int align = ALIGNOF(type) * CHAR_BIT;                  \
        char *const hex_one = hex_image(&one, sizeof one);           \
        char *const hex_minus_sixteen                                \
            = hex_image(&minus_sixteen, sizeof minus_sixteen);       \
        char *const hex_one_million                                  \
            = hex_image(&one_million, sizeof one_million);           \
        const char *looks_like = floating_looks_like                 \
            ( hex_one, hex_minus_sixteen, hex_one_million );         \
        free(hex_one);                                               \
        free(hex_minus_sixteen);                                     \
        free(hex_one_million);                                       \
        puts("    {");                                               \
        printf("        \"type\" : \"%s\",\n", #type);               \
        printf("        \"size\" : %d,\n", size);                    \
        printf("        \"alignment\" : %d,\n", align);              \
        if (mant_dig != 0) {                                         \
            printf("        \"mantissa_bits\" : %d,\n", mant_dig);   \
        }                                                            \
        if (min_exp != 0) {                                          \
            printf("        \"min_exp\" : %d,\n", min_exp);          \
        }                                                            \
        if (max_exp != 0) {                                          \
            printf("        \"max_exp\" : %d,\n", max_exp);          \
        }                                                            \
        if (one != 0.0) {                                            \
            printf("        \"looks_like\" : \"%s\"\n", looks_like); \
        }                                                            \
        if (size <= MAX_SIZE) {                                      \
            float_sizes[size] = true;                                \
        }                                                            \
        else {                                                       \
            huge_float = true;                                       \
        }                                                            \
        puts("    },");                                              \
    } while(0)

#define SHOW_RAW_TYPE_WITH_NAME(type, name)            \
    do {                                               \
        const int size = sizeof(type) * CHAR_BIT;      \
        const int align = ALIGNOF(type) * CHAR_BIT;    \
        puts("    {");                                 \
        printf("        \"type\" : \"%s\",\n", name);  \
        printf("        \"size\" : %d,\n", size);      \
        printf("        \"alignment\" : %d\n", align); \
        printf("    },\n");                            \
    } while(0)

#define SHOW_RAW_TYPE(type) SHOW_RAW_TYPE_WITH_NAME(type, #type)

#define define_endianness_function(the_type, func_name) \
static char *func_name(void) {           \
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

#ifdef STDBOOL_H_EXISTS
define_endianness_function(bool,               bool_endianness)
#endif

define_endianness_function(enum small_enum,    small_enum_endianness)
define_endianness_function(enum small_signed_enum, small_signed_enum_endianness)

define_endianness_function(char,               char_endianness)
#ifdef SIGNED_CHAR_EXISTS
define_endianness_function(signed char,        signed_char_endianness)
#endif
define_endianness_function(unsigned char,      unsigned_char_endianness)

define_endianness_function(short,              short_endianness)
define_endianness_function(unsigned short,     unsigned_short_endianness)

define_endianness_function(int,                int_endianness)
define_endianness_function(unsigned,           unsigned_endianness)

define_endianness_function(long,               long_endianness)
define_endianness_function(unsigned long,      unsigned_long_endianness)

#ifdef LONG_LONG_EXISTS
define_endianness_function(long long,          long_long_endianness)
define_endianness_function(unsigned long long, unsigned_long_long_endianness)
#endif

#ifdef STDINT_H_EXISTS
define_endianness_function(intmax_t,           intmax_t_endianness)
define_endianness_function(uintmax_t,          uintmax_t_endianness)
#endif

define_endianness_function(ptrdiff_t,          ptrdiff_t_endianness)
define_endianness_function(size_t,             size_t_endianness)
define_endianness_function(wchar_t,            wchar_t_endianness)
#ifndef FLOATING_TIME_T
define_endianness_function(time_t,             time_t_endianness)
#endif
#ifndef FLOATING_CLOCK_T
define_endianness_function(clock_t,            clock_t_endianness)
#endif

typedef void(*simple_func_ptr)(void);
typedef double(*complex_func_ptr)(int*,char**);

static void set_formats(void) {
#if defined(STDINT_H_EXISTS)
    char test[10];

    longest_signed_format = "%jd";
    longest_unsigned_format = "%ju";

    /*
     * Some implementations support intmax_t but not the corresponding
     * "%jd" format.  Test whether it actually works; if not, fall back
     * to something else.  Likewise for uintmax_t and "%ju".
     *
     * We assume that if long long exists, then printf supports "%lld"
     * and "%llu".  This assumption may not always be valid.
     */
    sprintf(test, longest_signed_format, (longest_signed)42);
    if (strcmp(test, "42") != 0) {
#if defined(LONG_LONG_EXISTS)
        longest_signed_format = "%lld";
#else
        longest_signed_format = "%ld";
#endif
    }

    sprintf(test, longest_unsigned_format, (longest_unsigned)42);
    if (strcmp(test, "42") != 0) {
#if defined(LONG_LONG_EXISTS)
        longest_unsigned_format = "%llu";
#else
        longest_unsigned_format = "%lu";
#endif
    }

#elif defined(LONG_LONG_EXISTS)
    longest_signed_format = "%lld";
    longest_unsigned_format = "%llu";
#else
    longest_signed_format = "%ld";
    longest_unsigned_format = "%lu";
#endif
}

#define PRINT_SIGNED(n) printf(longest_signed_format, (longest_signed)n)
#define PRINT_UnSIGNED(n) printf(longest_unsigned_format, (longest_unsigned)n)

static char *signed_image(longest_signed n) {
    static char result[CHAR_BIT * sizeof (longest_signed)];
    sprintf(result, longest_signed_format, n);
    return result;
}

static char *unsigned_image(longest_unsigned n) {
    static char result[CHAR_BIT * sizeof (longest_unsigned)];
    sprintf(result, longest_unsigned_format, n);
    return result;
}

static void check_size(char *kind, bool sizes[], int size) {
    if (! sizes[size]) {
        puts("    {");
        printf("        \"comment\" : \"There is no %d-bit %s type\"\n", size, kind);
        puts("    },");
    }
} /* check_size */

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

static char *floating_looks_like(const char *one, const char *minus_sixteen, const char *one_million) {
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

#ifdef LLONG_MIN
#define MY_LLONG_MIN LLONG_MIN
#else
#define MY_LLONG_MIN 0
#endif
#ifdef LLONG_MAX
#define MY_LLONG_MAX LLONG_MAX
#else
#define MY_LLONG_MAX 0
#endif
#ifdef ULLONG_MAX
#define MY_ULLONG_MAX ULLONG_MAX
#else
#define MY_ULLONG_MAX 0
#endif

#ifdef PTRDIFF_MIN
#define MY_PTRDIFF_MIN PTRDIFF_MIN
#else
#define MY_PTRDIFF_MIN 0
#endif
#ifdef PTRDIFF_MAX
#define MY_PTRDIFF_MAX PTRDIFF_MAX
#else
#define MY_PTRDIFF_MAX 0
#endif

#ifdef SIZE_MAX
#define MY_SIZE_MAX SIZE_MAX
#else
#define MY_SIZE_MAX 0
#endif

#ifdef WCHAR_MIN
#define MY_WCHAR_MIN WCHAR_MIN
#else
#define MY_WCHAR_MIN 0
#endif
#ifdef WCHAR_MAX
#define MY_WCHAR_MAX WCHAR_MAX
#else
#define MY_WCHAR_MAX 0
#endif

/*
 * Similar to the non-standard strdup() function
 */
char *dupstr(char *s) {
    char *result = malloc(strlen(s) + 1);
    if (result == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, s);
    return result;
}

static void show_configuration(int argc, char **argv) {
    puts("    {");
    puts("        \"info\" : \"configuration\",");
    printf("        \"SHOW_C_TYPES_VERSION\" : \"%s\"%s\n", SHOW_C_TYPES_VERSION, argc > 1 ? "," : "");
    if (argc > 1) {
        int i;
        for (i = 1; i < argc; i ++) {
            char *ptr_equals = strchr(argv[i], '=');
            if (ptr_equals == NULL) {
                printf("        \"arg-%d\" : \"%s\"", i, argv[i]);
            }
            else {
                char *const arg = dupstr(argv[i]);
                char *key;
                char *value;
                ptr_equals = arg + (ptr_equals - argv[i]);
                *ptr_equals = '\0';
                key = arg;
                value = ptr_equals + 1;
                printf("        \"%s\" : \"%s\"", key, value);
                free(arg);
            }
            if (i == argc-1) {
                putchar('\n');
            }
            else {
                puts(",");
            }
        }
    }
    puts("    },");
}

static void show_predefined_macros(void) {
    puts("    {");
    puts("        \"info\" : \"predefined macros\",");

#ifdef __STDC__
    printf("        \"__STDC__\" : %d,\n", __STDC__);
#else
    puts("        \"__STDC__\" : null,");
#endif

#ifdef __STDC_HOSTED__
    printf("        \"__STDC_HOSTED__\" : %d,\n", __STDC_HOSTED__);
#else
    puts("        \"__STDC_HOSTED__\" : null,");
#endif

#ifdef __STDC_VERSION__
    printf("        \"__STDC_VERSION__\" : %ldL,\n", __STDC_VERSION__);
#else
    puts("        \"__STDC_VERSION__\" : null,");
#endif

#ifdef __STDC_ISO_10646__
    printf("        \"__STDC_ISO_10646__\" : %ldL,\n", __STDC_ISO_10646__);
#else
    puts("        \"__STDC_ISO_10646__\" : null,");
#endif

#ifdef __STDC_MB_MIGHT_NEQ_WC__
    printf("        \"__STDC_MB_MIGHT_NEQ_WC__\" : %d,\n", __STDC_MB_MIGHT_NEQ_WC__);
#else
    puts("        \"__STDC_MB_MIGHT_NEQ_WC__\" : null,");
#endif

#ifdef __STDC_UTF_16__
    printf("        \"__STDC_UTF_16__\" : %d,\n", __STDC_UTF_16__);
#else
    puts("        \"__STDC_UTF_16__\" : null,");
#endif

#ifdef __STDC_UTF_32__
    printf("        \"__STDC_UTF_32__\" : %d,\n", __STDC_UTF_32__);
#else
    puts("        \"__STDC_UTF_32__\" : null,");
#endif

#ifdef __STDC_ANALYZABLE__
    printf("        \"__STDC_ANALYZABLE__\" : %d,\n", __STDC_ANALYZABLE__);
#else
    puts("        \"__STDC_ANALYZABLE__\" : null,");
#endif

#ifdef __STDC_IEC_559__
    printf("        \"__STDC_IEC_559__\" : %d,\n", __STDC_IEC_559__);
#else
    puts("        \"__STDC_IEC_559__\" : null,");
#endif

#ifdef __STDC_IEC_559_COMPLEX__
    printf("        \"__STDC_IEC_559_COMPLEX__\" : %d,\n", __STDC_IEC_559_COMPLEX__);
#else
    puts("        \"__STDC_IEC_559_COMPLEX__\" : null,");
#endif

#ifdef __STDC_LIB_EXT1__
    printf("        \"__STDC_LIB_EXT1__\" : %ldL,\n", __STDC_LIB_EXT1__);
#else
    puts("        \"__STDC_LIB_EXT1__\" : null,");
#endif

#ifdef __STDC_NO_ATOMICS__
    printf("        \"__STDC_NO_ATOMICS__\" : %d,\n", __STDC_NO_ATOMICS__);
#else
    puts("        \"__STDC_NO_ATOMICS__\" : null,");
#endif

#ifdef __STDC_NO_COMPLEX
    printf("        \"__STDC_NO_COMPLEX\" : %d,\n", __STDC_NO_COMPLEX);
#else
    puts("        \"__STDC_NO_COMPLEX\" : null,");
#endif

#ifdef __STDC_NO_THREADS
    printf("        \"__STDC_NO_THREADS\" : %d,\n", __STDC_NO_THREADS);
#else
    puts("        \"__STDC_NO_THREADS\" : null,");
#endif

#ifdef __STDC_NO_VLA
    printf("        \"__STDC_NO_VLA\" : %d\n", __STDC_NO_VLA);
#else
    puts("        \"__STDC_NO_VLA\" : null");
#endif

    puts("    },");
}

static void show_limits_h(void) {
    puts("    {");
    puts("        \"header\" : \"<limits.h>\",");
    printf("        \"CHAR_BIT\" : %d,\n", (int)CHAR_BIT);
    printf("        \"SCHAR_MIN\" : %d,\n", (int)SCHAR_MIN);
    printf("        \"SCHAR_MAX\" : %d,\n", (int)SCHAR_MAX);
    printf("        \"UCHAR_MAX\" : %u,\n", (unsigned)UCHAR_MAX);
    printf("        \"CHAR_MIN\" : %d,\n", (int)CHAR_MIN);
    printf("        \"CHAR_MAX\" : %u,\n", (unsigned)CHAR_MAX);
#ifdef MB_LEN_MAX
    printf("        \"MB_LEN_MAX\" : %u,\n", (unsigned)MB_LEN_MAX);
#else
    puts("        \"MB_LEN_MAX\" : null");
#endif
    printf("        \"SHRT_MIN\" : %d,\n", (int)SHRT_MIN);
    printf("        \"SHRT_MAX\" : %d,\n", (int)SHRT_MAX);
    printf("        \"USHRT_MAX\" : %u,\n", (unsigned)USHRT_MAX);
    printf("        \"INT_MIN\" : %d,\n", INT_MIN);
    printf("        \"INT_MAX\" : %d,\n", INT_MAX);
    printf("        \"UINT_MAX\" : %u,\n", UINT_MAX);
    printf("        \"LONG_MIN\" : %ld,\n", LONG_MIN);
    printf("        \"LONG_MAX\" : %ld,\n", LONG_MAX);
    printf("        \"ULONG_MAX\" : %lu,\n", ULONG_MAX);
#ifdef LLONG_MIN
    printf("        \"LLONG_MIN\" : %lld,\n", LLONG_MIN);
#else
    puts("        \"LLONG_MIN\" : null");
#endif
#ifdef LLONG_MAX
    printf("        \"LLONG_MAX\" : %lld,\n", LLONG_MAX);
#else
    puts("        \"LLONG_MAX\" : null");
#endif
#ifdef ULLONG_MAX
    printf("        \"ULLONG_MAX\" : %llu,\n", ULLONG_MAX);
#else
    puts("        \"ULLONG_MAX\" : null");
#endif
    puts("    },");
}

static char *FLT_ROUNDS_meaning(int flt_rounds) {
    switch (flt_rounds) {
        case -1: return "interminable";
        case 0: return "toward zero";
        case 1: return "to nearest";
        case 2: return "toward positive infinity";
        case 3: return "toward negative infinity";
        default: return "implementation-defined";
    }
}

static char *FLT_EVAL_METHOD_meaning(int flt_eval_method) {
    switch (flt_eval_method) {
        case -1: return "interminable";
        case 0: return "evaluate to range and precision of type";
        case 1: return "evaluate using double or long double";
        case 2: return "evaluate using long double";
        default: return "implementation-defined";
    }
}

static char *HAS_SUBNORM_meaning(int has_subnorm) {
    switch (has_subnorm) {
        case -1: return "interminable";
        case 0: return "absent";
        case 1: return "present";
        default: return "implementation-defined";
    }
}

static void show_float_h(void) {
    puts("    {");
    puts("        \"header\" : \"<float.h>\",");

#ifdef FLT_ROUNDS
    printf("        \"FLT_ROUNDS\" : %d,\n", FLT_ROUNDS);
    printf("        \"FLT_ROUNDS_meaning\" : \"%s\",\n", FLT_ROUNDS_meaning(FLT_ROUNDS));
#else
    printf("        \"FLT_ROUNDS\" : null,\n");
#endif

#ifdef FLT_EVAL_METHOD
    printf("        \"FLT_EVAL_METHOD\" : %d,\n", FLT_EVAL_METHOD);
    printf("        \"FLT_EVAL_METHOD_meaning\" : \"%s\",\n", FLT_EVAL_METHOD_meaning(FLT_EVAL_METHOD));
#else
    printf("        \"FLT_EVAL_METHOD\" : null,\n");
#endif

#ifdef FLT_HAS_SUBNORM
    printf("        \"FLT_HAS_SUBNORM\" : %d,\n", FLT_HAS_SUBNORM);
    printf("        \"FLT_HAS_SUBNORM_meaning\" : \"%s\",\n", HAS_SUBNORM_meaning(FLT_HAS_SUBNORM));
#else
    printf("        \"FLT_HAS_SUBNORM\" : null,\n");
#endif

#ifdef DBL_HAS_SUBNORM
    printf("        \"DBL_HAS_SUBNORM\" : %d,\n", DBL_HAS_SUBNORM);
    printf("        \"DBL_HAS_SUBNORM_meaning\" : \"%s\",\n", HAS_SUBNORM_meaning(DBL_HAS_SUBNORM));
#else
    printf("        \"DBL_HAS_SUBNORM\" : null,\n");
#endif

#ifdef LDBL_HAS_SUBNORM
    printf("        \"LDBL_HAS_SUBNORM\" : %d,\n", LDBL_HAS_SUBNORM);
    printf("        \"LDBL_HAS_SUBNORM_meaning\" : \"%s\",\n", HAS_SUBNORM_meaning(LDBL_HAS_SUBNORM));
#else
    printf("        \"LDBL_HAS_SUBNORM\" : null,\n");
#endif

    printf("        \"FLT_RADIX\" : %d,\n", FLT_RADIX);

    printf("        \"FLT_MANT_DIG\" : %d,\n", FLT_MANT_DIG);
    printf("        \"DBL_MANT_DIG\" : %d,\n", DBL_MANT_DIG);
    printf("        \"LDBL_MANT_DIG\" : %d,\n", LDBL_MANT_DIG);

#ifdef FLT_DECIMAL_DIG
    printf("        \"FLT_DECIMAL_DIG\" : %d,\n", FLT_DECIMAL_DIG);
#else
    printf("        \"FLT_DECIMAL_DIG\" : null,\n");
#endif
#ifdef DBL_DECIMAL_DIG
    printf("        \"DBL_DECIMAL_DIG\" : %d,\n", DBL_DECIMAL_DIG);
#else
    printf("        \"DBL_DECIMAL_DIG\" : null,\n");
#endif
#ifdef LDBL_DECIMAL_DIG
    printf("        \"LDBL_DECIMAL_DIG\" : %d,\n", LDBL_DECIMAL_DIG);
#else
    printf("        \"LDBL_DECIMAL_DIG\" : null,\n");
#endif
#ifdef DECIMAL_DIG
    printf("        \"DECIMAL_DIG\" : %d,\n", DECIMAL_DIG);
#else
    printf("        \"DECIMAL_DIG\" : null,\n");
#endif

    printf("        \"FLT_DIG\" : %d,\n", FLT_DIG);
    printf("        \"DBL_DIG\" : %d,\n", DBL_DIG);
    printf("        \"LDBL_DIG\" : %d,\n", LDBL_DIG);

    printf("        \"FLT_MIN_EXP\" : %d,\n", FLT_MIN_EXP);
    printf("        \"DBL_MIN_EXP\" : %d,\n", DBL_MIN_EXP);
    printf("        \"LDBL_MIN_EXP\" : %d,\n", LDBL_MIN_EXP);

    printf("        \"FLT_MIN_10_EXP\" : %d,\n", FLT_MIN_10_EXP);
    printf("        \"DBL_MIN_10_EXP\" : %d,\n", DBL_MIN_10_EXP);
    printf("        \"LDBL_MIN_10_EXP\" : %d,\n", LDBL_MIN_10_EXP);

    printf("        \"FLT_MAX_EXP\" : %d,\n", FLT_MAX_EXP);
    printf("        \"DBL_MAX_EXP\" : %d,\n", DBL_MAX_EXP);
    printf("        \"LDBL_MAX_EXP\" : %d,\n", LDBL_MAX_EXP);

    printf("        \"FLT_MAX_10_EXP\" : %d,\n", FLT_MAX_10_EXP);
    printf("        \"DBL_MAX_10_EXP\" : %d,\n", DBL_MAX_10_EXP);
    printf("        \"LDBL_MAX_10_EXP\" : %d,\n", LDBL_MAX_10_EXP);

    printf("        \"FLT_MAX\" : %e,\n", FLT_MAX);
    printf("        \"DBL_MAX\" : %e,\n", DBL_MAX);
    printf("        \"LDBL_MAX\" : %Le,\n", LDBL_MAX);

    printf("        \"FLT_EPSILON\" : %e,\n", FLT_EPSILON);
    printf("        \"DBL_EPSILON\" : %e,\n", DBL_EPSILON);
    printf("        \"LDBL_EPSILON\" : %Le,\n", LDBL_EPSILON);

    printf("        \"FLT_MIN\" : %e,\n", FLT_MIN);
    printf("        \"DBL_MIN\" : %e,\n", DBL_MIN);
    printf("        \"LDBL_MIN\" : %Le,\n", LDBL_MIN);

#ifdef FLT_TRUE_MIN
    printf("        \"FLT_TRUE_MIN\" : %e,\n", FLT_TRUE_MIN);
#else
    printf("        \"FLT_TRUE_MIN\" : null,\n");
#endif
#ifdef DBL_TRUE_MIN
    printf("        \"DBL_TRUE_MIN\" : %e,\n", DBL_TRUE_MIN);
#else
    printf("        \"DBL_TRUE_MIN\" : null,\n");
#endif
#ifdef LDBL_TRUE_MIN
    printf("        \"LDBL_TRUE_MIN\" : %Le,\n", LDBL_TRUE_MIN);
#else
    printf("        \"LDBL_TRUE_MIN\" : null,\n");
#endif

    puts("    },");
}

static void show_stdint_h(void) {
    puts("    {");
    puts("        \"header\" : \"<stdint.h>\",");
#ifdef STDINT_H_EXISTS
    puts("        \"header_exists\" : true,");

#ifdef INT8_MIN
    printf("        \"INT8_MIN\" : %s\n", signed_image(INT8_MIN));
#else
    printf("        \"INT8_MIN\" : null,\n");
#endif
#ifdef INT8_MAX
    printf("        \"INT8_MAX\" : %s\n", signed_image(INT8_MAX));
#else
    printf("        \"INT8_MAX\" : null,\n");
#endif
#ifdef UINT8_MAX
    printf("        \"UINT8_MAX\" : %s\n", unsigned_image(UINT8_MAX));
#else
    printf("        \"UINT8_MAX\" : null,\n");
#endif
#ifdef INT16_MIN
    printf("        \"INT16_MIN\" : %s\n", signed_image(INT16_MIN));
#else
    printf("        \"INT16_MIN\" : null,\n");
#endif
#ifdef INT16_MAX
    printf("        \"INT16_MAX\" : %s\n", signed_image(INT16_MAX));
#else
    printf("        \"INT16_MAX\" : null,\n");
#endif
#ifdef UINT16_MAX
    printf("        \"UINT16_MAX\" : %s\n", unsigned_image(UINT16_MAX));
#else
    printf("        \"UINT16_MAX\" : null,\n");
#endif
#ifdef INT32_MIN
    printf("        \"INT32_MIN\" : %s\n", signed_image(INT32_MIN));
#else
    printf("        \"INT32_MIN\" : null,\n");
#endif
#ifdef INT32_MAX
    printf("        \"INT32_MAX\" : %s\n", signed_image(INT32_MAX));
#else
    printf("        \"INT32_MAX\" : null,\n");
#endif
#ifdef UINT32_MAX
    printf("        \"UINT32_MAX\" : %s\n", unsigned_image(UINT32_MAX));
#else
    printf("        \"UINT32_MAX\" : null,\n");
#endif
#ifdef INT64_MIN
    printf("        \"INT64_MIN\" : %s\n", signed_image(INT64_MIN));
#else
    printf("        \"INT64_MIN\" : null,\n");
#endif
#ifdef INT64_MAX
    printf("        \"INT64_MAX\" : %s\n", signed_image(INT64_MAX));
#else
    printf("        \"INT64_MAX\" : null,\n");
#endif
#ifdef UINT64_MAX
    printf("        \"UINT64_MAX\" : %s\n", unsigned_image(UINT64_MAX));
#else
    printf("        \"UINT64_MAX\" : null,\n");
#endif

    printf("        \"INT_LEAST8_MIN\" : %s\n", signed_image(INT_LEAST8_MIN));
    printf("        \"INT_LEAST8_MAX\" : %s\n", signed_image(INT_LEAST8_MAX));
    printf("        \"UINT_LEAST8_MAX\" : %s\n", unsigned_image(UINT_LEAST8_MAX));
    printf("        \"INT_LEAST16_MIN\" : %s\n", signed_image(INT_LEAST16_MIN));
    printf("        \"INT_LEAST16_MAX\" : %s\n", signed_image(INT_LEAST16_MAX));
    printf("        \"UINT_LEAST16_MAX\" : %s\n", unsigned_image(UINT_LEAST16_MAX));
    printf("        \"INT_LEAST32_MIN\" : %s\n", signed_image(INT_LEAST32_MIN));
    printf("        \"INT_LEAST32_MAX\" : %s\n", signed_image(INT_LEAST32_MAX));
    printf("        \"UINT_LEAST32_MAX\" : %s\n", unsigned_image(UINT_LEAST32_MAX));
    printf("        \"INT_LEAST64_MIN\" : %s\n", signed_image(INT_LEAST64_MIN));
    printf("        \"INT_LEAST64_MAX\" : %s\n", signed_image(INT_LEAST64_MAX));
    printf("        \"UINT_LEAST64_MAX\" : %s\n", unsigned_image(UINT_LEAST64_MAX));

    printf("        \"INT_FAST8_MIN\" : %s\n", signed_image(INT_FAST8_MIN));
    printf("        \"INT_FAST8_MAX\" : %s\n", signed_image(INT_FAST8_MAX));
    printf("        \"UINT_FAST8_MAX\" : %s\n", unsigned_image(UINT_FAST8_MAX));
    printf("        \"INT_FAST16_MIN\" : %s\n", signed_image(INT_FAST16_MIN));
    printf("        \"INT_FAST16_MAX\" : %s\n", signed_image(INT_FAST16_MAX));
    printf("        \"UINT_FAST16_MAX\" : %s\n", unsigned_image(UINT_FAST16_MAX));
    printf("        \"INT_FAST32_MIN\" : %s\n", signed_image(INT_FAST32_MIN));
    printf("        \"INT_FAST32_MAX\" : %s\n", signed_image(INT_FAST32_MAX));
    printf("        \"UINT_FAST32_MAX\" : %s\n", unsigned_image(UINT_FAST32_MAX));
    printf("        \"INT_FAST64_MIN\" : %s\n", signed_image(INT_FAST64_MIN));
    printf("        \"INT_FAST64_MAX\" : %s\n", signed_image(INT_FAST64_MAX));
    printf("        \"UINT_FAST64_MAX\" : %s\n", unsigned_image(UINT_FAST64_MAX));

    printf("        \"INTPTR_MIN\" : %s\n", signed_image(INTPTR_MIN));
    printf("        \"INTPTR_MAX\" : %s\n", signed_image(INTPTR_MAX));
    printf("        \"UINTPTR_MAX\" : %s\n", unsigned_image(UINTPTR_MAX));

    printf("        \"PTRDIFF_MIN\" : %s\n", signed_image(PTRDIFF_MIN));
    printf("        \"PTRDIFF_MAX\" : %s\n", signed_image(PTRDIFF_MAX));

#ifdef SIG_ATOMIC_MIN
#if SIG_ATOMIC_MIN == 0
    printf("        \"SIG_ATOMIC_MIN\" : %s\n", unsigned_image(SIG_ATOMIC_MIN));
    printf("        \"SIG_ATOMIC_MAX\" : %s\n", unsigned_image(SIG_ATOMIC_MAX));
#else
    printf("        \"SIG_ATOMIC_MIN\" : %s\n", signed_image(SIG_ATOMIC_MIN));
    printf("        \"SIG_ATOMIC_MAX\" : %s\n", signed_image(SIG_ATOMIC_MAX));
#endif
#endif

    printf("        \"SIZE_MAX\" : %s\n", unsigned_image(SIZE_MAX));

#ifdef WCHAR_MIN
#if WCHAR_MIN == 0
    printf("        \"WCHAR_MIN\" : %s\n", unsigned_image(WCHAR_MIN));
    printf("        \"WCHAR_MAX\" : %s\n", unsigned_image(WCHAR_MAX));
#else
    printf("        \"WCHAR_MIN\" : %s\n", signed_image(WCHAR_MIN));
    printf("        \"WCHAR_MAX\" : %s\n", signed_image(WCHAR_MAX));
#endif
#endif

#ifdef WINT_MIN
#if WINT_MIN == 0
    printf("        \"WINT_MIN\" : %s\n", unsigned_image(WINT_MIN));
    printf("        \"WINT_MAX\" : %s\n", unsigned_image(WINT_MAX));
#else
    printf("        \"WINT_MIN\" : %s\n", signed_image(WINT_MIN));
    printf("        \"WINT_MAX\" : %s\n", signed_image(WINT_MAX));
#endif
#endif

#else
    puts("        \"header_exists\" : false");
#endif
    puts("    },");
}

int main(int argc, char **argv) {
    set_formats();

    puts("[");

    show_configuration(argc, argv);
    show_predefined_macros();
    show_limits_h();
    show_float_h();
    show_stdint_h();

#ifdef STDBOOL_H_EXISTS
    SHOW_INTEGER_TYPE(bool, bool_endianness(), 0, 0);
#endif

    SHOW_INTEGER_TYPE(enum small_enum, small_enum_endianness(), 0, 0);
    SHOW_INTEGER_TYPE(enum small_signed_enum, small_signed_enum_endianness(), 0, 0);

    SHOW_INTEGER_TYPE(char, char_endianness(), CHAR_MIN, CHAR_MAX);
#ifdef SIGNED_CHAR_EXISTS
    SHOW_INTEGER_TYPE(signed char, signed_char_endianness(), SCHAR_MIN, SCHAR_MAX);
#endif
    SHOW_INTEGER_TYPE(unsigned char, unsigned_char_endianness(), 0, UCHAR_MAX);

    SHOW_INTEGER_TYPE(short, short_endianness(), SHRT_MIN, SHRT_MAX);
    SHOW_INTEGER_TYPE(unsigned short, unsigned_short_endianness(), 0, USHRT_MAX);

    SHOW_INTEGER_TYPE(int, int_endianness(), INT_MIN, INT_MAX);
    SHOW_INTEGER_TYPE(unsigned, unsigned_endianness(), 0, UINT_MAX);

    SHOW_INTEGER_TYPE(long, long_endianness(), LONG_MIN, LONG_MAX);
    SHOW_INTEGER_TYPE(unsigned long, unsigned_long_endianness(), 0, ULONG_MAX);

#ifdef LONG_LONG_EXISTS
    SHOW_INTEGER_TYPE(long long, long_long_endianness(), MY_LLONG_MIN, MY_LLONG_MAX);
    SHOW_INTEGER_TYPE(unsigned long long, unsigned_long_long_endianness(), 0, MY_ULLONG_MAX);
#endif

#ifdef STDINT_H_EXISTS
    SHOW_INTEGER_TYPE(intmax_t, intmax_t_endianness(), INTMAX_MIN, INTMAX_MAX);
    SHOW_INTEGER_TYPE(uintmax_t, uintmax_t_endianness(), 0, UINTMAX_MAX);
#endif

    SHOW_FLOATING_TYPE(float,       FLT_MANT_DIG,  FLT_MIN_EXP,  FLT_MAX_EXP,
                                    f_one, f_minus_sixteen, f_one_million);
    SHOW_FLOATING_TYPE(double,      DBL_MANT_DIG,  DBL_MIN_EXP,  DBL_MAX_EXP,
                                    d_one, d_minus_sixteen, d_one_million);
#ifdef LONG_DOUBLE_EXISTS
    SHOW_FLOATING_TYPE(long double, LDBL_MANT_DIG, LDBL_MIN_EXP, LDBL_MAX_EXP,
                                    ld_one, ld_minus_sixteen, ld_one_million);
#endif

    SHOW_INTEGER_TYPE(ptrdiff_t, ptrdiff_t_endianness(), MY_PTRDIFF_MIN, MY_PTRDIFF_MAX);
    SHOW_INTEGER_TYPE(size_t, size_t_endianness(), 0, MY_SIZE_MAX);
    SHOW_INTEGER_TYPE(wchar_t, wchar_t_endianness(), MY_WCHAR_MIN, MY_WCHAR_MAX);

#ifdef FLOATING_TIME_T
    SHOW_FLOATING_TYPE(time_t, 0, 0, 0, 0.0, 0.0, 0.0);
#else
    SHOW_INTEGER_TYPE(time_t, time_t_endianness(), 0, 0);
#endif

#ifdef FLOATING_CLOCK_T
    SHOW_FLOATING_TYPE(clock_t, 0, 0, 0, 0.0, 0.0, 0.0);
#else
    SHOW_INTEGER_TYPE(clock_t, clock_t_endianness(), 0, 0);
#endif

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

    /*
     * Do this just to make the commas match up.
     */
    puts("    {");
    puts("        \"comment\" : \"done\"");
    puts("    }");

    puts("]");

    exit(EXIT_SUCCESS);
} /* main */
