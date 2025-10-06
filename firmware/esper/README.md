# Firmware

## Build and Flash

`idf.py reconfigure`

Modify `managed_components/lvgl__lvgl/src/core/lv_refr.c` and comment line: `LV_ASSERT_MSG(!disp->rendering_in_progress, "Invalidate area is not allowed during rendering.");`

`idf.py build`

`idf.py flash monitor`
