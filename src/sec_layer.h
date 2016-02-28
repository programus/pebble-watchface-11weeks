//
//  sec_layer.h
//  pebble-watchface-months
//
//  Created by Programus on 15/6/27.
//
//

#ifndef __pebble_watchface_months__sec_layer__
#define __pebble_watchface_months__sec_layer__

#include "necessary.h"

void sec_layer_create();
Layer* sec_layer_get_layer();
void sec_layer_destroy();
void sec_layer_update_time(time_t* time, struct tm* tm);

#endif /* defined(__pebble_watchface_months__sec_layer__) */
