/*********************
 *      INCLUDES
 *********************/
#include "perc_private.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"

#include "misc/lv_assert.h"
#include "themes/lv_theme.h"
#include "misc/lv_color.h"
#include "esp_log.h"

#include <stdio.h>
#include <string.h>

#include "utils/utils.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&perc_class)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _perc_t perc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void perc_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void perc_event(const lv_obj_class_t *class_p, lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t perc_class = {
    .name = "perc",
    .base_class = &lv_obj_class,
    .constructor_cb = perc_constructor,
    .event_cb = perc_event,
    .instance_size = sizeof(perc_t),
    .width_def = 32,
    .height_def = 12,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
};

static const char *TAG = "perc";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *perc_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void perc_set_min_max(lv_obj_t *obj, int min, int max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    perc_t *perc = (perc_t *)obj;
    perc->min = min;
    perc->max = max;
    lv_obj_invalidate(obj);
}

void perc_set_value(lv_obj_t *obj, const int *value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    perc_t *perc = (perc_t *)obj;
    perc->value = value;
    lv_obj_invalidate(obj);
}

void perc_set_format(lv_obj_t *obj, const char *format)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    perc_t *perc = (perc_t *)obj;
    perc->format = format;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

int perc_get_value(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    perc_t *perc = (perc_t *)obj;
    int value = *perc->value;
    lv_obj_invalidate(obj);
    return value;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void perc_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    perc_t *perc = (perc_t *)obj;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    // lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    style_hover(obj);
}

static void perc_event(const lv_obj_class_t *class_p, lv_event_t *e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(MY_CLASS, e);
    if (res != LV_RESULT_OK)
        return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);

    LV_ASSERT_OBJ(obj, MY_CLASS);
    perc_t *perc = (perc_t *)obj;

    // ESP_EARLY_LOGI("a", "perc_event: %d", code);
    if (code == LV_EVENT_DRAW_MAIN)
    {
        lv_layer_t *layer = lv_event_get_layer(e);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.base.layer = layer;

        lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

        // Default text
        sprintf(perc->text_buf, perc->format, *perc->value);

        label_dsc.text = perc->text_buf;
        label_dsc.align = LV_TEXT_ALIGN_CENTER;
        label_dsc.opa = LV_OPA_COVER;

        // Get updated object area
        lv_area_t label_area;
        lv_obj_get_coords(obj, &label_area);

        // Draw the label at the calculated area
        lv_draw_label(layer, &label_dsc, &label_area);
    }
    else if (code == LV_EVENT_CLICKED)
    {
        lv_group_t *g = lv_obj_get_group(obj);
        bool editing = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_active());
        if (indev_type == LV_INDEV_TYPE_ENCODER)
        {
            if (editing)
                lv_group_set_editing(g, false);
        }
    }
    else if (code == LV_EVENT_KEY)
    {
        uint32_t c = lv_event_get_key(e);

        int32_t dir = 0;
        if (c == LV_KEY_RIGHT)
            dir = 1;
        else if (c == LV_KEY_LEFT)
            dir = -1;
        int value = *perc->value + dir;
        value = value < perc->min ? perc->min : value;
        value = value > perc->max ? perc->max : value;
        *perc->value = value;
        lv_obj_invalidate(obj);
    }
}
