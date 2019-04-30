#include <string>
#include <memory>

#include "funset.hpp"
#include <opencv2/opencv.hpp>

// Blog: https://blog.csdn.net/fengbingchun/article/details/89715416

int parse_jpeg_file(const char* name)
{
	FILE* infile = nullptr;
	if ((infile = fopen(name, "rb")) == nullptr) {
		fprintf(stderr, "can't open %s\n", name);
		return -1;
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */
	jpeg_read_header(&cinfo, TRUE);
	fprintf(stdout, "image_width = %d\n", cinfo.image_width);
	fprintf(stdout, "image_height = %d\n", cinfo.image_height);
	fprintf(stdout, "num_components = %d\n", cinfo.num_components);

	/* Step 4: set parameters for decompression */
	cinfo.scale_num = 2;
	cinfo.scale_denom = 4;

	/* Step 5: Start decompressor */
	jpeg_start_decompress(&cinfo);
	fprintf(stdout, "output_width = %d\n", cinfo.output_width);
	fprintf(stdout, "output_height = %d\n", cinfo.output_height);
	fprintf(stdout, "output_components = %d\n", cinfo.output_components);

	/* JSAMPLEs per row in output buffer */
	int row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
	}

	/* Step 7: Finish decompression */
	jpeg_finish_decompress(&cinfo);

	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return 0;
}

int write_jpeg_file(const unsigned char* data, int width, int height, int channels, J_COLOR_SPACE color_space, int quality, const char* name)
{
	/* Step 1: allocate and initialize JPEG compression object */
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);

	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	FILE* outfile = nullptr;
	if ((outfile = fopen(name, "wb")) == nullptr) {
		fprintf(stderr, "can't open file: %s\n", name);
		return -1;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = channels;
	cinfo.in_color_space = color_space;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	/* Step 4: Start compressor */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	int row_stride = width * channels;
	int line = 0;
	JSAMPROW row_pointer[1];

	while (line < cinfo.image_height) {
		row_pointer[0] = (JSAMPROW)&data[line * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
		++line;
	}

	/* Step 6: Finish compression */
	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	/* Step 7: release JPEG compression object */
	jpeg_destroy_compress(&cinfo);

	return 0;
}

int get_jpeg_compress_data(const unsigned char* data, int width, int height, int channels, J_COLOR_SPACE color_space, int quality, unsigned char** out_buffer, unsigned long out_buffer_size, unsigned long& free_in_buffer)
{
	/* Step 1: allocate and initialize JPEG compression object */
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);

	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	jpeg_mem_dest(&cinfo, out_buffer, &out_buffer_size);

	/* Step 3: set parameters for compression */
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = channels;
	cinfo.in_color_space = color_space;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	/* Step 4: Start compressor */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	int row_stride = width * channels;
	int line = 0;
	JSAMPROW row_pointer[1];

	while (line < cinfo.image_height) {
		row_pointer[0] = (JSAMPROW)&data[line * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
		++line;
	}

	/* Step 6: Finish compression */
	jpeg_finish_compress(&cinfo);

	free_in_buffer = cinfo.dest->free_in_buffer;

	/* Step 7: release JPEG compression object */
	jpeg_destroy_compress(&cinfo);

	return 0;
}

int test_libjpeg_turbo()
{
#ifdef _MSC_VER
	std::string image_path{ "E:/GitCode/OCR_Test/test_data/" };
#else
	std::string image_path{ "test_data/" };
#endif
	std::string name1 = image_path + "tirg.jpg";
	parse_jpeg_file(name1.c_str());

	std::string name2 = image_path + "lena.png";
	std::string name3 = image_path + "lena.jpg";
	int quality = 80;

	cv::Mat mat = cv::imread(name2);
	if (!mat.data || mat.channels() != 3) {
		fprintf(stderr, "read image fail: %s\n", name2.c_str());
		return -1;
	}

	write_jpeg_file(mat.data, mat.cols, mat.rows, mat.channels(), JCS_EXT_BGR, quality, name3.c_str()); // bgr data

	name3 = image_path + "lena2.jpg";
	cv::cvtColor(mat, mat, CV_BGR2RGB);
	write_jpeg_file(mat.data, mat.cols, mat.rows, mat.channels(), JCS_RGB, quality, name3.c_str()); // rgb data

	name3 = image_path + "lena3.jpg";
	cv::cvtColor(mat, mat, CV_RGB2GRAY);
	write_jpeg_file(mat.data, mat.cols, mat.rows, mat.channels(), JCS_GRAYSCALE, quality, name3.c_str()); // gray data

	int length = mat.cols * mat.rows;
	std::unique_ptr<unsigned char[]> data(new unsigned char[length]);
	unsigned char* p = data.get();
	unsigned long free_in_buffer;
	get_jpeg_compress_data(mat.data, mat.cols, mat.rows, mat.channels(), JCS_GRAYSCALE, quality, &p, length, free_in_buffer);

	name3 = image_path + "lena4.jpg";
	FILE* outfile = nullptr;
	if ((outfile = fopen(name3.c_str(), "wb")) == nullptr) {
		fprintf(stderr, "can't open file: %s\n", name3.c_str());
		return -1;
	}

	fwrite(data.get(), sizeof(unsigned char), length - free_in_buffer, outfile);
	fclose(outfile);

	return 0;
}

