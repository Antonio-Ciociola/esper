#include "utils/utils.h"
#include "defines.h"
#include "esp_lvgl_port.h"

void style_hover(lv_obj_t *obj)
{
    static lv_style_t style_title;
    lv_style_init(&style_title);

    static lv_style_t style_title_focus;
    lv_style_init(&style_title_focus);
    lv_style_set_text_decor(&style_title_focus, LV_TEXT_DECOR_UNDERLINE);

    static lv_style_t style_title_edited;
    lv_style_init(&style_title_edited);
    lv_style_set_text_decor(&style_title_edited, LV_TEXT_DECOR_UNDERLINE);
    lv_style_set_bg_opa(&style_title_edited, LV_OPA_COVER);
    lv_style_set_bg_color(&style_title_edited, COLOR_FG);
    lv_style_set_text_color(&style_title_edited, COLOR_BG);

    lv_obj_add_style(obj, &style_title, LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &style_title_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &style_title_edited, LV_STATE_EDITED);

    // led->bright = perc_BRIGHT_MAX;
}

void exit_edit_cb(lv_event_t *e)
{

    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED)
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
}

void flex_newline(lv_obj_t *obj)
{
    lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
}

// void init_pitch_table()
// {
//     pitchTable = static_cast<float *>(heap_caps_malloc(PITCH_TABLE_SIZE * sizeof(float), MALLOC_CAP_INTERNAL));

//     float curPitch = 1;
//     for (int i = 0; i < PITCH_TABLE_SIZE; i++)
//     {
//         // Pitch table is 2**k with k from 0 to 6, evenly spaced in the 0-PITCH_TABLE_SIZE
//         pitchTable[i] = curPitch;
//         curPitch *= pow(2., 6.0 / PITCH_TABLE_SIZE);
//     }
// }

// float get_note_freq(unsigned note)
// {
//     note = note % (6 * 12);
//     return pitchTable[note * (PITCH_TABLE_SIZE / (6 * 12))];
// }