#include "engines/utils.h"
#include "utils/values.h"
#include <math.h>

void outputFrames(float *out, float *aux) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        outFrames[i].out = out[i];
        outFrames[i].aux = aux[i];
    }
}

float voltageToNote(float voltage){
    return powf(2.0f, voltage);
}

float voltageToNorm(float voltage){
    return voltage / 10.0f; 
}

float voltageToNormPos(float voltage){
    return std::max(0.0f, voltageToNorm(voltage));
}