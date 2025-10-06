#include "esp_timer.h"
#include "esp_log.h"
#include <math.h>


#include "utils/benchmark.h"

const static char *TAG = "BENCHMARK";


#pragma GCC optimize("O0")
float function(){
    const int N = 100;
    float a = 1.1;
    for (int i = 0; i < N; i++){
        // a += sin(i);
        // a /= 1.1;
        a /= 1.1;
    }
    return N;
}

void benchmark(){

    int64_t start_time = esp_timer_get_time();
    int64_t elapsed_time = 0;

    int count = 0;
    do {
        function();
        count++;
        elapsed_time = esp_timer_get_time() - start_time;
    } while (elapsed_time < 1000000); // 1 second

    int64_t time_per_function = elapsed_time / count;
    ESP_LOGI(TAG, "Time per function call: %lld µs", time_per_function);

}