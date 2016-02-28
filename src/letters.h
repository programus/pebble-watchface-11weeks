//
//  letters.h
//  pebble-watchface-months
//
//  Created by Programus on 15/6/30.
//
//

#ifndef __pebble_watchface_months__letters__
#define __pebble_watchface_months__letters__

#include "necessary.h"
#include "const.h"

#define LETTER_COUNT  26

#define TL_WIDTH      3
#define TL_HEIGHT     5

void letters_create();
void letters_destroy();
void graphics_draw_tiny_letter(GContext* ctx, char letter, int x, int y);
void graphics_draw_tiny_string(GContext* ctx, const char* string, int x, int y, int space);

#endif /* defined(__pebble_watchface_months__letters__) */
