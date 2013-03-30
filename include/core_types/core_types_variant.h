
/*
 * @file core_types_variant.h
 * @brief generic variant type
 */

#ifndef CORE_TYPES_VARIANT_H
#define CORE_TYPES_VARIANT_H

#define CORE_TYPES_MATRIX_ONLY_STRUCT
#include <core_types/core_types_matrix.h>
#undef CORE_TYPES_MATRIX_ONLY_STRUCT

#define COMPARE_EPSILON 0.000000001
#define COMPARE_DOUBLE(a,b) (fabs(a-b) < COMPARE_EPSILON)

typedef enum
{
    CORE_TYPES_VARIANT_EMPTY     = 0,
    CORE_TYPES_VARIANT_BOOL      = 1,
    CORE_TYPES_VARIANT_INT       = 2,
    CORE_TYPES_VARIANT_DOUBLE    = 3,
    CORE_TYPES_VARIANT_STRING    = 4,
    CORE_TYPES_VARIANT_MATRIX    = 5,
    CORE_TYPES_VARIANT_ERROR     = 6,
    CORE_TYPES_VARIANT_FUNCTION  = 7
} CORE_TYPES_VARIANT_TYPE;

struct core_types_variant
{
    CORE_TYPES_VARIANT_TYPE m_type;
    union
    {
        int                                         m_i;
        double                                      m_d;
        struct { char *m_s; int m_l; }              m_s;
        matrix                                      m_m;
        struct { int m_n; void* m_f; void* m_a; }   m_f;
    } m_v;
};
typedef struct core_types_variant variant;

/*
 * only needs to be called for matrix and string variants
 */
void variant_free(variant v);

//deep copy
variant variant_copy(variant v);

/*
 * @brief Returns a string representation of a variant
 * Note: Returned array is a local statically allocated array, so watch out!
 */
char* variant_to_string(variant v);
char* variant_to_string_(variant v);

// operators
int variant_equal(variant a, variant b);
int variant_less(variant a, variant b);
int variant_and(variant a, variant b);
int variant_or(variant a, variant b);
variant variant_add(variant a, variant b);
variant variant_multiply(variant a, variant b);

variant variant_call(variant f, ...);
int variant_params(variant f);

variant variant_from_string(char* c);

variant variant_from_error(char* c);

// copies the matrix
variant variant_from_matrix(matrix m);

// does not copy the matrix
variant variant_matrix(matrix m);

char* variant_as_string(variant v);

char* variant_as_error(variant v);

matrix variant_as_matrix(variant v);

#ifndef __cplusplus
#define VARIANT_USE_MACROS
#endif

#ifdef VARIANT_USE_MACROS

#define VARIANT_EMPTY (variant){CORE_TYPES_VARIANT_EMPTY, {.m_i=0} };
#define VARIANT_NIL (variant){CORE_TYPES_VARIANT_EMPTY, {.m_i=0} };

#define variant_from_bool(b) (variant){CORE_TYPES_VARIANT_BOOL, {.m_i=b}}

#define variant_from_int(i) (variant){CORE_TYPES_VARIANT_INT, {.m_i=i}}

#define variant_from_double(d) (variant){CORE_TYPES_VARIANT_DOUBLE, {.m_d=d}}

#define variant_from_function(f, n) (variant){CORE_TYPES_VARIANT_FUNCTION, {.m_f.m_n=n, .m_f.m_a=0, .m_f.m_f=f}}

#define variant_from_function_object(f, a, n) (variant){CORE_TYPES_VARIANT_FUNCTION, {.m_f.m_n=n, .m_f.m_a=a, .m_f.m_f=f}}

#define variant_as_bool(v) ((CORE_TYPES_VARIANT_BOOL == v.m_type)?v.m_v.m_i:((CORE_TYPES_VARIANT_INT == v.m_type)?v.m_v.m_i:0))

#define variant_as_int(v) ((CORE_TYPES_VARIANT_INT == v.m_type)?v.m_v.m_i:((CORE_TYPES_VARIANT_DOUBLE == v.m_type)?((int)v.m_v.m_i):0))

#define variant_as_double(v) ((CORE_TYPES_VARIANT_DOUBLE == v.m_type)?v.m_v.m_d:((CORE_TYPES_VARIANT_INT == v.m_type)?((double)v.m_v.m_i):0.0))

#define variant_is_empty(v) (CORE_TYPES_VARIANT_EMPTY == v.m_type)

#define variant_is_bool(v) (CORE_TYPES_VARIANT_BOOL == v.m_type)

#define variant_is_int(v) (CORE_TYPES_VARIANT_INT == v.m_type)

#define variant_is_double(v) (CORE_TYPES_VARIANT_DOUBLE == v.m_type)

#define variant_is_string(v) (CORE_TYPES_VARIANT_STRING == v.m_type)

#define variant_is_error(v) (CORE_TYPES_VARIANT_ERROR == v.m_type)

#define variant_is_matrix(v) (CORE_TYPES_VARIANT_MATRIX == v.m_type)

#define variant_is_function(v) (CORE_TYPES_VARIANT_FUNCTION == v.m_type)

#else

variant _variant_empty();
#define VARIANT_EMPTY() _variant_empty()

variant _variant_nil();
#define VARIANT_NIL() _variant_nil()

variant _variant_from_double(double d);
#define variant_from_double(d) _variant_from_double(d)

variant _variant_from_bool(int b);
#define variant_from_bool(b) _variant_from_bool(b)

variant _variant_from_int(int i);
#define variant_from_int(i) _variant_from_int(i)

variant _variant_from_function(void* f, void* a, int n);
#define variant_from_function(f, n) _variant_from_funcion(f, 0, n)
#define variant_from_function_object(f, a, n) _variant_from_funcion(f, a, n)

int _variant_as_bool(variant v);
#define variant_as_bool(v) _variant_as_bool(v)

int _variant_as_int(variant v);
#define variant_as_int(v) _variant_as_int(v)

double _variant_as_double(variant v);
#define variant_as_double(v) _variant_as_double(v)

int _variant_is_empty(variant v);
#define variant_is_empty(v) _variant_is_empty(v)

int _variant_is_bool(variant v);
#define variant_is_bool(v) _variant_is_bool(v)

int _variant_is_int(variant v);
#define variant_is_int(v) _variant_is_int(v)

int _variant_is_double(variant v);
#define variant_is_double(v) _variant_is_double(v)

int _variant_is_string(variant v);
#define variant_is_string(v) _variant_is_string(v)

int _variant_is_error(variant v);
#define variant_is_error(v) _variant_is_error(v)

int _variant_is_matrix(variant v);
#define variant_is_matrix(v) _variant_is_matrix(v)

int _variant_is_function(variant v);
#define variant_is_function(v) _variant_is_function(v)

#endif

#endif

