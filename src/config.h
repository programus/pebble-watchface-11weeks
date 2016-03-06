//
//  config.h
//  pebble-watchface-11weeks
//
//  Created by Programus on 2016/3/6.
//
//

#ifndef config_h
#define config_h

#include "necessary.h"

status_t save_config(const uint32_t key);
void load_config(const uint32_t key);

void set_config(int32_t config);

bool show_sec();
bool show_frame();
bool show_battery();
bool show_bt_phone();

bool need_sec_update();
bool need_bluetooth_handler();
bool need_battery_handler();

#endif /* config_h */
