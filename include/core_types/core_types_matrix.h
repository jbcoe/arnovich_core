
/*
 * @file core_types_matrix.h
 * @brief simple two_dimensional matrix
 */

#ifndef CORE_TYPES_MATRIX_STRUCT_H
#define CORE_TYPES_MATRIX_STRUCT_H

struct core_types_variant;

typedef struct core_types_matrix
{
    struct core_types_variant* m_values;
    unsigned int m_width;  //x-dimension
    unsigned int m_height; //y-dimension
} matrix;

#endif

#include <core_types/core_types_variant.h>

#ifndef CORE_TYPES_MATRIX_ONLY_STRUCT

#ifndef CORE_TYPES_MATRIX_H
#define CORE_TYPES_MATRIX_H

matrix matrix_init(unsigned int width, unsigned int height);

matrix matrix_copy(matrix m);

void matrix_swap(matrix to, matrix from);

void matrix_free(matrix m);

int matrix_compare(matrix a, matrix b);

matrix matrix_identity(int n);

/*
 * @brief Returns a string representation of the matrix.
 * Note: Returned array is a local statically allocated array, so watch out!
 */
char* matrix_to_string(matrix m);

void matrix_move_rows(matrix m, int row1, int row2);

matrix matrix_get_column(matrix m, int col);

matrix matrix_get_row(matrix m, int row);

void matrix_replace_row(matrix a, int row1, matrix b, int row2);

#define MATRIX_IDENTITY(N) matrix_identity(N)

#ifndef __cplusplus
#define MATRIX_USE_MACROS
#endif

#ifdef MATRIX_USE_MACROS

#define matrix_get_safe(m,i,j) (((i < (m).m_width) && (j < (m).m_height))?(m).m_values[(j)*m.m_width+i]:VARIANT_NIL)

#define matrix_get(m,i,j) (m).m_values[(j)*(m).m_width+i]

#define matrix_set_safe(m,i,j,v) if((i < (m).m_width) && (j < (m).m_height)){(m).m_values[(j)*m.m_width+i] = v;}

#define matrix_set(m,i,j,v) (m).m_values[(j)*(m).m_width+i] = v;

#else

variant _matrix_get(matrix m, unsigned int i, unsigned int j);
#define matrix_get(m, i, j) _matrix_get(m, i, j)
#define matrix_get_safe(m, i, j) _matrix_get(m, i, j)

void _matrix_set(matrix m, unsigned int i, unsigned int j, variant v);
#define matrix_set(m, i, j, v) _matrix_set(m, i, j, v)
#define matrix_set_safe(m, i, j, v) _matrix_set(m, i, j, v)

#endif

#endif

#endif

