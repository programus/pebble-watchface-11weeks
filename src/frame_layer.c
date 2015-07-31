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

#define MIN_H   83
#define MIN_W   71
#define HOUR_H  81
#define HOUR_W  69

#define FRAME_H 82
#define FRAME_W 70

static const GPathInfo HAND_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint[]) {
    {0, 2},
    {-2, 4},
    {2, 4}
  }
};
static GPath* s_hand_path;

static struct tm* s_now = NULL;

static Layer* s_frame_layer;

static void update_proc(Layer* layer, GContext* ctx);
static void draw_hour_line(GContext* ctx);
static void draw_min_line(GContext* ctx);
static void draw_line(GContext* ctx, int angle, int w, int h, bool is_line);

void frame_layer_update_time(time_t* time, struct tm* tm) {
  s_now = tm;
}

static void update_proc(Layer* layer, GContext* ctx) {
  if (s_now) {
    graphics_context_set_compositing_mode(ctx, GCompOpAnd);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
    
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "minutes...");
    draw_min_line(ctx);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "hours...");
    draw_hour_line(ctx);
  }
}

static void draw_hour_line(GContext* ctx) {
  int min = (s_now->tm_hour % 12) * 60 + s_now->tm_min;
  int32_t angle = (min * TRIG_MAX_ANGLE) / 720;
  draw_line(ctx, angle, FRAME_W, FRAME_H, true);
}

static void draw_min_line(GContext* ctx) {
  int sec = s_now->tm_min * 60 + s_now->tm_sec;
  int32_t angle = sec * TRIG_MAX_ANGLE / 3600;
  draw_line(ctx, angle, FRAME_W, FRAME_H, false);
}

static void draw_line(GContext* ctx, int angle, int w, int h, bool is_line) {
  GPoint center = GPoint(SCREEN_WIDTH >> 1, SCREEN_HEIGHT >> 1);
  GPoint topleft = GPoint(center.x - w - 1, center.y - h - 1);
  GPoint rightbottom = GPoint(center.x + w, center.y + h);
  GPoint hist[] = {
    GPoint(center.x, topleft.y),
    GPoint(rightbottom.x, topleft.y),
    rightbottom,
    GPoint(topleft.x, rightbottom.y),
    topleft
  };
  
  const uint32_t ROTATE_UNIT = TRIG_MAX_ANGLE >> 2;
  
  GPoint p[6];
  int num = 0;
  
  if (angle == 0) {
    if (is_line) {
      return;
    } else {
      p[0].x = center.x;
      p[0].y = topleft.y;
      num++;
    }
  } else if (angle == TRIG_MAX_ANGLE >> 1) {
    memcpy(p, hist, 3 * sizeof(GPoint));
    p[3].x = SCREEN_WIDTH >> 1;
    p[3].y = rightbottom.y;
    num = 4;
  } else {
    int32_t cos = abs(cos_lookup(angle));
    int32_t sin = abs(sin_lookup(angle));
    GPoint end = GPoint(0, 0);
    end.y = h;
    end.x = end.y * sin / cos;
//    APP_LOG(APP_LOG_LEVEL_DEBUG, " -- %d, %d (%d, %d)", (int)cos, (int)sin, end.x, end.y);
    if (end.x > w) {
      end.x = w;
      end.y = cos * end.x / sin;
//      APP_LOG(APP_LOG_LEVEL_DEBUG, " ++ %d, %d (%d, %d)", (int)cos, (int)sin, end.x, end.y);
    }
    
    int trig_90 = TRIG_MAX_ANGLE >> 2;
    
    if (angle >= 0 && angle < trig_90) {
      if (end.y < h) {
        num++;
      }
      end.x += center.x;
      end.y =center.y - end.y - 1;
      num += 1;
    } else if (angle >= trig_90 && angle < (trig_90 << 1)) {
      if (end.x < w) {
        num++;
      }
      end.x += center.x;
      end.y += center.y;
      num += 2;
    } else if (angle >= (trig_90 << 1) && angle < trig_90 * 3) {
      if (end.y < h) {
        num++;
      }
      end.x = center.x - end.x - 1;
      end.y += center.y;
      num += 3;
    } else {
      if (end.x < w) {
        num++;
      }
      end.x = center.x - end.x - 1;
      end.y = center.y - end.y - 1;
      num += 4;
    }
    
    if (is_line) {
      memcpy(p, hist, num * sizeof(GPoint));
    }
    p[num++] = end;
  }
  
  if (is_line) {
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "num: %d, angle: %d/ %d", num, angle, angle * 360 / TRIG_MAX_ANGLE);
    for (int i = 1; i < num; i++) {
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "(%d, %d) -> (%d, %d)", p[i].x, p[i].y, p[i + 1].x, p[i + 1].y);
      graphics_draw_line(ctx, p[i], p[i - 1]);
    }
  } else {
    gpath_move_to(s_hand_path, p[num - 1]);
    gpath_rotate_to(s_hand_path, ROTATE_UNIT * ((num - 2) & 0x03));
    gpath_draw_outline(ctx, s_hand_path);
    gpath_draw_filled(ctx, s_hand_path);
  }
}

void frame_layer_create() {
  s_frame_layer = layer_create(GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
  s_hand_path = gpath_create(&HAND_PATH_INFO);
  layer_set_update_proc(s_frame_layer, update_proc);
}

Layer* frame_layer_get_layer() {
  return s_frame_layer;
}

void frame_layer_destroy() {
  layer_destroy(s_frame_layer);
  gpath_destroy(s_hand_path);
}