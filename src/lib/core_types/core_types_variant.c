
/*
 * @file core_types_variant.c
 * @brief generic variant type
 */

#include <core_types/core_types_variant.h>

variant core_types_variant_double(double d)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_DOUBLE;
    v.m_v.m_d = d;
    return v;
}

variant core_types_variant_int(int i)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_INT;
    v.m_v.m_i = i;
    return v;
}

int core_types_int(variant v)
{
    if(CORE_TYPES_VARIANT_INT == v.m_type)
    {
        return v.m_v.m_i;
    }
    return 0;
}

double core_types_double(variant v)
{
    if(CORE_TYPES_VARIANT_DOUBLE == v.m_type)
    {
        return v.m_v.m_d;
    }
    return 0.0;
}

int core_types_variant_is_int(variant v)
{
    return (CORE_TYPES_VARIANT_INT == v.m_type);
}

int core_types_variant_is_double(variant v)
{
    return (CORE_TYPES_VARIANT_DOUBLE == v.m_type);
}


