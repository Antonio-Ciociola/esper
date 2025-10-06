#pragma once

#include "lvgl.h"
#include <memory>
#include <vector>

#include "widgets.h"
#include "update.h"
#include "utils/values.h"

class screen {
public:
  enableAdc enableadc = {{true, true, true, true}};
  lv_obj_t *scrn;
  lv_group_t *group;
  title *t;
  screen();
  virtual void select();
  virtual void unselect();
  virtual void IRAM_ATTR update();
  virtual void IRAM_ATTR refresh();
};

void title_event_cb(lv_event_t *e);

DRAM_ATTR extern std::vector<std::unique_ptr<screen>> screens;
extern int current_screen;
