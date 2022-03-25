
#ifndef ADAFRUIT_INTERPOLATION_H
#define ADAFRUIT_INTERPOLATION_H

#include <Arduino.h>

float get_point(float *p, int16_t rows, int16_t cols, int16_t x, int16_t y);
void set_point(float *p, int16_t rows, int16_t cols, int16_t x, int16_t y, float f);
void get_adjacents_1d(float *src, float *dest, int16_t rows, int16_t cols, int16_t x, int16_t y);
void get_adjacents_2d(float *src, float *dest, int16_t rows, int16_t cols, int16_t x, int16_t y);
float cubicInterpolate(float p[], float x);
float bicubicInterpolate(float p[], float x, float y);
void interpolate_image(float *src, int16_t src_rows, int16_t src_cols, 
                       float *dest, int16_t dest_rows, int16_t dest_cols);

#endif