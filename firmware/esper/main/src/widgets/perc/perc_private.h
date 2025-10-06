#ifndef PERC_PRIVATE_H
#define PERC_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*********************
     *      INCLUDES
     *********************/

#include "perc.h"

#include "core/lv_obj_private.h"

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /** Data of led */
    struct _perc_t
    {
        lv_obj_t obj;
        int *value;
        char *format;
        int min;
        int max;
        char text_buf[16]; // Persistent buffer for label
    };

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*perc_PRIVATE_H*/