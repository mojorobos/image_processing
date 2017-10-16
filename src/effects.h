/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#ifndef _EFFECTS_H
#define _EFFECTS_H

#include "include.h"
#include "jpeg.h"

typedef struct
{
  int           direction;
  bool          is_using;
  double        gradient;
  unsigned char color;
} EFFECTS_info;

void EFFECTS_grayscale(JPEG_info *jpeg_info);
void EFFECTS_gaussian_blur(JPEG_info *jpeg_info);
void EFFECTS_canny(JPEG_info *jpeg_info, EFFECTS_info *sobel_info);
void EFFECTS_sobel(JPEG_info *jpeg_info, EFFECTS_info *sobel_info);

#endif
