
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

static variant VARIANT_NIL = {CORE_TYPES_VARIANT_INT, {.m_i=0} };

variant core_types_variant_double(double d);

variant core_types_variant_int(int i);

int core_types_int(variant v);

double core_types_double(variant v);

int core_types_variant_is_int(variant v);

int core_types_variant_is_double(variant v);

