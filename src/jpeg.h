/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#ifndef _JPEG_H
#define _JPEG_H

#include "include.h"

typedef union _JPEG_pixel
{
  struct {
    unsigned char alpha;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
  } colors;

  int  value;
} JPEG_pixel;

typedef struct _JPEG_info
{
  int     width;
  int     height;
  int     quality;
  FILE    *file;
  char    *filename;
  JSAMPLE **buffer;
} JPEG_info;

bool JPEG_process_image(char *fileIn, char *fileOut);

#endif
