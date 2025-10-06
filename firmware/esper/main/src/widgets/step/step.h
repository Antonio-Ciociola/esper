#ifndef step_H
#define step_H

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

    LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t step_class;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    void step_set_current(lv_obj_t *obj);
    void step_unset_current(lv_obj_t *obj);
    uint8_t step_get_selected(const lv_obj_t *obj);

    lv_obj_t *step_create(lv_obj_t *parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*step_H*/