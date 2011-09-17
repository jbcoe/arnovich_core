
/*
 * @file core_types_matrix.h
 */

#ifndef CORE_TYPES_MATRIX_H
#define CORE_TYPES_MATRIX_H

#include <core_types/core_types_variant.h>

// two-dimensional matrix
typedef struct core_types_matrix
{
    variant* m_values;
    unsigned int m_width;  //x-dimension
    unsigned int m_height; //y-dimension
} matrix;

matrix core_types_matrix_init(unsigned int width, unsigned int height);

void core_types_matrix_free(matrix m);

variant core_types_matrix_get(matrix m, unsigned int i, unsigned int j);

void core_types_matrix_set(matrix m, unsigned int i, unsigned int j, variant v);

#endif

