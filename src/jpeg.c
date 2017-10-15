/* This is the source code of Image Processing.
 * It is licensed under GNU GPL v. 3 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright MOJO ROBOS, 2017.
 */

#include "jpeg.h"
#include "util.h"
#include "effects.h"

// ====================== JPEG Data Structures

struct JPEG_error_mgr
{
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct JPEG_error_mgr *JPEG_error_ptr;

typedef struct jpeg_compress_struct jpeg_compress_struct;

// ====================== FUNCTION DECLARATIONs

static jpeg_compress_struct* JPEG_start_new_image(JPEG_info *jpeg_info);
static void JPEG_handle_new_image(JPEG_info *jpeg_info, jpeg_compress_struct *cinfo);
static bool JPEG_save_new_image(JPEG_info *jpeg_info, jpeg_compress_struct *cinfo);
static void JPEG_error_exit(j_common_ptr cinfo);
static void JPEG_put_scanline_someplace(JSAMPLE* ba, int row_stride);

static void JPEG_error_exit(j_common_ptr cinfo)
{
  /* cinfo->err really points to a JPEG_error_mgr struct, so coerce pointer */
  JPEG_error_ptr myerr = (JPEG_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

static jpeg_compress_struct* JPEG_start_new_image(JPEG_info *jpeg_info)
{
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct *cinfo =
    (jpeg_compress_struct *) malloc(sizeof(jpeg_compress_struct));
  struct jpeg_error_mgr jerr;
  FILE *outfile;		/* target file */

  /* Step 1: allocate and initialize JPEG compression object */

  cinfo->err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(cinfo);

  /* Step 2: specify data destination (eg, a file) */
  if ((outfile = fopen(jpeg_info->filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", jpeg_info->filename);
    exit(1);
  }

  jpeg_info->file = outfile;
  jpeg_stdio_dest(cinfo, outfile);

  /* Step 3: set parameters for compression */

  cinfo->image_width = jpeg_info->width; 	/* image width and height, in pixels */
  cinfo->image_height = jpeg_info->height;
  cinfo->input_components = 3;		/* # of color components per pixel */
  cinfo->in_color_space = JCS_RGB; 	/* colorspace of input image */

  jpeg_set_defaults(cinfo);
  jpeg_set_quality(cinfo, jpeg_info->quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */
  jpeg_start_compress(cinfo, TRUE);

  return cinfo;
}

static void JPEG_handle_new_image(JPEG_info *jpeg_info, jpeg_compress_struct *cinfo)
{
  EFFECTS_grayscale(jpeg_info);
  EFFECTS_gaussian_blur(jpeg_info);
  int total_size = jpeg_info->height * jpeg_info->width;
  EFFECTS_info *effects_info =
    (EFFECTS_info *) malloc(sizeof(EFFECTS_info) * total_size);
  EFFECTS_sobel(jpeg_info, effects_info);
  EFFECTS_canny(jpeg_info, effects_info);

  JSAMPROW row_pointer[1];
  for (int col = 0; col < jpeg_info->height; col++) {
    row_pointer[0] = jpeg_info->buffer[col];
    (void) jpeg_write_scanlines(cinfo, row_pointer, 1);
  }

  free(effects_info);
}

static bool JPEG_save_new_image(JPEG_info *jpeg_info, jpeg_compress_struct *cinfo)
{
  /* Step 6: Finish compression */
  jpeg_finish_compress(cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(jpeg_info->file);

  /* Step 7: release JPEG compression object */
  jpeg_destroy_compress(cinfo);
  free(cinfo);
  return true;
}

static void JPEG_put_scanline_someplace (JSAMPLE* ba, int row_stride)
{
  static int height;
  int i;

  for (i=0; i < row_stride; i++)
    printf("%d\n", ba[i]);

  //printf ("width: %3d height: %3d\n", row_stride, height++);
}

bool JPEG_process_image(char *file_in, char *file_out)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  // We use our private extension JPEG error handler.
  struct JPEG_error_mgr jerr;
  FILE *infile;
  /* Output row buffer */
  JSAMPARRAY buffer;
  /* physical row width in output buffer */
  int row_stride;

  if ((infile = fopen(file_in, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", file_in);
    return false;
  }

  /* Step 1: allocate and initialize JPEG decompression object */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = JPEG_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error. */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return false;
  }

  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */
  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */
  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);

  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  // set info for the output jpeg.
  JPEG_info *jpeg_info = (JPEG_info *) malloc(sizeof(JPEG_info));
  jpeg_info->buffer   = (JSAMPLE **) malloc(sizeof(JSAMPLE* ) * cinfo.output_height);
  jpeg_info->filename = file_out;
  jpeg_info->width    = cinfo.output_width;
  jpeg_info->height   = cinfo.output_height;
  jpeg_info->quality  = 100;
  jpeg_info->file     = NULL;

  jpeg_compress_struct *new_cinfo = JPEG_start_new_image(jpeg_info);
  int i = 0;

  // loadPixels
  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    //JPEG_put_scanline_someplace(buffer[0], row_stride);
    jpeg_info->buffer[i] = (JSAMPLE *) malloc(sizeof(JSAMPLE) * cinfo.output_width * 3);
    memcpy(jpeg_info->buffer[i], buffer[0], sizeof(JSAMPLE) * cinfo.output_width * 3);
    i++;
  }

  // Process new pixels
  JPEG_handle_new_image(jpeg_info, new_cinfo);

  // Save new image and free allocated memory
  JPEG_save_new_image(jpeg_info, new_cinfo);

  for (int i = 0; i < cinfo.output_height; i++) {
    free(jpeg_info->buffer[i]);
  }

  free(jpeg_info->buffer);
  free(jpeg_info);

  /* Step 7: Finish decompression */
  (void) jpeg_finish_decompress(&cinfo);

  /* Step 8: Release JPEG decompression object */
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);

  return true;
}
