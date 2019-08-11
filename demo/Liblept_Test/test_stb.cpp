#include "funset.hpp"
#include <iostream>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

// Blog: https://blog.csdn.net/fengbingchun/article/details/99176162

int test_stb_image()
{
#ifdef _MSC_VER
	const std::string files_path {"E:/GitCode/OCR_Test/test_data/"};
#else
	const std::string files_path {"test_data/"};
#endif

	const std::vector<std::string> images_name{"marge.jpg", "lena.png"};

	for (auto name : images_name) {
		const std::string image = files_path + name;

		// load image
		int x, y, channels_in_file, desired_channels = 3;
		unsigned char* data = stbi_load(image.c_str(), &x, &y, &channels_in_file, desired_channels);
		if (!data) {
			fprintf(stderr, "fail to read image: %s\n", image.c_str());
			return -1;
		}
		fprintf(stdout, "image: %s, x: %d, y: %d, channels_in_file: %d, desired_channels: %d\n", name.c_str(), x, y, channels_in_file, desired_channels);

		// resize image
		int width_resize = x * 1.5, height_resize = y * 1.4;
		unsigned char* output_pixels = (unsigned char*)malloc(width_resize * height_resize * desired_channels);
		int ret = stbir_resize_uint8(data, x, y, 0, output_pixels, width_resize, height_resize, 0, desired_channels);
		if (ret == 0) {
			fprintf(stderr, "fail to resize image: %s\n", image.c_str());
			return -1;
		}

		// write(save) image
		const std::string save_name_png = image + ".png";
		const std::string save_name_jpg = image + ".jpg";

		ret = stbi_write_png(save_name_png.c_str(), width_resize, height_resize, desired_channels, output_pixels, 0);
		if (ret == 0) {
			fprintf(stderr, "fail to write image png: %s\n", image.c_str());
			return -1;
		}

		ret = stbi_write_jpg(save_name_jpg.c_str(), width_resize, height_resize, desired_channels, output_pixels, 90);
		if (ret == 0) {
			fprintf(stderr, "fail to write image jpg: %s\n", image.c_str());
			return -1;
		}

		free(data);
		free(output_pixels);
	}

	return 0;
}
