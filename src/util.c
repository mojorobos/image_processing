/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#include "util.h"

double UTIL_brightness(char r, char g, char b)
{
   return sqrt(0.299 * pow(r, 2) + 0.587 * pow(g, 2) + 0.114 * pow(b, 2));
}

char UTIL_grayscale(char r, char g, char b)
{
  char bgts = round(UTIL_brightness(r, g, b));
  return bgts * 2;
}
