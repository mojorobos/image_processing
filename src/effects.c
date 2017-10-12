/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#include "effects.h"
#include "util.h"

void EFFECTS_grayscale(JPEG_info *jpeg_info)
{
  JSAMPROW row_pointer[1];
  for (int col = 0; col < jpeg_info->height; col++) {
    row_pointer[0] = jpeg_info->buffer[col];
    // we are comparing the right pixel, therefore "-3"
    for (int row = 0; row < jpeg_info->width * 3; row += 3) {
      char r1 = row_pointer[0][row];
      char g1 = row_pointer[0][row + 1];
      char b1 = row_pointer[0][row + 2];

      char gray = UTIL_grayscale(r1, g1, b1);

      row_pointer[0][row]     = gray;
      row_pointer[0][row + 1] = gray;
      row_pointer[0][row + 2] = gray;
    }
  }
}

void EFFECTS_gaussian_kernel(JPEG_info *jpeg_info)
{
  JSAMPROW row_1st_ptr[1];
  JSAMPROW row_2nd_ptr[1];
  JSAMPROW row_3rd_ptr[1];
  for (int col = 0; col < jpeg_info->height - 2; col++) {
    row_1st_pointer[0] = jpeg_info->buffer[col];
    row_2nd_pointer[0] = jpeg_info->buffer[col + 1];
    row_3rd_pointer[0] = jpeg_info->buffer[col + 2];
    // we are comparing the right pixel, therefore "-3"
    for (int row = 0; row < jpeg_info->width * 3; row += 3) {
      char r1 = row_pointer[0][row];
      char g1 = row_pointer[0][row + 1];
      char b1 = row_pointer[0][row + 2];
    }
  }
}
