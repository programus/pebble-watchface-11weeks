//
//  sec_layer.c
//  pebble-watchface-months
//
//  Created by Programus on 2015/6/27.
//
//

#include "const.h"
#include "sec_layer.h"
#include "numbers.h"

#define MARK_W    10
#define MARK_H    3

static struct tm* s_now = NULL;

static Layer* s_sec_layer;

static void update_proc(Layer* layer, GContext* ctx);
static void draw_sec_number(GContext* ctx);
static void draw_sec_marks(GContext* ctx);

void sec_layer_update_time(time_t* time, struct tm* tm) {
  s_now = tm;
}

static void update_proc(Layer* layer, GContext* ctx) {
  if (s_now) {
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
    
    draw_sec_marks(ctx);
    draw_sec_number(ctx);
  }
}

static void draw_sec_number(GContext* ctx) {
  int x = (SEC_W >> 1) - TN_WIDTH;
  int y = 0;
  graphics_draw_tiny_number(ctx, s_now->tm_sec / 10, x, y);
  graphics_draw_tiny_number(ctx, s_now->tm_sec % 10, x + TN_WIDTH + 1, y);
}

static void draw_sec_marks(GContext* ctx) {
  GRect base_rect = GRect(((CW - MARK_W) >> 1) - 1, (SEC_H - MARK_H) >> 1, MARK_W, MARK_H);
  for (int i = 0; i < DW; i++) {
    GRect rect = base_rect;
    int index = (i > 3) ? i - 1 : i;
    int cent = s_now->tm_sec / 10;
    if (index <= cent && i != 3) {
      if (index == cent) {
        rect.size.w = s_now->tm_sec % 10;
      }
      graphics_fill_rect(ctx, rect, 0, GCornerNone);
    }
    base_rect.origin.x += CW;
  }
}

void sec_layer_create() {
  s_sec_layer = layer_create(GRect(SEC_SX, SEC_SY, SEC_W, SEC_H));
  layer_set_update_proc(s_sec_layer, update_proc);
}

Layer* sec_layer_get_layer() {
  return s_sec_layer;
}

void sec_layer_destroy() {
  layer_destroy(s_sec_layer);
}