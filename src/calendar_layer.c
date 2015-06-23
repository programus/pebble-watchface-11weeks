#include <pebble.h>
#include "calendar_layer.h"

#define N2X5  4
#define N3X5  10

static GBitmap* s_bitmap_background;
static Layer* s_layer_calendar;

// numbers 2x5: 0-3
static GBitmap* number_2x5[N2X5];
// numbers 3x5: 0-9
static GBitmap* number_3x5[N3X5];

static void loadnumbers();

static void load_numbers() {
}