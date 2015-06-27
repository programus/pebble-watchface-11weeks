#include <pebble.h>
#include "calendar_layer.h"
#include "const.h"
#include "numbers.h"
  
typedef uint8_t buffer_t;

#define MIN_END_MON   28  // min days in a month

static GBitmap* s_bitmap_background;
static Layer* s_layer_calendar;

// a pointer to painted background buffer.
static buffer_t* s_bg_buffer = NULL;
static size_t s_bg_buffer_size = 0;
static GSize s_bg_size = {0};
// row size byte for background buffer.
static int s_bg_row_size_byte;

// variable to store current time in time_t
static time_t now_t;
// variable to store current time in struct tm
static struct tm now;

static void update_time();
static void calendar_layer_update_proc(Layer* layer, GContext* ctx);
static void calendar_layer_draw_time(GContext* ctx);
static void calendar_layer_draw_sec(GContext* ctx);
static void calendar_layer_draw_dates(GContext* ctx);
static void calendar_layer_draw_date(GContext* ctx, int wday, int week, int mday, bool is_today);
static void update_bg_buffer(GContext* ctx);
static void destroy_bg_buffer();
static uint8_t get_pixel_from_buffer(int x, int y);

static void update_bg_buffer(GContext* ctx) {
  GBitmap* bmp = graphics_capture_frame_buffer(ctx);
  GRect bounds = gbitmap_get_bounds(bmp);
  memcpy(&s_bg_size, &(bounds.size), sizeof(GSize));
  
  buffer_t* frame_buffer = gbitmap_get_data(bmp);
  s_bg_row_size_byte = gbitmap_get_bytes_per_row(bmp);
#ifdef PBL_BW
  size_t size = s_bg_row_size_byte * bounds.size.h;
#elif PBL_COLOR
  size_t size = bounds.size.w * bounds.size.h;
#endif
  if (!s_bg_buffer) {
    s_bg_buffer_size = size;
    s_bg_buffer = (buffer_t*) malloc(s_bg_buffer_size * sizeof(buffer_t));
  } else if (s_bg_buffer_size < size) {
    s_bg_buffer_size = size;
    s_bg_buffer = (buffer_t*) realloc(s_bg_buffer, s_bg_buffer_size * sizeof(buffer_t));
  }
  memcpy(s_bg_buffer, frame_buffer, s_bg_buffer_size * sizeof(buffer_t));
  graphics_release_frame_buffer(ctx, bmp);
  
//   // log all frame buffer
//   char* buff = (char*) calloc(sizeof(char), s_bg_size.w + 1);
  
//   int index = 0;
//   for (int y = 0; y < s_bg_size.h; y++) {
//     for (int x = 0; x < s_bg_size.w; x++) {
//       int di = x >> 3;
//       int shift = x & 07;
//       buffer_t mask = 1 << shift;
//       if ((s_bg_buffer[index + di] & mask) == mask) {
//         buff[x] = 'X';
//       } else {
//         buff[x] = '.';
//       }
//     }
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "%3d: %144s", y, buff);
//     index += s_bg_row_size_byte;
//   }
//   free(buff);
}

static void destroy_bg_buffer() {
  if (s_bg_buffer) {
    free(s_bg_buffer);
    s_bg_buffer = NULL;
    s_bg_buffer_size = 0;
  }
}

static uint8_t get_pixel_from_buffer(int x, int y) {
#ifdef PBL_BW
  int index = y * (s_bg_row_size_byte) + (x >> 3);
  buffer_t shift = x & 0x07;
  buffer_t mask = 1 << shift;
  return (s_bg_buffer[index] & mask) ? 1 : 0;
#elif PBL_COLOR
  int index = y * s_bg_size.w + x;
  return s_bg_buffer[index];
#endif
}

static void update_time() {
  time(&now_t);
  struct tm* st = localtime(&now_t);
  memcpy(&now, st, sizeof(now));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Now: %d-%d-%d %d:%d:%d", 
          now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
}

static void calendar_layer_update_proc(Layer* layer, GContext* ctx) {
  // update current time
  update_time();
  
  // draw second
  calendar_layer_draw_sec(ctx);
  // draw time
  calendar_layer_draw_time(ctx);
  
  // draw calendar grids
  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  GRect rect = gbitmap_get_bounds(s_bitmap_background);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_background, rect);
  
  // draw calendar
  calendar_layer_draw_dates(ctx);
}

static void calendar_layer_draw_time(GContext* ctx) {
  int x = SX;
  int y = SY;
  int dx = CW << 2;
  int dy = CH * 6;
  graphics_draw_big_number(ctx, now.tm_hour / 10, x, y);
  x += dx;
  graphics_draw_big_number(ctx, now.tm_hour % 10, x, y);
  y += dy;
  graphics_draw_big_number(ctx, now.tm_min % 10, x, y);
  x -= dx;
  graphics_draw_big_number(ctx, now.tm_min / 10, x, y);
}

static void calendar_layer_draw_sec(GContext* ctx) {
  int x = (SCREEN_WIDTH >> 1) - TN_WIDTH - 1;
  int y = SEC_SY + DY;
  graphics_draw_tiny_number(ctx, now.tm_sec / 10, x, y);
  graphics_draw_tiny_number(ctx, now.tm_sec % 10, x + TN_WIDTH + 1, y);
}

static void calendar_layer_draw_dates(GContext* ctx) {
  // update background buffer
  update_bg_buffer(ctx);
  
  // at least 1 previous weeks
  time_t t = now_t - 604800;        // 3600 * 24 * 7
  struct tm* st = localtime(&t);
  // get the 1st week of this calendar.
  for (st->tm_mday -= st->tm_wday; st->tm_mday > 1; st->tm_mday -= 7);
  mktime(st);
  
  for (int week = 0; week < WN; week++) {
    for (int wday = 0; wday < DW; wday++) {
      calendar_layer_draw_date(ctx, wday, week, st->tm_mday, 
                               (st->tm_mon == now.tm_mon && st->tm_mday == now.tm_mday && st->tm_year == now.tm_year));
      st->tm_mday++;
      if (st->tm_mday >= MIN_END_MON) {
        mktime(st);
      }
    }
  }
}

static void calendar_layer_draw_date(GContext* ctx, int wday, int week, int mday, bool is_today) {
  GPoint start_point = GPoint(SX + DX + CW * wday, SY + DY + CH * week);
  bool is_black_bg = !get_pixel_from_buffer(start_point.x - DX + 1, start_point.y - DY + 1);
  graphics_context_set_compositing_mode(ctx, is_black_bg ? GCompOpAssign : GCompOpAssignInverted);
  if (mday > 9) {
    graphics_draw_tiny_number(ctx, mday / 10, start_point.x, start_point.y);
  }
  graphics_draw_tiny_number(ctx, mday % 10, start_point.x + 4, start_point.y);
  if (is_today) {
    // mark current day
    GRect rect_mark = GRect(start_point.x - DX + 1, start_point.y - DX + 0, CW - 3, CH - 1);
    graphics_context_set_stroke_color(ctx, is_black_bg ? GColorWhite : GColorBlack);
    graphics_draw_rect(ctx, rect_mark);
    // mark the week
    int x = SX - 2;
    int y = rect_mark.origin.y + (rect_mark.size.h >> 1);
    GPoint p[3] = {
      GPoint(x - 2, y - 2),
      GPoint(x, y),
      GPoint(x - 2, y + 2)
    };
    graphics_context_set_stroke_color(ctx, GColorWhite);
    for (int i = 0; i < 2; i++) {
      graphics_draw_pixel(ctx, p[i]);
      graphics_draw_line(ctx, p[i], p[i + 1]);
    }
    // mark weekday
    rect_mark.size.h -= 2;
    rect_mark.origin.y = SY - rect_mark.size.h;
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_rect(ctx, rect_mark);
  }
}

void calendar_layer_create() {
  // create background image.
  s_bitmap_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  // get background image bounds.
  GRect bounds = gbitmap_get_bounds(s_bitmap_background);
  // create calendar layer
  s_layer_calendar = layer_create(bounds);
  // set custom update proc
  layer_set_update_proc(s_layer_calendar, calendar_layer_update_proc);
}

Layer* calendar_layer_get_layer() {
  return s_layer_calendar;
}

void calendar_layer_destroy() {
  destroy_bg_buffer();
  layer_destroy(s_layer_calendar);
  gbitmap_destroy(s_bitmap_background);
}
