#ifndef OPENCV_CVX_TEXT_HPP_
#define OPENCV_CVX_TEXT_HPP_

// source from: http://www.opencv.org.cn/forum.php?mod=viewthread&tid=2083&extra=&page=1
// 支持OpenCV中文汉字输入

#include <ft2build.h>
#include FT_FREETYPE_H

#include <opencv2/opencv.hpp>

class CvxText {
public:
	/**
	* 装载字库文件
	*/
	CvxText(const char* freeType);
	virtual ~CvxText();

	/**
	* 获取字体.目前有些参数尚不支持.
	*
	* \param font        字体类型, 目前不支持
	* \param size        字体大小/空白比例/间隔比例/旋转角度
	* \param underline   下画线
	* \param diaphaneity 透明度
	*
	* \sa setFont, restoreFont
	*/
	void getFont(int* type, cv::Scalar* size=nullptr, bool* underline=nullptr, float* diaphaneity=nullptr);

	/**
	* 设置字体.目前有些参数尚不支持.
	*
	* \param font        字体类型, 目前不支持
	* \param size        字体大小/空白比例/间隔比例/旋转角度
	* \param underline   下画线
	* \param diaphaneity 透明度
	*
	* \sa getFont, restoreFont
	*/
	void setFont(int* type, cv::Scalar* size=nullptr, bool* underline=nullptr, float* diaphaneity=nullptr);

	/**
	* 恢复原始的字体设置.
	*
	* \sa getFont, setFont
	*/
	void restoreFont();

	/**
	* 输出汉字(颜色默认为黑色).遇到不能输出的字符将停止.
	*
	* \param img  输出的影象
	* \param text 文本内容
	* \param pos  文本位置
	*
	* \return 返回成功输出的字符长度，失败返回-1.
	*/
	int putText(cv::Mat& img, const char* text, cv::Point pos);

	/**
	* 输出汉字(颜色默认为黑色).遇到不能输出的字符将停止.
	*
	* \param img  输出的影象
	* \param text 文本内容
	* \param pos  文本位置
	*
	* \return 返回成功输出的字符长度，失败返回-1.
	*/
	int putText(cv::Mat& img, const wchar_t* text, cv::Point pos);

	/**
	* 输出汉字.遇到不能输出的字符将停止.
	*
	* \param img   输出的影象
	* \param text  文本内容
	* \param pos   文本位置
	* \param color 文本颜色
	*
	* \return 返回成功输出的字符长度，失败返回-1.
	*/
	int putText(cv::Mat& img, const char* text, cv::Point pos, cv::Scalar color);

	/**
	* 输出汉字.遇到不能输出的字符将停止.
	*
	* \param img   输出的影象
	* \param text  文本内容
	* \param pos   文本位置
	* \param color 文本颜色
	*
	* \return 返回成功输出的字符长度，失败返回-1.
	*/
	int putText(cv::Mat& img, const wchar_t* text, cv::Point pos, cv::Scalar color);

private:
	// 禁止copy
	CvxText& operator=(const CvxText&);
	// 输出当前字符, 更新m_pos位置
	void putWChar(cv::Mat& img, wchar_t wc, cv::Point& pos, cv::Scalar color);

	FT_Library   m_library;   // 字库
	FT_Face      m_face;      // 字体

	// 默认的字体输出参数
	int         m_fontType;
	cv::Scalar   m_fontSize;
	bool      m_fontUnderline;
	float      m_fontDiaphaneity;
};

#endif // OPENCV_CVX_TEXT_HPP_

