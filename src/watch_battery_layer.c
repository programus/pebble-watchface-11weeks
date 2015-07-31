//
//  watch_battery_layer.c
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#include "watch_battery_layer.h"
#include "const.h"
#include "numbers.h"
#include "letters.h"

#define MARGIN    2
#define SPACING_X 2
#define SPACING_Y -1

static Layer* s_layer = NULL;
static GBitmap* s_bitmap_battery = NULL;
static GBitmap* s_bitmap_battery_charging = NULL;
static BatteryChargeState s_charge_state = {0};

static void update_proc(Layer* layer, GContext* ctx);

static void update_proc(Layer* layer, GContext* ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  GRect rect = gbitmap_get_bounds(s_bitmap_battery);
  // draw battery mark
  GBitmap* bmp = s_charge_state.is_charging ? s_bitmap_battery_charging : s_bitmap_battery;
  graphics_draw_bitmap_in_rect(ctx, bmp, rect);
  // get number
  int num = s_charge_state.charge_percent / 10;
  GPoint p = {
    .x = rect.size.w + MARGIN,
    .y = ((rect.size.h - TN_HEIGHT) >> 1) + 1
  };
  if (num < 10) {
    graphics_draw_tiny_number(ctx, num, p.x, p.y);
  } else {
    graphics_draw_tiny_letter(ctx, 'F', p.x, p.y);
  }
}

void watch_battery_layer_create() {
  if (!s_bitmap_battery) {
    s_bitmap_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WATCH_BATTERY);
    s_bitmap_battery_charging = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WATCH_BATTERY_CHARGING);
  }
  
  if (!s_layer) {
    GRect rect = gbitmap_get_bounds(s_bitmap_battery);
    rect.size.w += MARGIN + TN_WIDTH;
    rect.origin.x = SX - SPACING_X - 1 - rect.size.w;
    rect.origin.y = SY + CH * WN + SPACING_Y;
    s_layer = layer_create(rect);
    layer_set_update_proc(s_layer, update_proc);
  }
}

void watch_battery_layer_destroy() {
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

Layer* watch_battery_layer_get_layer() {
  return s_layer;
}

void watch_battery_layer_update(BatteryChargeState state) {
  s_charge_state = state;
}