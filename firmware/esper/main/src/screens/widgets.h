#pragma once

#include "lvgl.h"

struct position
{
    int x;
    int y;
};

class arc
{
public:
    int &current;
    lv_obj_t *arc_obj;
    arc(lv_obj_t *scrn, lv_group_t *group, int &current, position pos={0,0}, int min = 0, int max = 255, int step = 1);
};

class bar
{
public:
    int &current;
    lv_obj_t *bar_obj;
    bar(lv_obj_t *scrn, lv_group_t *group, int &current, position pos={0,0}, int min = 0, int max = 100);
};

class container
{
public:
    lv_obj_t *container_obj;
    container(lv_obj_t *scrn, lv_group_t *group,
              lv_flex_flow_t flow = LV_FLEX_FLOW_ROW_WRAP,
              lv_flex_align_t place1 = LV_FLEX_ALIGN_CENTER,
              lv_flex_align_t place2 = LV_FLEX_ALIGN_CENTER,
              lv_flex_align_t place3 = LV_FLEX_ALIGN_START);
};
class containerVert
{
public:
    lv_obj_t *container_obj;
    containerVert(lv_obj_t *scrn, lv_group_t *group,
              lv_flex_flow_t flow = LV_FLEX_FLOW_COLUMN_WRAP,
              lv_flex_align_t place1 = LV_FLEX_ALIGN_CENTER,
              lv_flex_align_t place2 = LV_FLEX_ALIGN_CENTER,
              lv_flex_align_t place3 = LV_FLEX_ALIGN_START);
};

class button
{
public:
    lv_obj_t *button_obj;
    button(lv_obj_t *scrn, lv_group_t *group, position pos, char *name);
};

class arc_text
{
public:
    int &current;
    lv_obj_t *arc;
    arc_text(lv_obj_t *scrn, lv_group_t *group, position pos, int &current, int min = 0, int max = 255, int step = 1);
};

class led
{
public:
    lv_obj_t *led_obj;
    led(lv_obj_t *scrn, lv_group_t *group, position pos, int brightness = 255, int size = 8, int *value = NULL);
    void set_brightness(int brightness);
};

class step
{
public:
    lv_obj_t *step_obj;
    step(lv_obj_t *scrn, lv_group_t *group, position pos);
    void set_current();
    void unset_current();
    uint8_t get_selected();
};

class spin
{
public:
    lv_obj_t *spin_obj;
    int &current;
    spin(lv_obj_t *scrn, lv_group_t *group, position pos, int &current, int min = 0, int max = 255, int n_digits = 3, char *format = NULL);
};

class perc
{
public:
    lv_obj_t *perc_obj;
    perc(lv_obj_t *scrn, lv_group_t *group, int *value = NULL, position pos = {0, 0}, char *format = NULL, int n_chr_max = 4, int min = 0, int max = 100);
};

class roller
{
public:
    lv_obj_t *roller_obj;
    roller(lv_obj_t *scrn, lv_group_t *group, position pos, char *options, int &current, int num_show, int width);
};

class label
{
public:
    lv_obj_t *label_obj;
    label(lv_obj_t *scrn, lv_group_t *group, char *name, position pos = {0, 0});
};

void title_event_cb(lv_event_t *e);

class title
{
public:
    lv_obj_t *title_obj;
    title(lv_obj_t *scrn, lv_group_t *group, char *name);
};

class switchWid
{
public:
    int &current;
    switchWid(lv_obj_t *scrn, lv_group_t *group, position pos, int &current, bool vertical = false);
};