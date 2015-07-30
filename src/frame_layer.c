//
//  frame_layer.c
//  pebble-watchface-months
//
//  Created by Programus on 2015/6/27.
//
//

#include "const.h"
#include "frame_layer.h"
#include "numbers.h"

#include "frame_lines_data.h"

static struct tm* s_now = NULL;

static Layer* s_frame_layer;

static void update_proc(Layer* layer, GContext* ctx);
static void draw_hour_line(GContext* ctx);
static void draw_min_line(GContext* ctx);
static void draw_line(GContext* ctx, TimeFrame* line_data);

void frame_layer_update_time(time_t* time, struct tm* tm) {
  s_now = tm;
}

static void update_proc(Layer* layer, GContext* ctx) {
  if (s_now) {
    graphics_context_set_compositing_mode(ctx, GCompOpAnd);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
    
    draw_min_line(ctx);
    draw_hour_line(ctx);
  }
}

static void draw_hour_line(GContext* ctx) {
  int min = (s_now->tm_hour % 12) * 60 + s_now->tm_min;
  TimeFrame* line_data = s_hour_lines + min;
  draw_line(ctx, line_data);
}

static void draw_min_line(GContext* ctx) {
  int sec = s_now->tm_min * 60 + s_now->tm_sec;
  TimeFrame* line_data = s_min_lines + sec;
  draw_line(ctx, line_data);
}

static void draw_line(GContext* ctx, TimeFrame* line_data) {
  int n = line_data->number;
  uint16_t* points = line_data->points;
  GPoint start = GPoint(points[0] >> 8, points[0] & 0xff);
  for (int i = 1; i < n; i++) {
    GPoint end = GPoint(points[i] >> 8, points[i] & 0xff);
    graphics_draw_line(ctx, start, end);
    start = end;
  }
}

void frame_layer_create() {
  s_frame_layer = layer_create(GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
  layer_set_update_proc(s_frame_layer, update_proc);
}

Layer* frame_layer_get_layer() {
  return s_frame_layer;
}

void frame_layer_destroy() {
  layer_destroy(s_frame_layer);
}