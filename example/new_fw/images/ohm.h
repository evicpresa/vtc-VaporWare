#include <stdint.h>

#ifndef OHM_H
#define OHM_H

uint8_t ohm[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04, 0xC0,
		0x3F, 0x04, 0xE0, 0xFF, 0x04, 0x30, 0xC0, 0x05,
		0x10, 0x80, 0x07, 0x10, 0x00, 0x03, 0x10, 0x00,
		0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10,
		0x00, 0x00, 0x10, 0x00, 0x03, 0x10, 0x80, 0x07,
		0x30, 0xC0, 0x05, 0xE0, 0xFF, 0x04, 0xC0, 0x3F,
		0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x06, 0x00,
		0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

int ohm_width  = 24;
int ohm_height = 24;

#endif
