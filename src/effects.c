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
  for (int row = 0; row < jpeg_info->height; row++) {
    row_pointer[0] = jpeg_info->buffer[row];
    // we are comparing the right pixel, therefore "-3"
    for (int col = 0; col < jpeg_info->width * 3; col += 3) {
      char r1 = row_pointer[0][col];
      char g1 = row_pointer[0][col + 1];
      char b1 = row_pointer[0][col + 2];

      char gray = UTIL_grayscale(r1, g1, b1);

      row_pointer[0][col]     = gray;
      row_pointer[0][col + 1] = gray;
      row_pointer[0][col + 2] = gray;
    }
  }
}

// kernel convolution
// [1] [2] [1]
// [2] [4] [2]
// [1] [2] [1]
void EFFECTS_gaussian_blur(JPEG_info *jpeg_info)
{
  JSAMPROW rows[3];
  for (int row = 0; row < jpeg_info->height; row++) {
    rows[0] = row > 0 ? jpeg_info->buffer[row - 1] : NULL;
    rows[1] = jpeg_info->buffer[row];
    rows[2] = row < jpeg_info->height - 1 ? jpeg_info->buffer[row + 1] : NULL;
    for (int col = 0; col < jpeg_info->width * 3; col += 3) {
      int div = 0;
      int sum = 0;

      if (col > 0) {
        if (rows[0]) { sum += rows[0][col - 3]; div += 1; }
        sum += rows[1][col - 3] * 2; div += 2;
        if (rows[2]) { sum += rows[2][col - 3]; div += 1; }
      }

      if (rows[0]) { sum += rows[0][col] * 2; div += 2; }
      sum += rows[1][col] * 4; div += 4;
      if (rows[2]) { sum += rows[2][col] * 2; div += 2; }

      if (col < (jpeg_info->width - 1) * 3) {
        if (rows[0]) { sum += rows[0][col + 3]; div += 1; }
        sum += rows[1][col + 3] * 2; div += 2;
        if(rows[2]) { sum += rows[2][col + 3]; div += 1; }
      }

      char new_color = sum/div;
      rows[1][col]     = new_color;
      rows[1][col + 1] = new_color;
      rows[1][col + 2] = new_color;
    }
  }
}
