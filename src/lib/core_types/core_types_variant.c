
/*
 * @file core_types_variant.c
 * @brief generic variant type
 */

#include <core_types/core_types_variant.h>

#include <core_types/core_types_matrix.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void variant_free(variant v)
{
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
    case CORE_TYPES_VARIANT_DOUBLE:
        break;
    case CORE_TYPES_VARIANT_STRING:
    case CORE_TYPES_VARIANT_ERROR:
        free(v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        matrix_free(v.m_v.m_m);
    default:
        break;
    }
}

variant variant_copy(variant v)
{
    variant rtn = v;
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
    case CORE_TYPES_VARIANT_DOUBLE:
        break;
    case CORE_TYPES_VARIANT_STRING:
    case CORE_TYPES_VARIANT_ERROR:
        strcpy(rtn.m_v.m_s.m_s, v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        rtn.m_v.m_m = matrix_copy(v.m_v.m_m);
    default:
        break;
    }
    return rtn;
}

char* variant_to_string(variant v)
{
    static char str[50];
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
        sprintf(str, "%i", v.m_v.m_i);
        break;
    case CORE_TYPES_VARIANT_DOUBLE:
        sprintf(str, "%f", v.m_v.m_d);
        break;
    case CORE_TYPES_VARIANT_STRING:
        sprintf(str, "%s", v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_ERROR:
        sprintf(str, "#ERROR: %s", v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        return matrix_to_string(v.m_v.m_m);
    default:
        strcpy(str, "");
        break;
    }
    return str;
}

int variant_equal(variant a, variant b)
{
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            return (a.m_v.m_i == b.m_v.m_i);
        case CORE_TYPES_VARIANT_DOUBLE:
            return COMPARE_DOUBLE(a.m_v.m_i, b.m_v.m_d);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_DOUBLE:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            return COMPARE_DOUBLE(a.m_v.m_d, b.m_v.m_i);
        case CORE_TYPES_VARIANT_DOUBLE:
            return COMPARE_DOUBLE(a.m_v.m_d, b.m_v.m_d);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_STRING:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_STRING:
            return (strcmp(a.m_v.m_s.m_s, b.m_v.m_s.m_s) == 0);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_ERROR:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_ERROR:
            return (strcmp(a.m_v.m_s.m_s, b.m_v.m_s.m_s) == 0);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        switch(b.m_type)
        {
            case CORE_TYPES_VARIANT_MATRIX:
                return matrix_compare(a.m_v.m_m, b.m_v.m_m);
            default:
                break;
        }
        break;
    default:
        break;
    }
    return 0;
}

variant _variant_from_double(double d)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_DOUBLE;
    v.m_v.m_d = d;
    return v;
}

variant _variant_from_int(int i)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_INT;
    v.m_v.m_i = i;
    return v;
}

variant variant_from_string(char *c)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_STRING;
    v.m_v.m_s.m_l = strlen(c);
    v.m_v.m_s.m_s = (char*)malloc(sizeof(char)*v.m_v.m_s.m_l); 
    strcpy(v.m_v.m_s.m_s, c);
    return v;
}

variant variant_from_error(char *c)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_ERROR;
    v.m_v.m_s.m_l = strlen(c);
    v.m_v.m_s.m_s = (char*)malloc(sizeof(char)*v.m_v.m_s.m_l); 
    strcpy(v.m_v.m_s.m_s, c);
    return v;
}

variant variant_from_matrix(matrix m)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_MATRIX;
    v.m_v.m_m = matrix_copy(m);
    return v;
}

variant variant_matrix(matrix m)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_MATRIX;
    v.m_v.m_m = m;
    return v;
}

char* variant_as_string(variant v)
{
    if(CORE_TYPES_VARIANT_STRING == v.m_type)
    {
        return v.m_v.m_s.m_s;
    }
    return "";
}

char* variant_as_error(variant v)
{
    if(CORE_TYPES_VARIANT_ERROR == v.m_type)
    {
        return v.m_v.m_s.m_s;
        //return variant_to_string(v);
    }
    return "";
}

matrix variant_as_matrix(variant v)
{
    if(CORE_TYPES_VARIANT_MATRIX == v.m_type)
    {
        return v.m_v.m_m;
    }
    return MATRIX_IDENTITY(0);
}

int _variant_as_int(variant v)
{
    if(CORE_TYPES_VARIANT_INT == v.m_type)
    {
        return v.m_v.m_i;
    }
    if(CORE_TYPES_VARIANT_DOUBLE == v.m_type)
    {
        return (int)v.m_v.m_d;
    }
    return 0;
}

double _variant_as_double(variant v)
{
    if(CORE_TYPES_VARIANT_DOUBLE == v.m_type)
    {
        return v.m_v.m_d;
    }
    if(CORE_TYPES_VARIANT_INT == v.m_type)
    {
        return (double)v.m_v.m_i;
    }
    return 0.0;
}

variant _variant_empty()
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_EMPTY;
    v.m_v.m_i = 0;
    return v;
}

variant _variant_nil()
{
    return _variant_empty();
}

int _variant_is_empty(variant v)
{
    return (CORE_TYPES_VARIANT_EMPTY == v.m_type);
}

int _variant_is_int(variant v)
{
    return (CORE_TYPES_VARIANT_INT == v.m_type);
}

int _variant_is_double(variant v)
{
    return (CORE_TYPES_VARIANT_DOUBLE == v.m_type);
}

int _variant_is_string(variant v)
{
    return (CORE_TYPES_VARIANT_STRING == v.m_type);
}

int _variant_is_error(variant v)
{
    return (CORE_TYPES_VARIANT_ERROR == v.m_type);
}

int _variant_is_matrix(variant v)
{
    return (CORE_TYPES_VARIANT_MATRIX == v.m_type);
}

