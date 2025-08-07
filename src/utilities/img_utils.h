#pragma once
#include <wx/wxprec.h>
#include <opencv2/opencv.hpp>
#include <filesystem>

class ImgUtils
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
		KEEP_AR,
		STRETCH,
		CENTER
	};

	static wxImage GetThumbnail_ThumbnailCache(std::filesystem::path path, int width, int height, int fit);
	static wxImage GetThumbnail_FFmpeg(std::filesystem::path path, int width, int height, int fit);
	static wxImage GetThumbnail_OpenCV(std::filesystem::path path, int width, int height, int fit);
	static wxImage GetThumbnail_libwebp(std::filesystem::path path, int width, int height, int fit);
};
