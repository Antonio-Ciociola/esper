#define PWM0_GPIO       (12)
#define PWM0_CHANNEL    LEDC_CHANNEL_0
#define PWM1_GPIO       (11)
#define PWM1_CHANNEL    LEDC_CHANNEL_1

#define POT0_GPIO    7
#define POT1_GPIO    8
#define JACK0_GPIO    1
#define JACK1_GPIO    2
#define JACK2_GPIO    3
#define DIGI1_GPIO    13
#define DIGI2_GPIO    14

#define PROB_GPIO    13

#define TIMER_RESOLUTION 5000000 // 5MHz, 1 tick = 0.2us
#define SAMPRATE 48000
#define SAMPSTEP (1/SAMPRATE)
#define BLOCK_SIZE 24
#define FREQUENCY (SAMPRATE/BLOCK_SIZE)
#define TIMER_COUNT (TIMER_RESOLUTION/FREQUENCY)
#define TIMER_PERIOD_MICRO (1000000/FREQUENCY)
#define TIMER_PERIOD_SEC (1/FREQUENCY)

#define MAX_ADC_VAL 2605
#define MAX_ACTUAL_POTS_VAL 290
#define MAX_POTS_VAL 300
#define MAX_JACK_VAL 5430

#define COLOR_FG    lv_color_white()
#define COLOR_BG    lv_color_black()