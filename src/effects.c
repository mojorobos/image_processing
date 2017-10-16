/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#include "effects.h"
#include "util.h"

#define PI 3.14159265

#define MAX_GRADIENT  1001
#define MAX_THRASHOLD 200
#define MIN_THRASHOLD 50

#define COLOR_BLACK 0
#define COLOR_WHITE 255

static int EFFECTS_discretize_angle(double angle);

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

      char new_color   = sum/div;
      rows[1][col]     = new_color;
      rows[1][col + 1] = new_color;
      rows[1][col + 2] = new_color;
    }
  }
}

// kernel convolution
// [-1] [0] [1]        [-1] [-2] [-1]
// [-2] [0] [2]   and  [0]  [0]  [0]
// [-1] [0] [1]        [1]  [2]  [1]
void EFFECTS_sobel(JPEG_info *jpeg_info, EFFECTS_info *effects_info)
{
  JSAMPROW rows[3];
  for (int row = 0; row < jpeg_info->height; row++) {
    rows[0] = row > 0 ? jpeg_info->buffer[row - 1] : NULL;
    rows[1] = jpeg_info->buffer[row];
    rows[2] = row < jpeg_info->height - 1 ? jpeg_info->buffer[row + 1] : NULL;
    for (int col = 0; col < jpeg_info->width * 3; col += 3) {
      int sumGY = 0;
      int sumGX = 0;

      if (col > 0) {
        if (rows[0]) { sumGX += rows[0][col - 3] * -1; }
        sumGX += rows[1][col - 3] * -2;
        if (rows[2]) { sumGX += rows[2][col - 3] * -1; }
      }

      if (col < (jpeg_info->width - 1) * 3) {
        if (rows[0]) { sumGX += rows[0][col + 3]; }
        sumGX += rows[1][col + 3] * 2;
        if(rows[2]) { sumGX += rows[2][col + 3]; }
      }

      if (rows[0]) {
        if (col > 0) { sumGY += rows[0][col - 3] * -1; }
        sumGY += rows[0][col] * -2;
        if (col < (jpeg_info->width - 1) * 3) { sumGY += rows[0][col + 3] * -1; }
      }

      if (rows[2]) {
        if (col > 0) { sumGY += rows[2][col - 3]; }
        sumGY += rows[2][col] * 2;
        if (col < (jpeg_info->width - 1) * 3) { sumGY += rows[2][col + 3]; }
      }

      int cur_rc = (row * jpeg_info->width) + (col / 3);
      double gradient = sqrt(pow(sumGX, 2) + pow(sumGY, 2));
      double atanGYGX = sumGX ? atan(sumGY/sumGX) : atan(sumGY);
      atanGYGX = atanGYGX * 180.0 / PI;
      effects_info[cur_rc].gradient  = gradient;
      effects_info[cur_rc].direction = EFFECTS_discretize_angle(atanGYGX);
      effects_info[cur_rc].is_using  = true;
      effects_info[cur_rc].color     = COLOR_BLACK;
    }
  }
}

void EFFECTS_canny(JPEG_info *jpeg_info, EFFECTS_info *effects_info)
{
  JSAMPROW row_pointer[1];
  char naughtValue = COLOR_BLACK;
  char strongValue = COLOR_WHITE;
  double value = naughtValue;

  for (int row = 1; row < jpeg_info->height - 1; row++) {
    row_pointer[0] = jpeg_info->buffer[row];
    for (int col = 3; col < (jpeg_info->width - 1) * 3; col += 3) {
      int m11 = ((row - 1) * jpeg_info->width) + ((col - 1) / 3);
      int m12 = ((row - 1) * jpeg_info->width) + (col / 3);
      int m13 = ((row - 1) * jpeg_info->width) + ((col + 1) / 3);
      int m21 = (row * jpeg_info->width) + ((col - 1) / 3);
      int m22 = (row * jpeg_info->width) + (col / 3);
      int m23 = (row * jpeg_info->width) + ((col + 1) / 3);
      int m31 = ((row + 1) * jpeg_info->width) + ((col - 1) / 3);
      int m32 = ((row + 1) * jpeg_info->width) + (col / 3);
      int m33 = ((row + 1) * jpeg_info->width) + ((col + 1) / 3);

      if (effects_info[m22].direction == 0) { // [-pi/8,pi/8]
        if (effects_info[m22].gradient <= effects_info[m12].gradient || effects_info[m22].gradient <= effects_info[m32].gradient) {
          effects_info[m22].color = naughtValue;
        }
      } else if (effects_info[m22].direction == 45) { // [pi/8, 3pi/8]
        if (effects_info[m22].gradient <= effects_info[m13].gradient || effects_info[m22].gradient <= effects_info[m31].gradient) {
          effects_info[m22].color = naughtValue;
        }
      } else if (effects_info[m22].direction == 135) { // [-3pi/8,-pi/8]
        if (effects_info[m22].gradient <= effects_info[m11].gradient || effects_info[m22].gradient <= effects_info[m33].gradient) {
          effects_info[m22].color = naughtValue;
        }
      } else {
        if (effects_info[m22].gradient <= effects_info[m23].gradient || effects_info[m22].gradient <= effects_info[m21].gradient) {
          effects_info[m22].color = naughtValue;
        }
      }

      // Double Threshold
      if (effects_info[m22].gradient > MAX_THRASHOLD) {
        effects_info[m22].color = COLOR_WHITE;
      } else if (effects_info[m22].gradient > MIN_THRASHOLD &&
        (effects_info[m21].color == strongValue ||
         effects_info[m12].color == strongValue ||
         effects_info[m11].color == strongValue)) {
          effects_info[m22].color = COLOR_WHITE;
      } else {
        effects_info[m22].color = COLOR_BLACK;
      }

      char new_color = effects_info[m22].color;
      row_pointer[0][col]     = new_color;
      row_pointer[0][col + 1] = new_color;
      row_pointer[0][col + 2] = new_color;
    }
  }
}

static int EFFECTS_discretize_angle(double angle)
{
  int newAngle;
  if (angle >= -22.5 && angle < 22.5) { // [-pi/8,pi/8]
    newAngle = 0;
  } else if (angle >= 22.5 && angle < 67.5) { // [pi/8, 3pi/8]
    newAngle = 45;
  } else if (angle >= -67.5 && angle < -22.5) { // [-3pi/8,-pi/8]
    newAngle = 135;
  } else {
    newAngle = 90;
  }

  return newAngle;
}
