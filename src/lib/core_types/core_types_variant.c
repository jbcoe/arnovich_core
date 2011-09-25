
/*
 * @file core_types_variant.c
 * @brief generic variant type
 */

#include <core_types/core_types_variant.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

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
    default:
        break;
    }
    return 0;
}

#ifndef VARIANT_USE_MACROS

variant variant_from_double(double d)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_DOUBLE;
    v.m_v.m_d = d;
    return v;
}

variant variant_from_int(int i)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_INT;
    v.m_v.m_i = i;
    return v;
}

int variant_as_int(variant v)
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

double variant_as_double(variant v)
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

int variant_is_int(variant v)
{
    return (CORE_TYPES_VARIANT_INT == v.m_type);
}

int variant_is_double(variant v)
{
    return (CORE_TYPES_VARIANT_DOUBLE == v.m_type);
}

#endif

