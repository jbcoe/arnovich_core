

/**
 * @package core_types
 * @brief Variant types for usage with C/C++/Python.
 *
 * This modules provides two main types: the \c variant type, with both a C (core_types_variant.h) and C++ (core_types_variant.hpp) interface, and the matrix type (core_types_matrix.h), which is a C type that embeds into the \c variant type.
 * 
 * The \c variant type interfaces with Python through the core_python module.
 *
 * <b>Example</b>
 *
 * A few simple C examples of how to use the \c variant type.
 * \code
 * {
 *   variant v = variant_from_double(2.0);
 *   variant w = variant_from_int(3);
 *   if(variant_is_equal(v, w))
 *   {
 *     //this doesn't happen
 *   }
 *   if(variant_is_less(v, w))
 *   {
 *     //this happens
 *   }
 *   //ints and doubles casts easily between them
 *   double d = variant_from_double(w);
 *   // actually no need to free these
 *   // but it is good practice to do so
 *   variant_free(v);
 *   variant_free(w);
 * }
 *
 * {
 *   matrix m = matrix_init(2,2);
 *   matrix_set(m, 0, 0, variant_from_double(1.0));
 *   double d = variant_from_double(matrix_get(m, 0, 0));
 *   // since the matrix only holds simple variants
 *   // i.e. no other matrices or strings
 *   // we don't need to deep-free this matrix
 *   matrix_free(m);
 * }
 * 
 * \endcode
 *
 *
 * <b>Files of interest</b>
 * - core_types/core_types_variant.h
 * - core_types/core_types_variant.hpp
 * - core_types/core_types_matrix.h
 */

/**
 * @file core_types_variant.h
 * @brief This is the C-interface, and the main functionality, of the \c variant type.
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

/**
  * @brief The C-style \c variant type.
  */
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

/** @name Other functions
  * Functions for free'ing, copying ,serialising and calling variants.
  */
///@{
/**
  * @brief Method for deep-free'ing variant.
  * This is really only needed for \c matrix or \c string types.
  */
void variant_free(variant v);

/**
  * @brief Deep copy of variant. As with \c variant_free this is only really needed for \c matrix and \c string types.
  */
variant variant_copy(variant v);

/**
 * @brief Returns a string representation of a variant.
 * Note: The returned char array is a local statically allocated array, so watch out! This should not be \c free by the caller and can overflow.
 */
char* variant_to_string(variant v);

/**
 * @brief Returns a string representation of a variant.
 * Note: This version is similar to \c variant_to_string but does not copy \c string type variants, so will not overflow the for strings.
 */
char* variant_to_string_(variant v);

/**
 * @brief Calls a \c function type variant with the given inputs.
 */
variant variant_call(variant f, ...);

/**
 * @brief Returns the number of expected inputs for a \c function type variant.
 */
int variant_params(variant f);
///@}


/** @name Operator functions
  * Functions for applying typical operators to variants.
  */
///@{
/**
  * @brief Test for equality between two variants
  */
int variant_equal(variant a, variant b);
/**
  * @brief Test whether \c a is smaller than \c b.
  * This function only works for integers, doubles and strings otherwise returns false.
  */
int variant_less(variant a, variant b);
/**
  * @brief AND operator for variants. Returns false if \c a and \c b are not of bool type.
  */
int variant_and(variant a, variant b);
/**
  * @brief OR operator for variants. Returns false if \c a and \c b are not of bool type.
  */
int variant_or(variant a, variant b);
/**
  * @brief Operator for adding two variants. Acts as numeric addition oi \c a and \c b are of integer or double type, 
  * as concatenation if \c a and \c b are of string type and otherwise as NOOP.
  */
variant variant_add(variant a, variant b);
/**
  * @brief Operator for multiplying two variants. Acts as numeric multiplication if \c a and \c b are of integer or double type and otherwise as NOOP.
  */
variant variant_multiply(variant a, variant b);
///@}


/** 
  * @name Creation functions
  * Functions for creating variants.
  */
///@{
/**
  * @brief Create a variant from a string
  */
variant variant_from_string(char* c);

/**
  * @brief Create an Error variant from a string
  */
variant variant_from_error(char* c);

/**
  * @brief Create a variant from a \c matrix by deep-copying the matrix.
  */
variant variant_from_matrix(matrix m);

/**
  * @brief Create a variant from a \c matrix by shallow-copying the matrix.
  */
variant variant_matrix(matrix m);
///@}

/** 
  * @name Conversion functions
  * Functions for converting variants into other types.
  */
///@{
/**
  * @brief Convert a variant into an string. Returns an empty string if the variant is not of string type.
  */
char* variant_as_string(variant v);
/**
  * @brief Convert a variant into an error. Returns an empty string if the variant is not of error type.
  */
char* variant_as_error(variant v);
/**
  * @brief Convert a variant into a \c matrix. Returns an empty matrix(identity matrix of dimension 0) if the variant is not of matrix type.
  */
matrix variant_as_matrix(variant v);
///@}

#ifndef __cplusplus
#ifndef _WIN32
#define MATRIX_USE_MACROS
#endif
#endif

#ifdef VARIANT_USE_MACROS


/** @name Creation functions
 *  Functions for creating variants.
 */
///@{
/**
  * @brief Create an Empty variant
  */
#define VARIANT_EMPTY (variant){CORE_TYPES_VARIANT_EMPTY, {.m_i=0} };
/**
  * @brief Create an NIL/Empty variant
  */
#define VARIANT_NIL (variant){CORE_TYPES_VARIANT_EMPTY, {.m_i=0} };
/**
  * @brief Create a variant from a bool
  */
#define variant_from_bool(b) (variant){CORE_TYPES_VARIANT_BOOL, {.m_i=b}}
/**
  * @brief Create a variant from a int
  */
#define variant_from_int(i) (variant){CORE_TYPES_VARIANT_INT, {.m_i=i}}
/**
  * @brief Create a variant from a double
  */
#define variant_from_double(d) (variant){CORE_TYPES_VARIANT_DOUBLE, {.m_d=d}}
/**
  * @brief Create a variant from a state-less function
  */
#define variant_from_function(f, n) (variant){CORE_TYPES_VARIANT_FUNCTION, {.m_f.m_n=n, .m_f.m_a=0, .m_f.m_f=f}}
/**
  * @brief Create a variant from a function with a corresponding state
  */
#define variant_from_function_object(f, a, n) (variant){CORE_TYPES_VARIANT_FUNCTION, {.m_f.m_n=n, .m_f.m_a=a, .m_f.m_f=f}}
///@}


/** @name Conversion functions
 *  Functions for converting variants into other types.
 */
///@{
/**
  * @brief Convert a variant into an integer bool. Returns false if the variant is not of bool type.
  */
#define variant_as_bool(v) ((CORE_TYPES_VARIANT_BOOL == v.m_type)?v.m_v.m_i:((CORE_TYPES_VARIANT_INT == v.m_type)?v.m_v.m_i:0))

/**
  * @brief Convert a variant into an integer. Returns 0 if the variant is not of integer type.
  */

#define variant_as_int(v) ((CORE_TYPES_VARIANT_INT == v.m_type)?v.m_v.m_i:((CORE_TYPES_VARIANT_DOUBLE == v.m_type)?((int)v.m_v.m_i):0))

/**
  * @brief Convert a variant into a double. Returns 0.0 if the variant is not of double type.
  */
#define variant_as_double(v) ((CORE_TYPES_VARIANT_DOUBLE == v.m_type)?v.m_v.m_d:((CORE_TYPES_VARIANT_INT == v.m_type)?((double)v.m_v.m_i):0.0))
///@}


/** 
  * @name Type test functions
  * Functions for testing type of a variant.
  */
///@{
/**
  * @brief Test if the variant is of empty type.
  */
#define variant_is_empty(v) (CORE_TYPES_VARIANT_EMPTY == v.m_type)

/**
  * @brief Test if the variant is of bool type.
  */
#define variant_is_bool(v) (CORE_TYPES_VARIANT_BOOL == v.m_type)

/**
  * @brief Test if the variant is of integer type.
  */
#define variant_is_int(v) (CORE_TYPES_VARIANT_INT == v.m_type)

/**
  * @brief Test if the variant is of double type.
  */
#define variant_is_double(v) (CORE_TYPES_VARIANT_DOUBLE == v.m_type)

/**
  * @brief Test if the variant is of string type.
  */
#define variant_is_string(v) (CORE_TYPES_VARIANT_STRING == v.m_type)

/**
  * @brief Test if the variant is of error type.
  */
#define variant_is_error(v) (CORE_TYPES_VARIANT_ERROR == v.m_type)

/**
  * @brief Test if the variant is of matrix type.
  */
#define variant_is_matrix(v) (CORE_TYPES_VARIANT_MATRIX == v.m_type)

/**
  * @brief Test if the variant is of function type.
  */
#define variant_is_function(v) (CORE_TYPES_VARIANT_FUNCTION == v.m_type)
///@}

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
#define variant_from_function(f, n) _variant_from_function(f, 0, n)
#define variant_from_function_object(f, a, n) _variant_from_function(f, a, n)

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

