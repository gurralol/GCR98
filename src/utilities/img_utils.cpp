#include <winsock2.h>    // Must come before windows.h
#include <windows.h>
#include <wincodec.h>    // WIC
#include <shobjidl.h>    // IShellItem
#include <shlwapi.h>
#include <comdef.h>
#include <algorithm>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

#include "img_utils.h"

wxImage ImgUtils::Resize_Fit(wxImage img, int targetSizeX, int targetSizeY)
{
    int boxSizeX = targetSizeX;
    int boxSizeY = targetSizeY;
    float boxAr = static_cast<float>(boxSizeX) / static_cast<float>(boxSizeY);

    int srcImgX = img.GetSize().GetWidth();
    int srcImgY = img.GetSize().GetHeight();
    float srcAr = static_cast<float>(srcImgX) / static_cast<float>(srcImgY);

    if (boxAr > srcAr) {
        srcImgY = boxSizeY;
        srcImgX = boxSizeY * srcAr;
    }
    else if (boxAr < srcAr) {
        srcImgX = boxSizeX;
        srcImgY = boxSizeX / srcAr;
    }
    else if (boxAr == srcAr) {
        srcImgX = boxSizeX;
        srcImgY = boxSizeY;
    }

    wxImage temp = img;
    temp.Rescale(srcImgX, srcImgY, wxIMAGE_QUALITY_BICUBIC);

    return temp;
}

wxPoint ImgUtils::Position_Center(wxImage img, int windowSizeX, int windowSizeY)
{
    int boxSizeX = windowSizeX;
    int boxSizeY = windowSizeY;
    float boxAr = static_cast<float>(boxSizeX) / static_cast<float>(boxSizeY);

    int srcImgX = img.GetSize().GetWidth();
    int srcImgY = img.GetSize().GetHeight();
    float srcAr = static_cast<float>(srcImgX) / static_cast<float>(srcImgY);

    if (boxAr > srcAr) {
        srcImgY = boxSizeY;
        srcImgX = boxSizeY * srcAr;
    }
    else if (boxAr < srcAr) {
        srcImgX = boxSizeX;
        srcImgY = boxSizeX / srcAr;
    }
    else if (boxAr == srcAr) {
        srcImgX = boxSizeX;
        srcImgY = boxSizeY;
    }

    int posX = (boxSizeX - srcImgX) / 2;
    int posY = (boxSizeY - srcImgY) / 2;

    return wxPoint(posX, posY);
}

cv::Mat ImgUtils::ResizeCvMat(cv::Mat img, int targetSizeX, int targetSizeY)
{
    int boxSizeX = targetSizeX;
    int boxSizeY = targetSizeY;
    float boxAr = static_cast<float>(boxSizeX) / static_cast<float>(boxSizeY);

    int srcImgX = img.cols;
    int srcImgY = img.rows;
    float srcAr = static_cast<float>(srcImgX) / static_cast<float>(srcImgY);

    if (boxAr > srcAr) {
        srcImgY = boxSizeY;
        srcImgX = boxSizeY * srcAr;
    }
    else if (boxAr < srcAr) {
        srcImgX = boxSizeX;
        srcImgY = boxSizeX / srcAr;
    }
    else if (boxAr == srcAr) {
        srcImgX = boxSizeX;
        srcImgY = boxSizeY;
    }

    cv::Mat temp = img;
    cv::resize(temp, temp, cv::Size(srcImgX, srcImgY), 0, 0, cv::INTER_CUBIC);

    return temp;
}

wxImage ImgUtils::wxFromCv(const cv::Mat& mat)
{
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    wxImage img = wxImage(rgb.cols, rgb.rows, rgb.data, true);
    wxImage safe = img.Copy();
    return safe;
}

cv::Mat ImgUtils::cvFromWx(wxImage img)
{
    wxImage copy = img.Copy();
    cv::Mat mat(img.GetHeight(), img.GetWidth(), CV_8UC3, (void*)copy.GetData());
    cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
    return mat.clone();
}

void ImgUtils::Borders_Rounded(wxImage& img, int radius)
{
    int width = img.GetWidth();
    int height = img.GetHeight();

    if (!img.HasAlpha())
    {
        img.InitAlpha();
    }

    unsigned char* alpha = img.GetAlpha();

    auto RoundCorners = [&](int cx, int cy, int startX, int startY, int endX, int endY)
        {
            for (int y = startY; y < endY; y++)
            {
                for (int x = startX; x < endX; x++)
                {
                    int dx = x - cx;
                    int dy = y - cy;
                    if (dx * dx + dy * dy > radius * radius)
                    {
                        alpha[y * width + x] = 0;
                    }
                }
            }
        };

    // Top-left
    RoundCorners(radius, radius, 0, 0, radius, radius);
    // Top-right
    RoundCorners(width - radius - 1, radius, width - radius, 0, width, radius);
    // Bottom-left
    RoundCorners(radius, height - radius - 1, 0, height - radius, radius, height);
    // Bottom-right
    RoundCorners(width - radius - 1, height - radius - 1, width - radius, height - radius, width, height);
}

wxImage ImgUtils::GetThumbnail_ThumbnailCache(std::filesystem::path path, int width, int height, int fit)
{
    wxImage result;

    // Initialize COM - make sure CoInitialize is called before using this function!
    HRESULT hr;
    //CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IShellItem* pItem = nullptr;
    hr = SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&pItem));
    if (FAILED(hr) || !pItem) {
        OutputDebugString(L"Failed to create shell item\n");
        return result;
    }

    IShellItemImageFactory* pImageFactory = nullptr;
    hr = pItem->QueryInterface(IID_PPV_ARGS(&pImageFactory));
    pItem->Release();
    if (FAILED(hr) || !pImageFactory) {
        OutputDebugString(L"Failed to get IShellItemImageFactory\n");
        return result;
    }

    SIZE requestedSize = { width, height };
    HBITMAP hBitmap = nullptr;

    // Get original thumbnail (no forced resize, so we get native aspect ratio)
    hr = pImageFactory->GetImage(requestedSize, 0, &hBitmap);
    pImageFactory->Release();
    if (FAILED(hr) || !hBitmap) {
        OutputDebugString(L"Failed to get thumbnail bitmap\n");
        return result;
    }

    // Get bitmap info
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Create a DIB section to read pixels
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight;  // negative for top-down
    bi.biPlanes = 1;
    bi.biBitCount = 32;  // Use 32-bit to avoid palette issues
    bi.biCompression = BI_RGB;

    void* pixels = nullptr;
    HDC hdc = GetDC(nullptr);
    HBITMAP hDibSrc = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pixels, nullptr, 0);
    ReleaseDC(nullptr, hdc);

    if (!pixels) {
        DeleteObject(hBitmap);
        return result;
    }

    // Copy hBitmap into our DIB section for easy pixel access
    HDC hMemDC = CreateCompatibleDC(nullptr);
    HGDIOBJ oldBmp = SelectObject(hMemDC, hDibSrc);
    HDC hSrcDC = CreateCompatibleDC(nullptr);
    SelectObject(hSrcDC, hBitmap);

    if (!BitBlt(hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hSrcDC, 0, 0, SRCCOPY)) {
        OutputDebugString(L"BitBlt failed\n");
        SelectObject(hMemDC, oldBmp);
        DeleteDC(hMemDC);
        DeleteDC(hSrcDC);
        DeleteObject(hDibSrc);
        DeleteObject(hBitmap);
        return result;
    }

    SelectObject(hMemDC, oldBmp);
    DeleteDC(hMemDC);
    DeleteDC(hSrcDC);
    DeleteObject(hBitmap);

    if (fit == 0 || fit == 1) {
        int finalWidth = height;
        int finalHeight = width;

        // For PreserveAspectRatio, compute new size
        if (fit == 0) {
            double scaleX = (double)width / bmp.bmWidth;
            double scaleY = (double)height / bmp.bmHeight;
            double scale = std::min(scaleX, scaleY);
            finalWidth = static_cast<int>(bmp.bmWidth * scale);
            finalHeight = static_cast<int>(bmp.bmHeight * scale);
        }

        // Create DIB for the scaled image
        BITMAPINFOHEADER biDst = {};
        biDst.biSize = sizeof(BITMAPINFOHEADER);
        biDst.biWidth = finalWidth;
        biDst.biHeight = -finalHeight;
        biDst.biPlanes = 1;
        biDst.biBitCount = 32;
        biDst.biCompression = BI_RGB;

        void* dstPixels = nullptr;
        hdc = GetDC(nullptr);
        HBITMAP hDibDst = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&biDst), DIB_RGB_COLORS, &dstPixels, nullptr, 0);
        ReleaseDC(nullptr, hdc);

        if (!dstPixels) {
            DeleteObject(hDibSrc);
            return result;
        }

        HDC srcDC2 = CreateCompatibleDC(nullptr);
        SelectObject(srcDC2, hDibSrc);
        HDC dstDC = CreateCompatibleDC(nullptr);
        SelectObject(dstDC, hDibDst);

        StretchBlt(dstDC, 0, 0, finalWidth, finalHeight, srcDC2, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

        DeleteDC(srcDC2);
        DeleteDC(dstDC);
        DeleteObject(hDibSrc);

        // If PreserveAspectRatio, center in target size
        if (fit == 0) {
            result.Create(width, height, false);
            unsigned char* dst = result.GetData();
            memset(dst, 0, width * height * 3);  // fill black background

            unsigned char* src = static_cast<unsigned char*>(dstPixels);
            int offsetX = (width - finalWidth) / 2;
            int offsetY = (height - finalHeight) / 2;

            for (int y = 0; y < finalHeight; ++y) {
                int dstY = y + offsetY;
                for (int x = 0; x < finalWidth; ++x) {
                    int dstIndex = (dstY * width + x + offsetX) * 3;
                    int srcIndex = (y * finalWidth + x) * 4;

                    dst[dstIndex + 0] = src[srcIndex + 2]; // R
                    dst[dstIndex + 1] = src[srcIndex + 1]; // G
                    dst[dstIndex + 2] = src[srcIndex + 0]; // B
                }
            }
        }
        else { // StretchToFit - copy directly
            result.Create(width, height, false);
            unsigned char* dst = result.GetData();
            unsigned char* src = static_cast<unsigned char*>(dstPixels);

            for (int i = 0; i < width * height; ++i) {
                dst[i * 3 + 0] = src[i * 4 + 2];
                dst[i * 3 + 1] = src[i * 4 + 1];
                dst[i * 3 + 2] = src[i * 4 + 0];
            }
        }
        DeleteObject(hDibDst);
    }
    else if (fit == 2) {
        // Create wxImage with black background
        result.Create(width, height, false);
        unsigned char* dst = result.GetData();
        memset(dst, 0, width * height * 3);  // black background

        int offsetX = (width - bmp.bmWidth) / 2;
        int offsetY = (height - bmp.bmHeight) / 2;

        unsigned char* src = static_cast<unsigned char*>(pixels);

        for (int y = 0; y < bmp.bmHeight; ++y) {
            int dstY = y + offsetY;
            if (dstY < 0 || dstY >= height) continue;
            for (int x = 0; x < bmp.bmWidth; ++x) {
                int dstX = x + offsetX;
                if (dstX < 0 || dstX >= width) continue;

                int dstIndex = (dstY * width + dstX) * 3;
                int srcIndex = (y * bmp.bmWidth + x) * 4;

                dst[dstIndex + 0] = src[srcIndex + 2];
                dst[dstIndex + 1] = src[srcIndex + 1];
                dst[dstIndex + 2] = src[srcIndex + 0];
            }
        }

        DeleteObject(hDibSrc);
    }
    else {
        // Unknown mode - fallback: return original bitmap size wxImage
        result.Create(bmp.bmWidth, bmp.bmHeight, false);
        unsigned char* dst = result.GetData();
        unsigned char* src = static_cast<unsigned char*>(pixels);
        for (int i = 0; i < bmp.bmWidth * bmp.bmHeight; ++i) {
            dst[i * 3 + 0] = src[i * 4 + 2];
            dst[i * 3 + 1] = src[i * 4 + 1];
            dst[i * 3 + 2] = src[i * 4 + 0];
        }

        DeleteObject(hDibSrc);
    }

    return result;
}

wxImage ImgUtils::GetThumbnail_OpenCV(std::filesystem::path path, int width, int height, int fit)
{
    return wxImage();
}

wxImage ImgUtils::GetThumbnail_FFmpeg(std::filesystem::path path, int width, int height, int fit)
{
    return wxImage();
}

wxImage ImgUtils::GetThumbnail_libwebp(std::filesystem::path path, int width, int height, int fit)
{
    return wxImage();
}
