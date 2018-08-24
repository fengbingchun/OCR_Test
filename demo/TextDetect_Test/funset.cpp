#include "funset.hpp"
#include <string>
#include <opencv2/opencv.hpp>
#include "trg.hpp"

int test_TiRG()
{
	// Blog: https://blog.csdn.net/fengbingchun/article/details/79819800
#ifdef _MSC_VER
	const std::string name{ "E:/GitCode/OCR_Test/test_data/tirg.jpg" };
#else
	const std::string name{ "test_data/tirg.jpg" };
#endif
	cv::Mat mat = cv::imread(name, 1);
	if (!mat.data || mat.channels() != 3) {
		fprintf(stderr, "read image fail: %s\n", name.c_str());
		return -1;
	}

	const int width{ mat.cols }, height{ mat.rows };
	std::vector<std::vector<trg::Rgb>> data(height, std::vector<trg::Rgb>(width));

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			cv::Vec3b pixel = mat.at<cv::Vec3b>(cv::Point(x, y));
			trg::Rgb rgb = { pixel.val[2], pixel.val[1], pixel.val[0] };
			data[y][x] = rgb;
		}
	}

	std::vector<trg::Rect> rect = trg::get_textlike_regions(data);

	for (int i = 0; i < rect.size(); ++i) {
		cv::rectangle(mat, cv::Point(rect[i].x1, rect[i].y1), cv::Point(rect[i].x2, rect[i].y2), cv::Scalar(0, 255, 0));
	}

#ifdef _MSC_VER
	cv::imwrite("E:/GitCode/OCR_Test/test_data/result_tirg.jpg", mat);
#else
	cv::imwrite("test_data/result_tirg.jpg", mat);
#endif
	return 0;
}
