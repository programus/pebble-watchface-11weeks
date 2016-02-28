//
//  watch_battery_layer.h
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#ifndef __pebble_watchface_months__watch_battery_layer__
#define __pebble_watchface_months__watch_battery_layer__

#include "necessary.h"

void watch_battery_layer_create();
void watch_battery_layer_destroy();
Layer* watch_battery_layer_get_layer();
void watch_battery_layer_update(BatteryChargeState state);

#endif /* defined(__pebble_watchface_months__watch_battery_layer__) */
