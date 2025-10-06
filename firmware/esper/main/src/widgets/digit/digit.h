#ifndef DIGIT_H
#define DIGIT_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*********************
     *      INCLUDES
     *********************/

#include "core/lv_obj.h"

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t digit_class;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    void digit_set_current(lv_obj_t *obj);
    void digit_unset_current(lv_obj_t *obj);
    uint8_t digit_get_selected(const lv_obj_t *obj);

    lv_obj_t *digit_create(lv_obj_t *parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*DIGIT_H*/