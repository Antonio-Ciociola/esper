#ifndef step_PRIVATE_H
#define step_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*********************
     *      INCLUDES
     *********************/

#include "step.h"

#include "core/lv_obj_private.h"

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /** Data of led */
    struct _step_t
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

#endif /*step_PRIVATE_H*/