
/*
 * @file core_types_variant.h
 * @brief generic variant type
 */

typedef enum
{
    CORE_TYPES_VARIANT_INT       = 0,
    CORE_TYPES_VARIANT_DOUBLE    = 1,
    CORE_TYPES_VARIANT_STRING    = 2,
    CORE_TYPES_VARIANT_MATRIX    = 3
} CORE_TYPES_VARIANT_TYPE;


//TODO for starters only int and double
//TODO should also support string and matrices
//TODO maybe also an exception/null object
//TODO then to_python returns a NULL plus sets the string value as an exception
//TODO so in the python layer just do return to_python(result)
//TODO then we can easily generate python wrappers for functions taking only variants as arguments
struct core_types_variant
{
    CORE_TYPES_VARIANT_TYPE m_type;
    union
    {
        int      m_i;
        double   m_d;
    } m_v;
};
typedef struct core_types_variant variant;

/*
 * @brief Returns a string representation of a variant
 * Note: Returned array is a local statically allocated array, so watch out!
 */
char* variant_to_string(variant v);

#define VARIANT_NIL (variant){CORE_TYPES_VARIANT_INT, {.m_i=0} };

#define VARIANT_USE_MACROS

#ifdef VARIANT_USE_MACROS

#define variant_from_int(i) (variant){CORE_TYPES_VARIANT_INT, {.m_i=i}}

#define variant_from_double(d) (variant){CORE_TYPES_VARIANT_DOUBLE, {.m_d=d}}

#define variant_as_int(v) ((CORE_TYPES_VARIANT_INT == v.m_type)?v.m_v.m_i:((CORE_TYPES_VARIANT_DOUBLE == v.m_type)?((int)v.m_v.m_i):0))

#define variant_as_double(v) ((CORE_TYPES_VARIANT_DOUBLE == v.m_type)?v.m_v.m_d:((CORE_TYPES_VARIANT_INT == v.m_type)?((double)v.m_v.m_i):0.0))

#define variant_is_int(v) (CORE_TYPES_VARIANT_INT == v.m_type)

#define variant_is_double(v) (CORE_TYPES_VARIANT_DOUBLE == v.m_type)

#else

variant variant_from_double(double d);

variant variant_from_int(int i);

int variant_as_int(variant v);

double variant_as_double(variant v);

int variant_is_int(variant v);

int variant_is_double(variant v);

#endif

