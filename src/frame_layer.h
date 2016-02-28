//
//  frame_layer.h
//  pebble-watchface-months
//
//  Created by Programus on 15/6/27.
//
//

#ifndef __pebble_watchface_months__frame_layer__
#define __pebble_watchface_months__frame_layer__

#include "necessary.h"

void frame_layer_create();
Layer* frame_layer_get_layer();
void frame_layer_destroy();
void frame_layer_update_time(time_t* time, struct tm* tm);

#endif /* defined(__pebble_watchface_months__frame_layer__) */
