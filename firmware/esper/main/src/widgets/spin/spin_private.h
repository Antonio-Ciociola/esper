#ifndef SPIN_PRIVATE_H
#define SPIN_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*********************
     *      INCLUDES
     *********************/

#include "widgets/usr_textarea/usr_textarea_private.h"
#include "spin.h"

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /** Data of spinbox */
    struct _spin_t
    {
        usr_textarea_t ta; /**< Ext. of ancestor */
        /*New data for this type*/
        int32_t *value;
        int32_t range_max;
        int32_t range_min;
        int32_t step;
        int32_t last_value;
        int32_t last_step;
        char * format;
        uint32_t digit_count : 4;
        uint32_t dec_point_pos : 4;  /**< if 0, there is no separator and the number is an integer */
        uint32_t rollover : 1;       /**< Set to true for rollover functionality */
        uint32_t digit_step_dir : 2; /**< the direction the digit will step on encoder button press when editing */
        uint32_t selected_digit : 1; /**< true if a digit is selected for editing */
    };

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    /**********************
     *      MACROS
     **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SPIN_PRIVATE_H*/
