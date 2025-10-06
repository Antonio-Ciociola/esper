#ifndef MONOLED_PRIVATE_H
#define MONOLED_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*********************
     *      INCLUDES
     *********************/

#include "monoled.h"

#include "core/lv_obj_private.h"

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /** Data of led */
    struct _monoled_t
    {
        lv_obj_t obj;
        uint8_t bright;
        int * value;
    };

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*MONOLED_PRIVATE_H*/