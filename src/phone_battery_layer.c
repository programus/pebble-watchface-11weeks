//
//  phone_battery_layer.c
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#include "phone_battery_layer.h"
#include "const.h"
#include "numbers.h"
#include "letters.h"

#define MARGIN          1
#define SPACING_X       1
#define SPACING_Y       -1

#define UNKNOW_LEVEL    "XX"
#define FULL_LEVEL      "FL"

static Layer* s_layer = NULL;
static GBitmap* s_bitmap_battery = NULL;
static GBitmap* s_bitmap_battery_charging = NULL;
static uint8_t s_charge_state = 0;

static void update_proc(Layer* layer, GContext* ctx);

static void update_proc(Layer* layer, GContext* ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  GRect rect = gbitmap_get_bounds(s_bitmap_battery);
  // get number
  int num = s_charge_state & LEVEL_MASK;
  GPoint p = {
//    .x = rect.size.w + MARGIN,
    .x = 0,
    .y = ((rect.size.h - TN_HEIGHT) >> 1) + 1
  };
  if (num < 100) {
    graphics_draw_tiny_number(ctx, num / 10, p.x, p.y);
    p.x += MARGIN + TN_WIDTH;
    graphics_draw_tiny_number(ctx, num, p.x, p.y);
  } else if (num == 100){
    graphics_draw_tiny_string(ctx, FULL_LEVEL, p.x, p.y, MARGIN);
    p.x += MARGIN + TN_WIDTH;
  } else if (num == LEVEL_UNKNOWN){
    graphics_draw_tiny_string(ctx, UNKNOW_LEVEL, p.x, p.y, MARGIN);
    p.x += MARGIN + TN_WIDTH;
  }
  
  if (num != BATTERY_API_UNSUPPORTED) {
    // draw battery mark
    GBitmap* bmp = (s_charge_state & CHARGING_MASK) ? s_bitmap_battery_charging : s_bitmap_battery;
    rect.origin.x = p.x + MARGIN + TN_WIDTH;
    graphics_draw_bitmap_in_rect(ctx, bmp, rect);
  }
}

void phone_battery_layer_create() {
  if (!s_bitmap_battery) {
    s_bitmap_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_BATTERY);
    s_bitmap_battery_charging = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_BATTERY_CHARGING);
  }
  
  if (!s_layer) {
    GRect rect = gbitmap_get_bounds(s_bitmap_battery);
    rect.size.w += (MARGIN + TN_WIDTH) << 1;
    rect.origin.x = SX + CW * DW + SPACING_X;
    rect.origin.y = SY + CH * WN + SPACING_Y;
    s_layer = layer_create(rect);
    layer_set_update_proc(s_layer, update_proc);
  }
}

void phone_battery_layer_destroy() {
  if (s_bitmap_battery) {
    gbitmap_destroy(s_bitmap_battery_charging);
    s_bitmap_battery_charging = NULL;
    gbitmap_destroy(s_bitmap_battery);
    s_bitmap_battery = NULL;
  }
  
  if (s_layer) {
    layer_destroy(s_layer);
    s_layer = NULL;
  }
}

Layer* phone_battery_layer_get_layer() {
  return s_layer;
}

void phone_battery_layer_update(uint8_t state) {
  s_charge_state = state;
}