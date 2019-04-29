#ifndef FBC_OCR_TEST_LIBJPEG_TURBO_FUNSET_HPP_
#define FBC_OCR_TEST_LIBJPEG_TURBO_FUNSET_HPP_

#include <turbojpeg.h>
#include <jpeglib.h>

int test_libjpeg_turbo();

int parse_jpeg_file(const char* name);
int write_jpeg_file(const unsigned char* data, int width, int height, int channels, J_COLOR_SPACE color_space, int quality, const char* name);
int get_jpeg_compress_data(const unsigned char* data, int width, int height, int channels, J_COLOR_SPACE color_space, int quality, unsigned char** out_buffer, unsigned long out_buffer_size, unsigned long& free_in_buffer);

#endif // FBC_OCR_TEST_LIBJPEG_TURBO_FUNSET_HPP_
