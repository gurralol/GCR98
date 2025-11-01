#pragma once
#include <wx/wxprec.h>
#include <opencv2/opencv.hpp>
#include <filesystem>

class img_utils
{
public:
	static wxImage Resize_Fit(wxImage img, int targetSizeX, int targetSizeY);
	static wxPoint Position_Center(wxImage img, int windowSizeX, int windowSizeY);

	static cv::Mat ResizeCvMat(cv::Mat img, int targetSizeX, int targetSizeY);

	static wxImage wxFromCv(const cv::Mat& mat);
	static cv::Mat cvFromWx(wxImage img);

	static void Borders_Rounded(wxImage &img, int radius);

	enum //Fit
	{
		KEEP_AR = 0,
		STRETCH = 1,
		CENTER = 3,
	};

	static wxImage get_thumbnail_cache(std::filesystem::path path, int width, int height, int fit);
	static wxImage get_thumbnail_ffmpeg(std::filesystem::path path, int width, int height, int fit);
	static wxImage get_thumbnail_opencv(std::filesystem::path path, int width, int height, int fit);
	static wxImage get_thumbnail_libwebp(std::filesystem::path path, int width, int height, int fit);
};
