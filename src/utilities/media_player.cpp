#include "media_player.h"

media_player::media_player(std::function<void(const unsigned char*, int, int, int)> paint_cb)
	: paint_cb(paint_cb)
{
	this->volume = 1.0f;
	this->loop = false;
}

media_player::~media_player()
{
}
