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

#define FRAME_H 82
#define FRAME_W 70

#define PATH_SZ 2

#define BLINK_DELAY 650

static const GPathInfo HAND_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint[]) {
    {0, 2},
    {-2, 4},
    {2, 4}
  }
};
static const GPoint* const SCALE_POINTS = (GPoint[]) {
  {11, 0},
  {23, 0},
  {24, 0},
  {34, 0},
  {44, 0},
  {53, 0},
  {62, 0},
  {71, 0},
  {72, 0},
  {81, 0},
  {90, 0},
  {99, 0},
  {109, 0},
  {119, 0},
  {120, 0},
  {132, 0},
  {24, 1},
  {71, 1},
  {72, 1},
  {119, 1},
  {24, 2},
  {71, 2},
  {72, 2},
  {119, 2},
  {0, 4},
  {143, 4},
  {0, 19},
  {143, 19},
  {0, 31},
  {143, 31},
  {0, 42},
  {143, 42},
  {0, 43},
  {1, 43},
  {2, 43},
  {141, 43},
  {142, 43},
  {143, 43},
  {0, 51},
  {143, 51},
  {0, 60},
  {143, 60},
  {0, 68},
  {143, 68},
  {0, 76},
  {143, 76},
  {0, 83},
  {1, 83},
  {2, 83},
  {141, 83},
  {142, 83},
  {143, 83},
  {0, 84},
  {1, 84},
  {2, 84},
  {141, 84},
  {142, 84},
  {143, 84},
  {0, 91},
  {143, 91},
  {0, 99},
  {143, 99},
  {0, 107},
  {143, 107},
  {0, 116},
  {143, 116},
  {0, 124},
  {1, 124},
  {2, 124},
  {141, 124},
  {142, 124},
  {143, 124},
  {0, 125},
  {143, 125},
  {0, 136},
  {143, 136},
  {0, 148},
  {143, 148},
  {0, 163},
  {143, 163},
  {24, 165},
  {71, 165},
  {72, 165},
  {119, 165},
  {24, 166},
  {71, 166},
  {72, 166},
  {119, 166},
  {11, 167},
  {23, 167},
  {24, 167},
  {34, 167},
  {44, 167},
  {53, 167},
  {62, 167},
  {71, 167},
  {72, 167},
  {81, 167},
  {90, 167},
  {99, 167},
  {109, 167},
  {119, 167},
  {120, 167},
  {132, 167}
};
static const int SCALE_POINTS_NUM = 104;
static GPath* s_hand_path;

static struct tm* s_now = NULL;
static struct tm  s_prev = {0};

static Layer* s_frame_layer;

static void update_proc(Layer* layer, GContext* ctx);
static void draw_scales(GContext* ctx);
static void draw_hour_line(GContext* ctx);
static void draw_min_line(GContext* ctx);
static void draw_line(GContext* ctx, int angle, int w, int h, bool is_line);
static void blink_hand_pointer(void* data);

void frame_layer_update_time(time_t* time, struct tm* tm) {
  s_now = tm;
}

static void update_proc(Layer* layer, GContext* ctx) {
  if (s_now) {
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
    
    draw_scales(ctx);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "minutes...");
    draw_min_line(ctx);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "hours...");
    draw_hour_line(ctx);
    
    if (memcmp(s_now, &s_prev, sizeof(struct tm))) {
      memcpy(&s_prev, s_now, sizeof(struct tm));
    }
  }
}

static void draw_scales(GContext* ctx) {
  for (int i = 0; i < SCALE_POINTS_NUM; i++) {
    graphics_draw_pixel(ctx, SCALE_POINTS[i]);
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
  
  const uint32_t ROTATE_UNIT = TRIG_MAX_ANGLE >> 3;
  
  static GPoint p[6];
  static int num = 0;
  static int corner = 0;
  
  if (memcmp(s_now, &s_prev, sizeof(struct tm))) {
    num = 0;
    corner = 0;
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
      
      if (w - end.x < PATH_SZ && h - end.y < PATH_SZ) {
        corner = 1;
      }
      
      int trig_90 = TRIG_MAX_ANGLE >> 2;
      
      if (angle >= 0 && angle < trig_90) {
        if (end.y < h) {
          corner = -corner;
          num++;
        }
        end.x += center.x;
        end.y =center.y - end.y - 1;
        num += 1;
      } else if (angle >= trig_90 && angle < (trig_90 << 1)) {
        if (end.x < w) {
          corner = -corner;
          num++;
        }
        end.x += center.x;
        end.y += center.y;
        num += 2;
      } else if (angle >= (trig_90 << 1) && angle < trig_90 * 3) {
        if (end.y < h) {
          corner = -corner;
          num++;
        }
        end.x = center.x - end.x - 1;
        end.y += center.y;
        num += 3;
      } else {
        if (end.x < w) {
          corner = -corner;
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
      gpath_rotate_to(s_hand_path, ROTATE_UNIT * ((((num - 2) & 0x03) << 1) + corner));
      switch (corner) {
        case 1:
          gpath_move_to(s_hand_path, hist[num - 1]);
          break;
        case -1:
          gpath_move_to(s_hand_path, hist[num - 2]);
          break;
        default:
          gpath_move_to(s_hand_path, p[num - 1]);
          break;
      }
      app_timer_register(BLINK_DELAY, blink_hand_pointer, NULL);
      gpath_draw_outline(ctx, s_hand_path);
      gpath_draw_filled(ctx, s_hand_path);
    }
  } else {
    if (is_line) {
  //    APP_LOG(APP_LOG_LEVEL_DEBUG, "num: %d, angle: %d/ %d", num, angle, angle * 360 / TRIG_MAX_ANGLE);
      for (int i = 1; i < num; i++) {
  //      APP_LOG(APP_LOG_LEVEL_DEBUG, "(%d, %d) -> (%d, %d)", p[i].x, p[i].y, p[i + 1].x, p[i + 1].y);
        graphics_draw_line(ctx, p[i], p[i - 1]);
      }
    } else {
      // do nothing to remove pointer.
    }
  }
}

static void blink_hand_pointer(void* data) {
  layer_mark_dirty(s_frame_layer);
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