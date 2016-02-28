#pragma once
#include "necessary.h"

void calendar_layer_create();
Layer* calendar_layer_get_layer();
void calendar_layer_destroy();
void calendar_layer_update_time(time_t* time, struct tm* tm);