//
//  config.c
//  pebble-watchface-11weeks
//
//  Created by Programus on 2016/3/6.
//
//

#include "config.h"

// all flags are for hiding.
#define SEC_FLG = 1
#define FRAME_FLG = (1 << 1)
#define BATTERY_FLG = (1 << 2)
#define BT_PHONE_FLG = (1 << 3)

static int32_t s_config = 0;

bool is_flag_marked(int flag);

status_t save_config(const uint32_t key) {
  persist_write_int(key, s_config);
}

void load_config(const uint32_t key) {
  s_config = persist_read_int(key);
}

void set_config(int32_t config) {
  s_config = config;
}

bool show_sec() {
  return !is_flag_marked(SEC_FLG);
}

bool show_frame() {
  return !is_flag_marked(FRAME_FLG);
}

bool show_battery() {
  return !is_flag_marked(BATTERY_FLG);
}

bool show_bt_phone() {
  return !is_flag_marked(BT_PHONE_FLG);
}

bool need_sec_update() {
  return show_sec() || show_frame();
}

bool need_bluetooth_handler() {
  return show_bt_phone();
}

bool need_battery_handler() {
  return show_battery();
}

bool is_flag_marked(int flag) {
  return (s_config & flag) == flag;
}