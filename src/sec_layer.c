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
#define MARK_H    2

static struct tm* s_now;

static Layer* s_sec_layer;

static void update_time();
static void update_proc(Layer* layer, GContext* ctx);
static void draw_sec_number(GContext* ctx);
static void draw_sec_marks(GContext* ctx);
static void update_sec_frame(Layer* layer);

static void update_time() {
  time_t t = time(NULL);
  s_now = localtime(&t);
}

static void update_proc(Layer* layer, GContext* ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  update_time();
  draw_sec_marks(ctx);
  draw_sec_number(ctx);
  update_sec_frame(layer);
}

static void draw_sec_number(GContext* ctx) {
  int x = (SEC_W >> 1) - TN_WIDTH;
  int y = 0;
  graphics_draw_tiny_number(ctx, s_now->tm_sec / 10, x, y);
  graphics_draw_tiny_number(ctx, s_now->tm_sec % 10, x + TN_WIDTH + 1, y);
}

static void draw_sec_marks(GContext* ctx) {
  GRect rect = GRect(((CW - MARK_W) >> 1) - 1, (SEC_H - MARK_H) >> 1, MARK_W, MARK_H);
  for (int i = 0; i < DW; i++) {
    if (i != 3) {
      graphics_draw_rect(ctx, rect);
    }
    rect.origin.x += CW;
  }
}

static void update_sec_frame(Layer* layer) {
  int margin = (CW - MARK_W);
  int lost = (margin >> 1) + (s_now->tm_sec >> 1);
  lost += s_now->tm_sec / (MARK_W << 1) * margin;
  GRect frame = GRect(SEC_SX, SEC_SY, SEC_W, SEC_H);
  GRect bounds = frame;
  int l_lost = lost + (s_now->tm_sec & 1);
  int r_lost = lost;
  frame.origin.x += l_lost;
  frame.size.w -= l_lost + r_lost;
  layer_set_frame(layer, frame);
  bounds.origin.x = -l_lost;
  bounds.origin.y = 0;
  layer_set_bounds(layer, bounds);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "frame: (%d, %d, %d, %d) / bounds: (%d, %d, %d, %d)", frame.origin.x, frame.origin.y, frame.size.w, frame.size.h, bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
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