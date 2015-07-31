//
//  bluetooth_layer.c
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#include "bluetooth_layer.h"
#include "const.h"

#define MARGIN          1
#define SPACING_X       1
#define SPACING_Y       0

static Layer* s_layer = NULL;
static GBitmap* s_bitmap_bluetooth = NULL;

static GBitmap* s_bitmap_yes_no = NULL;
static GBitmap* s_bitmap_yes = NULL;
static GBitmap* s_bitmap_no = NULL;
static bool s_bt_on = false;

static void update_proc(Layer* layer, GContext* ctx);

static void update_proc(Layer* layer, GContext* ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  // draw bluetooth mark
  GRect rect = gbitmap_get_bounds(s_bitmap_bluetooth);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_bluetooth, rect);
  
  // draw yes/no
  GBitmap* bmp = s_bt_on ? s_bitmap_yes : s_bitmap_no;
  GRect rect_yn = gbitmap_get_bounds(bmp);
  rect_yn.origin.x = rect.origin.x + rect.size.w + MARGIN;
  rect_yn.origin.y = rect.origin.y + ((rect.size.h - rect_yn.size.h) >> 1);
  graphics_draw_bitmap_in_rect(ctx, bmp, rect_yn);
}

void bluetooth_layer_create() {
  if (!s_bitmap_bluetooth) {
    s_bitmap_bluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_MARK);
    s_bitmap_yes_no = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YES_NO);
    GRect rect = gbitmap_get_bounds(s_bitmap_yes_no);
    rect.size.w >>= 1;
    s_bitmap_yes = gbitmap_create_as_sub_bitmap(s_bitmap_yes_no, rect);
    rect.origin.x += rect.size.w;
    s_bitmap_no = gbitmap_create_as_sub_bitmap(s_bitmap_yes_no, rect);
  }
  
  if (!s_layer) {
    GRect rect = gbitmap_get_bounds(s_bitmap_bluetooth);
    GRect rect_yn = gbitmap_get_bounds(s_bitmap_yes);
    rect.size.w += MARGIN + rect_yn.size.w;
    rect.origin.x = SX + CW * DW + SPACING_X;
    rect.origin.y = SY + CH * WN + SPACING_Y;
    s_layer = layer_create(rect);
    layer_set_update_proc(s_layer, update_proc);
  }
}

void bluetooth_layer_destroy() {
  if (s_bitmap_bluetooth) {
    gbitmap_destroy(s_bitmap_yes);
    gbitmap_destroy(s_bitmap_no);
    gbitmap_destroy(s_bitmap_yes_no);
    gbitmap_destroy(s_bitmap_bluetooth);
    s_bitmap_bluetooth = NULL;
  }
  
  if (s_layer) {
    layer_destroy(s_layer);
    s_layer = NULL;
  }
}

Layer* bluetooth_layer_get_layer() {
  return s_layer;
}

void bluetooth_layer_update(bool is_bt_on) {
  s_bt_on = is_bt_on;
}