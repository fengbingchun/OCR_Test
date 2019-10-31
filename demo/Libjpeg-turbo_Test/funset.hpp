#ifndef FBC_OCR_TEST_LIBJPEG_TURBO_FUNSET_HPP_
#define FBC_OCR_TEST_LIBJPEG_TURBO_FUNSET_HPP_

#include <memory>
#include <chrono>
#include <turbojpeg.h>
#include <jpeglib.h>

class Timer {
public:
	static long long getNowTime() { // milliseconds
		auto now = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	}
};

int test_libjpeg_turbo_decompress();
std::unique_ptr<unsigned char[]> get_jpeg_decompress_data(const char* image_name, int& width, int& height, int& channels); // jpeg_read_scanlines
std::unique_ptr<unsigned char[]> get_jpeg_decompress_data2(const char* image_name, int& width, int& height, int& channels);

int test_libjpeg_turbo();
int parse_jpeg_file(const char* name);
int write_jpeg_file(const unsigned char* data, int width, int height, int channels, J_COLOR_SPACE color_space, int quality, const char* name);

int test_libjpeg_turbo_compress();
int get_jpeg_compress_data(const unsigned char* data, int width, int height, int channels, J_COLOR_SPACE color_space, int quality, unsigned char** out_buffer, unsigned long out_buffer_size, unsigned long& free_in_buffer);
int get_jpeg_compress_data2(const unsigned char* data, int width, int height, int pixelFormat, unsigned char** jpegBuf, unsigned long* jpegSize, int jpegSubsamp, int jpegQual, int flags);

#endif // FBC_OCR_TEST_LIBJPEG_TURBO_FUNSET_HPP_
