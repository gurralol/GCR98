#pragma once
#include <wx/wxprec.h>
#include "../gui/panel_media.h"
#include "../gui/panel_mediactrls.h"
#include "../gui/panel_playlist.h"

class MediaPlayer
{
public:
	MediaPlayer();
	~MediaPlayer();

	wxString path;
	wxSize resolution;
	float fps;
	float duration;
	float size;

	bool seek;
	int loop;
	bool autoplay;

	enum //LOOP
	{
		NONE,
		LOOP,
		LOOP1,
		SHUFFLE
	};

	// FFmpeg contexts & libwebp data

	void ClearAll();
	void ClearFFmpeg();
	void ClearWebp();

	void ReadFrameFFmpeg(int frame);
	void ReadFrameLibwebp(int frame);
	
	void SeekFFmpeg(double targetPts);
	void SeekLibwebp(double targetPts);

	void ProcessFFmpeg();
	void ProcessLibwebp();
};
