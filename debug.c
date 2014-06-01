/*
 * debug.c
 *
 *  Created on: 01.06.2014
 *      Author: andreasbehnke
 */

#include "include/debug.h"

#ifdef DEBUG

char debug_rom_buffer[128];

char *debug_read_from_rom(const char *addr) {
    char c;
    uint8_t pos = 0;
    while ((c = pgm_read_byte(addr + pos))) {
        debug_rom_buffer[pos] = c;
        pos++;
    }
    debug_rom_buffer[pos] = 0x00;
    return debug_rom_buffer;
}

#endif
