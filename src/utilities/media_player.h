#pragma once
#include <wx/wxprec.h>
#include <filesystem>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <SDL.h>

class media_player
{
public:
	media_player(std::function<void(const unsigned char*, int, int, int)> paint_cb);
	~media_player();

	std::function<void(const unsigned char*, int, int, int)> paint_cb;

	float volume;
	bool loop;

	void load_media(const std::filesystem::path& media_path);

	void play_media();
	void pause_media();
	void stop_media();

	void set_volume(float volume);

	void seek_to_position(double position);

	void rewind_media(float speed_multiplied);
	void fast_forward_media(float speed_multiplied);

	void jump_forward(double seconds);
	void jump_backward(double seconds);

	void jump_to_media_start();
	void jump_to_media_end();

	void jump_to_next_media();
	void jump_to_previous_media();
};
