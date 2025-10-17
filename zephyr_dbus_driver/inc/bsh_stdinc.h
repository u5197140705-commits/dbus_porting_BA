#ifndef BSH_STDINC_H
#define BSH_STDINC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LibDefines.h"
#include LD_STRINGIZE(VARIANT.h) // include the header for current variant

// Placeholder for other standard includes or definitions from original bsh_stdinc.h
// For example, if it defines common integer types:
// #include <stdint.h>
// #include <stdbool.h>

#ifdef __cplusplus
}
#endif

#endif // BSH_STDINC_H