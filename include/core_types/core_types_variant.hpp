/**
 * @file core_types_variant.hpp
 * @brief C++ interface for the variant type.
 * This is really only a memory manager/smart pointer of the C-version.
 * The idea is to make it easier in C++ to work with the variant type and be able to easily share it with C-interfaces.
 */

#ifndef CORE_TYPES_VARIANT_HPP
#define CORE_TYPES_VARIANT_HPP

#include <string>
#include <stdarg.h>

namespace CVariant
{

extern "C"
{
    #include <core_types/core_types_variant.h>
    #include <core_types/core_types_matrix.h>
}

}

/**
  * @brief The variant type wrapper-class
  */
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
    variant(const variant& v)
    {
        m_count = v.m_count;
        m_v = v.m_v;
        ++(*m_count);
    }
    const variant& operator=(const variant& v)
    {
    	//Self-assignment protection
    	if ( &v == this ) return *this;
    	
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
    
    /** @name Constructors
      * Construction of \c variant from other types.
      */
    ///@{
    /**
     * @brief Create an EMPTY variant
     */
    variant() : m_v(CVariant::VARIANT_EMPTY())
    {
        m_count = new int;
        *m_count = 1;
    }
    /**
     * @brief Create a \c variant from a \c bool
     */
    variant(bool b) : m_v(CVariant::_variant_from_bool((int)b))
    {
        m_count = new int;
        *m_count = 1;
    }
    /**
     * @brief Create a \c variant from an \c int
     */
    variant(int i) : m_v(CVariant::_variant_from_int(i))
    {
        m_count = new int;
        *m_count = 1;
    }
    /**
     * @brief Create a \c variant from a \c double
     */
    variant(double d) : m_v(CVariant::_variant_from_double(d))
    {
        m_count = new int;
        *m_count = 1;
    }
    /**
     * @brief Create a \c variant from a \c std::string
     */
    variant(const std::string& s) : m_v(CVariant::variant_from_string(const_cast<char*>(s.c_str())))
    {
        m_count = new int;
        *m_count = 1;
    }
    /**
     * @brief Create a matrix \c variant of size \c rows * \c cols
     */
    variant(unsigned int rows, unsigned int cols) : m_v(CVariant::variant_matrix(CVariant::matrix_init(cols, rows)))
    {
        m_count = new int;
        *m_count = 1;
    }
    /**
     * @brief Create a state-less function \c variant
     */
    variant(void* f, int n) : m_v(CVariant::_variant_from_function(f, 0, n))
    {
        m_count = new int;
        *m_count = 1;        
    }
    /**
     * @brief Create a function \c variant with a state
     */
    variant(void* f, void* a, int n) : m_v(CVariant::_variant_from_function(f, a, n))
    {
        m_count = new int;
        *m_count = 1;        
    }
    /**
     * @brief Create an error \c variant from a \c std::string
     */
    static variant from_error(const std::string& s)
    {
        return variant(CVariant::variant_from_error(const_cast<char*>(s.c_str())));
    }
    ///@}
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
    /** @name Casts
      * Cast \c variant to other types.
      */
    ///@{
    /**
      * @brief Cast to a \c bool. Returns \c false if not a bool type.
      */
    operator bool() const
    {
        return (CVariant::_variant_as_bool(m_v) != 0);
    }
    /**
      * @brief Cast to an \c int. Returns 0 if not a int type.
      */
    operator int() const
    {
        return CVariant::_variant_as_int(m_v);
    }
    /**
      * @brief Cast to a \c double. Returns 0.0 if not a double type.
      */
    operator double() const
    {
        return CVariant::_variant_as_double(m_v);
    }
    /**
      * @brief Cast to a \c std::string. Returns an empty string if not a string type.
      */  
    operator std::string() const
    {
        return CVariant::variant_as_string(m_v);
    }
    /**
      * @brief Cast to a \c matrix. Returns an empty if not a matrix type.
      */
    operator CVariant::matrix() const
    {
        return CVariant::variant_as_matrix(m_v);
    }
    ///@}

    /** @name Function callers
      * Call/execute a \c variant of type function.
      */
    ///@{
    /**
      * @brief Call function with no arguments.
      */
    variant call() const
    {
        if(CVariant::variant_params(m_v) != 0)
            return variant::from_error("wrong number of arguments");
        CVariant::variant crtn = CVariant::variant_call(m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    /**
      * @brief Call function with one arguments.
      */
    variant call(variant v1) const
    {
        if(CVariant::variant_params(m_v) != 1)
            return variant::from_error("wrong number of arguments");
        CVariant::variant crtn = CVariant::variant_call(m_v, v1.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    /**
      * @brief Call function with two arguments.
      */
    variant call(variant v1, variant v2) const
    {
        if(CVariant::variant_params(m_v) != 2)
            return variant::from_error("wrong number of arguments");
        CVariant::variant crtn = CVariant::variant_call(m_v, v1.m_v, v2.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    /**
      * @brief Call function with three arguments.
      */
    variant call(variant v1, variant v2, variant v3) const
    {
        if(CVariant::variant_params(m_v) != 3)
            return variant::from_error("wrong number of arguments");
        CVariant::variant crtn = CVariant::variant_call(m_v, v1.m_v, v2.m_v, v3.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    /**
      * @brief Call function with four arguments.
      */
    variant call(variant v1, variant v2, variant v3, variant v4) const
    {
        if(CVariant::variant_params(m_v) != 4)
            return variant::from_error("wrong number of arguments");
        CVariant::variant crtn = CVariant::variant_call(m_v, v1.m_v, v2.m_v, v3.m_v, v4.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    /**
      * @brief Call function with five arguments.
      */
    variant call(variant v1, variant v2, variant v3, variant v4, variant v5) const
    {
        if(CVariant::variant_params(m_v) != 5)
            return variant::from_error("wrong number of arguments");
        CVariant::variant crtn = CVariant::variant_call(m_v, v1.m_v, v2.m_v, v3.m_v, v4.m_v, v5.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    ///@}

    /** @name Type tests
      * Test the type of a \c variant
      */
    ///@{
    /**
      * @brief Test if \c variant is empty.
      */
    bool is_empty() const
    {
        return (CVariant::_variant_is_empty(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is a bool type.
      */
    bool is_bool() const
    {
        return  (CVariant::_variant_is_bool(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is an int type.
      */
    bool is_int() const
    {
        return (CVariant::_variant_is_int(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is a double type.
      */  
    bool is_double() const
    {
        return (CVariant::_variant_is_double(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is a string type.
      */
    bool is_string() const
    {
        return (CVariant::_variant_is_string(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is an error type.
      */
    bool is_error() const
    {
        return (CVariant::_variant_is_error(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is a matrix type.
      */
    bool is_matrix() const
    {
        return (CVariant::_variant_is_matrix(m_v) != 0);
    }
    /**
      * @brief Test if \c variant is a function type.
      */
    bool is_function() const
    {
        return (CVariant::_variant_is_function(m_v) != 0);
    }
    ///@}

    /** @name Operators
      * Operators on \c variant
      */
    ///@{
    /**
      * @brief Test equality of \c variant types.
      */
    bool operator ==(const variant& v) const
    {
        return (CVariant::variant_equal(m_v, v.m_v) != 0);
    }
    /**
      * @brief Test whether \c this is less than \c v. 
      * If the types of both are \c int or \c double, numeric comparison is used, if they are both \c string standard string comparison is used, otherwise false is returned.
      */
    bool operator <(const variant& v) const
    {
        return (CVariant::variant_less(m_v, v.m_v) != 0);
    }
    /**
      * @brief Test whether \c this is greater than \c v. 
      * If the types of both are \c int or \c double, numeric comparison is used, if they are both \c string standard string comparison is used, otherwise false is returned.
      */
    bool operator >(const variant& v) const
    {
        return (CVariant::variant_less(v.m_v, m_v) != 0);
    }
    /**
      * @brief Use the \c AND operator on variants. If either variant is not of bool type then false is returned.
      */
    bool operator &&(const variant& v) const
    {
        return (CVariant::variant_and(m_v, v.m_v) != 0);
    }
    /**
      * @brief Use the \c OR operator on variants. If either variant is not of bool type then false is returned.
      */
    bool operator ||(const variant& v) const
    {
        return (CVariant::variant_or(m_v, v.m_v) != 0);
    }
    /**
      * @brief Add \c v to \c this. 
      * If the types of both are \c int or \c double, numeric addition is used, if they are both \c string concatenation is used, otherwise nothing happens.
      */
    variant operator +(const variant& v) const
    {
        CVariant::variant crtn = CVariant::variant_add(m_v, v.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    /**
      * @brief Multiply \c this with \c v. 
      * If the types of both are \c int or \c double, numeric multiplication is used, otherwise nothing happens.
      */
    variant operator *(const variant& v) const
    {
        CVariant::variant crtn = CVariant::variant_multiply(m_v, v.m_v);
        variant rtn(crtn);
        CVariant::variant_free(crtn);
        return rtn;
    }
    ///@}

    /** @name Matrix Operations
      * Operators on \c variant
      */
    ///@{
    /**
      * @brief Get the variant at (\c row , \c col ). If the variant is not a matrix it returns itself.
      */
    const variant get(unsigned int row, unsigned int col) const
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            return variant(CVariant::matrix_get(CVariant::variant_as_matrix(m_v), col, row));
        }
        return *this;
    }
    /**
      * @brief Set the variant at (\c row , \c col ). If the variant is not a matrix nothing happens.
      */
    void set(unsigned int row, unsigned int col, const variant& v)
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            CVariant::variant_free(CVariant::matrix_get(CVariant::variant_as_matrix(m_v), col, row));
            CVariant::matrix_set(CVariant::variant_as_matrix(m_v), col, row, CVariant::variant_copy(v.m_v));
        }
    }
    /**
      * @brief Get the number of rows. If the variant is not a matrix it returns 1.
      */
    unsigned int nrows() const
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            return m_v.m_v.m_m.m_height;
        }
        return 1;
    }
    /**
      * @brief Get the number of columns. If the variant is not a matrix it returns 1.
      */
    unsigned int ncols() const
    {
        if(CVariant::_variant_is_matrix(m_v))
        {
            return m_v.m_v.m_m.m_width;
        }
        return 1;
    }
    ///@}

    /** @name Other functions
      * A few other functions.
      */
    ///@{
    /**
      * @brief Returns a copy of the \c variant as a C-style \c variant.
      */
    CVariant::variant to_c() const
    {
        return CVariant::variant_copy(m_v);
    }
    /**
      * @brief Return a string representation of the \c variant.
      */
    std::string to_string() const
    {
        return std::string(CVariant::variant_to_string_(m_v));
    }
    ///@}

private:
    int *m_count;
    CVariant::variant m_v;
};

#endif

