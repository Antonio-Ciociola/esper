/*********************
 *      INCLUDES
 *********************/
#include "digit_private.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"

#include "misc/lv_assert.h"
#include "themes/lv_theme.h"
#include "misc/lv_color.h"
#include "esp_log.h"

#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&digit_class)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _digit_t digit_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void digit_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void digit_event(const lv_obj_class_t *class_p, lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t digit_class = {
    .name = "digit",
    .base_class = &lv_obj_class,
    .constructor_cb = digit_constructor,
    .event_cb = digit_event,
    .instance_size = sizeof(digit_t),
    .width_def = 20,
    .height_def = 12,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
};

static const char *TAG = "digit";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *digit_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void digit_set_current(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    digit_t *digit = (digit_t *)obj;
    digit->current = true;
    lv_obj_invalidate(obj);
}

void digit_unset_current(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    digit_t *digit = (digit_t *)obj;
    digit->current = false;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

uint8_t digit_get_selected(const lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    digit_t *digit = (digit_t *)obj;
    return digit->sel;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void digit_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    digit_t *digit = (digit_t *)obj;

    digit->sel = 1;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    // lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    static lv_style_t style_title;
    lv_style_init(&style_title);

    static lv_style_t style_title_focus;
    lv_style_init(&style_title_focus);
    lv_style_set_text_decor(&style_title_focus, LV_TEXT_DECOR_UNDERLINE);

    static lv_style_t style_title_edited;
    lv_style_init(&style_title_edited);
    lv_style_set_text_decor(&style_title_edited, LV_TEXT_DECOR_UNDERLINE);
    lv_style_set_bg_opa(&style_title_edited, LV_OPA_COVER);
    lv_style_set_bg_color(&style_title_edited, lv_color_black());
    lv_style_set_text_color(&style_title_edited, lv_color_white());

    lv_obj_add_style(obj, &style_title, LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &style_title_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &style_title_edited, LV_STATE_EDITED);

    // led->bright = digit_BRIGHT_MAX;

    digit->num_notes = 12 * 5 + 1;
}

static void digit_event(const lv_obj_class_t *class_p, lv_event_t *e)
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
    digit_t *digit = (digit_t *)obj;

    const char *notes[] = {"---",
                           "C1", "Cs1", "D1", "Ds1", "E1", "F1", "Fs1", "G1", "Gs1", "A1", "As1", "B1",
                           "C2", "Cs2", "D2", "Ds2", "E2", "F2", "Fs2", "G2", "Gs2", "A2", "As2", "B2",
                           "C3", "Cs3", "D3", "Ds3", "E3", "F3", "Fs3", "G3", "Gs3", "A3", "As3", "B3",
                           "C4", "Cs4", "D4", "Ds4", "E4", "F4", "Fs4", "G4", "Gs4", "A4", "As4", "B4",
                           "C5", "Cs5", "D5", "Ds5", "E5", "F5", "Fs5", "G5", "Gs5", "A5", "As5", "B5"};

    if (code == LV_EVENT_DRAW_MAIN)
    {
        lv_layer_t *layer = lv_event_get_layer(e);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.base.layer = layer;

        lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

        // If the object is focused, underline the text
        label_dsc.text = notes[digit->sel];
        label_dsc.align = LV_TEXT_ALIGN_CENTER;
        // label_dsc.color = lv_color_black();
        label_dsc.opa = LV_OPA_COVER;

        // Get updated object area
        lv_area_t label_area;
        lv_obj_get_coords(obj, &label_area);

        // Draw the label at the calculated area
        lv_draw_label(layer, &label_dsc, &label_area);
    }
    else if (code == LV_EVENT_DRAW_POST)
    {
        lv_layer_t *layer = lv_event_get_layer(e);
        lv_area_t orig_clip;
        lv_area_copy(&orig_clip, &layer->_clip_area);

        // Expand clip area to include your drawing
        lv_area_t new_clip = orig_clip;
        new_clip.y1 -= 10; // Expand left by 10 pixels
        new_clip.y1 = new_clip.y1 < 0 ? 0 : new_clip.y1;

        layer->_clip_area = new_clip;

        if (digit->current)
        {
            // Draw a small arrow on the left side
            lv_draw_line_dsc_t line_dsc;
            lv_draw_line_dsc_init(&line_dsc);
            line_dsc.color = lv_color_black(); // Adjust color if needed
            line_dsc.width = 1;
            line_dsc.round_start = false;
            line_dsc.round_end = false;
            line_dsc.round_end = false;
            line_dsc.opa = LV_OPA_COVER;

            lv_area_t label_area;
            lv_obj_get_coords(obj, &label_area);

            int x_centre = (label_area.x1 + label_area.x2) / 2;
            int y_centre = (label_area.y1 + label_area.y2) / 2;

            lv_point_precise_t points[3] = {
                {x_centre - 3, label_area.y1 - 3},
                {x_centre, label_area.y1},
                {x_centre + 3, label_area.y1 - 3},
            };

            line_dsc.p1 = points[0];
            line_dsc.p2 = points[1];
            lv_draw_line(layer, &line_dsc);

            line_dsc.p1 = points[1];
            line_dsc.p2 = points[2];
            lv_draw_line(layer, &line_dsc);

            line_dsc.p1 = points[0];
            line_dsc.p2 = points[2];
            lv_draw_line(layer, &line_dsc);
        }
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
        if (c == LV_KEY_RIGHT || c == LV_KEY_DOWN)
            dir = 1;
        else if (c == LV_KEY_LEFT || c == LV_KEY_UP)
            dir = -1;
        int new_sel = digit->sel + dir;
        new_sel = new_sel < 0 ? 0 : new_sel;
        new_sel = new_sel > digit->num_notes-1 ? digit->num_notes-1 : new_sel;
        digit->sel = new_sel;
        lv_obj_invalidate(obj);
    }
}
