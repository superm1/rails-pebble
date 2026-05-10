/*
 * Rails for Pebble - Utility functions header
 */

#pragma once

#include <pebble.h>

// Recolor a GDrawCommandImage (PDC vector) with fill and stroke colors
void gdraw_command_image_recolor(GDrawCommandImage *img, GColor fill_color, GColor stroke_color);
