#include <pebble.h>
#include "main.h"
  
static Window* s_main_window;
static BitmapLayer* s_background_layer;
static GBitmap* s_background_bitmap;


static void init() {
  // initialize main window instance
  s_main_window = window_create();
  
  // set the window handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
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
  // Create background BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_background_layer));
}

static void main_window_unload(Window* window) {
  // destroy bitmap
  gbitmap_destroy(s_background_bitmap);
  // destroy background layer
  bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_time() {
  // Get a tm struct
  time_t temp = time(NULL);
  struct tm* tick_time = localtime(&temp);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}