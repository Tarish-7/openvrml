//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "jpgread.h"

extern "C" {
#include <jpeglib.h>
}

#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}



unsigned char *jpgread (FILE *fp, int *w, int *h, int *nc)
{
  struct jpeg_decompress_struct cinfo;
  /*struct jpeg_error_mgr jerr;*/
  struct my_error_mgr jerr;
  int bytes_per_line, i, row;

  /* Reading multiple lines at a time avoids some internal buffering */
#define BUFFER_HEIGHT 5

  unsigned char *pixels, *buffer[BUFFER_HEIGHT];


  /* Initialize the JPEG decompression object with default error handling. */
  memset(&cinfo, 0, sizeof(cinfo));
  /*cinfo.err = jpeg_std_error(&jerr);*/
  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    return 0;
  }

  jpeg_create_decompress(&cinfo);

  /* Specify data source for decompression */
  jpeg_stdio_src(&cinfo, fp);

  /* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&cinfo, TRUE);

  /* Start decompressor */
  (void) jpeg_start_decompress(&cinfo);

  /* Allocate image buffer */
  bytes_per_line = cinfo.num_components * cinfo.image_width;
  pixels = (unsigned char *)malloc(bytes_per_line  * cinfo.image_height);
  if (! pixels) return 0;

  row = 0;

  /* Process data */
  while (cinfo.output_scanline < cinfo.output_height) {
    for (i=0; i<BUFFER_HEIGHT; ++i) {
      /* read in "upside down" because opengl says the
       * texture origin is lower left 
       */
      int rrow = cinfo.output_height - row - 1;
      //buffer[i] = &pixels[bytes_per_line * (row + i)];
      buffer[i] = &pixels[bytes_per_line * (rrow - i)];
    }
    row += jpeg_read_scanlines(&cinfo, buffer, BUFFER_HEIGHT);
  }

  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  *w = cinfo.image_width;
  *h = cinfo.image_height;
  *nc = cinfo.num_components;

  return pixels;
}