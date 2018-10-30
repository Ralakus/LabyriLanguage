#pragma once

#if defined(__clang__)
    #define LAB_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define LAB_COMPILER_GCC
#elif defined(_MSC_VER)
    #define LAB_COMPILER_MSVC
#else
    #define LAB_COMPILER_OTHER
#endif

#ifdef LAB_COMPILER_MSVC
#define LAB_FORCEINLINE __forceinline
#define LAB_FORCENOINLINE __declspec(noinline)
#elif defined(LAB_COMPILER_GCC) || defined(LAB_COMPILER_CLANG)
#define LAB_FORCEINLINE inline __attribute__ ((always_inline))
#define LAB_FORCENOINLINE __attribute__(( noinline ))
#else
#define LAB_FORCEINLINE inline
#define LAB_FORCENOINLINE
#endif