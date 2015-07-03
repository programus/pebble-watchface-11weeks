//
//  phone_battery_layer.h
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#ifndef __pebble_watchface_months__phone_battery_layer__
#define __pebble_watchface_months__phone_battery_layer__

#include <pebble.h>

void phone_battery_layer_create();
void phone_battery_layer_destroy();
Layer* phone_battery_layer_get_layer();
void phone_battery_layer_update(uint8_t state);

#endif /* defined(__pebble_watchface_months__phone_battery_layer__) */
