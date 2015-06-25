#include <pebble.h>
#include "calendar_layer.h"
  
typedef uint8_t buffer_t;
#define SCREEN_WIDTH  144
#define SCREEN_HEIGHT 168

#define N2X5  4
#define N3X5  10
  
#define SX    23    // start point x
#define SY    19    // start point y
#define DX    3     // margin x
#define DY    3     // margin y
#define CW    14    // cell width
#define CH    12    // cell height
#define WN    11    // number of weeks
  
#define DW    7     // days in a week
#define MIN_END_MON   28  // min days in a month

static GBitmap* s_bitmap_background;
static Layer* s_layer_calendar;

// a pointer to painted background buffer.
static buffer_t* s_bg_buffer = NULL;
static size_t s_bg_buffer_size = 0;
static GSize s_bg_size = {0};
// row size byte for background buffer.
static int s_bg_row_size_byte;

// // numbers 2x5 in one pic
// static GBitmap* s_bitmap_number_2x5;
// // numbers 2x5: 0-3
// static GBitmap* s_bitmap_numbers_2x5[N2X5];
// numbers 3x5 in one pic
static GBitmap* s_bitmap_number_3x5;
// numbers 3x5: 0-9
static GBitmap* s_bitmap_numbers_3x5[N3X5];
// big numbers 3x5 in one pic
static GBitmap* s_bitmap_big_number_3x5;
// big numbers 3x5: 0-9
static GBitmap* s_bitmap_big_numbers_3x5[N3X5];

// variable to store current time in time_t
static time_t now_t;
// variable to store current time in struct tm
static struct tm now;

static void create_numbers();
static void destroy_numbers();
static void update_time();
static void calendar_layer_update_proc(Layer* layer, GContext* ctx);
static void calendar_layer_draw_time(GContext* ctx);
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "s_bg_size = (%d, %d), s_bg_row_size_byte = %d, s_bg_buffer_size = %d, size = %d", 
          s_bg_size.w, s_bg_size.h, s_bg_row_size_byte, s_bg_buffer_size, size);
  memcpy(s_bg_buffer, frame_buffer, s_bg_buffer_size * sizeof(buffer_t));
  graphics_release_frame_buffer(ctx, bmp);
  
  char* buff = (char*) malloc(s_bg_size.w);
  
  int index = 0;
  for (int y = 0; y < s_bg_size.h; y++) {
    for (int x = 0; x < s_bg_size.w; x++) {
      int di = x >> 3;
      int shift = x & 07;
      buffer_t mask = 0x80 >> shift;
      if (s_bg_buffer[index + di] & mask) {
        buff[x] = 'X';
      } else {
        buff[x] = '.';
      }
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%3d, %4d: %s", y, index, buff);
    index += s_bg_row_size_byte;
  }
  free(buff);
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
  buffer_t mask = 0x80 >> shift;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "(%d, %d) => %d, %x", x, y, index, mask);
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
  GRect rect = GRect(SX, SY, CW * 3 - 1, CH * 5 - 1);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_big_numbers_3x5[now.tm_hour / 10], rect);
  rect.origin.x += rect.size.w + 1 + CW;
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_big_numbers_3x5[now.tm_hour % 10], rect);
  rect.origin.y += rect.size.h + 1 + CH;
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_big_numbers_3x5[now.tm_min % 10], rect);
  rect.origin.x -= rect.size.w + 1 + CW;
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_big_numbers_3x5[now.tm_min / 10], rect);
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
  GRect rect3x5 = GRect(start_point.x, start_point.y, 3, 5);
  bool is_black_bg = !get_pixel_from_buffer(start_point.x - DX + 1, start_point.y - DY + 1);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "(%d, %d) == (%d, %d) -> %d", wday, week, start_point.x, start_point.y, is_black_bg);
  graphics_context_set_compositing_mode(ctx, is_black_bg ? GCompOpAssign : GCompOpAssignInverted);
  if (mday > 9) {
    graphics_draw_bitmap_in_rect(ctx, s_bitmap_numbers_3x5[mday / 10], rect3x5);
  }
  rect3x5.origin.x += 4;
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_numbers_3x5[mday % 10], rect3x5);
  if (is_today) {
    GRect rect_mark = GRect(start_point.x - DX + 1, start_point.y - DX + 0, CW - 3, CH - 1);
    graphics_context_set_stroke_color(ctx, is_black_bg ? GColorWhite : GColorBlack);
    graphics_draw_rect(ctx, rect_mark);
  }
}

void calendar_layer_create() {
  // create background image.
  s_bitmap_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  // create all numbers images.
  create_numbers();
  // get background image bounds.
  GRect bounds = gbitmap_get_bounds(s_bitmap_background);
  // create calendar layer
  s_layer_calendar = layer_create(bounds);
  // set custom update proc
  layer_set_update_proc(s_layer_calendar, calendar_layer_update_proc);
}

Layer* calendar_layer_get() {
  return s_layer_calendar;
}

void calendar_layer_destroy() {
  destroy_bg_buffer();
  layer_destroy(s_layer_calendar);
  destroy_numbers();
  gbitmap_destroy(s_bitmap_background);
}

static void create_numbers() {
//   s_bitmap_number_2x5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBER_2X5);
//   GRect rect = GRect(0, 0, 2, 5);
//   for (int i = 0; i < N2X5; i++) {
//     s_bitmap_numbers_2x5[i] = gbitmap_create_as_sub_bitmap(s_bitmap_number_2x5, rect);
//     rect.origin.x += rect.size.w;
//   }
  
  s_bitmap_number_3x5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBER_3X5);
  GRect rect = GRect(0, 0, 3, 5);
  for (int i = 0; i < N3X5; i++) {
    s_bitmap_numbers_3x5[i] = gbitmap_create_as_sub_bitmap(s_bitmap_number_3x5, rect);
    rect.origin.x += rect.size.w;
  }
  s_bitmap_big_number_3x5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BIG_NUMBER_3X5);
  rect = GRect(0, 0, 3 * CW - 1, 5 * CH - 1);
  for (int i = 0; i < N3X5; i++) {
    s_bitmap_big_numbers_3x5[i] = gbitmap_create_as_sub_bitmap(s_bitmap_big_number_3x5, rect);
    rect.origin.x += rect.size.w + 1;
  }
}

static void destroy_numbers() {
//   for (int i = 0; i < N2X5; i++) {
//     gbitmap_destroy(s_bitmap_numbers_2x5[i]);
//   }
//   gbitmap_destroy(s_bitmap_number_2x5);
  
  for (int i = 0; i < N3X5; i++) {
    gbitmap_destroy(s_bitmap_numbers_3x5[i]);
  }
  gbitmap_destroy(s_bitmap_number_3x5);
  
  for (int i = 0; i < N3X5; i++) {
    gbitmap_destroy(s_bitmap_big_numbers_3x5[i]);
  }
  gbitmap_destroy(s_bitmap_big_number_3x5);
}