// The following code is licensed under GNU General Public License 3.0.
// Copyright (C) 2026 orca.pet3910YT.
// This code was designed to fit Josh OS and thus has been modified.
// See original at https://github.com/orca-pet3910YT/C-kernel-OS

#include <port.h>
#include <kb.h>

static uint8_t shift = 0;
static uint8_t ctrl = 0;
static uint8_t alt = 0;
static uint8_t capslock = 0;
static uint8_t ext = 0;
static uint8_t lang = 0;

static const char scancode_map_lower[] = {
	0,    0,   '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',  0,    0,
	'q',  'w', 'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',  '[',  ']',  '\n', 0,    'a',  's',
	'd',  'f', 'g',  'h',  'j',  'k',  'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
	'b',  'n', 'm',  ',',  '.',  '/',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
	0,    0,   0,    0,    0,    0,    0,    '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
	'2',  '3', '0',  '.',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};
static const char scancode_map_upper[] = {
	0,    0,   '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',  '_',  '+',  0,    0,
	'Q',  'W', 'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',  '{',  '}',  '\n', 0,    'A',  'S',
	'D',  'F', 'G',  'H',  'J',  'K',  'L',  ':',  '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',
	'B',  'N', 'M',  '<',  '>',  '?',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
	0,    0,   0,    0,    0,    0,    0,    '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
	'2',  '3', '0',  '.',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};

static const char rus_scancode_map_lower[] = {
	0,    0,   '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',  0,    0,
	0xA9, 0xE6, 0xE3, 0xAA, 0xA5, 0xAD, 0xA3, 0xE8, 0xE9, 0xA7, 0xE5, 0xEA, '\n', 0,    0xE4, 0xEB,
	0xA2, 0xA0, 0xAF, 0xE0, 0xAE, 0xAB, 0xA4, 0xA6, 0xED, '\\', 0,    '\\', 0xF1, 0xE7, 0xE1, 0xAC,
	0xA8, 0xE2, 0xEC, 0xA1, 0xEE, '.',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
	0,    0,   0,    0,    0,    0,    0,    '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
	'2',  '3', '0',  '.',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};
static const char rus_scancode_map_upper[] = {
	0,    0,   '!',  '"',  0xFC, ';',  '%',  ':',  '?',  '*',  '(',  ')',  '_',  '+',  0,    0,
	0x89, 0x96, 0x93, 0x8A, 0x85, 0x8D, 0x83, 0x98, 0x99, 0x87, 0x95, 0x9A, '\n', 0,    0x94, 0x9B,
	0x82, 0x80, 0x8F, 0x90, 0x8E, 0x8B, 0x84, 0x86, 0x9D, '/',  0,    '/',  0xF0, 0x97, 0x91, 0x9C,
	0x88, 0x92, 0x9C, 0x81, 0x9E, '.',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
	0,    0,   0,    0,    0,    0,    0,    '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
	'2',  '3', '0',  '.',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};


uint8_t shift_pressed() { return shift; }
uint8_t ctrl_pressed() { return ctrl; }
uint8_t alt_pressed() { return alt; }

void kb_init() {
	outb(0x64, 0xAE);
	shift = 0; ctrl = 0; alt = 0; capslock = 0;
	lang = 0;
	kb_flush_buf();
}

void kb_flush_buf() {
	for (int i = 0; i < 16; i++) {
		uint8_t status = inb(0x64);
		if (status & 0x01) {
			inb(0x60);
		} else { break; }
	}
}

char scancode_to_c(uint8_t sc) {
	if (sc == SCANCODE_EXT) {
		ext = 1;
		return 0;
	}
	if (ext) {
		ext = 0;
		if (sc == SCANCODE_UP) return 0x1E; // up
		if (sc == SCANCODE_DOWN) return 0x1F;
		if (sc == SCANCODE_LEFT) return 0x1A;
		if (sc == SCANCODE_RIGHT) return 0x1B;
		if (sc & 0x80) return 0;
		return 0;
	}
	if (sc == SCANCODE_LSHIFT_PRESS || sc == SCANCODE_RSHIFT_PRESS) { shift = 1; return 0; }
	if (sc == SCANCODE_LSHIFT_RELEASE || sc == SCANCODE_RSHIFT_RELEASE) { shift = 0; return 0; }
	if (sc == SCANCODE_CTRL_PRESS) { ctrl = 1; return 0; }
	if (sc == SCANCODE_CTRL_RELEASE) { ctrl = 0; return 0; }
	if (sc == SCANCODE_ALT_PRESS) { alt = 1; lang = !lang; return 0; }
	if (sc == SCANCODE_ALT_RELEASE) { alt = 0; return 0; }
	if (sc == SCANCODE_CAPSLOCK)  { capslock = !capslock; return 0; }
	if (sc & 0x80) return 0;
	if (sc == 0x1C) return '\n';
	if (sc == 0x0E) return '\b';
	if (sc == 0x0F) return '\t';
	if (sc == 0x39) return ' '; // space
	if (sc == 0x01) return 0x1B; // escape
	if (sc >= sizeof(scancode_map_lower)) return 0;
	char c;
	if (lang) {
		if (shift) { c = rus_scancode_map_upper[sc]; }
		else       { c = rus_scancode_map_lower[sc]; }
	} else {
		if (shift) { c = scancode_map_upper[sc]; }
		else       { c = scancode_map_lower[sc]; }
	}
	if      (capslock && c >= 'a' && c <= 'z') { c = c - 'a' + 'A'; }
	else if (capslock && c >= 'A' && c <= 'Z' && !shift) {}
	else if (capslock && c >= 'A' && c <= 'Z' && shift ) { c = c - 'A' + 'a'; }
	return c; // finally holy shit
}

uint8_t kb_get_scancode() {
	uint8_t status = inb(0x64);
	for (int i = 0; i < 16; i++) {
		if (status & 0x01) {
		uint8_t data = inb(0x60); // keyboard port
			if (status & 0x20) { // mouse data - but we're accessing KB data right? right!
				return 0;
			}
		return data; // data
		}
	}
	return 0; // no data
}

unsigned char loop_until_keypress() {
	for (;;) {
		char c = scancode_to_c(kb_get_scancode());
		if (c) return c;
	}
}
