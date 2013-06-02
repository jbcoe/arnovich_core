
/**
  * @file core_types_matrix.h
  * @brief Simple 2-dimensional matrix type
  */

#ifndef CORE_TYPES_MATRIX_STRUCT_H
#define CORE_TYPES_MATRIX_STRUCT_H

struct core_types_variant;

/**
  * @brief The \c matrix type
  */
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

/** @name Management function
  * Functions for creating, free'ing etc.
  */
///@{
/**
  * @brief Create matrix of \c width * \c height. All entries are initialised to EMPTY.
  */
matrix matrix_init(unsigned int width, unsigned int height);

/**
  * @brief Shallow-copy of matrix.
  */
matrix matrix_copy(matrix m);

/**
  * @brief Shallow-copy of matrix.
  */
void matrix_swap(matrix to, matrix from);

/**
  * @brief Shallow-free matrix. Note that this only free's the matrix and not the elements.
  */
void matrix_free(matrix m);

/**
  * @brief This compares two matrices. They are equal if and only if the sizes are equal and every single element is equal by \c variant_equal.
  */
int matrix_compare(matrix a, matrix b);

/**
  * @brief Create an identity matrix of dimension \c n.
  */
matrix matrix_identity(int n);

/**
  * @brief Create an identity matrix of dimension \c n.
  */
#define MATRIX_IDENTITY(N) matrix_identity(N)

/**
  * @brief Returns a string representation of the matrix.
  * Note: Returned array is a local statically allocated array, so watch out!
  */
char* matrix_to_string(matrix m);
///@}

/** @name Getters and setters.
  * Function for setting, getting and replace elements.
  */
///@{
/**
  * @brief Swaps row \c row1 and row \c row2.
  */
void matrix_move_rows(matrix m, int row1, int row2);

/**
  * @brief Returns columns \c col as a column matrix.
  */
matrix matrix_get_column(matrix m, int col);

/**
  * @brief Returns row \c row as a row matrix.
  */
matrix matrix_get_row(matrix m, int row);

/**
  * @brief Swaps row \c row1 and row \c row2.
  */
void matrix_replace_row(matrix a, int row1, matrix b, int row2);

///@}

#ifndef __cplusplus
#define MATRIX_USE_MACROS
#endif

#ifdef MATRIX_USE_MACROS

/** @name Getters and setters.
  * Function for setting, getting and replace elements.
  */
///@{

/**
  * @brief Get element at ( \c i , \c j) with bounds check and return EMPTY if out-of-bounds.
  */
#define matrix_get_safe(m,i,j) (((i < (m).m_width) && (j < (m).m_height))?(m).m_values[(j)*m.m_width+i]:VARIANT_NIL)

/**
  * @brief Get element at ( \c i , \c j) without checking bounds.
  */
#define matrix_get(m,i,j) (m).m_values[(j)*(m).m_width+i]

/**
  * @brief GSt element at ( \c i , \c j) with bounds check and do nothing if out-of-bounds.
  */
#define matrix_set_safe(m,i,j,v) if((i < (m).m_width) && (j < (m).m_height)){(m).m_values[(j)*m.m_width+i] = v;}

/**
  * @brief GSt element at ( \c i , \c j) without checking bounds.
  */
#define matrix_set(m,i,j,v) (m).m_values[(j)*(m).m_width+i] = v;

///@}

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

