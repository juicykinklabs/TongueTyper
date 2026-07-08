#pragma once
#include <Arduino.h>

inline double mapfloat(double x, double in_min, double in_max, double out_min, double out_max) {
double run = in_max - in_min;
  if (run == 0) {
    return NAN;
  }
  double rise = out_max - out_min;
  double delta = x - in_min;
  return (delta * rise) / run + out_min;
}