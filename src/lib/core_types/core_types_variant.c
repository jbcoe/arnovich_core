
/**
 * @file core_types_variant.c
 * @brief The implementation of the C-style variant
 */

#include <core_types/core_types_variant.h>

#include <core_types/core_types_matrix.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define STRING_LENGTH 1000

void variant_free(variant v)
{
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_BOOL:
    case CORE_TYPES_VARIANT_INT:
    case CORE_TYPES_VARIANT_DOUBLE:
        break;
    case CORE_TYPES_VARIANT_STRING:
    case CORE_TYPES_VARIANT_ERROR:
        free(v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        matrix_free(v.m_v.m_m);
        break;
    case CORE_TYPES_VARIANT_FUNCTION:
        //free(v.m_v.m_f.m_a);
        break;
    default:
        break;
    }
}

variant variant_copy(variant v)
{
    variant rtn = v;
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_BOOL:
    case CORE_TYPES_VARIANT_INT:
    case CORE_TYPES_VARIANT_DOUBLE:
    case CORE_TYPES_VARIANT_FUNCTION:
        break;
    case CORE_TYPES_VARIANT_STRING:
    case CORE_TYPES_VARIANT_ERROR:
		rtn.m_v.m_s.m_s = (char*)malloc(sizeof(char)*(rtn.m_v.m_s.m_l+1));
        strcpy(rtn.m_v.m_s.m_s, v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        rtn.m_v.m_m = matrix_copy(v.m_v.m_m);
        break;
    default:
        break;
    }
    return rtn;
}

char* variant_to_string(variant v)
{
    static char str[STRING_LENGTH];
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_BOOL:
        if(v.m_v.m_i)
        {
            sprintf(str, "TRUE");
        } else {
            sprintf(str, "FALSE");
        }
        break;
    case CORE_TYPES_VARIANT_INT:
        sprintf(str, "%i", v.m_v.m_i);
        break;
    case CORE_TYPES_VARIANT_DOUBLE:
        sprintf(str, "%f", v.m_v.m_d);
        break;
    case CORE_TYPES_VARIANT_STRING:
        if(STRING_LENGTH < v.m_v.m_s.m_l) return "ERROR: allocated more...";
        sprintf(str, "%s", v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_ERROR:
        if(STRING_LENGTH < v.m_v.m_s.m_l) return "ERROR: allocated more...";
        sprintf(str, "#ERROR: %s", v.m_v.m_s.m_s);
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        return matrix_to_string(v.m_v.m_m);
    case CORE_TYPES_VARIANT_FUNCTION:
        sprintf(str, "Function: %i params", v.m_v.m_f.m_n);
        break;
    default:
        strcpy(str, "");
        break;
    }
    return str;
}

char* variant_to_string_(variant v)
{
    switch(v.m_type)
    {
    case CORE_TYPES_VARIANT_STRING:
        return v.m_v.m_s.m_s;
    case CORE_TYPES_VARIANT_BOOL:
    case CORE_TYPES_VARIANT_INT:
    case CORE_TYPES_VARIANT_DOUBLE:
    case CORE_TYPES_VARIANT_MATRIX:
    case CORE_TYPES_VARIANT_FUNCTION:
    default:
        break;
    }
    return variant_to_string(v);
}


int variant_equal(variant a, variant b)
{
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_BOOL:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_BOOL:
            return (a.m_v.m_i == b.m_v.m_i);
        default:
            break;
        }
        break;
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
    case CORE_TYPES_VARIANT_FUNCTION:
        switch(b.m_type)
        {
            case CORE_TYPES_VARIANT_FUNCTION:
                return (a.m_v.m_f.m_n == b.m_v.m_f.m_n) && (a.m_v.m_f.m_f == b.m_v.m_f.m_f);
            default:
                break;
        }
        break;
    default:
        break;
    }
    return 0;
}

int variant_less(variant a, variant b)
{
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            return (a.m_v.m_i < b.m_v.m_i);
        case CORE_TYPES_VARIANT_DOUBLE:
            return (a.m_v.m_i < (double)b.m_v.m_d);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_DOUBLE:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            return (a.m_v.m_d < (double)b.m_v.m_i);
        case CORE_TYPES_VARIANT_DOUBLE:
            return (a.m_v.m_d < b.m_v.m_d);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_STRING:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_STRING:
            return (strcmp(a.m_v.m_s.m_s, b.m_v.m_s.m_s) < 0);
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_BOOL:
    case CORE_TYPES_VARIANT_ERROR:
    case CORE_TYPES_VARIANT_FUNCTION:
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        //TODO
        break;
    default:
        break;
    }
    return 0;
}

int variant_and(variant a, variant b)
{
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_BOOL:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_BOOL:
            return (a.m_v.m_i && b.m_v.m_i);
        default:
            break;
        }
        break;
        default:
            break;
    }
    return 0;
}
int variant_or(variant a, variant b)
{
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_BOOL:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_BOOL:
            return (a.m_v.m_i || b.m_v.m_i);
        default:
            break;
        }
        break;
        default:
            break;
    }
    return 0;
}
variant variant_add(variant a, variant b)
{
    variant rtn = VARIANT_EMPTY;
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            rtn.m_type = CORE_TYPES_VARIANT_INT;
            rtn.m_v.m_i = (a.m_v.m_i + b.m_v.m_i);
            break;
        case CORE_TYPES_VARIANT_DOUBLE:
            rtn.m_type = CORE_TYPES_VARIANT_DOUBLE;
            rtn.m_v.m_d = ((double)a.m_v.m_i + b.m_v.m_d);
            break;
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_DOUBLE:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            rtn.m_type = CORE_TYPES_VARIANT_DOUBLE;
            rtn.m_v.m_d = (a.m_v.m_d + (double)b.m_v.m_i);
            break;
        case CORE_TYPES_VARIANT_DOUBLE:
            rtn.m_type = CORE_TYPES_VARIANT_DOUBLE;
            rtn.m_v.m_d = (a.m_v.m_d + b.m_v.m_d);
            break;
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_STRING:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_STRING:
            rtn.m_type = CORE_TYPES_VARIANT_STRING;
            rtn.m_v.m_s.m_l = a.m_v.m_s.m_l+b.m_v.m_s.m_l;
            rtn.m_v.m_s.m_s = (char*)malloc(sizeof(char)*(rtn.m_v.m_s.m_l+1));
            strcpy(rtn.m_v.m_s.m_s, a.m_v.m_s.m_s);
            strcat(rtn.m_v.m_s.m_s, b.m_v.m_s.m_s);
            break;
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_BOOL:
    case CORE_TYPES_VARIANT_ERROR:
    case CORE_TYPES_VARIANT_FUNCTION:
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        //TODO
        break;
    default:
        break;
    }
    return rtn;
}

variant variant_multiply(variant a, variant b)
{
    variant rtn = VARIANT_EMPTY;
    switch(a.m_type)
    {
    case CORE_TYPES_VARIANT_INT:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            rtn.m_type = CORE_TYPES_VARIANT_INT;
            rtn.m_v.m_i = (a.m_v.m_i * b.m_v.m_i);
            break;
        case CORE_TYPES_VARIANT_DOUBLE:
            rtn.m_type = CORE_TYPES_VARIANT_DOUBLE;
            rtn.m_v.m_d = ((double)a.m_v.m_i * b.m_v.m_d);
            break;
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_DOUBLE:
        switch(b.m_type)
        {
        case CORE_TYPES_VARIANT_INT:
            rtn.m_type = CORE_TYPES_VARIANT_DOUBLE;
            rtn.m_v.m_d = (a.m_v.m_d * (double)b.m_v.m_i);
            break;
        case CORE_TYPES_VARIANT_DOUBLE:
            rtn.m_type = CORE_TYPES_VARIANT_DOUBLE;
            rtn.m_v.m_d = (a.m_v.m_d * b.m_v.m_d);
            break;
        default:
            break;
        }
        break;
    case CORE_TYPES_VARIANT_STRING:
    case CORE_TYPES_VARIANT_BOOL:
    case CORE_TYPES_VARIANT_ERROR:
    case CORE_TYPES_VARIANT_FUNCTION:
        break;
    case CORE_TYPES_VARIANT_MATRIX:
        //TODO
        break;
    default:
        break;
    }
    return rtn;
}

typedef variant (*variant_function0)();
typedef variant (*variant_function_object0)(void*);
typedef variant (*variant_function1)(variant);
typedef variant (*variant_function_object1)(void*, variant);
typedef variant (*variant_function2)(variant, variant);
typedef variant (*variant_function_object2)(void*, variant, variant);
typedef variant (*variant_function3)(variant, variant, variant);
typedef variant (*variant_function_object3)(void*, variant, variant, variant);
typedef variant (*variant_function4)(variant, variant, variant, variant);
typedef variant (*variant_function_object4)(void*, variant, variant, variant, variant);
typedef variant (*variant_function5)(variant, variant, variant, variant, variant);
typedef variant (*variant_function_object5)(void*, variant, variant, variant, variant, variant);

static variant _variant_call(variant f, va_list vl)
{
    if(!variant_is_function(f))
    {
        return variant_from_error("cannot call on a non-function variant");
    }
 
    variant rtn;
    switch(f.m_v.m_f.m_n)
    {
        case 0:
        {
            if(f.m_v.m_f.m_a)
            {
                rtn = ((variant_function_object0)f.m_v.m_f.m_f)(f.m_v.m_f.m_a);
            } else {
                rtn = ((variant_function0)f.m_v.m_f.m_f)();
            }
            break;
        }
        case 1:
        {
            variant v1 = va_arg(vl, variant);
            if(f.m_v.m_f.m_a)
            {
                rtn = ((variant_function_object1)f.m_v.m_f.m_f)(f.m_v.m_f.m_a, v1);
            } else {
                rtn = ((variant_function1)f.m_v.m_f.m_f)(v1);
            }
            break;
        }
        case 2:
        {
            variant v1 = va_arg(vl, variant);
            variant v2 = va_arg(vl, variant);
            if(f.m_v.m_f.m_a)
            {
                rtn = ((variant_function_object2)f.m_v.m_f.m_f)(f.m_v.m_f.m_a, v1, v2);
            } else {
                rtn = ((variant_function2)f.m_v.m_f.m_f)(v1, v2);
            }
            break;
        }
        case 3:
        {
            variant v1 = va_arg(vl, variant);
            variant v2 = va_arg(vl, variant);
            variant v3 = va_arg(vl, variant);
            if(f.m_v.m_f.m_a)
            {
                rtn = ((variant_function_object3)f.m_v.m_f.m_f)(f.m_v.m_f.m_a, v1, v2, v3);
            } else {
                rtn = ((variant_function3)f.m_v.m_f.m_f)(v1, v2, v3);
            }
            break;
        }
        case 4:
        {
            variant v1 = va_arg(vl, variant);
            variant v2 = va_arg(vl, variant);
            variant v3 = va_arg(vl, variant);
            variant v4 = va_arg(vl, variant);
            if(f.m_v.m_f.m_a)
            {
                rtn = ((variant_function_object4)f.m_v.m_f.m_f)(f.m_v.m_f.m_a, v1, v2, v3, v4);
            } else {
                rtn = ((variant_function4)f.m_v.m_f.m_f)(v1, v2, v3, v4);
            }
            break;
        }
        case 5:
        {
            variant v1 = va_arg(vl, variant);
            variant v2 = va_arg(vl, variant);
            variant v3 = va_arg(vl, variant);
            variant v4 = va_arg(vl, variant);
            variant v5 = va_arg(vl, variant);
            if(f.m_v.m_f.m_a)
            {
                rtn = ((variant_function_object5)f.m_v.m_f.m_f)(f.m_v.m_f.m_a, v1, v2, v3, v4, v5);
            } else {
                rtn = ((variant_function5)f.m_v.m_f.m_f)(v1, v2, v3, v4, v5);
            }
            break;
        }
        default:
        {
            return variant_from_error("un-supported number of arguments");
        }
    }
    return rtn;
}

variant variant_call(variant f, ...)
{
    va_list vl;
    va_start(vl, f);
    variant rtn;
    rtn = _variant_call(f, vl);
    va_end(vl);
    return rtn;
}

int variant_params(variant f)
{
    return f.m_v.m_f.m_n;
}

variant _variant_from_bool(int b)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_BOOL;
    v.m_v.m_i = b;
    return v;
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

variant _variant_from_function(void* f, void* a, int n)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_FUNCTION;
    v.m_v.m_f.m_f = f;
    v.m_v.m_f.m_a = a;
    v.m_v.m_f.m_n = n;
    return v;
}

variant variant_from_string(char *c)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_STRING;
    v.m_v.m_s.m_l = strlen(c);
    v.m_v.m_s.m_s = (char*)malloc(sizeof(char)*(v.m_v.m_s.m_l+1));
    strcpy(v.m_v.m_s.m_s, c);
    return v;
}

variant variant_from_error(char *c)
{
    variant v;
    v.m_type = CORE_TYPES_VARIANT_ERROR;
    v.m_v.m_s.m_l = strlen(c);
    v.m_v.m_s.m_s = (char*)malloc(sizeof(char)*(v.m_v.m_s.m_l+1));
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

int _variant_as_bool(variant v)
{
    if(CORE_TYPES_VARIANT_BOOL == v.m_type)
    {
        return v.m_v.m_i;
    }
		if(CORE_TYPES_VARIANT_INT == v.m_type)
    {
        return v.m_v.m_i;
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

int _variant_is_bool(variant v)
{
    return (CORE_TYPES_VARIANT_BOOL == v.m_type);
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

int _variant_is_function(variant v)
{
    return (CORE_TYPES_VARIANT_FUNCTION == v.m_type);
}
