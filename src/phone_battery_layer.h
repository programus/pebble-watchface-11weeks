//
//  phone_battery_layer.h
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#ifndef __pebble_watchface_months__phone_battery_layer__
#define __pebble_watchface_months__phone_battery_layer__

#include "necessary.h"

#define CHARGING_MASK   0x80
#define LEVEL_MASK      0x7f
#define BATTERY_API_UNSUPPORTED   0x70
#define LEVEL_UNKNOWN   0x7f

void phone_battery_layer_create();
void phone_battery_layer_destroy();
Layer* phone_battery_layer_get_layer();
void phone_battery_layer_update(uint8_t state);

#endif /* defined(__pebble_watchface_months__phone_battery_layer__) */
