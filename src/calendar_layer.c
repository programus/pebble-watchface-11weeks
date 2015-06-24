#include <pebble.h>
#include "calendar_layer.h"

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

// numbers 2x5 on glance
static GBitmap* s_bitmap_number_2x5;
// numbers 2x5: 0-3
static GBitmap* s_bitmap_numbers_2x5[N2X5];
// numbers 3x5 on glance
static GBitmap* s_bitmap_number_3x5;
// numbers 3x5: 0-9
static GBitmap* s_bitmap_numbers_3x5[N3X5];

static void create_numbers();
static void destroy_numbers();
static void calendar_layer_update_proc(Layer* layer, GContext* ctx);
static void calendar_layer_draw_dates(GContext* ctx);
static void calendar_layer_draw_date(GContext* ctx, int wday, int week, int mday, bool is_today);

static void calendar_layer_update_proc(Layer* layer, GContext* ctx) {
  // draw background
  GRect rect = gbitmap_get_bounds(s_bitmap_background);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_background, rect);
  
  // draw calendar
  calendar_layer_draw_dates(ctx);
}

static void calendar_layer_draw_dates(GContext* ctx) {
  struct tm* st; 
  // get current date
  time_t t = time(NULL);
  struct tm today;
  st = localtime(&t);
  memcpy(&today, st, sizeof(today));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "TODAY: %d-%d-%d", today.tm_year, today.tm_mon, today.tm_mday);
  // at least 2 previous weeks
  t -= 1209600;        // 3600 * 24 * 14
  st = localtime(&t);
  // get the 1st week of this calendar.
  for (st->tm_mday -= st->tm_wday; st->tm_mday > 1; st->tm_mday -= 7);
  mktime(st);
  
  for (int week = 0; week < WN; week++) {
    for (int wday = 0; wday < DW; wday++) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "CURST: %d-%d-%d", st->tm_year, st->tm_mon, st->tm_mday);
      calendar_layer_draw_date(ctx, wday, week, st->tm_mday, 
                               (st->tm_mon == today.tm_mon && st->tm_mday == today.tm_mday && st->tm_year == today.tm_year));
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
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_numbers_3x5[mday / 10], rect3x5);
  rect3x5.origin.x += 4
  graphics_draw_bitmap_in_rect(ctx, s_bitmap_numbers_3x5[mday % 10], rect3x5);
  if (is_today) {
    GRect rect_mark = GRect(start_point.x - DX + 1, start_point.y - DX + 0, CW - 3, CH - 1);
    graphics_context_set_stroke_color(ctx, GColorWhite);
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
  layer_destroy(s_layer_calendar);
  destroy_numbers();
  gbitmap_destroy(s_bitmap_background);
}

static void create_numbers() {
  s_bitmap_number_2x5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBER_2X5);
  GRect rect = GRect(0, 0, 2, 5);
  for (int i = 0; i < N2X5; i++) {
    s_bitmap_numbers_2x5[i] = gbitmap_create_as_sub_bitmap(s_bitmap_number_2x5, rect);
    rect.origin.x += rect.size.w;
  }
  
  s_bitmap_number_3x5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBER_3X5);
  rect = GRect(0, 0, 3, 5);
  for (int i = 0; i < N3X5; i++) {
    s_bitmap_numbers_3x5[i] = gbitmap_create_as_sub_bitmap(s_bitmap_number_3x5, rect);
    rect.origin.x += rect.size.w;
  }
}

static void destroy_numbers() {
  for (int i = 0; i < N2X5; i++) {
    gbitmap_destroy(s_bitmap_numbers_2x5[i]);
  }
  gbitmap_destroy(s_bitmap_number_2x5);
  
  for (int i = 0; i < N3X5; i++) {
    gbitmap_destroy(s_bitmap_numbers_3x5[i]);
  }
  gbitmap_destroy(s_bitmap_number_3x5);
}