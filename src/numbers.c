#include <pebble.h>
#include "const.h"
#include "numbers.h"

// numbers 3x5 in one pic
static GBitmap* s_bitmap_number = NULL;
// numbers 3x5: 0-9
static GBitmap* s_bitmap_numbers[NUM_COUNT];
// big numbers 3x5 in one pic
static GBitmap* s_bitmap_big_number;
// big numbers 3x5: 0-9
static GBitmap* s_bitmap_big_numbers[NUM_COUNT];


void numbers_create() {
  if (!s_bitmap_number) {
    s_bitmap_number = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBER_3X5);
    GRect rect = GRect(0, 0, TN_WIDTH, TN_HEIGHT);
    for (int i = 0; i < NUM_COUNT; i++) {
      s_bitmap_numbers[i] = gbitmap_create_as_sub_bitmap(s_bitmap_number, rect);
      rect.origin.x += rect.size.w;
    }
  }
  if (!s_bitmap_big_number) {
    s_bitmap_big_number = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BIG_NUMBER_3X5);
    GRect rect = GRect(0, 0, BN_WIDTH, BN_HEIGHT);
    for (int i = 0; i < NUM_COUNT; i++) {
      s_bitmap_big_numbers[i] = gbitmap_create_as_sub_bitmap(s_bitmap_big_number, rect);
      rect.origin.x += rect.size.w + 1;
    }
  }
}

void numbers_destroy() {
  if (s_bitmap_number) {
    for (int i = 0; i < NUM_COUNT; i++) {
      gbitmap_destroy(s_bitmap_numbers[i]);
    }
    gbitmap_destroy(s_bitmap_number);
  }
  if (s_bitmap_big_number) {
    for (int i = 0; i < NUM_COUNT; i++) {
      gbitmap_destroy(s_bitmap_big_numbers[i]);
    }
    gbitmap_destroy(s_bitmap_big_number);
  }
  
  s_bitmap_number = NULL;
}

void graphics_draw_tiny_number(GContext* ctx, int number, int x, int y) {
  GRect rect = GRect(x, y, TN_WIDTH, TN_HEIGHT);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_numbers[number % 10], rect);
}

void graphics_draw_big_number(GContext* ctx, int number, int x, int y) {
  GRect rect = GRect(x, y, BN_WIDTH, BN_HEIGHT);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_big_numbers[number % 10], rect);
}
