#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "widgets.h"
#include "widgets/monoled/monoled.h"
#include "widgets/step/step.h"
#include "widgets/perc/perc.h"
#include "widgets/spin/spin.h"

#include "src/widgets/switch/lv_switch.h"

#include "utils/utils.h"

static const char *TAG = "widgets";

static void arc_cb(lv_event_t *e)
{
    lv_obj_t *arc = (lv_obj_t *)lv_event_get_target(e);
    int *value = static_cast<int *>(lv_event_get_user_data(e));
    *value = lv_arc_get_value(arc);
    // ESP_LOGI(TAG, "Value changed to %d", *value);
}

static void bar_cb(lv_event_t *e)
{
    lv_obj_t *bar = (lv_obj_t *)lv_event_get_target(e);
    int *value = static_cast<int *>(lv_event_get_user_data(e));
    lv_event_code_t code = lv_event_get_code(e);

    lvgl_port_lock(0);

    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        int cur = lv_bar_get_value(bar);

        if (key == LV_KEY_RIGHT || key == LV_KEY_DOWN) {
            cur++;
        } else if (key == LV_KEY_LEFT || key == LV_KEY_UP) {
            cur--;
        }

        // Apply limits
        int min = lv_bar_get_min_value(bar);
        int max = lv_bar_get_max_value(bar);
        if (cur < min) cur = min;
        if (cur > max) cur = max;

        lv_bar_set_value(bar, cur, LV_ANIM_ON);
        *value = cur;
        // lv_obj_invalidate(bar);
        // ESP_LOGI(TAG, "Bar value changed to %d", *value);
    }
    lvgl_port_unlock();
}


static void spin_cb(lv_event_t *e)
{
    lv_obj_t *spin = (lv_obj_t *)lv_event_get_target(e);
    int *value = static_cast<int *>(lv_event_get_user_data(e));
    *value = spin_get_value(spin);
    // ESP_LOGI(TAG, "Value changed to %d", *value);
}

static void switch_cb(lv_event_t *e)
{
    lv_obj_t *switch_obj = (lv_obj_t *)lv_event_get_target(e);
    int *value = static_cast<int *>(lv_event_get_user_data(e));
    *value = lv_obj_has_state(switch_obj, LV_STATE_CHECKED);
    // ESP_LOGI(TAG, "Value changed to %d", *value);
}

struct arc_text_data_t
{
    int *current;
    lv_obj_t *label;
};

static void arc_text_cb(lv_event_t *e)
{
    lv_obj_t *arc = (lv_obj_t *)lv_event_get_target(e);
    arc_text_data_t *data = (arc_text_data_t *)lv_event_get_user_data(e);
    *(data->current) = lv_arc_get_value(arc);

    static char buf[8];
    snprintf(buf, sizeof(buf), "%d", *(data->current));
    lv_label_set_text(data->label, buf);
    // lv_obj_align_to(data->label, arc, LV_ALIGN_CENTER, 0, -18);
}

container::container(lv_obj_t *scrn, lv_group_t *group, lv_flex_flow_t flow, lv_flex_align_t place1, lv_flex_align_t place2, lv_flex_align_t place3)
{
    lvgl_port_lock(0);

    container_obj = lv_obj_create(scrn);
    lv_obj_set_size(container_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container_obj, flow);
    lv_obj_set_flex_align(container_obj, place1, place2, place3);
    lv_obj_set_style_border_width(container_obj, 0, 0);

    lvgl_port_unlock();
}


containerVert::containerVert(lv_obj_t *scrn, lv_group_t *group, lv_flex_flow_t flow, lv_flex_align_t place1, lv_flex_align_t place2, lv_flex_align_t place3)
{
    lvgl_port_lock(0);

    container_obj = lv_obj_create(scrn);
    lv_obj_set_size(container_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container_obj, flow);
    lv_obj_set_flex_align(container_obj, place1, place2, place3);
    lv_obj_set_style_border_width(container_obj, 0, 0);

    lvgl_port_unlock();
}



arc::arc(lv_obj_t *scrn, lv_group_t *group, int &current, position pos, int min, int max, int step) : current(current)
{
    lvgl_port_lock(0);

    arc_obj = lv_arc_create(scrn);
    int arc_size = 20; // Size of the arc
    lv_obj_set_size(arc_obj, arc_size, arc_size);
    lv_arc_set_rotation(arc_obj, 135);
    lv_arc_set_bg_angles(arc_obj, 0, 270);
    lv_obj_align(arc_obj, LV_ALIGN_TOP_LEFT, pos.x - (arc_size / 2), pos.y - (arc_size / 2));

    lv_group_add_obj(group, arc_obj); // Add the arc to the group

    lv_arc_set_range(arc_obj, min, max);
    lv_arc_set_value(arc_obj, current);
    lv_arc_set_change_rate(arc_obj, step);

    lv_obj_add_flag(arc_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_add_event_cb(arc_obj, arc_cb, LV_EVENT_VALUE_CHANGED, &current);

    lvgl_port_unlock();

    // ESP_LOGI(TAG, "Finished creating arc");
}


bar::bar(lv_obj_t *scrn, lv_group_t *group, int &current, position pos, int min, int max) : current(current)
{
    lvgl_port_lock(0);

    bar_obj = lv_bar_create(scrn);
    lv_obj_set_size(bar_obj, 20, 7);
    // lv_obj_center(bar_obj);    
    // lv_obj_align(bar_obj, LV_ALIGN_TOP_LEFT, pos.x - (arc_size / 2), pos.y - (arc_size / 2));
    // lv_obj_center(bar_obj);

    lv_group_add_obj(group, bar_obj); // Add the arc to the group

    lv_bar_set_value(bar_obj, current, LV_ANIM_OFF);
    lv_bar_set_range(bar_obj, min, max);

    static lv_style_t style_edit;

    lv_style_init(&style_edit);
    // lv_style_set_bg_color(&style_edit, lv_color_hex(0xFF8800)); // orange fill
    // lv_style_set_border_color(&style_edit, lv_color_black());
    lv_style_set_outline_width(&style_edit, 2);

    lv_obj_add_style(bar_obj, &style_edit, LV_STATE_EDITED);


    lv_obj_add_flag(bar_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    // lv_obj_set_style_pad_bottom(bar_obj, 5, 0);
    lv_obj_add_flag(bar_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(bar_obj, bar_cb, LV_EVENT_KEY, &current);
    lv_obj_add_event_cb(bar_obj, exit_edit_cb, LV_EVENT_RELEASED, NULL);

    lvgl_port_unlock();

    // ESP_LOGI(TAG, "Finished creating arc");
}

button::button(lv_obj_t *scrn, lv_group_t *group, position pos, char *name)
{
    lvgl_port_lock(0);

    button_obj = lv_button_create(scrn);
    lv_obj_align(button_obj, LV_ALIGN_TOP_LEFT, pos.x, pos.y);
    lv_obj_t *label = lv_label_create(button_obj);
    lv_label_set_text(label, name);

    lv_group_add_obj(group, button_obj);
    lvgl_port_unlock();

    // ESP_LOGI(TAG, "Finished creating arc");
}

arc_text::arc_text(lv_obj_t *scrn, lv_group_t *group, position pos, int &current, int min, int max, int step)
    : current(current)
{
    lvgl_port_lock(0);

    arc = lv_arc_create(scrn);
    int arc_size = 20;
    lv_obj_set_size(arc, arc_size, arc_size);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_obj_align(arc, LV_ALIGN_TOP_LEFT, pos.x - (arc_size / 2), pos.y - (arc_size / 2));

    lv_arc_set_range(arc, min, max);
    lv_arc_set_value(arc, current);
    lv_arc_set_change_rate(arc, step);

    // Create the label
    lv_obj_t *label = lv_label_create(scrn);
    static char buf[8];
    snprintf(buf, sizeof(buf), "%d", current);
    lv_label_set_text(label, buf);

    lv_obj_align_to(label, arc, LV_ALIGN_CENTER, 0, -15);

    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);

    // Adjust arc size to fit the label
    // lv_obj_set_size(arc, arc_size, arc_size + 20); // Increase height to accommodate the label

    arc_text_data_t *data = (arc_text_data_t *)lv_malloc(sizeof(arc_text_data_t));
    data->current = &current;
    data->label = label;

    lv_obj_add_flag(arc, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(arc, arc_text_cb, LV_EVENT_VALUE_CHANGED, data);

    lv_group_add_obj(group, arc);

    lvgl_port_unlock();

    // ESP_LOGI(TAG, "Finished creating arc with label");
}

led::led(lv_obj_t *scrn, lv_group_t *group, position pos, int brightness, int size, int *value)
{
    lvgl_port_lock(0);

    led_obj = monoled_create(scrn);
    monoled_set_brightness(led_obj, brightness);

    if (value != NULL)
    {
        monoled_set_value(led_obj, value);
    }

    lv_obj_set_size(led_obj, size, size);
    lv_obj_align(led_obj, LV_ALIGN_TOP_LEFT, pos.x - size / 2, pos.y - size / 2);

    lvgl_port_unlock();
}

void led::set_brightness(int brightness)
{
    lvgl_port_lock(0);
    monoled_set_brightness(led_obj, brightness);
    lvgl_port_unlock();
}

step::step(lv_obj_t *scrn, lv_group_t *group, position pos)
{
    lvgl_port_lock(0);
    step_obj = step_create(scrn);
    lv_group_add_obj(group, step_obj);
    lvgl_port_unlock();
}

void step::set_current()
{
    lvgl_port_lock(0);
    step_set_current(step_obj);
    lvgl_port_unlock();
}

void step::unset_current()
{
    lvgl_port_lock(0);
    step_unset_current(step_obj);
    lvgl_port_unlock();
}

uint8_t step::get_selected()
{
    return step_get_selected(step_obj);
}

spin::spin(lv_obj_t *scrn, lv_group_t *group, position pos, int &current, int min, int max, int n_digits, char *format) : current(current)
{
    lvgl_port_lock(0);

    spin_obj = spin_create(scrn);

    spin_set_digit_format(spin_obj, n_digits, 0);
    spin_set_range(spin_obj, min, max);

    if (format != NULL)
    {
        spin_set_format(spin_obj, format);
    }

    lv_obj_align(spin_obj, LV_ALIGN_TOP_LEFT, pos.x, pos.y);
    // lv_obj_set_width(spin_obj, 30);
    // lv_obj_set_height(spin_obj, 40);

    // static lv_style_t style_title_focus;
    // lv_style_init(&style_title_focus);
    // lv_style_set_text_decor(&style_title_focus, LV_TEXT_DECOR_UNDERLINE);

    // lv_obj_add_style(spin_obj, &style_title_focus, LV_STATE_FOCUSED);

    lv_obj_add_flag(spin_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    spin_set_value_p(spin_obj, (int32_t *)&current);

    // lv_obj_add_event_cb(spin_obj, spin_cb, LV_EVENT_VALUE_CHANGED, &current);

    lv_group_add_obj(group, spin_obj);
    lvgl_port_unlock();
}

perc::perc(lv_obj_t *scrn, lv_group_t *group, int *value, position pos, char *format, int n_chr_max, int min, int max)
{
    lvgl_port_lock(0);

    perc_obj = perc_create(scrn);

    if (value == NULL)
    {
        value = new int;
        *value = 100;
    }
    if (format == NULL)
    {
        format = new char[8];
        format = "%d%%";
    }
    perc_set_value(perc_obj, value);
    perc_set_format(perc_obj, format);
    perc_set_min_max(perc_obj, min, max);

    lv_obj_set_width(perc_obj, n_chr_max * 8);

    lv_group_add_obj(group, perc_obj);

    lvgl_port_unlock();
}

static void roller_event_key(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    uint32_t key = lv_event_get_key(e);
    if (key == LV_KEY_LEFT || key == LV_KEY_RIGHT)
    {
        int *current = static_cast<int *>(lv_event_get_user_data(e));
        *current = lv_roller_get_selected(obj);
        // ESP_LOGI(TAG, "Selected: %d %s", *current, buf);
    }
}

roller::roller(lv_obj_t *scrn, lv_group_t *group, position pos, char *options, int &current, int num_show, int width)
{
    lvgl_port_lock(0);

    roller_obj = lv_roller_create(scrn);
    lv_roller_set_options(roller_obj, options, LV_ROLLER_MODE_INFINITE);
    lv_obj_set_width(roller_obj, width);
    lv_roller_set_visible_row_count(roller_obj, num_show);

    lv_obj_update_layout(roller_obj);
    int height = lv_obj_get_height(roller_obj);
    lv_obj_align(roller_obj, LV_ALIGN_TOP_LEFT, pos.x - width / 2, pos.y - height / 2);

    lv_obj_add_flag(roller_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(roller_obj, roller_event_key, LV_EVENT_KEY, &current);

    lv_group_add_obj(group, roller_obj); // Add the arc to the group

    lvgl_port_unlock();
}

label::label(lv_obj_t *scrn, lv_group_t *group, char *name, position pos)
{
    lvgl_port_lock(0);
    label_obj = lv_label_create(scrn);
    lv_label_set_text(label_obj, name);
    // Set text alignment to center
    lv_obj_set_style_text_align(label_obj, LV_TEXT_ALIGN_CENTER, 0);

    // Refresh the layout to get correct width and height
    lv_obj_update_layout(label_obj);

    // Get label size
    int label_w = lv_obj_get_width(label_obj);
    int label_h = lv_obj_get_height(label_obj);
    lv_obj_align(label_obj, LV_ALIGN_TOP_LEFT, pos.x - (label_w / 2), pos.y - (label_h / 2));

    lvgl_port_unlock();
}

title::title(lv_obj_t *scrn, lv_group_t *group, char *nome)
{
    lvgl_port_lock(0);

    title_obj = lv_label_create(scrn);
    lv_label_set_text(title_obj, nome);
    lv_obj_align(title_obj, LV_ALIGN_TOP_MID, 0, 0);
    style_hover(title_obj);

    lv_obj_add_flag(title_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(title_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(title_obj, title_event_cb, LV_EVENT_ALL, NULL); // Handle rotation

    lv_group_add_obj(group, title_obj);
    lv_group_set_editing(group, true);

    lvgl_port_unlock();
}

switchWid::switchWid(lv_obj_t *scrn, lv_group_t *group, position pos, int &current, bool vertical) : current(current)
{

    lvgl_port_lock(0);

    lv_obj_t *switch_obj = lv_switch_create(scrn);

    lv_obj_set_size(switch_obj, 16, 10);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, switch_obj);
    lv_anim_set_time(&anim, 100); // Set animation time to 200ms
    lv_anim_set_exec_cb(&anim, [](void *obj, int32_t v)
                        { lv_obj_set_style_anim_time(static_cast<lv_obj_t *>(obj), v, 0); });
    lv_anim_set_values(&anim, 0, 100);
    lv_anim_start(&anim);

    // if (vertical)
    // {
    //     lv_switch_set_orientation(switch_obj, LV_SWITCH_ORIENTATION_VERTICAL);
    // }

    lv_obj_add_flag(switch_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(switch_obj, switch_cb, LV_EVENT_VALUE_CHANGED, &current);

    lv_group_add_obj(group, switch_obj);

    lvgl_port_unlock();
}
