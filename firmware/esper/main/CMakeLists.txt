file(GLOB screens_srcs "src/screens/*/*.c"   "src/screens/*/*.cpp")
file(GLOB widgets_srcs "src/widgets/*/*.c" "src/widgets/*/*.cpp")
file(GLOB_RECURSE plaits_srcs "src/plaits/*.cc")


set(srcs
       "src/main.c"
       "src/peripherals/display/display.c"
       "src/peripherals/digital/digital.c"
       "src/peripherals/probe/probe.c"
       "src/peripherals/encoder/encoder.c"
       "src/peripherals/lvgl/lvgl.c"
       "src/peripherals/i2s/i2s.c"
       "src/peripherals/adcSpi/adcSpi.c"
       "src/peripherals/adc/testadc.c"
       "src/update.c"
       "src/utils/utils.c"
       "src/utils/benchmark.c"
       "src/utils/i2cscan.c"
       "src/graphic.cpp"
       "src/screens/screen.cpp"
       "src/screens/widgets.cpp"
       "src/engines/utils.cpp"
       "src/engines/tables.c"
       ${screens_srcs}
       ${widgets_srcs}
       ${plaits_srcs}
       )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "src"
                       REQUIRES esp_driver_dac esp_driver_gpio esp_driver_gptimer  esp_adc esp_phy esp_driver_ledc  esp_lcd encoder esp_idf_lib_helpers ADS1115 stmlib
                       )

target_compile_definitions(${COMPONENT_LIB} PRIVATE TEST)
# Add custom compile options
target_compile_options(${COMPONENT_LIB} PRIVATE
  -mtext-section-literals
  -w  # Disable all warnings
)

# # Apply -mtext-section-literals only for graphic.cpp
# set_source_files_properties("graphic.cpp" PROPERTIES COMPILE_OPTIONS "-mtext-section-literals")
