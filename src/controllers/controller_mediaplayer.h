#pragma once
#include "../utilities/mediaplayer.h"

class ControllerMediaplayer
{
public:
	ControllerMediaplayer();
	~ControllerMediaplayer();

	MediaPlayer* m_mediaPlayer;
};