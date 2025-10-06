#ifndef PERC_H
#define PERC_H

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

    LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t perc_class;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    void perc_set_value(lv_obj_t *obj, const int *value);
    void perc_set_min_max(lv_obj_t *obj, int min, int max);
    void perc_set_format(lv_obj_t *obj, const char *format);
    int perc_get_value(lv_obj_t *obj);

    lv_obj_t *perc_create(lv_obj_t *parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*perc_H*/