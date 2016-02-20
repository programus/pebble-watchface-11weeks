#include <pebble.h>
#include "calendar_layer.h"
#include "const.h"
#include "numbers.h"
#include "letters.h"
  
typedef uint8_t buffer_t;

#define MIN_END_MON   28  // min days in a month

static const GPathInfo RARROW_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint[]) {
    {-2, -2},
    {0, 0},
    {-2, 2}
  }
};
static GPath* s_right_arrow_path;

static const char* const MON_NAMES[] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

static GBitmap* s_bitmap_background;
static Layer* s_layer_calendar;

// a pointer to painted background buffer.
static buffer_t* s_bg_buffer = NULL;
static size_t s_bg_buffer_size = 0;
static GSize s_bg_size = {0};
// row size byte for background buffer.
static int s_bg_row_size_byte;

// variable to store current time in time_t
static time_t* s_now_t = NULL;
// variable to store current time in struct tm
static struct tm* s_now = NULL;
// variable to cache the time
static struct tm s_prev = {0};

static void calendar_layer_update_proc(Layer* layer, GContext* ctx);
static void calendar_layer_draw_time(GContext* ctx);
static void calendar_layer_draw_year(GContext* ctx, int tm_year, int week);
static void calendar_layer_draw_mon(GContext* ctx, int tm_mon, int week);
static void calendar_layer_draw_curr_week_indicator(GContext* ctx, int week, bool is_left_side);
static void calendar_layer_draw_dates(GContext* ctx);
static void calendar_layer_draw_date(GContext* ctx, int wday, int week, int mday, bool is_today);

static void update_bg_buffer(GContext* ctx);
static void destroy_bg_buffer();
static uint8_t get_pixel_from_buffer(int x, int y);

static void store_buffer(GContext* ctx);
static void calendar_layer_draw_buffer(GContext* ctx);

static void update_bg_buffer(GContext* ctx) {
  GBitmap* bmp = graphics_capture_frame_buffer(ctx);
  GRect bounds = gbitmap_get_bounds(bmp);
  memcpy(&s_bg_size, &(bounds.size), sizeof(GSize));
  
  buffer_t* frame_buffer = gbitmap_get_data(bmp);
  s_bg_row_size_byte = gbitmap_get_bytes_per_row(bmp);
#if defined(PBL_BW)
  size_t size = s_bg_row_size_byte * bounds.size.h;
#elif defined(PBL_COLOR)
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
#if defined(PBL_BW)
  int index = y * (s_bg_row_size_byte) + (x >> 3);
  buffer_t shift = x & 0x07;
  buffer_t mask = 1 << shift;
  return (s_bg_buffer[index] & mask) ? 1 : 0;
#elif defined(PBL_COLOR)
  int index = y * s_bg_size.w + x;
  return s_bg_buffer[index];
#endif
}

void calendar_layer_update_time(time_t* time, struct tm* tm) {
  s_now_t = time;
  s_now = tm;
}

static void calendar_layer_update_proc(Layer* layer, GContext* ctx) {
  if (s_now && s_now_t) {
    if (!s_bg_buffer || s_prev.tm_min != s_now->tm_min || s_prev.tm_hour != s_now->tm_hour || s_prev.tm_yday != s_now->tm_yday || s_prev.tm_year != s_now->tm_year) {
      // draw time
      calendar_layer_draw_time(ctx);
      
      // draw calendar grids
      graphics_context_set_compositing_mode(ctx, GCompOpSet);
      GRect rect = gbitmap_get_bounds(s_bitmap_background);
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_background, rect);
      
      // draw calendar
      calendar_layer_draw_dates(ctx);
      
      // store drawn buffer
      store_buffer(ctx);
    } else {
      calendar_layer_draw_buffer(ctx);
    }
    
    memcpy(&s_prev, s_now, sizeof(struct tm));
  }
}

static void store_buffer(GContext* ctx) {
  update_bg_buffer(ctx);
}

static void calendar_layer_draw_buffer(GContext* ctx) {
  GBitmap* bmp = graphics_capture_frame_buffer(ctx);
  buffer_t* frame_buffer = gbitmap_get_data(bmp);
  memcpy(frame_buffer, s_bg_buffer, s_bg_buffer_size * sizeof(buffer_t));
  graphics_release_frame_buffer(ctx, bmp);
}

static void calendar_layer_draw_time(GContext* ctx) {
  int x = SX;
  int y = SY;
  int dx = CW << 2;
  int dy = CH * 6;
  int hour = s_now->tm_hour;
  if (!clock_is_24h_style()) {
    if (hour > 12) {
      hour -= 12;
    } else if (hour == 0) {
      hour = 12;
    }
  }
  graphics_draw_big_number(ctx, hour / 10, x, y);
  x += dx;
  graphics_draw_big_number(ctx, hour % 10, x, y);
  y += dy;
  graphics_draw_big_number(ctx, s_now->tm_min % 10, x, y);
  x -= dx;
  graphics_draw_big_number(ctx, s_now->tm_min / 10, x, y);
}

static void calendar_layer_draw_dates(GContext* ctx) {
  // update background buffer
  update_bg_buffer(ctx);
  
  // at least 1 previous weeks
  time_t t = *s_now_t - 604800;        // 3600 * 24 * 7
  struct tm* st = localtime(&t);
  // get the 1st week of this calendar.
  for (st->tm_mday -= st->tm_wday; st->tm_mday > 1; st->tm_mday -= 7);
  mktime(st);
  
  for (int week = 0; week < WN; week++) {
    bool include_today = false;
    for (int wday = 0; wday < DW; wday++) {
      bool is_today = st->tm_mon == s_now->tm_mon && st->tm_mday == s_now->tm_mday && st->tm_year == s_now->tm_year;
      calendar_layer_draw_date(ctx, wday, week, st->tm_mday, is_today);
      st->tm_mday++;
      if (st->tm_mday >= MIN_END_MON) {
        mktime(st);
      }
      include_today = include_today || is_today;
    }
    
    bool need_display_mon = st->tm_mday > 1 && st->tm_mday <= DW + 1;
    bool need_display_year = week == 0 || (st->tm_mon == 0 && need_display_mon);
    
    if (need_display_year) {
      // draw year at the beginning and for new year
      graphics_context_set_compositing_mode(ctx, GCompOpAssign);
      calendar_layer_draw_year(ctx, st->tm_year, week);
    } else if (include_today) {
      calendar_layer_draw_curr_week_indicator(ctx, week, true);
    }
    
    if (need_display_mon) {
      // draw month infomation at the beginning of the month
      graphics_context_set_compositing_mode(ctx, GCompOpAssign);
      calendar_layer_draw_mon(ctx, st->tm_mon, week);
    } else if (include_today) {
      calendar_layer_draw_curr_week_indicator(ctx, week, false);
    }
  }
}

static void calendar_layer_draw_year(GContext* ctx, int tm_year, int week) {
  GPoint p = GPoint(SX - (TN_WIDTH << 1) - DX - 3, SY + CH * week);
  int year = tm_year + 1900;
  graphics_draw_tiny_number(ctx, year / 1000, p.x, p.y);
  p.x += TN_WIDTH + 2;
  graphics_draw_tiny_number(ctx, year / 100, p.x, p.y);
  p.y += TN_HEIGHT + 1;
  graphics_draw_tiny_number(ctx, year, p.x, p.y);
  p.x -= TN_WIDTH + 2;
  graphics_draw_tiny_number(ctx, year / 10, p.x, p.y);
}

static void calendar_layer_draw_mon(GContext* ctx, int tm_mon, int week) {
  GPoint p = GPoint(SX + DW * CW + DX, SY + DY + CH * week);
  graphics_draw_tiny_string(ctx, MON_NAMES[tm_mon], p.x, p.y, 1);
}

static void calendar_layer_draw_curr_week_indicator(GContext* ctx, int week, bool is_left_side) {
  GPoint p = GPoint(is_left_side ? SX - DX - 2 : SX + DW * CW + DX, SY + CH * week + (CH >> 1) - 1);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  // rotate 180 degrees if on right.
  gpath_rotate_to(s_right_arrow_path, is_left_side ? 0 : TRIG_MAX_ANGLE >> 1);
  gpath_move_to(s_right_arrow_path, p);
  gpath_draw_outline(ctx, s_right_arrow_path);
  gpath_draw_filled(ctx, s_right_arrow_path);
}

static void calendar_layer_draw_date(GContext* ctx, int wday, int week, int mday, bool is_today) {
  GPoint start_point = GPoint(SX + DX + CW * wday, SY + DY + CH * week);
#if defined(PBL_BW)
  bool is_black_bg = !get_pixel_from_buffer(start_point.x - DX + 1, start_point.y - DY + 1);
  graphics_context_set_compositing_mode(ctx, is_black_bg ? GCompOpAssign : GCompOpAssignInverted);
  if (mday > 9) {
    graphics_draw_tiny_number(ctx, mday / 10, start_point.x, start_point.y);
  }
  graphics_draw_tiny_number(ctx, mday % 10, start_point.x + 4, start_point.y);
#elif defined(PBL_COLOR)
  bool is_black_bg = get_pixel_from_buffer(start_point.x - DX + 1, start_point.y - DY + 1) == GColorBlackARGB8;
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  void (*draw_number)(GContext*, int, int ,int);
  draw_number = is_black_bg ? &graphics_draw_tiny_number : &graphics_draw_tiny_number_rc;
  if (mday > 9) {
    (*draw_number)(ctx, mday / 10, start_point.x, start_point.y);
  }
  (*draw_number)(ctx, mday % 10, start_point.x + 4, start_point.y);
#endif
  if (is_today) {
    // mark current day
    GRect rect_mark = GRect(start_point.x - DX + 1, start_point.y - DX + 0, CW - 3, CH - 1);
    graphics_context_set_stroke_color(ctx, is_black_bg ? GColorWhite : GColorBlack);
    graphics_draw_rect(ctx, rect_mark);
    // mark weekday
    rect_mark.origin.x -= 1;
    rect_mark.size.w += 2;
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
  // create arrow path
  s_right_arrow_path = gpath_create(&RARROW_PATH_INFO);
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
  gpath_destroy(s_right_arrow_path);
}
