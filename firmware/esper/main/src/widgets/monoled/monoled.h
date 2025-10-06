#ifndef MONOLED_H
#define MONOLED_H

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

#define MONOLED_BRIGHT_MIN 0

#define MONOLED_BRIGHT_MAX 255

    /**********************
     *      TYPEDEFS
     **********************/

    LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t monoled_class;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    lv_obj_t *monoled_create(lv_obj_t *parent);

    void monoled_set_brightness(lv_obj_t *led, uint8_t bright);
    void monoled_set_value(lv_obj_t *led, int *value);
    void monoled_on(lv_obj_t *led);
    void monoled_off(lv_obj_t *led);
    void monoled_toggle(lv_obj_t *led);

    uint8_t monoled_get_brightness(const lv_obj_t *obj);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*MONOLED_H*/