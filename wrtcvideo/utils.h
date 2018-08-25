#ifndef _UTILS
#define _UTILS

#ifndef assert
#ifndef WIN32
#include <assert.h>
#else
#ifndef NDEBUG
#define assert(expr)  ((void)((expr) ? true : __debugbreak()))
#else
#define assert(expr)  ((void)0)
#endif  // NDEBUG
#endif  // WIN32
#endif  // assert

#include <string>

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

std::string int2str(int i);
std::string size_t2str(size_t i);

#endif
