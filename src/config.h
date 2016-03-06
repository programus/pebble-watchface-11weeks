//
//  config.h
//  pebble-watchface-11weeks
//
//  Created by 王元 on 16/3/6.
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

#endif /* config_h */
