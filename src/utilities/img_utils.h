#pragma once
#include <wx/wxprec.h>
#include <filesystem>

class ImgUtils
{
public:
	static wxImage Resize_Fit(wxImage img, int targetSizeX, int targetSizeY);
	static wxImage Resize_Fill(wxImage img, int targetSizeX, int targetSizeY);
	static wxPoint Position_Center(wxImage img, int windowSizeX, int windowSizeY);

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
