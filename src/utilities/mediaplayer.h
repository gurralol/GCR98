#pragma once
#include <wx/wxprec.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <SDL.h>

#include <webp/demux.h>
#include <webp/decode.h>

class MediaPlayer
{
public:
	MediaPlayer();
	~MediaPlayer();

	std::filesystem::path path;
	int mediaWidth;
	int mediaHeight;
	float fps;
	float duration;
	std::function<void()> paintCallback;
	std::mutex frameMutex;

	bool loop;

	wxImage currFrame;
	double currTime;
	double seekTarget;

	std::atomic_bool restrictProcessing;
	std::atomic_bool isProcessing;

	// FFmpeg
	AVFormatContext* formatCtx;
	AVCodecContext* vCodecCtx;
	AVCodecContext* aCodecCtx;
	int vStreamIdx;
	int aStreamIdx;

	// Libwebp
	WebPData* webpData;
	WebPDemuxer* webpDemux;

	void InitFFmpeg();
	void InitWebP();

	void FreeAll();
	void FreeFFmpeg();
	void FreeWebp();
	
	void SeekFFmpeg();
	void SeekLibwebp();

	void DecodeFrameFFmpeg();
	void DecodeFrameLibwebp();

	void ProcessFFmpeg();
	void ProcessLibwebp();

	void StopProcessing();
};
