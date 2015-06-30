#include <pebble.h>
#include "main.h"
#include "calendar_layer.h"
#include "sec_layer.h"
#include "numbers.h"
  
static Window* s_main_window;
static Layer* s_calendar_layer;
static Layer* s_sec_layer;

static time_t s_now_t;
static struct tm s_now_tm;

static void init();
static void deinit();
static void main_window_load(Window*);
static void main_window_unload(Window*);
static void tick_handler(struct tm*, TimeUnits);
static void update_time(bool is_init);

static void init() {
  // init numbers
  numbers_create();
  // initialize main window instance
  s_main_window = window_create();
  
  // set the window handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // set background color
  window_set_background_color(s_main_window, GColorBlack);
  
  // show window on the watch, with animated = true
  window_stack_push(s_main_window, true);
  
  // display time from the beginning
  update_time(true);
  
  // register the time tick service
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
  numbers_destroy();
}

static void main_window_load(Window* window) {
  // create calendar layer
  calendar_layer_create();
  s_calendar_layer = calendar_layer_get_layer();
  layer_add_child(window_get_root_layer(s_main_window), s_calendar_layer);
  // create sec layer
  sec_layer_create();
  s_sec_layer = sec_layer_get_layer();
  layer_add_child(window_get_root_layer(s_main_window), s_sec_layer);
}

static void main_window_unload(Window* window) {
  // destroy calendar layer
  calendar_layer_destroy();
  // destroy sec layer
  sec_layer_destroy();
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  update_time(false);
}

static void update_time(bool is_init) {
  time(&s_now_t);
  struct tm* st = localtime(&s_now_t);
  memcpy(&s_now_tm, st, sizeof(s_now_tm));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Now: %d-%d-%d %d:%d:%d",
          s_now_tm.tm_year + 1900, s_now_tm.tm_mon + 1, s_now_tm.tm_mday, s_now_tm.tm_hour, s_now_tm.tm_min, s_now_tm.tm_sec);
  
  sec_layer_update_time(&s_now_t, &s_now_tm);
  layer_mark_dirty(s_sec_layer);
  if (is_init || s_now_tm.tm_sec == 0) {
    calendar_layer_update_time(&s_now_t, &s_now_tm);
    layer_mark_dirty(s_calendar_layer);
  }
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}