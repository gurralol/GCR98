#include "mediaplayer.h"

MediaPlayer::MediaPlayer()
{
	mediaWidth = 0;
	mediaHeight = 0;
	fps = 0;
	duration = 0;

	loop = false;

	restrictProcessing = false;
	isProcessing = false;

	formatCtx = nullptr;
	vCodecCtx = nullptr;
	aCodecCtx = nullptr;
	vStreamIdx = -1;
	aStreamIdx = -1;

	webpData = nullptr;
	webpDemux = nullptr;
}

MediaPlayer::~MediaPlayer()
{
}

void MediaPlayer::InitFFmpeg()
{
	if (avformat_open_input(&formatCtx, path.string().c_str(), nullptr, nullptr) < 0)
	{
		avformat_close_input(&formatCtx);
		return;
	}

	if (avformat_find_stream_info(formatCtx, nullptr) < 0)
	{
		avformat_close_input(&formatCtx);
		return;
	}

	vStreamIdx = -1;
	for (unsigned int i = 0; i < formatCtx->nb_streams; i++)
	{
		if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			vStreamIdx = i;
			break;
		}
	}
	if (vStreamIdx == -1)
	{
		avformat_close_input(&formatCtx);
		return;
	}

	aStreamIdx = -1;
	for (unsigned int i = 0; i < formatCtx->nb_streams; i++)
	{
		if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			aStreamIdx = i;
			break;
		}
	}
	if (aStreamIdx == -1)
	{
	}

	AVCodecParameters* vCodecPar = formatCtx->streams[vStreamIdx]->codecpar;
	const AVCodec* vCodec = avcodec_find_decoder(vCodecPar->codec_id);
	if (!vCodec)
	{
		avformat_close_input(&formatCtx);
		return;
	}
	vCodecCtx = avcodec_alloc_context3(vCodec);
	avcodec_parameters_to_context(vCodecCtx, vCodecPar);
	if (avcodec_open2(vCodecCtx, vCodec, nullptr) < 0)
	{
		avcodec_free_context(&vCodecCtx);
		avformat_close_input(&formatCtx);
		return;
	}

	if (aStreamIdx != -1)
	{
		AVCodecParameters* aCodecPar = formatCtx->streams[aStreamIdx]->codecpar;
		const AVCodec* aCodec = avcodec_find_decoder(aCodecPar->codec_id);
		if (!aCodec)
		{
			avformat_close_input(&formatCtx);
			return;
		}
		aCodecCtx = avcodec_alloc_context3(aCodec);
		avcodec_parameters_to_context(aCodecCtx, aCodecPar);
		if (avcodec_open2(aCodecCtx, aCodec, nullptr) < 0)
		{
			avcodec_free_context(&vCodecCtx);
			avcodec_free_context(&aCodecCtx);
			avformat_close_input(&formatCtx);
			return;
		}
	}
	
	duration = 0;
	if (formatCtx->duration != AV_NOPTS_VALUE)
	{
		duration = formatCtx->duration / (double)AV_TIME_BASE;
	}
	else if (formatCtx->streams[vStreamIdx]->duration != AV_NOPTS_VALUE)
	{
		duration = formatCtx->streams[vStreamIdx]->duration * av_q2d(formatCtx->streams[vStreamIdx]->time_base);
	}

	AVRational framerate = av_guess_frame_rate(formatCtx, formatCtx->streams[vStreamIdx], nullptr);
	fps = av_q2d(framerate);
}

void MediaPlayer::InitWebP()
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file)
	{
		return;
	}

	std::streamsize size = file.tellg();
	if (size <= 0)
	{
		return;
	}

	file.seekg(0, std::ios::beg);
	std::vector<uint8_t> buffer(static_cast<size_t>(size));
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
	{
		return;
	}

	webpData = new WebPData();
	webpData->bytes = static_cast<uint8_t*>(malloc(buffer.size()));
	if (!webpData->bytes)
	{
		return;
	}
	std::memcpy((void*)webpData->bytes, buffer.data(), buffer.size());
	webpData->size = buffer.size();

	webpDemux = WebPDemux(webpData);
	if (!webpDemux)
	{
		return;
	}
}

void MediaPlayer::FreeAll()
{
	FreeFFmpeg();
	FreeWebp();
}

void MediaPlayer::FreeFFmpeg()
{
	if (formatCtx) avformat_close_input(&formatCtx);
	if (vCodecCtx) avcodec_free_context(&vCodecCtx);
	if (aCodecCtx) avcodec_free_context(&aCodecCtx);
	vStreamIdx = -1;
	aStreamIdx = -1;
}

void MediaPlayer::FreeWebp()
{
	if (webpData) WebPDataClear(webpData);
}

void MediaPlayer::SeekFFmpeg()
{
	int64_t frameTarget = static_cast<int64_t>(seekTarget / av_q2d(formatCtx->streams[vStreamIdx]->time_base));
	av_seek_frame(formatCtx, vStreamIdx, frameTarget, AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(vCodecCtx);
}

void MediaPlayer::SeekLibwebp()
{
}

void MediaPlayer::DecodeFrameFFmpeg()
{
	isProcessing = true;

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	AVFrame* rgbFrame = av_frame_alloc();

	int width = vCodecCtx->width;
	int height = vCodecCtx->height;

	rgbFrame->format = AV_PIX_FMT_RGB24;
	rgbFrame->width = width;
	rgbFrame->height = height;

	if (av_frame_get_buffer(rgbFrame, 0) < 0)
	{
		av_packet_unref(packet);
		av_frame_free(&frame);
		av_frame_free(&rgbFrame);
	}

	struct SwsContext* swsCtx = sws_getContext(
		width, height, vCodecCtx->pix_fmt,
		width, height, AV_PIX_FMT_RGB24,
		SWS_BICUBIC, nullptr, nullptr, nullptr
	);
	
	bool framePainted = false;

	loopPoint:
	auto startTime = std::chrono::steady_clock::now();

	while (av_read_frame(formatCtx, packet) == 0)
	{
		if (packet->stream_index == vStreamIdx)
		{
			if (avcodec_send_packet(vCodecCtx, packet) == 0)
			{
				while (avcodec_receive_frame(vCodecCtx, frame) == 0)
				{
					int pts = frame->pts * av_q2d(formatCtx->streams[vStreamIdx]->time_base);
					if (pts < seekTarget) continue;

					sws_scale(
						swsCtx,
						frame->data,
						frame->linesize,
						0,
						frame->height,
						rgbFrame->data,
						rgbFrame->linesize);

					{
						std::lock_guard<std::mutex> lock(frameMutex);

						wxImage decoded = wxImage(frame->width, frame->height, false);
						currFrame = decoded.Copy();
						currTime = frame->pts * av_q2d(formatCtx->streams[vStreamIdx]->time_base);

						unsigned char* dst = currFrame.GetData();
						for (int i = 0; i < frame->height; i++)
						{
							memcpy(dst + i * frame->width * 3,
								rgbFrame->data[0] + i * rgbFrame->linesize[0],
								frame->width * 3);
						}
					}

					wxTheApp->CallAfter([this]() {
						paintCallback();
						});

					framePainted = true;
					if (restrictProcessing) break;

					double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
					double delay = (currTime - seekTarget) - elapsed;
					std::this_thread::sleep_for(std::chrono::duration<double>(delay));
				}
			}
			av_packet_unref(packet);
		}
		if (restrictProcessing && framePainted) break;
	}

	if (loop && !restrictProcessing)
	{
		seekTarget = 0;
		SeekFFmpeg();
		goto loopPoint;
	}

	av_frame_free(&frame);
	av_frame_free(&rgbFrame);
	sws_freeContext(swsCtx);

	isProcessing = false;
}

void MediaPlayer::DecodeFrameLibwebp()
{
	isProcessing = true;

	int width = WebPDemuxGetI(webpDemux, WEBP_FF_CANVAS_WIDTH);
	int height = WebPDemuxGetI(webpDemux, WEBP_FF_CANVAS_HEIGHT);
	int frameCount = WebPDemuxGetI(webpDemux, WEBP_FF_FRAME_COUNT);

	WebPIterator iter;
	if (!WebPDemuxGetFrame(webpDemux, 1, &iter))
	{
		printf("No frames found.\n");
	}

	std::vector<uint8_t> canvas(width * height * 4, 0);

	int totalDuration = 0;
	WebPIterator iterTemp;
	if (WebPDemuxGetFrame(webpDemux, 1, &iterTemp))
	{
		do
		{
			totalDuration += iterTemp.duration;
		} while (WebPDemuxNextFrame(&iterTemp));
		WebPDemuxReleaseIterator(&iterTemp);
	}
	int elapsedTime = 0;
	
	for (int k = 0; k < frameCount; k++)
	{
		WebPDecoderConfig config;
		if (!WebPInitDecoderConfig(&config))
		{
			printf("Failed to init decoder config.\n");
			break;
		}
		
		config.output.colorspace = MODE_RGBA;
		config.options.use_threads = 1;

		if (WebPDecode(iter.fragment.bytes, iter.fragment.size, &config) == VP8_STATUS_OK) {
			int w = config.output.width;
			int h = config.output.height;
			uint8_t* rgba = config.output.u.RGBA.rgba;

			// Frame info
			int x_offset = iter.x_offset;
			int y_offset = iter.y_offset;
			int frame_width = config.output.width;
			int frame_height = config.output.height;
			bool blend = iter.blend_method == WEBP_MUX_BLEND;

			// Composite onto canvas
			for (int y = 0; y < frame_height; ++y) {
				for (int x = 0; x < frame_width; ++x) {
					int dst_x = x + x_offset;
					int dst_y = y + y_offset;
					if (dst_x < 0 || dst_x >= width || dst_y < 0 || dst_y >= height)
						continue;

					int src_idx = (y * frame_width + x) * 4;
					int dst_idx = (dst_y * width + dst_x) * 4;

					uint8_t src_a = rgba[src_idx + 3];

					if (blend && src_a < 255) {
						// Alpha blend with canvas
						float alpha = src_a / 255.0f;
						for (int c = 0; c < 3; ++c) {
							canvas[dst_idx + c] = static_cast<uint8_t>(
								canvas[dst_idx + c] * (1 - alpha) + rgba[src_idx + c] * alpha);
						}
						canvas[dst_idx + 3] = std::max(canvas[dst_idx + 3], src_a);
					}
					else {
						// No blend, just copy
						for (int c = 0; c < 4; ++c)
							canvas[dst_idx + c] = rgba[src_idx + c];
					}
				}
			}

			// Allocate RGB + Alpha from canvas
			uint8_t* rgb = new uint8_t[width * height * 3];
			uint8_t* alpha = new uint8_t[width * height];

			for (int i = 0; i < width * height; ++i) {
				rgb[i * 3 + 0] = canvas[i * 4 + 0];
				rgb[i * 3 + 1] = canvas[i * 4 + 1];
				rgb[i * 3 + 2] = canvas[i * 4 + 2];
				alpha[i] = canvas[i * 4 + 3];
			}

			wxImage image(width, height, rgb, true);
			image.SetAlpha(alpha, true);

			currFrame = image.Copy();

			delete[] rgb;
			delete[] alpha;

			wxTheApp->CallAfter([this]() {
				paintCallback();
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(iter.duration));
			elapsedTime += iter.duration;

			WebPFreeDecBuffer(&config.output);
		}

		if (WebPDemuxNextFrame(&iter))
		{
			break;
		}

		if (restrictProcessing) break;
	}

	WebPDemuxReleaseIterator(&iter);

	isProcessing = false;
}

void MediaPlayer::ProcessFFmpeg()
{
}

void MediaPlayer::ProcessLibwebp()
{
}

void MediaPlayer::StopProcessing()
{
	restrictProcessing = true;
	while (isProcessing) continue;
}
