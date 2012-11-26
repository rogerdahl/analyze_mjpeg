#include "stdafx.h"

#include "cdjpeg.h" /* Common decls for cjpeg/djpeg applications */
#include "jversion.h" /* for version message */
#include "jpeg.h"

using namespace std;


Image decompress_jpeg(char* compressed_frame, int compressed_frame_size)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];
  char *raw_image = NULL;
  int location(0);
  int i(0);

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo, (unsigned char*)compressed_frame, compressed_frame_size);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  row_pointer[0] = (unsigned char *)malloc(cinfo.output_width * cinfo.num_components);

  ImageData image_data;
  image_data.resize(cinfo.output_width * cinfo.output_height * cinfo.num_components);

  u8* image_data_ptr = &image_data[0];
  while(cinfo.output_scanline < cinfo.image_height) {
    jpeg_read_scanlines(&cinfo, row_pointer, 1);
    for (u32 i(0); i < cinfo.image_width * cinfo.num_components; i++) {
      *image_data_ptr++ = row_pointer[0][i];
    }
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  Image m;
  m.w = cinfo.output_width;
  m.h = cinfo.output_height;
  m.image_data = image_data;
  m.bytes_per_pixel = cinfo.num_components;

  return m;
}
