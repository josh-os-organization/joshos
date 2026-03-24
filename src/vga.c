// The following code is licensed under GNU General Public License 3.0.
// Copyright (C) 2026 orca.pet3910YT.
// The following code has been modified to fit a different project.

#include <vga.h>
#include <stdint.h>
#include <port.h>

static uint16_t *vga_buffer = (uint16_t*)0xB8000;
int col = 0;
int row = 0;
void set_cursor_pos(int row, int col) {
    unsigned short pos = row*80+col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}
void putc(char c) {
    if (!c) return;
    if (c == '\n') {
        col = 0;
        row++;
    } else {
        vga_buffer[row*80+col] = (0x07 << 8) | c; col++;
    }
    if (col >= 80) {
        col = 0; row++;
    }
    if (row >= 25) {
        row = 24;
    }
    set_cursor_pos(row, col);
}
void puts(const char *s) {
    while (*s) putc(*s++);
}
