#include "defines.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

    struct Frame
    {
        float out;
        float aux;
    };

    extern bool enabledac;
    extern int updateFreq;
    extern bool updated;
    extern uint16_t inVal[4];
    extern float inValF[4];
    extern struct Frame outFrames[BLOCK_SIZE];

#ifdef __cplusplus
}
#endif