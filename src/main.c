/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#include "include.h"
#include "jpeglib.h"
#include "jpeg.h"

int main()
{
  JPEG_process_image("assets/1.jpg", "assets/2.jpg");
  //read_JPEG_file ("assets/savanna.jpg");
  //JPEG_saveImage("assets/brau.jpg", 100);
  return 0;
}
