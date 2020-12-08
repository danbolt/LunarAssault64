#ifndef SMALL_FONT_H
#define SMALL_FONT_H

#include <nusys.h>

extern unsigned char font_8px_bin[] __attribute__((aligned(8)));
extern unsigned int font_8px_bin_len;

void drawSmallString(int x, int y, const char* string);
void drawSmallStringCol(int x, int y, const char* string, unsigned char r, unsigned char g, unsigned char b);

void my_strlen(const char *str, u32 *len);

#endif