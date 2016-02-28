//
//  bluetooth_layer.h
//  pebble-watchface-months
//
//  Created by Programus on 15/7/1.
//
//

#ifndef __pebble_watchface_months__bluetooth_layer__
#define __pebble_watchface_months__bluetooth_layer__

#include "necessary.h"

void bluetooth_layer_create();
void bluetooth_layer_destroy();
Layer* bluetooth_layer_get_layer();
void bluetooth_layer_update(bool is_bt_on);

#endif /* defined(__pebble_watchface_months__bluetooth_layer__) */
