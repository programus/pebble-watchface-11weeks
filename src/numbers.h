#pragma once
#include "necessary.h"

#define NUM_COUNT  10

#define TN_WIDTH  3
#define TN_HEIGHT 5
  
#define BN_WIDTH  (TN_WIDTH * CW - 1)
#define BN_HEIGHT (TN_HEIGHT * CH - 1)

void numbers_create();
void numbers_destroy();
void graphics_draw_tiny_number(GContext* ctx, int number, int x, int y);
void graphics_draw_tiny_number_rc(GContext* ctx, int number, int x, int y);
void graphics_draw_big_number(GContext* ctx, int number, int x, int y);