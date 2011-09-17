
/*
 * @file core_types_matrix.c
 */

#include <core_types/core_types_matrix.h>
#include <stdlib.h>

matrix core_types_matrix_init(unsigned int width, unsigned int height)
{
    matrix m;
    m.m_width = width;
    m.m_height = height;
    m.m_values = (variant*)malloc(sizeof(variant)*width*height);
    return m;
}

void core_types_matrix_free(matrix m)
{
    m.m_width = 0;
    m.m_height = 0;
    free(m.m_values);
    m.m_values = NULL;
}

variant core_types_matrix_get(matrix m, unsigned int i, unsigned int j)
{
    if((i < m.m_width) && (j < m.m_height))
    {
        return m.m_values[j*m.m_width+i];
    }
    return VARIANT_NIL;
}

void core_types_matrix_set(matrix m, unsigned int i, unsigned int j, variant v)
{
    if((i < m.m_width) && (j < m.m_height))
    {
        m.m_values[j*m.m_width+i] = v;
    }
}

