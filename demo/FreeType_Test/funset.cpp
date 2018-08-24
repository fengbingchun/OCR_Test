#include "funset.hpp"
#include <iostream>

//It contains various macro declarations that are later used to #include the
//appropriate public FreeType 2 header files.
#include <ft2build.h>
//FT_FREETYPE_H is a special macro defined in file ftheader.h. It contains some
//installation-specific macros to name other public header files of the FreeType 2 API.
#include FT_FREETYPE_H
#include <ftglyph.h>

#include <opencv2/opencv.hpp>

#include "CvxText.hpp"

int test_opencv_support_chinese_text()
{
	// Blog: https://blog.csdn.net/fengbingchun/article/details/79806616
#ifdef _MSC_VER
	cv::Mat mat = cv::imread("E:/GitCode/OCR_Test/test_data/lena.png", 1);
#else
	cv::Mat mat = cv::imread("test_data/lena.png", 1);
#endif
	if (!mat.data || mat.channels() != 3) {
		fprintf(stderr, "read image fail\n");
		return -1;
	}

#ifdef _MSC_VER
	CvxText text("E:/GitCode/OCR_Test/test_data/simhei.ttf"); //指定字体
#else
	CvxText text("test_data/simhei.ttf"); //指定字体
#endif
	cv::Scalar size1{ 100, 0.5, 0.1, 0 }, size2{ 100, 0, 0.1, 0 }, size3{ 50, 0, 1, 0 }, size4{50, 0, 0.1, 0}; // (字体大小, 无效的, 字符间距, 无效的 }

	text.setFont(nullptr, &size1, nullptr, 0);
	text.putText(mat, "中国", cv::Point(50, 100));

	text.setFont(nullptr, &size2, nullptr, 0);
	text.putText(mat, "北京", cv::Point(50, 200), cv::Scalar(255, 0, 0));

	text.setFont(nullptr, &size3, nullptr, 0);
	text.putText(mat, "China", cv::Point(50, 250), cv::Scalar(0, 255, 0));

	text.setFont(nullptr, &size4, nullptr, 0);
	text.putText(mat, "BeiJing", cv::Point(50, 300), cv::Scalar(0, 0, 255));

#ifdef _MSC_VER
	cv::imwrite("E:/GitCode/OCR_Test/test_data/result_lena.png", mat);
#else
	cv::imwrite("test_data/result_lena.png", mat);
#endif

	return 0;
}

///////////////////////////////////////////////////////////////////////
int test_freetype_1()
{
	FT_Library library; /* handle to library */
	FT_Error error;
	FT_Face face; /* handle to face object */

	//1. Library Initialization
	//To initialize the FreeType library, create a variable of type FT_Library named, and call the function FT_Init_FreeType.
	error = FT_Init_FreeType(&library);
	if (error) {
		fprintf(stderr, "an error occurred during library initialization\n");
		return -1;
	}

	//2. Loading a Font Face
	//From a Font File, Create a new face object by calling FT_New_Face. A face describes a given typeface and style.
#ifdef _MSC_VER
	char* filename = "E:/GitCode/OCR_Test/test_data/kaiu.ttf";
#else
	char* filename = "test_data/kaiu.ttf";
#endif
	error = FT_New_Face(library, filename, 0, &face);

	if (error == FT_Err_Unknown_File_Format) {
		fprintf(stderr, "the font file could be opened and read, its font format is unsupported\n");
		return -1;
	} else if (error) {
		fprintf(stderr, "the font file could not be opened or read, or that it is broken\n");
		return -1;
	}

	//3. Accessing the Face Data
	//A face object models all information that globally describes the face.
	//Usually, this data can be accessed directly by dereferencing a handle, like in face−>num_glyphs.
	FT_Long numGlygphs = face->num_glyphs;
	FT_Long numFaces = face->num_faces;
	FT_String* familyName = face->family_name;
	fprintf(stdout, "num_glyphs = %ld, num_faces = %ld, family_name = %s\n", numGlygphs, numFaces, familyName);

	//4. Setting the Current Pixel Size
	//FreeType 2 uses size objects to model all information related to a given character size for a given face.
	//error = FT_Set_Pixel_Sizes(face, 0, 16);
	error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0);
	if (error) {
		fprintf(stderr, "an error occurs when trying to set the pixel size to a value\n");
		return -1;
	}

	//5. Loading a Glyph Image
	//Converting a Character Code Into a Glyph Index
	FT_UInt charIndex = FT_Get_Char_Index(face, 65); //65 => 'A'
	//Once you have a glyph index, you can load the corresponding glyph image
	error = FT_Load_Glyph(face, charIndex, FT_LOAD_DEFAULT);
	if (error) {
		fprintf(stderr, "an error occurs when trying to load the corresponding glgyh image\n");
		return -1;
	}

	//6. Simple Text Rendering
	error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	if (error) {
		fprintf(stderr, "an error occurs when trying to render glyph\n");
		return -1;
	}

	FT_Glyph glyph;
	error = FT_Get_Glyph(face->glyph, &glyph);
	if (error) {
		fprintf(stderr, "get glyph error\n");
		return -1;
	}

	//convert glyph to bitmap with 256 gray 
	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
	FT_Bitmap& bitmap = bitmap_glyph->bitmap;
	for (int i = 0; i < bitmap.rows; ++i) {
		for (int j = 0; j < bitmap.width; ++j) {
			//if it has gray > 0 we set show it as 1, 0 otherwise  
			fprintf(stdout, " %d ", bitmap.buffer[i * bitmap.width + j] ? 1 : 0);
		}
		fprintf(stdout, "\n");
	}

	FT_Done_Glyph(glyph);
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return 0;
}

