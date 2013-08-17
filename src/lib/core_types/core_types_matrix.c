
/**
 * @file core_types_matrix.c
 * @brief The implementation of the matrix variant
 */

#include <core_types/core_types_matrix.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STRING_LENGTH 1000

matrix matrix_init(unsigned int width, unsigned int height)
{
    matrix m;
    m.m_width = width;
    m.m_height = height;
    m.m_values = (variant*)malloc(sizeof(variant)*width*height);
    memset(m.m_values, 0, sizeof(variant)*width*height);
    return m;
}

matrix matrix_copy(matrix m)
{
    matrix mm = matrix_init(m.m_width,m.m_height);
    memcpy(mm.m_values, m.m_values, sizeof(variant)*m.m_width*m.m_height);
    return mm;
}

void matrix_swap(matrix to, matrix from)
{
    if((to.m_width == from.m_width) && 
       (to.m_height == from.m_height))
    {
        unsigned int i, j;
        for(i=0; i<to.m_width; ++i)
        {
            for(j=0; j<to.m_height; ++j)
            {
                matrix_set(to, i, j, matrix_get(from, i, j)); 
            }
        }
    }
}

void matrix_free(matrix m)
{
    m.m_width = 0;
    m.m_height = 0;
    free(m.m_values);
    m.m_values = NULL;
}

int matrix_compare(matrix a, matrix b)
{
    if((a.m_width == b.m_width) && (a.m_height == b.m_height))
    {
        unsigned int i,j;
        for(i=0;i<a.m_width;++i)
        {
            for(j=0;j<a.m_height;++j)
            {
                if(!(variant_equal(matrix_get(a,i,j), matrix_get(b,i,j))))
                {
                    return 0;
                }
            }
        }
        return 1;
    }
    return 0;
}

matrix matrix_identity(int n)
{
    matrix m = matrix_init(n,n);
    int i;
    for(i = 0; i < n; ++i)
    {
        matrix_set(m, i, i, variant_from_int(1));
    }
    return m;
}

char* matrix_to_string(matrix m)
{
    static char str[10][STRING_LENGTH];
    static int n = 0;
	unsigned int i, j;
    ++n;
    if(n == 10) n = 0;
    strcpy(str[n], "[");
    for(i = 0; i < m.m_height; ++i)
    {
        strcat(str[n], "[");
        for( j = 0; j < m.m_width; ++j)
        {
            strcat(str[n], variant_to_string(matrix_get(m,j,i)));
            strcat(str[n], ",");
        }
        strcat(str[n], "]");
        //if(strlen(str[n]) > STRING_LENGTH) return "ERROR: allocated more...";
    }
    strcat(str[n], "]");
    return str[n];
}

void matrix_move_rows(matrix m, int row1, int row2)
{
    unsigned int i;
    for(i = 0; i < m.m_width; ++i)
    {
        variant tmp = matrix_get(m, i, row1);
        matrix_set(m, i, row1, matrix_get(m, i, row2));
        matrix_set(m, i, row2, tmp);
    }
}

matrix matrix_get_column(matrix m, unsigned int col)
{
    matrix c = matrix_init(1, m.m_height);
    if(col < m.m_width)
    {
        unsigned int i;
        for(i = 0; i<m.m_height; ++i)
        {
            matrix_set(c, 0, i, matrix_get(m, col, i));
        }
    }
    return c;
}

matrix matrix_get_row(matrix m, unsigned int row)
{
    matrix c = matrix_init(m.m_width, 1);
    if(row < m.m_height)
    {
        unsigned int i;
        for(i = 0;  i < m.m_width; ++i)
        {
            matrix_set(c, i, 0, matrix_get(m, i, row));
        }
    }
    return c;
}

void matrix_replace_row(matrix a, int row1, matrix b, int row2)
{
    int i, n;
    n = (a.m_width > b.m_width)?b.m_width:a.m_width;
    for(i=0; i<n; ++i)
    {
        matrix_set(a, i, row1, matrix_get(b, i, row2));
    }
}

variant _matrix_get(matrix m, unsigned int i, unsigned int j)
{
    if((i < m.m_width) && (j < m.m_height))
    {
        return m.m_values[j*m.m_width+i];
    }
    return VARIANT_NIL();
}

void _matrix_set(matrix m, unsigned int i, unsigned int j, variant v)
{
    if((i < m.m_width) && (j < m.m_height))
    {
        m.m_values[j*m.m_width+i] = v;
    }
}

