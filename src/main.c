#include <pebble.h>
#include "calendar_layer.h"
#include "main.h"
  
static Window* s_main_window;
static Layer* s_calendar_layer;


static void init() {
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
  update_time();
  
  // register the time tick service
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

static void main_window_load(Window* window) {
  // create calendar layer
  calendar_layer_create();
  s_calendar_layer = calendar_layer_get();
  layer_add_child(window_get_root_layer(s_main_window), s_calendar_layer);
}

static void main_window_unload(Window* window) {
  // destroy calendar layer
  calendar_layer_destroy();
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_time() {
  layer_mark_dirty(s_calendar_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}