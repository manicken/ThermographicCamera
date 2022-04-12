
#ifndef ADAFRUIT_INTERPOLATION_H
#define ADAFRUIT_INTERPOLATION_H

#include <Arduino.h>

// the following is set to match the target processor, for maximum performance
#define INT_TYPE int32_t 

float get_point(float *p, INT_TYPE rows, INT_TYPE cols, INT_TYPE x, INT_TYPE y);
void set_point(float *p, INT_TYPE rows, INT_TYPE cols, INT_TYPE x, INT_TYPE y, float f);
void get_adjacents_1d(float *src, float *dest, INT_TYPE rows, INT_TYPE cols, INT_TYPE x, INT_TYPE y);
void get_adjacents_2d(float *src, float *dest, INT_TYPE rows, INT_TYPE cols, INT_TYPE x, INT_TYPE y);
float cubicInterpolate(float p[], float x);
float bicubicInterpolate(float p[], float x, float y);
void interpolate_image(float *src, INT_TYPE src_rows, INT_TYPE src_cols, 
                       float *dest, INT_TYPE dest_rows, INT_TYPE dest_cols);

#endif