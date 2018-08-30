#include "funset.hpp"

#include <string>

#include <zlib.h>
#include <png.h>
#include <jpeglib.h>
#include <tiff.h>
#include <tiffio.h>
#include <gif_lib.h>
#include <allheaders.h>

// Blog: https://blog.csdn.net/fengbingchun/article/details/44275233

int test_read_tiff()
{
	// reference: http://research.cs.wisc.edu/graphics/Courses/638-f1999/libtiff_tutorial.htm
#ifdef _MSC_VER
	const std::string image_name { "E:/GitCode/OCR_Test/test_data/eurotext.tif" };
#else
	const std::string image_name { "test_data/eurotext.tif" };
#endif
	TIFF* tif = TIFFOpen(image_name.c_str(), "r");
	int width{ 0 }, height{ 0 };
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	fprintf(stdout, "width: %d, height: %d\n", width, height);

	TIFFClose(tif);

	// Note: liblept library read tiff file will crash: Error in findTiffCompression: tif not opened
	//PIX* pixs = pixRead(image_name.c_str());
	//if (pixs == nullptr) {
	//	fprintf(stderr, "read image error\n");
	//	return -1;
	//}

	return 0;
}

//////////////////////////////////////////////////////
int test_read_png()
{
#ifdef _MSC_VER
	const std::string strSrc{ "E:/GitCode/OCR_Test/test_data/weasel.png" };
	const std::string strDst{ "E:/GitCode/OCR_Test/test_data/dst.gif" };
#else
	const std::string strSrc{ "test_data/weasel.png" };
	const std::string strDst{ "test_data/dst.gif" };
#endif
	PIX* pixs = pixRead(strSrc.c_str());
	if (pixs == nullptr) {
		fprintf(stderr, "read image error\n");
		return -1;
	}

#ifdef _MSC_VER
	const l_float32  ANGLE1 = 3.14159265 / 12.;
	l_int32 w, h, d;
	pixGetDimensions(pixs, &w, &h, &d);
	PIX* pixd = pixRotate(pixs, ANGLE1, L_ROTATE_SHEAR, L_BRING_IN_WHITE, w, h);
	pixWrite(strDst.c_str(), pixd, IFF_GIF);
	pixDestroy(&pixd);
#endif

	pixDestroy(&pixs);
	return 0;
}

/////////////////////////////////////////////////////////////
int test_read_jpeg()
{
#ifdef _MSC_VER
	std::string strSrc{ "E:/GitCode/OCR_Test/test_data/marge.jpg" };
	std::string strDst{ "E:/GitCode/OCR_Test/test_data/dst.bmp" };
#else
	std::string strSrc{ "test_data/marge.jpg" };
	std::string strDst{ "test_data/dst.bmp" };
#endif

	PIX* pixs = pixRead(strSrc.c_str());
	if (pixs == nullptr) {
		fprintf(stderr, "read image error\n");
		return -1;
	}

	l_int32 w, h, d;
	pixGetDimensions(pixs, &w, &h, &d);
	if (d != 8) {
		fprintf(stderr, "pixs not 8 bpp\n");
		return -1;
	}

	PIX* pixf = pixSobelEdgeFilter(pixs, L_HORIZONTAL_EDGES);
	PIX* pixd = pixThresholdToBinary(pixf, 10);
	pixInvert(pixd, pixd);
	pixWrite(strDst.c_str(), pixd, IFF_BMP);

	pixDestroy(&pixs);
	pixDestroy(&pixf);
	pixDestroy(&pixd);

	return 0;
}

