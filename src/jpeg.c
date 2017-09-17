/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#include "jpeg.h"

/* Points to large array of R,G,B-order data */
extern JSAMPLE * image_buffer;
/* Number of rows in image */
extern int image_height;
/* Number of columns in image */
extern int image_width;

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;


bool JPEG_loadImage()
{
  return true;
}
