#ifndef DIGIT_PRIVATE_H
#define DIGIT_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*********************
     *      INCLUDES
     *********************/

#include "digit.h"

#include "core/lv_obj_private.h"

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /** Data of led */
    struct _digit_t
    {
        lv_obj_t obj;
        uint8_t sel;
        uint8_t length;
        bool current;
        int num_notes;

    };

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*DIGIT_PRIVATE_H*/