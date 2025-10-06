#pragma once

#include <cstddef>

#define SMOOTH_VAL_SCREEN(out, inp) (out += (inp - out) * 0.001f)

// class ParameterInterpolator {
// public:
//     ParameterInterpolator(float start, float end, int steps)
//         : current(start), increment((end - start) / steps), steps_left(steps) {}

//     float Next() {
//         if (steps_left > 0) {
//             current += increment;
//             steps_left--;
//         }
//         return current;
//     }

// private:
//     float current;
//     float increment;
//     int steps_left;
// };



class ParameterInterpolator {
 public:
  ParameterInterpolator() { }
  ParameterInterpolator(float* state, float new_value, size_t size) {
    state_ = state;
    value_ = *state;
    increment_ = (new_value - *state) / static_cast<float>(size);
  }

  ParameterInterpolator(float* state, float new_value, float step) {
    state_ = state;
    value_ = *state;
    increment_ = (new_value - *state) * step;
  }

  ~ParameterInterpolator() {
    *state_ = value_;
  }

  inline float Next() {
    value_ += increment_;
    return value_;
  }

  inline float subsample(float t) {
    return value_ + increment_ * t;
  }
  
 private:
  float* state_;
  float value_;
  float increment_;
};



void outputFrames(float *out, float *aux);


float voltageToNote(float voltage);

float voltageToNorm(float voltage);

float voltageToNormPos(float voltage);






inline float ThisBlepSample(float t) {
  return 0.5f * t * t;
}

inline float NextBlepSample(float t) {
  t = 1.0f - t;
  return -0.5f * t * t;
}

inline float NextIntegratedBlepSample(float t) {
  const float t1 = 0.5f * t;
  const float t2 = t1 * t1;
  const float t4 = t2 * t2;
  return 0.1875f - t1 + 1.5f * t2 - t4;
}

inline float ThisIntegratedBlepSample(float t) {
  return NextIntegratedBlepSample(1.0f - t);
}
  
