//
//  letters.c
//  pebble-watchface-months
//
//  Created by Programus on 15/6/30.
//
//

#include "letters.h"

// captial letters in one pic
static GBitmap* s_bitmap_cap_letter = NULL;
// captial letters: A-Z
static GBitmap* s_bitmap_cap_letters[LETTER_COUNT];

void letters_create() {
  if (!s_bitmap_cap_letter) {
    s_bitmap_cap_letter = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CAP_LETTERS_3X5);
    GRect rect = GRect(0, 0, TL_WIDTH, TL_HEIGHT);
    for (int i = 0; i < LETTER_COUNT; i++) {
      s_bitmap_cap_letters[i] = gbitmap_create_as_sub_bitmap(s_bitmap_cap_letter, rect);
      rect.origin.x += rect.size.w;
    }
  }
}

void letters_destroy() {
  if (s_bitmap_cap_letter) {
    for (int i = 0; i < LETTER_COUNT; i++) {
      gbitmap_destroy(s_bitmap_cap_letters[i]);
    }
    gbitmap_destroy(s_bitmap_cap_letter);
  }
}

void graphics_draw_tiny_letter(GContext* ctx, char letter, int x, int y) {
  int index = letter >= 'a' ? letter - 'a' : letter - 'A';
  if (index < LETTER_COUNT) {
    graphics_draw_bitmap_in_rect(ctx, s_bitmap_cap_letters[index], GRect(x, y, TL_WIDTH, TL_HEIGHT));
  }
}

void graphics_draw_tiny_string(GContext* ctx, const char* string, int x, int y, int space) {
  for (const char* p = string; *p; p++) {
    graphics_draw_tiny_letter(ctx, *p, x, y);
    x += TL_WIDTH + space;
  }
}
