
/*
 * @file core_types_variant.hpp
 * @brief generic variant type
 */

#ifndef CORE_TYPES_VARIANT_HPP
#define CORE_TYPES_VARIANT_HPP

#include <string>

namespace CVariant
{

extern "C"
{
    #include <core_types/core_types_variant.h>
}

}

class variant
{
public:
    variant() : m_v(VARIANT_EMPTY)
    {
        m_count = new int;
        *m_count = 1;
    }
    variant(const variant& v)
    {
        m_count = v.m_count;
        m_v = v.m_v;
        ++(*m_count);
    }
    const variant& operator=(const variant& v)
    {
        m_count = v.m_count;
        m_v = v.m_v;
        ++(*m_count);
    }
    ~variant()
    {
        --(*m_count);
        if(!(*m_count))
        {
            delete m_count;
            variant_free(m_v);
        }
    }

    //TODO add error constructor and implicit cast
    variant(int i) : m_v(CVariant::variant_from_int(i))
    {
    }

    variant(double d) : m_v(CVariant::variant_from_double(d))
    {
    }
    
    variant(const std::string& s) : m_v(CVariant::variant_from_string(s.c_str()))
    {
    }
    
    variant(matrix m) : m_v(CVariant::variant_from_matrix(m))
    {
    }

    operator int() const
    {
        return CVariant::variant_as_int(m_v);
    }

    operator double() const
    {
        return CVariant::variant_as_double(m_v);
    }
    
    operator std::string() const
    {
        return CVariant::variant_as_string(m_v);
    }

    operator matrix() const
    {
        return CVariant::variant_as_matrix(m_v);
    }

    bool is_int() const
    {
        return CVariant::variant_is_int(m_v);
    }
    
    bool is_double() const
    {
        return CVariant::variant_is_double(m_v);
    }
    
    bool is_string() const
    {
        return CVariant::variant_is_string(m_v);
    }
    
    bool is_error() const
    {
        return CVariant::variant_is_error(m_v);
    }

    bool is_matrix() const
    {
        return CVariant::variant_is_matrix(m_v);
    }

    operator ==(const variant& v) const
    {
        return variant_equal(m_v, v.m_v);
    }

    std::string to_string() const
    {
        return std::string(variant_to_string(m_v));
    }

private:
    int *m_count;
    CVariant::variant m_v;
};

#endif

