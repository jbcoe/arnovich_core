
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
    #include <core_types/core_types_matrix.h>
}

}

class variant
{
	void release()
	{
		if(m_count)
		{
			--(*m_count);
			if(!(*m_count))
			{
				delete m_count;
				variant_free(m_v);
			}
		}
	}
 
public:
    variant() : m_v(CVariant::VARIANT_EMPTY())
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
				release();
        m_count = v.m_count;
        m_v = v.m_v;
        ++(*m_count);
        return *this;
    }
    ~variant()
    {
			release();
    }

    variant(bool b) : m_v(CVariant::_variant_from_bool((int)b))
    {
        m_count = new int;
        *m_count = 1;
    }

    variant(int i) : m_v(CVariant::_variant_from_int(i))
    {
        m_count = new int;
        *m_count = 1;
    }

    variant(double d) : m_v(CVariant::_variant_from_double(d))
    {
        m_count = new int;
        *m_count = 1;
    }
    
    variant(const std::string& s) : m_v(CVariant::variant_from_string(const_cast<char*>(s.c_str())))
    {
        m_count = new int;
        *m_count = 1;
    }
    
    variant(unsigned int rows, unsigned int cols) : m_v(CVariant::variant_matrix(CVariant::matrix_init(cols, rows)))
    {
        m_count = new int;
        *m_count = 1;
    }

    static variant from_error(const std::string& s)
    {
        return variant(CVariant::variant_from_error(const_cast<char*>(s.c_str())));
    }
private:
		//NOTE: this passes on ownership 
		variant(CVariant::matrix m) : m_v(CVariant::variant_matrix(m))
    {
        m_count = new int;
        *m_count = 1;
    }
public:
		//NOTE: this does not pass on ownership 
    variant(CVariant::variant v) : m_v(CVariant::variant_copy(v))
    {
        m_count = new int;
        *m_count = 1;
    }
public:
    operator bool() const
    {
        return (bool)CVariant::_variant_as_bool(m_v);
    }

    operator int() const
    {
        return CVariant::_variant_as_int(m_v);
    }

    operator double() const
    {
        return CVariant::_variant_as_double(m_v);
    }
    
    operator std::string() const
    {
        return CVariant::variant_as_string(m_v);
    }

    operator CVariant::matrix() const
    {
        return CVariant::variant_as_matrix(m_v);
    }

    bool is_empty() const
    {
        return CVariant::_variant_is_empty(m_v);
    }

    bool is_bool() const
    {
        return CVariant::_variant_is_bool(m_v);
    }

    bool is_int() const
    {
        return CVariant::_variant_is_int(m_v);
    }
    
    bool is_double() const
    {
        return CVariant::_variant_is_double(m_v);
    }
    
    bool is_string() const
    {
        return CVariant::_variant_is_string(m_v);
    }
    
    bool is_error() const
    {
        return CVariant::_variant_is_error(m_v);
    }

    bool is_matrix() const
    {
        return CVariant::_variant_is_matrix(m_v);
    }

    bool operator ==(const variant& v) const
    {
        return CVariant::variant_equal(m_v, v.m_v);
    }

    bool operator <(const variant& v) const
    {
        return CVariant::variant_less(m_v, v.m_v);
    }

    bool operator >(const variant& v) const
    {
        return CVariant::variant_less(v.m_v, m_v);
    }

    bool operator &&(const variant& v) const
    {
        return CVariant::variant_and(m_v, v.m_v);
    }

    bool operator ||(const variant& v) const
    {
        return CVariant::variant_or(m_v, v.m_v);
    }

    variant operator +(const variant& v) const
    {
        CVariant::variant crtn = CVariant::variant_add(m_v, v.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }

    variant operator *(const variant& v) const
    {
        CVariant::variant crtn = CVariant::variant_multiply(m_v, v.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }


    std::string to_string() const
    {
        return std::string(CVariant::variant_to_string_(m_v));
    }

    const variant get(unsigned int row, unsigned int col) const
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            return variant(CVariant::matrix_get(CVariant::variant_as_matrix(m_v), col, row));
        }
        return *this;
    }

    void set(unsigned int row, unsigned int col, const variant& v)
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            CVariant::variant_free(CVariant::matrix_get(CVariant::variant_as_matrix(m_v), col, row));
            CVariant::matrix_set(CVariant::variant_as_matrix(m_v), col, row, CVariant::variant_copy(v.m_v));
        }
    }

    unsigned int nrows() const
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            return m_v.m_v.m_m.m_height;
        }
        return 1;
    }

    unsigned int ncols() const
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            return m_v.m_v.m_m.m_width;
        }
        return 1;
    }

		CVariant::variant to_c() const
		{
				return CVariant::variant_copy(m_v);
		}

private:
    int *m_count;
    CVariant::variant m_v;
};

#endif

