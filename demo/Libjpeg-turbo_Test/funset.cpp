#include <string>
#include <memory>

#include "funset.hpp"
#include <opencv2/opencv.hpp>

// Blog: https://blog.csdn.net/fengbingchun/article/details/102837111
int test_libjpeg_turbo_decompress()
{
#ifdef _MSC_VER
	std::string image_path{ "E:/GitCode/OCR_Test/test_data/" };
#else
	std::string image_path{ "test_data/" };
#endif

	std::string image_name = image_path + "tirg.jpg";

	int width, height, channels;
	long long t1 = Timer::getNowTime();
	std::unique_ptr<unsigned char[]> data = get_jpeg_decompress_data(image_name.c_str(), width, height, channels);
	long long t2 = Timer::getNowTime();
	if (data == nullptr) {
		fprintf(stderr, "fail to decompress: %s\n", image_name.c_str());
		return -1;
	}

	fprintf(stdout, "decompress time 1: %lldms, width: %d, height: %d, channels: %d\n", t2 - t1, width, height, channels);
	std::string result_image = image_path + "result_tirg.png";
	cv::Mat mat(height, width, CV_8UC3, data.get());
	cv::cvtColor(mat, mat, CV_RGB2BGR);
	cv::imwrite(result_image, mat); // save *.jpg will crash in linux

	int width2, height2, channels2;
	t1 = Timer::getNowTime();
	std::unique_ptr<unsigned char[]> data2 = get_jpeg_decompress_data2(image_name.c_str(), width2, height2, channels2);
	t2 = Timer::getNowTime();
	if (data2 == nullptr) {
		fprintf(stderr, "fail to decompress: %s\n", image_name.c_str());
		return -1;
	}

	fprintf(stdout, "decompress time 2: %lldms, width2: %d, height2: %d, channels2: %d\n", t2 - t1, width2, height2, channels2);
	std::string result_image2 = image_path + "result_tirg2.png";
	cv::Mat mat2(height2, width2, CV_8UC3, data2.get());
	cv::cvtColor(mat2, mat2, CV_RGB2BGR);
	cv::imwrite(result_image2, mat2);

	return 0;
}

std::unique_ptr<unsigned char[]> get_jpeg_decompress_data2(const char* image_name, int& width, int& height, int& channels)
{
	FILE* infile = fopen(image_name, "rb");
	if (infile == nullptr) {
		fprintf(stderr, "can't open %s\n", image_name);
		return nullptr;
	}

	fseek(infile, 0, SEEK_END);
	unsigned long srcSize = ftell(infile);
	std::unique_ptr<unsigned char[]> srcBuf(new unsigned char[srcSize]);
	fseek(infile, 0, SEEK_SET);
	fread(srcBuf.get(), srcSize, 1, infile);
	fclose(infile);

	tjhandle handle = tjInitDecompress();
	int subsamp, cs;
	int ret = tjDecompressHeader3(handle, srcBuf.get(), srcSize, &width, &height, &subsamp, &cs);
	if (cs == TJCS_GRAY) channels = 1;
	else channels = 3;

	int pf = TJCS_RGB;
	int ps = tjPixelSize[pf];
	std::unique_ptr<unsigned char[]> data(new unsigned char[width * height * channels]);
	ret = tjDecompress2(handle, srcBuf.get(), srcSize, data.get(), width, width * channels, height, TJPF_RGB, TJFLAG_NOREALLOC);

	tjDestroy(handle);

	return data;
}

std::unique_ptr<unsigned char[]> get_jpeg_decompress_data(const char* image_name, int& width, int& height, int& channels)
{
	FILE* infile = fopen(image_name, "rb");
	if (infile == nullptr) {
		fprintf(stderr, "can't open %s\n", image_name);
		return nullptr;
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	/* Step 2: specify data source (eg, a file) */
	jpeg_stdio_src(&cinfo, infile);
	/* Step 3: read file parameters with jpeg_read_header() */
	jpeg_read_header(&cinfo, TRUE);
	/* Step 4: set parameters for decompression */
	/* Step 5: Start decompressor */
	jpeg_start_decompress(&cinfo);

	cinfo.out_color_space = JCS_RGB; //JCS_EXT_BGR;

	int row_stride = cinfo.output_width * cinfo.output_components;
	/* Output row buffer */
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	width = cinfo.output_width;
	height = cinfo.output_height;
	channels = cinfo.output_components;
	std::unique_ptr<unsigned char[]> data(new unsigned char[width * height * channels]);

	/* Step 6: while (scan lines remain to be read) */
	for (int j = 0; j < cinfo.output_height; ++j) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		unsigned char* p = data.get() + j * row_stride;
		memcpy(p, buffer[0], row_stride);
	}

	/* Step 7: Finish decompression */
	jpeg_finish_decompress(&cinfo);
	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

	return data;
}

/////////////////////////////////////////////////////////////////////////////
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

int get_jpeg_compress_data2(const unsigned char* data, int width, int height, int pixelFormat, unsigned char** jpegBuf, unsigned long* jpegSize, int jpegSubsamp, int jpegQual, int flags)
{
	tjhandle handle = tjInitCompress();
	int ret = tjCompress2(handle, data, width, 0, height, pixelFormat, jpegBuf, jpegSize, jpegSubsamp, jpegQual, flags);
	tjDestroy(handle);

	return 0;
}

int test_libjpeg_turbo_compress()
{
#ifdef _MSC_VER
	std::string image_path{ "E:/GitCode/OCR_Test/test_data/" };
#else
	std::string image_path{ "test_data/" };
#endif

	std::string name = image_path + "lena.png";
	int quality = 80;

	cv::Mat mat = cv::imread(name);
	if (!mat.data || mat.channels() != 3) {
		fprintf(stderr, "read image fail: %s\n", name.c_str());
		return -1;
	}

	int length = mat.cols * mat.rows * mat.channels();
	std::unique_ptr<unsigned char[]> data(new unsigned char[length]);
	unsigned char* p = data.get();
	unsigned long free_in_buffer;

	long long t1 = Timer::getNowTime();
	get_jpeg_compress_data(mat.data, mat.cols, mat.rows, mat.channels(), JCS_EXT_BGR, quality, &p, length, free_in_buffer);
	long long t2 = Timer::getNowTime();
	fprintf(stdout, "compress time1: %lldms\n", t2 - t1);

	name = image_path + "result1_lena.jpg";
	FILE* outfile = nullptr;
	if ((outfile = fopen(name.c_str(), "wb")) == nullptr) {
		fprintf(stderr, "can't open file: %s\n", name.c_str());
		return -1;
	}
	fwrite(data.get(), sizeof(unsigned char), length - free_in_buffer, outfile);
	fclose(outfile);

	std::unique_ptr<unsigned char[]> data2(new unsigned char[length]);
	p = data2.get();
	unsigned long jpegSize = length;

	unsigned char* pp = nullptr;
	t1 = Timer::getNowTime();
	get_jpeg_compress_data2(mat.data, mat.cols, mat.rows, TJPF_BGR, &p, &jpegSize, TJSAMP_420, quality, 0);
	t2 = Timer::getNowTime();
	fprintf(stdout, "compress time2: %lldms\n", t2 - t1);

	name = image_path + "result2_lena.jpg";
	FILE* outfile2 = nullptr;
	if ((outfile2 = fopen(name.c_str(), "wb")) == nullptr) {
		fprintf(stderr, "can't open file: %s\n", name.c_str());
		return -1;
	}
	fwrite(p, sizeof(unsigned char), jpegSize, outfile2);
	fclose(outfile2);

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

