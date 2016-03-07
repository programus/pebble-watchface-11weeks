#include "necessary.h"
#include "main.h"

#include "config.h"

#include "numbers.h"
#include "letters.h"

#include "calendar_layer.h"
#include "sec_layer.h"
#include "frame_layer.h"
#include "watch_battery_layer.h"
#include "phone_battery_layer.h"
#include "bluetooth_layer.h"

#define KEY_PHONE_BATTERY     8
// persistent key for config
#define PKEY_CONFIG           0x0f

#define SEC_TICK_FLG          1
#define BATTERY_FLG           (1 << 1)
#define BT_PHONE_FLG          (1 << 2)

static uint8_t s_stat = 0;

static AppSync s_sync;
static uint8_t* s_sync_buffer;

static Window* s_main_window;
static Layer* s_calendar_layer;
static Layer* s_sec_layer;
static Layer* s_frame_layer;
static Layer* s_watch_battery_layer;
static Layer* s_phone_battery_layer;
static Layer* s_bluetooth_layer;

static time_t s_now_t;
static struct tm s_now_tm;

static bool s_battery_api_supported = false;

static void init();
static void deinit();
static void main_window_load(Window*);
static void main_window_unload(Window*);
static void tick_handler(struct tm*, TimeUnits);
static void update_time(bool is_init);
static void battery_handler(BatteryChargeState charge_state);

static void init_sync();
static void deinit_sync();
static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);
static void update_phone_battery(uint8_t state);

static void bt_handler(bool connected);
static ConnectionHandlers conn_handlers = {
  .pebble_app_connection_handler = bt_handler,
  .pebblekit_connection_handler = NULL
};

static bool is_flag_marked(uint8_t flag);
static bool is_bt_phone_subscribed();

static bool is_flag_marked(uint8_t flag) {
  return (s_stat & flag) == flag;
}

static bool is_bt_phone_subscribed() {
  return is_flag_marked(BT_PHONE_FLG);
}


static void apply_config();

static void init() {
  // load config from storage
  load_config(PKEY_CONFIG);
  // init numbers
  numbers_create();
  // init letters
  letters_create();
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
}

static void deinit() {
  // save config to storage
  status_t rc = save_config(PKEY_CONFIG);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "saved config status is: %d", (int)rc);
  
  if (is_bt_phone_subscribed()) {
    deinit_sync();
  }
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  
  window_destroy(s_main_window);
  letters_destroy();
  numbers_destroy();
}

static void apply_config() {
  // update tick rate
  tick_timer_service_subscribe(need_sec_update() ? SECOND_UNIT : MINUTE_UNIT, tick_handler);
  // update battery service
  if (need_battery_handler()) {
    battery_state_service_subscribe(battery_handler);
    battery_handler(battery_state_service_peek());
  } else {
    battery_state_service_unsubscribe();
  }
  // update connection service
  if (need_bluetooth_handler() != is_bt_phone_subscribed()) {
    if (need_bluetooth_handler()) {
      connection_service_subscribe(conn_handlers);
      conn_handlers.pebble_app_connection_handler(connection_service_peek_pebble_app_connection());
      init_sync();
    } else {
      connection_service_unsubscribe();
      deinit_sync();
    }
    s_stat ^= BT_PHONE_FLG;
  }
  // update display
  layer_set_hidden(s_sec_layer, hide_sec());
  layer_set_hidden(s_frame_layer, hide_frame());
  layer_set_hidden(s_watch_battery_layer, hide_battery());
  layer_set_hidden(s_bluetooth_layer, hide_bt_phone());
  layer_set_hidden(s_phone_battery_layer, hide_bt_phone() || !s_battery_api_supported);
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
  // create frame layer
  frame_layer_create();
  s_frame_layer = frame_layer_get_layer();
  layer_add_child(window_get_root_layer(s_main_window), s_frame_layer);
  // create watch battery layer
  watch_battery_layer_create();
  s_watch_battery_layer = watch_battery_layer_get_layer();
  layer_add_child(window_get_root_layer(s_main_window), s_watch_battery_layer);
  // create phone battery layer
  phone_battery_layer_create();
  s_phone_battery_layer = phone_battery_layer_get_layer();
  layer_add_child(window_get_root_layer(s_main_window), s_phone_battery_layer);
  // create bluetooth layer
  bluetooth_layer_create();
  s_bluetooth_layer = bluetooth_layer_get_layer();
  layer_add_child(window_get_root_layer(s_main_window), s_bluetooth_layer);
  
  apply_config();
  
  // display time from the beginning
  update_time(true);
}

static void main_window_unload(Window* window) {
  // destroy calendar layer
  calendar_layer_destroy();
  // destroy sec layer
  sec_layer_destroy();
  // destroy frame layer
  frame_layer_destroy();
  // destroy watch battery layer
  watch_battery_layer_destroy();
  // destroy phone battery layer
  phone_battery_layer_destroy();
  // destroy bluetooth layer
  bluetooth_layer_destroy();
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  update_time(false);
}

static void update_time(bool is_init) {
  time(&s_now_t);
  struct tm* st = localtime(&s_now_t);
  memcpy(&s_now_tm, st, sizeof(s_now_tm));
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "Now: %d-%d-%d %d:%d:%d",
//          s_now_tm.tm_year + 1900, s_now_tm.tm_mon + 1, s_now_tm.tm_mday, s_now_tm.tm_hour, s_now_tm.tm_min, s_now_tm.tm_sec);
  
  if (!hide_sec()) {
    sec_layer_update_time(&s_now_t, &s_now_tm);
    layer_mark_dirty(s_sec_layer);
  }
  if (!hide_frame()) {
    frame_layer_update_time(&s_now_t, &s_now_tm);
    layer_mark_dirty(s_frame_layer);
  }
  if (is_init || s_now_tm.tm_sec == 0 || !need_sec_update()) {
    calendar_layer_update_time(&s_now_t, &s_now_tm);
    layer_mark_dirty(s_calendar_layer);
  }
}

static void battery_handler(BatteryChargeState charge_state) {
  watch_battery_layer_update(charge_state);
  layer_mark_dirty(s_watch_battery_layer);
}

static void init_sync() {
  // setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // setup initial value
  Tuplet initial_values[] = {
    TupletInteger(KEY_PHONE_BATTERY, BATTERY_API_UNSUPPORTED),
  };
  
  // create buffer
  uint32_t size = dict_calc_buffer_size_from_tuplets(initial_values, ARRAY_LENGTH(initial_values));
  s_sync_buffer = malloc(size * sizeof(uint8_t));
  
  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, size, initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

static void deinit_sync() {
  app_sync_deinit(&s_sync);
}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  switch (key) {
    case KEY_PHONE_BATTERY:
    {
      update_phone_battery(new_tuple->value->int8);
    }
    break;
    
    default:
    break;
  }
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "phone battery info sync error!");
}

static void update_phone_battery(uint8_t state) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "get battery state: %d, %d", state & CHARGING_MASK, state & LEVEL_MASK);
  s_battery_api_supported = !(state == BATTERY_API_UNSUPPORTED);
  layer_set_hidden(s_phone_battery_layer, !s_battery_api_supported);
  layer_set_hidden(s_bluetooth_layer, s_battery_api_supported);
  if (s_battery_api_supported) {
    phone_battery_layer_update(state);
    layer_mark_dirty(s_phone_battery_layer);
  }
}

static void bt_handler(bool connected) {
  bool need_bt_layer = true;
  if (connected) {
    if (s_battery_api_supported) {
      layer_set_hidden(s_phone_battery_layer, false);
      need_bt_layer = false;
    }
  } else {
    vibes_double_pulse();
  }
  
  if (need_bt_layer) {
    layer_set_hidden(s_bluetooth_layer, false);
    bluetooth_layer_update(connected);
    layer_mark_dirty(s_bluetooth_layer);
    layer_set_hidden(s_phone_battery_layer, true);
  } else {
    layer_set_hidden(s_bluetooth_layer, true);
    layer_set_hidden(s_phone_battery_layer, false);
  }
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}