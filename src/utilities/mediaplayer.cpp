#include "mediaplayer.h"

MediaPlayer::MediaPlayer()
{
	path = "";
	resolution = wxSize(0, 0);
	fps = 0.0;
	duration = 0.0;
	size = 0.0;

	seek = false;
	loop = false;
	autoplay = false;
}

MediaPlayer::~MediaPlayer()
{
}
