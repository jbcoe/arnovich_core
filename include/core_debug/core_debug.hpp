/*
 * @file core_debug.hpp
 */

#ifndef CORE_DEBUG_HPP_
#define CORE_DEBUG_HPP_

#define CASTOP(op) const_cast<char*>(op)

extern "C" {
#include <core_debug/core_debug.h>
}

inline void  set_core_debug_group_env(const char* env) { return set_core_debug_group_env(const_cast<char*>(env)); }
inline void  set_core_debug_file(const char* file) { return set_core_debug_file(const_cast<char*>(file)); }
inline int is_core_debug_group(const char* group) { return is_core_debug_group(const_cast<char*>(group)); }

#endif /* CORE_DEBUG_HPP_ */
