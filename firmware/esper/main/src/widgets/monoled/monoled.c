/*********************
 *      INCLUDES
 *********************/
#include "monoled_private.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"

#include "misc/lv_assert.h"
#include "themes/lv_theme.h"
#include "misc/lv_color.h"
#include "esp_log.h"

#include "defines.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&monoled_class)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _monoled_t monoled_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void monoled_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void monoled_event(const lv_obj_class_t *class_p, lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t monoled_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = monoled_constructor,
    .width_def = LV_DPI_DEF / 5,
    .height_def = LV_DPI_DEF / 5,
    .event_cb = monoled_event,
    .instance_size = sizeof(monoled_t),
    .name = "led",
};

static const char *TAG = "monoled";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *monoled_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void monoled_set_brightness(lv_obj_t *obj, uint8_t bright)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    monoled_t *led = (monoled_t *)obj;
    if (led->bright == bright)
        return;

    led->bright = LV_CLAMP(MONOLED_BRIGHT_MIN, bright, MONOLED_BRIGHT_MAX);

    /*Invalidate the object therefore it will be redrawn*/
    lv_obj_invalidate(obj);
}

void monoled_set_value(lv_obj_t *obj, int *value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    monoled_t *led = (monoled_t *)obj;
    led->value = value;

    /*Invalidate the object therefore it will be redrawn*/
    lv_obj_invalidate(obj);
}

void monoled_on(lv_obj_t *led)
{
    monoled_set_brightness(led, MONOLED_BRIGHT_MAX);
}

void monoled_off(lv_obj_t *led)
{
    monoled_set_brightness(led, MONOLED_BRIGHT_MIN);
}

void monoled_toggle(lv_obj_t *obj)
{
    uint8_t bright = monoled_get_brightness(obj);
    if (bright > (MONOLED_BRIGHT_MIN + MONOLED_BRIGHT_MAX) >> 1)
        monoled_off(obj);
    else
        monoled_on(obj);
}

/*=====================
 * Getter functions
 *====================*/

uint8_t monoled_get_brightness(const lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    monoled_t *led = (monoled_t *)obj;
    return led->bright;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void monoled_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    monoled_t *led = (monoled_t *)obj;
    led->bright = MONOLED_BRIGHT_MAX;
    led->value = NULL;
}
static void monoled_event(const lv_obj_class_t *class_p, lv_event_t *e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    lv_event_code_t code = lv_event_get_code(e);
    res = lv_obj_event_base(MY_CLASS, e);
    if (res != LV_RESULT_OK)
        return;

    lv_obj_t *obj = lv_event_get_current_target(e);
    // ESP_EARLY_LOGI("a", "monoled_event: %d", code);
    if (code == LV_EVENT_DRAW_MAIN)
    {

        monoled_t *led = (monoled_t *)obj;

        int brightness;

        if (led->value != NULL)
        {
            brightness = LV_CLAMP(MONOLED_BRIGHT_MIN, *(led->value), MONOLED_BRIGHT_MAX);
        }
        else
        {
            brightness = led->bright;
        }

        lv_layer_t *layer = lv_event_get_layer(e);

        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.base.layer = layer;
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &rect_dsc);

        rect_dsc.bg_color = COLOR_FG;
        rect_dsc.bg_opa = LV_OPA_COVER;

        int base_size = lv_obj_get_width(obj); /* Object size */
        // int radius = (base_size / 2) * (led->bright + 1) / (MONOLED   _BRIGHT_MAX + 1);
        int diameter = (base_size - 1) * (brightness) / (MONOLED_BRIGHT_MAX + 1);
        // int diameter = radius * 2;

        /* Set radius to make it a circle */
        rect_dsc.radius = LV_RADIUS_CIRCLE;

        /* Define the area for the circle */
        lv_area_t area;
        int32_t center_x, center_y;
        area.x1 = obj->coords.x1 + (lv_obj_get_width(obj) / 2) - diameter / 2;
        area.y1 = obj->coords.y1 + (lv_obj_get_height(obj) / 2) - diameter / 2;
        area.x2 = area.x1 + diameter;
        area.y2 = area.y1 + diameter;

        /* Draw the filled circle */
        lv_draw_rect(layer, &rect_dsc, &area);
    }else if (code == LV_EVENT_REFRESH)
    {
        lv_obj_invalidate(obj);
    }
}
