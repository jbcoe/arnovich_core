
/*
 * @file core_types_matrix.h
 * @brief simple two_dimensional matrix
 */

#ifndef CORE_TYPES_MATRIX_H
#define CORE_TYPES_MATRIX_H

#include <core_types/core_types_variant.h>

typedef struct core_types_matrix
{
    variant* m_values;
    unsigned int m_width;  //x-dimension
    unsigned int m_height; //y-dimension
} matrix;

#define MATRIX_USE_MACROS

matrix matrix_init(unsigned int width, unsigned int height);

matrix matrix_copy(matrix m);

void matrix_free(matrix m);

int matrix_compare(matrix a, matrix b);

matrix matrix_identity(int n);

#define MATRIX_IDENTITY(N) matrix_identity(N)

/*
 * @brief Returns a string representation of the matrix.
 * Note: Returned array is a local statically allocated array, so watch out!
 */
char* matrix_to_string(matrix m);

void matrix_move_rows(matrix m, int row1, int row2);

matrix matrix_get_column(matrix m, int col);

matrix matrix_get_row(matrix m, int row);

void matrix_replace_row(matrix a, int row1, matrix b, int row2);

#ifdef MATRIX_USE_MACROS

#define matrix_get_safe(m,i,j) (((i < (m).m_width) && (j < (m).m_height))?(m).m_values[(j)*m.m_width+i]:VARIANT_NIL)

#define matrix_get(m,i,j) (m).m_values[(j)*(m).m_width+i]

#define matrix_set_safe(m,i,j,v) if((i < (m).m_width) && (j < (m).m_height)){(m).m_values[(j)*m.m_width+i] = v;}

#define matrix_set(m,i,j,v) (m).m_values[(j)*(m).m_width+i] = v;

#else

variant matrix_get(matrix m, unsigned int i, unsigned int j);

void matrix_set(matrix m, unsigned int i, unsigned int j, variant v);

#endif

#endif

