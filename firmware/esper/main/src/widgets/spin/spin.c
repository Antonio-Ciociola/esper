/**
 * @file spin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "spin_private.h"
#include "core/lv_obj_class_private.h"

#include "misc/lv_assert.h"
#include "indev/lv_indev.h"
#include "stdlib/lv_string.h"

#include "defines.h"

#include "esp_log.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&spin_class)
#define SPIN_MAX_DIGIT_COUNT_WITH_8BYTES (SPIN_MAX_DIGIT_COUNT + 8U)
#define SPIN_MAX_DIGIT_COUNT_WITH_4BYTES (SPIN_MAX_DIGIT_COUNT + 4U)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _spin_t spin_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void spin_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void spin_event(const lv_obj_class_t *class_p, lv_event_t *e);
static void spin_updatevalue(lv_obj_t *obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t spin_class = {
    .constructor_cb = spin_constructor,
    .event_cb = spin_event,
    .width_def = LV_DPI_DEF,
    .instance_size = sizeof(spin_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &usr_textarea_class,
    .name = "spinbox",
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *spin_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void spin_set_value(lv_obj_t *obj, int32_t v)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    if (v > spinbox->range_max)
        v = spinbox->range_max;
    if (v < spinbox->range_min)
        v = spinbox->range_min;

    *spinbox->value = v;

    spin_updatevalue(obj);
}

void spin_set_value_p(lv_obj_t *obj, int32_t *v)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    spinbox->value = v;
    spin_updatevalue(obj);
}

void spin_set_format(lv_obj_t *obj, char *format)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    spinbox->format = format;
    spin_updatevalue(obj);
}

void spin_set_rollover(lv_obj_t *obj, bool rollover)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    spinbox->rollover = rollover;
}

void spin_set_digit_format(lv_obj_t *obj, uint32_t digit_count, uint32_t sep_pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    if (digit_count > SPIN_MAX_DIGIT_COUNT)
        digit_count = SPIN_MAX_DIGIT_COUNT;

    if (sep_pos >= digit_count)
        sep_pos = 0;

    if (digit_count < SPIN_MAX_DIGIT_COUNT)
    {
        const int64_t max_val = lv_pow(10, digit_count);
        if (spinbox->range_max > max_val - 1)
            spinbox->range_max = max_val - 1;
        if (spinbox->range_min < -max_val + 1)
            spinbox->range_min = -max_val + 1;
    }

    spinbox->digit_count = digit_count;
    spinbox->dec_point_pos = sep_pos;

    spin_updatevalue(obj);
}

void spin_set_step(lv_obj_t *obj, uint32_t step)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    spinbox->step = step;
    spin_updatevalue(obj);
}

void spin_set_range(lv_obj_t *obj, int32_t range_min, int32_t range_max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    spinbox->range_max = range_max;
    spinbox->range_min = range_min;

    if (*spinbox->value > spinbox->range_max)
        *spinbox->value = spinbox->range_max;
    if (*spinbox->value < spinbox->range_min)
        *spinbox->value = spinbox->range_min;

    spin_updatevalue(obj);
}

void spin_set_cursor_pos(lv_obj_t *obj, uint32_t pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    const int32_t step_limit = LV_MAX(spinbox->range_max, LV_ABS(spinbox->range_min));
    const int32_t new_step = lv_pow(10, pos);

    if (pos <= 0)
        spinbox->step = 1;
    else if (new_step <= step_limit)
        spinbox->step = new_step;

    spin_updatevalue(obj);
}

void spin_set_digit_step_direction(lv_obj_t *obj, lv_dir_t direction)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;
    spinbox->digit_step_dir = direction;

    spin_updatevalue(obj);
}
/*=====================
 * Getter functions
 *====================*/

int32_t spin_get_value(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    return *spinbox->value;
}

int32_t spin_get_step(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    return spinbox->step;
}

/*=====================
 * Other functions
 *====================*/

void spin_step_next(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    const int32_t new_step = spinbox->step / 10;
    spinbox->step = (new_step > 0) ? new_step : 1;

    spin_updatevalue(obj);
}

void spin_step_prev(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    const int32_t step_limit = LV_MAX(spinbox->range_max, LV_ABS(spinbox->range_min));
    const int32_t new_step = spinbox->step * 10;
    if (new_step <= step_limit)
        spinbox->step = new_step;

    spin_updatevalue(obj);
}

bool spin_get_rollover(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    return spinbox->rollover;
}

void spin_increment(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    int32_t v = *spinbox->value;
    /* Special mode when zero crossing. E.g -3+10 should be 3, not 7.
     * Pretend we are on -7 now.*/
    if ((*spinbox->value < 0) && (*spinbox->value + spinbox->step) > 0)
    {
        v = -(spinbox->step + *spinbox->value);
    }

    if (v + spinbox->step <= spinbox->range_max)
    {
        v += spinbox->step;
    }
    else
    {
        /*Rollover?*/
        if ((spinbox->rollover) && (*spinbox->value == spinbox->range_max))
            v = spinbox->range_min;
        else
            v = spinbox->range_max;
    }

    if (v != *spinbox->value)
    {
        *spinbox->value = v;
        spin_updatevalue(obj);
    }
}

void spin_decrement(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    spin_t *spinbox = (spin_t *)obj;

    int32_t v = *spinbox->value;
    /* Special mode when zero crossing. E.g 3-10 should be -3, not -7.
     * Pretend we are on 7 now.*/
    if ((*spinbox->value > 0) && (*spinbox->value - spinbox->step) < 0)
    {
        v = spinbox->step - *spinbox->value;
    }

    if (v - spinbox->step >= spinbox->range_min)
    {
        v -= spinbox->step;
    }
    else
    {
        /*Rollover?*/
        if ((spinbox->rollover) && (*spinbox->value == spinbox->range_min))
            v = spinbox->range_max;
        else
            v = spinbox->range_min;
    }

    if (v != *spinbox->value)
    {
        *spinbox->value = v;
        spin_updatevalue(obj);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void spin_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    LV_LOG_TRACE("begin");

    spin_t *spinbox = (spin_t *)obj;

    /*Initialize the allocated 'ext'*/
    spinbox->value = malloc(sizeof(int32_t));
    *spinbox->value = 0;
    spinbox->dec_point_pos = 0;
    spinbox->digit_count = 5;
    spinbox->step = 1;
    spinbox->last_value = -1;
    spinbox->last_step = -1;
    spinbox->format = "%s";
    spinbox->range_max = 99999;
    spinbox->range_min = -99999;
    spinbox->rollover = false;
    spinbox->digit_step_dir = LV_DIR_RIGHT;
    spinbox->selected_digit = false;

    usr_textarea_set_one_line(obj, true);
    usr_textarea_set_cursor_click_pos(obj, true);

    spin_updatevalue(obj);

    static lv_style_t style_spin_focused;
    lv_style_init(&style_spin_focused);

    lv_style_set_text_decor(&style_spin_focused, LV_TEXT_DECOR_UNDERLINE);

    lv_obj_add_style(obj, &style_spin_focused, LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &style_spin_focused, LV_STATE_USER_1 | LV_STATE_EDITED);

    static lv_style_t style_spin_edited;
    lv_style_init(&style_spin_edited);

    lv_style_set_text_decor(&style_spin_edited, LV_TEXT_DECOR_NONE);

    lv_obj_add_style(obj, &style_spin_edited, LV_STATE_EDITED);

    static lv_style_t style_cursor_focused;
    lv_style_init(&style_cursor_focused);

    lv_style_set_bg_opa(&style_cursor_focused, LV_OPA_COVER);
    lv_style_set_bg_color(&style_cursor_focused, COLOR_FG);
    lv_style_set_text_color(&style_cursor_focused, COLOR_BG);
    // lv_style_set_pad_bottom(&style_cursor_focused, 1);

    // lv_style_set_pad_top(&style_cursor_focused, -1);
    // lv_style_set_outline_color(&style_cursor_focused, COLOR_FG);
    // lv_style_set_outline_width(&style_cursor_focused, 1);
    // lv_style_set_outline_pad(&style_cursor_focused, 0);
    // lv_style_set_outline_opa(&style_cursor_focused, LV_OPA_COVER);
    // lv_style_set_outline_opa(&style_cursor_focused, LV_OPA_COVER);

    lv_obj_add_style(obj, &style_cursor_focused, LV_PART_CURSOR | LV_STATE_EDITED);

    /* Set default height */
    // lv_obj_set_height(obj, LV_SIZE_CONTENT);

    // int height = lv_obj_get_height(obj);
    // lv_obj_set_height(obj, height + 2);
    lv_obj_set_width(obj, LV_SIZE_CONTENT);
    LV_LOG_TRACE("Spinbox constructor finished");
}

static void spin_event(const lv_obj_class_t *class_p, lv_event_t *e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_result_t res = LV_RESULT_OK;
    res = lv_obj_event_base(MY_CLASS, e);
    if (res != LV_RESULT_OK)
        return;

    const lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);
    spin_t *spinbox = (spin_t *)obj;

    // ESP_EARLY_LOGI("a", "step: %d", spinbox->step);
    // ESP_EARLY_LOGI("a", "spin_event: %d", code);
    // LV_ASSERT_MSG(!disp->rendering_in_progress, "Invalidate area is not allowed during rendering.");
    if (code == LV_EVENT_DRAW_MAIN)
    {
        if (*spinbox->value != spinbox->last_value || spinbox->step != spinbox->last_step)
        {
            spin_updatevalue(obj);
        }
    }

    else if (code == LV_EVENT_CLICKED)
    {
        if (spinbox->selected_digit)
        {
            spinbox->selected_digit = false;
            lv_obj_remove_state(obj, LV_STATE_USER_1);
        }
        else
        {
            spinbox->selected_digit = true;
            lv_obj_add_state(obj, LV_STATE_USER_1);
        }
    }
    else if (code == LV_EVENT_KEY)
    {
        uint32_t c = lv_event_get_key(e);
        if (c == LV_KEY_RIGHT)
        {
            if (spinbox->selected_digit)
            {
                spin_increment(obj);
            }
            else
            {
                spin_step_next(obj);
            }
        }
        else if (c == LV_KEY_LEFT)
        {
            if (spinbox->selected_digit)
            {
                spin_decrement(obj);
            }
            else
            {
                spin_step_prev(obj);
            }
        }
    }
}

static void spin_updatevalue(lv_obj_t *obj)
{
    spin_t *spinbox = (spin_t *)obj;

    // if (*spinbox->value == spinbox->last_value && spinbox->step == spinbox->last_step)
    //     return;
    // spinbox->last_value = *spinbox->value;
    // spinbox->last_step = spinbox->step;

    /* SPIN_MAX_DIGIT_COUNT_WITH_8BYTES (18): Max possible digit_count value (15) + sign + decimal point + NULL terminator */
    char textarea_txt[SPIN_MAX_DIGIT_COUNT_WITH_8BYTES] = {0U};
    char *buf_p = textarea_txt;

    uint32_t cur_shift_left = 0;
    if (spinbox->range_min < 0)
    { /*hide sign if there are only positive values*/
        /*Add the sign*/
        (*buf_p) = *spinbox->value >= 0 ? '+' : '-';
        buf_p++;
    }
    else
    {
        /*Cursor need shift to left*/
        cur_shift_left++;
    }

    /*Convert the numbers to string (the sign is already handled so always convert positive number)*/
    char digits[SPIN_MAX_DIGIT_COUNT_WITH_4BYTES];
    lv_snprintf(digits, SPIN_MAX_DIGIT_COUNT_WITH_4BYTES, "%" LV_PRId32, LV_ABS(*spinbox->value));

    /*Add leading zeros*/
    int32_t i;
    const size_t digits_len = lv_strlen(digits);

    const int leading_zeros_cnt = spinbox->digit_count - digits_len;
    if (leading_zeros_cnt)
    {
        for (i = (int32_t)digits_len; i >= 0; i--)
        {
            digits[i + leading_zeros_cnt] = digits[i];
        }
        /* NOTE: Substitute with memset? */
        for (i = 0; i < leading_zeros_cnt; i++)
        {
            digits[i] = '0';
        }
    }

    /*Add the decimal part*/
    const uint32_t intDigits = (spinbox->dec_point_pos == 0) ? spinbox->digit_count : spinbox->dec_point_pos;
    for (i = 0; i < (int32_t)intDigits && digits[i] != '\0'; i++)
    {
        (*buf_p) = digits[i];
        buf_p++;
    }

    /*Insert the decimal point*/
    if (spinbox->dec_point_pos)
    {
        (*buf_p) = '.';
        buf_p++;

        for (/*Leave i*/; i < spinbox->digit_count && digits[i] != '\0'; i++)
        {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    lv_snprintf(textarea_txt, SPIN_MAX_DIGIT_COUNT_WITH_4BYTES, spinbox->format, textarea_txt);

    /*Refresh the text*/
    usr_textarea_set_text(obj, (char *)textarea_txt);

    /*Set the cursor position*/
    int32_t step = spinbox->step;
    uint32_t cur_pos = (uint32_t)spinbox->digit_count;
    while (step >= 10)
    {
        step /= 10;
        cur_pos--;
    }

    if (cur_pos > intDigits)
        cur_pos++; /*Skip the decimal point*/

    cur_pos -= cur_shift_left;

    usr_textarea_set_cursor_pos(obj, cur_pos);
}
