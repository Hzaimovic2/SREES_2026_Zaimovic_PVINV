#pragma once

#include <compiler/Definitions.h>

#if defined(MU_WINDOWS)
    #if defined(MATRIX_EXPORTS)
        #define MATRIX_API __declspec(dllexport)
    #else
        #define MATRIX_API __declspec(dllimport)
    #endif
#elif defined(MATRIX_EXPORTS)
    #define MATRIX_API __attribute__((visibility("default")))
#else
    #define MATRIX_API
#endif
