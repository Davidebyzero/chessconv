#include <stdio.h>
#include <malloc.h>

typedef   signed char            int8;
typedef unsigned char           Uint8;
typedef   signed short int       int16;
typedef unsigned short int      Uint16;
typedef   signed long int        int32;
typedef unsigned long int       Uint32;
typedef   signed long long int 	 int64;
typedef unsigned long long int  Uint64;

typedef unsigned char Uchar;
typedef unsigned int  Uint;

typedef Uint8 bool8;

#define inrange(n,a,b) ((Uint)((n)-(a))<=(Uint)((b)-(a)))
#define inrangex(n,a,b) ((Uint)((n)-(a))<(Uint)((b)-(a)))

#define inrange64(n,a,b) ((Uint64)((n)-(a))<=(Uint64)((b)-(a)))
#define inrangex64(n,a,b) ((Uint64)((n)-(a))<(Uint64)((b)-(a)))

#define MAX_EXTEND(n) ((Uint64)((n)+1)-1)

template <size_t size>
char (*__strlength_helper(char const (&_String)[size]))[size];
#define strlength(_String) (sizeof(*__strlength_helper(_String))-1)

#ifdef __has_builtin
#define HAS_BUILTIN(x) __has_builtin(x)
#else
#define HAS_BUILTIN(x) 0
#endif

#ifdef _MSC_VER
#define UNREACHABLE_CODE __assume(0)
#elif defined(__GNUC__) || HAS_BUILTIN(__builtin_unreachable)
#define UNREACHABLE_CODE __builtin_unreachable()
#else
#endif

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#elif __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define FLEXIBLE_SIZE_ARRAY 1
#else
#define FLEXIBLE_SIZE_ARRAY
#endif

template <typename TYPE>
inline void tellCompilerVariableIsntUninitialized(TYPE &n)
{
}
