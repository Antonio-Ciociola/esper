#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "math.h"
#include "esp_timer.h"

#include "defines.h"
#include "simpleVco.h"

#include "utils/utils.h"
#include "engines/waveengines.h"
#include "engines/utils.h"
#include "engines/simple.h"
#include "engines/plaits.h"

using namespace std;

static const char *TAG = "Vco";

void my_focus_cb(lv_group_t * group)
{
    lv_obj_t * obj = lv_group_get_focused(group);
    if(!obj) return;

    lv_obj_t * cont = lv_obj_get_parent(obj);
    if(!cont) return;

    // Center the focused object manually
    lv_coord_t cont_w = lv_obj_get_width(cont);
    lv_coord_t cont_h = lv_obj_get_height(cont);

    lv_coord_t obj_x = lv_obj_get_x(obj);
    lv_coord_t obj_y = lv_obj_get_y(obj);
    lv_coord_t obj_w = lv_obj_get_width(obj);
    lv_coord_t obj_h = lv_obj_get_height(obj);

    lv_coord_t scroll_x = obj_x + obj_w/2 - cont_w/2;
    lv_coord_t scroll_y = obj_y + obj_h - cont_h/2;

    lv_obj_scroll_to(cont, scroll_x, scroll_y, LV_ANIM_ON);
}


simpleVcoScreen::simpleVcoScreen()
{
    screen::enableadc = enableadc;


    lvgl_port_lock(0);

    t = new title(scrn, group, "Simple VCO");

    lv_obj_t *cont = container(scrn, group).container_obj;
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 15);
    lv_obj_set_size(cont, 128, LV_SIZE_CONTENT);

    // lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);


    lv_obj_t *label_freq = label(cont, group, "Freq").label_obj;
    lv_obj_t *spin_freq = spin(cont, group, {80, 10}, freq, 0, 8000, 4, "%sHz").spin_obj;
    lv_obj_set_width(spin_freq, 45);
    lv_obj_set_style_text_align(spin_freq, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *bar_freq = bar(cont, group, freq).bar_obj;
    flex_newline(label_freq);
    flex_newline(bar_freq);

    lv_obj_t *label_amplitude = label(cont, group, "Amplitude").label_obj;
    lv_obj_t *perc_amplitude = perc(cont, group, &amplitude).perc_obj;
    lv_obj_set_style_text_align(perc_amplitude, LV_TEXT_ALIGN_CENTER, 0);
    flex_newline(label_amplitude);

    // lv_obj_t *label_engine = label(cont, group, "Engine").label_obj;
    lv_obj_t *roller_engine = roller(
                                  cont,
                                  group,
                                  {93, 35},
                                  (char*)plaitsEngine.engines,
                                  engine,
                                  1,
                                  100)
                                  .roller_obj;
        
    flex_newline(roller_engine);

    lv_obj_t *label_harmonics = label(cont, group, "Harmonics").label_obj;
    lv_obj_t *perc_harmonics = perc(cont, group, &harmonics).perc_obj;
    lv_obj_set_style_text_align(perc_harmonics, LV_TEXT_ALIGN_CENTER, 0);
    flex_newline(label_harmonics);

    lv_obj_t *label_timbre = label(cont, group, "Timbre").label_obj;
    lv_obj_t *perc_timbre = perc(cont, group, &timbre).perc_obj;
    lv_obj_set_style_text_align(perc_timbre, LV_TEXT_ALIGN_CENTER, 0);
    flex_newline(label_timbre);

    lv_obj_t *label_morph = label(cont, group, "Morph").label_obj;
    lv_obj_t *perc_morph = perc(cont, group, &morph).perc_obj;
    lv_obj_set_style_text_align(perc_morph, LV_TEXT_ALIGN_CENTER, 0);
    flex_newline(label_morph);


    lv_obj_t *cont_1 = containerVert(cont, group).container_obj;
    lv_obj_t *cont_2 = containerVert(cont, group).container_obj;
    lv_obj_t *cont_3 = containerVert(cont, group).container_obj;

    lv_obj_t *label_1 = label(cont_1, group, "Timb").label_obj;
    lv_obj_t *label_2 = label(cont_2, group, "Fm").label_obj;
    lv_obj_t *label_3 = label(cont_3, group, "Morph").label_obj;

    lv_obj_t * bar_timbre = bar(cont_1,
                          group,
                          timbreMod).bar_obj;
    lv_obj_t * bar_fm = bar(cont_2,
                          group,
                          fmMod).bar_obj;
    lv_obj_t * bar_morph = bar(cont_3,
                          group,
                          morphMod).bar_obj;

    flex_newline(cont_1);


    lvgl_port_unlock();

}

void IRAM_ATTR simpleVcoScreen::update()
{
    static float freq_ = 100.0f;
    static float amplitude_ = 100.0f;
    static float harmonics_ = 50.0f;
    static float timbre_ = 50.0f;
    static float morph_ = 50.0f;
    static float timbreMod_ = 50.0f;
    static float fmMod_ = 50.0f;
    static float morphMod_ = 50.0f;

    // float vOct = voltageToNote(inValF[0]);
    SMOOTH_VAL_SCREEN(freq_, freq);
    SMOOTH_VAL_SCREEN(amplitude_, amplitude);
    SMOOTH_VAL_SCREEN(harmonics_, harmonics);
    SMOOTH_VAL_SCREEN(timbre_, timbre);
    SMOOTH_VAL_SCREEN(morph_, morph);
    SMOOTH_VAL_SCREEN(timbreMod_, timbreMod);
    SMOOTH_VAL_SCREEN(fmMod_, fmMod);
    SMOOTH_VAL_SCREEN(morphMod_, morphMod);

    float harmonics_scaled = harmonics_ / 100.0f;
    float timbre_scaled = timbre_ / 100.0f;
    float morph_scaled = morph_ / 100.0f;
    float timbre_mod_scaled = timbreMod_ / 50.0f - 1.0f;
    float fm_mod_scaled = fmMod_ / 50.0f - 1.0f;
    float morph_mod_scaled = morphMod_ / 50.0f - 1.0f;

    float out[BLOCK_SIZE];
    float aux[BLOCK_SIZE];

    plaitsEngine.setPatch(freq_, harmonics_scaled, timbre_scaled, morph_scaled, engine, timbre_mod_scaled, fm_mod_scaled, morph_mod_scaled);
    plaitsEngine.Render(amplitude_ / 100.0f, out, aux, BLOCK_SIZE);

    outputFrames(out, aux);
}

void IRAM_ATTR simpleVcoScreen::refresh()
{

    lvgl_port_lock(0);
    // lv_obj_invalidate(scrn);
    lvgl_port_unlock();
}
