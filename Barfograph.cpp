/************************
*       Includes        *
************************/

// wxWidgets
#include <wx/wxprec.h>
#include <wx/dir.h>
#include <wx/grid.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

// FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

// SDL (audio-playback)
#include <SDL.h>

// openCV (image-editing)
#include <opencv2/opencv.hpp>

// STD (standard libraries)
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>


class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

class MyFrame : public wxFrame
{
public:
    /************************
    *       Main window     *
    ************************/

    MyFrame(const wxString& title);
    wxDECLARE_EVENT_TABLE();

    // File loading variables
    wxArrayString files;
    wxArrayString dirs;
    std::vector<wxBitmapButton*> fileThumbButtons;
    int threadCount = 0;
    int fileCount = 0;
    int chunksCompleted = 0;
    int loadingProgress = 0;
    std::mutex loadingProgressMutex;
    bool fileSearchRecursive = true;

    // FFMPEG members
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* vCodecCtx = nullptr;
    AVCodecContext* aCodecCtx = nullptr;
    AVStream* vStream = nullptr;
    int vStreamIdx = -1;
    int aStreamIdx = -1;
    double vStreamDuration = 0.0;
    std::mutex loadFileMutex;

    
    // Mediaplayer variables
    wxString* srcPath = nullptr;
    wxImage srcImg;
    int srcImgX = 800;
    int srcImgY = 600;
    float srcAr = 1.3f;
    wxImage currentFrame;
    std::mutex imgMutex;
    struct vFrame {
        wxImage frame;
        double pts;
    };
    std::vector<vFrame> bufferedFrames;
    std::vector<vFrame> processedFrames;

    // wxWidgets windows
    wxScrolledWindow* scrolledWindowFiles;
    wxGridSizer* gridSizerFiles;
    wxPanel* panelMedia;
    wxSlider* mediaSlider;
    wxSlider* volumeSlider;
    wxButton* btnMediaLoop;
    wxButton* btnMediaAutoplay;

    // MediaCtrl variables
    std::thread* threadPlayBack = NULL;
    std::atomic_bool playbackThreadRunning = false;
    double targetSec = 0.0;
    std::atomic_bool isPlaying = false;
    std::atomic_bool isSeeking = false;
    std::atomic_bool loop = false;
    std::atomic_bool autoplay = false;
    float volume = 1.0;

    /************************
    *       Helpers         *
    ************************/

    wxImage ResizeFit(wxImage img)
    {
        int windowSizeX = panelMedia->GetSize().x;
        int windowSizeY = panelMedia->GetSize().y;

        int boxSizeX = windowSizeX;
        int boxSizeY = windowSizeY;

        float boxAr = static_cast<float>(boxSizeX) / static_cast<float>(boxSizeY);

        srcImgX = img.GetSize().GetWidth();
        srcImgY = img.GetSize().GetHeight();
        srcAr = static_cast<float>(srcImgX) / static_cast<float>(srcImgY);

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
        temp.Rescale(srcImgX, srcImgY, wxIMAGE_QUALITY_HIGH);

        return temp;
    }

    wxPoint MediaPosition(wxImage img)
    {
        int windowSizeX = panelMedia->GetSize().x;
        int windowSizeY = panelMedia->GetSize().y;

        int boxSizeX = windowSizeX;
        int boxSizeY = windowSizeY;

        float boxAr = static_cast<float>(boxSizeX) / static_cast<float>(boxSizeY);

        srcImgX = img.GetSize().GetWidth();
        srcImgY = img.GetSize().GetHeight();
        srcAr = static_cast<float>(srcImgX) / static_cast<float>(srcImgY);

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

    bool IsMediaFileType(wxString fileName)
    {
        wxString ext = fileName.AfterLast('.').Lower();

        if (ext == "jpg" || ext == "jpeg" ||
            ext == "png" || ext == "bmp" ||
            ext == "gif" || ext == "webp" ||
            ext == "mp4" || ext == "mkv" ||
            ext == "mov" || ext == "avi" ||
            ext == "m4v" || ext == "webm")
        {
            return true;
        }

        return false;
    }
    
    void FileChunksLoaded()
    {
        std::lock_guard<std::mutex> lock(loadingProgressMutex);
        chunksCompleted += 1;
        if (chunksCompleted == threadCount)
        {
            wxTheApp->CallAfter([this]() {
                wxBusyCursor busyCursor;
                this->Disable();
                scrolledWindowFiles->SetSizerAndFit(gridSizerFiles);
                this->Layout();
                this->Enable();
                });
        }
    }
    
    void FileLoadingProgress(int i)
    {
        std::lock_guard<std::mutex> lock(loadingProgressMutex);
        loadingProgress += i;

        wxTheApp->CallAfter([this]() {
            SetStatusText("File: " + std::to_string(loadingProgress) + "/" + std::to_string(fileCount));
            });
    }

    void OnResize(wxSizeEvent& event)
    {
        if (!isPlaying) {
            panelMedia->Refresh();
        }
        event.Skip();
    }

    void PanelMediaPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(panelMedia);
        dc.Clear();
        wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

        wxImage safeImg;
        {
            std::lock_guard<std::mutex> lock(imgMutex);
            if (srcImg.IsOk() && srcImg.GetData() != nullptr) {
                safeImg = srcImg.Copy();
            }
        }

        if (safeImg.IsOk() && srcImg.GetData() != nullptr)
        {
            currentFrame = ResizeFit(safeImg);
            wxPoint pos = MediaPosition(currentFrame);
            dc.DrawBitmap(wxBitmap(currentFrame), pos.x, pos.y, false);
        }
        delete gc;
        event.Skip();
    }

    /**********************************
    *       Threaded functions        *
    **********************************/

    void ThreadLoadFiles(int chunkStart, int chunkEnd)
    {
        for (int j = chunkStart; j < chunkEnd; j++)
        {
            if (files[j].empty()) continue;

            wxString fname = files[j];
            wxString ext = fname.AfterLast('.').Lower();
            wxCharBuffer utf8Path = fname.ToUTF8();
            const char* fullPath = utf8Path.data();
            wxImage imgThumb;

            if (IsMediaFileType(ext))
            {
                // Open format context
                AVFormatContext* tempFormatCtx = nullptr;
                if (avformat_open_input(&tempFormatCtx, fullPath, nullptr, nullptr) < 0)
                {
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                    continue;
                }
                if (avformat_find_stream_info(tempFormatCtx, nullptr) < 0)
                {
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                    continue;
                }

                // Find video stream
                int tempVStreamIdx = -1;
                for (unsigned int i = 0; i < tempFormatCtx->nb_streams; i++)
                {
                    if (tempFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                    {
                        tempVStreamIdx = i;
                        break;
                    }
                }
                if (tempVStreamIdx == -1)
                {
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                    continue;
                }

                AVCodecParameters* codecpar = tempFormatCtx->streams[tempVStreamIdx]->codecpar;
                const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
                if (!codec)
                {
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                    continue;
                }

                AVCodecContext* tempCodecCtx = nullptr;
                tempCodecCtx = avcodec_alloc_context3(codec);
                if (!tempCodecCtx)
                {
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                    continue;
                }

                avcodec_parameters_to_context(tempCodecCtx, codecpar);
                if (avcodec_open2(tempCodecCtx, codec, nullptr) < 0)
                {
                    if (tempCodecCtx) avcodec_free_context(&tempCodecCtx);
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                    continue;
                }

                AVPacket* packet = av_packet_alloc();
                AVFrame* frame = av_frame_alloc();
                AVFrame* rgbFrame = av_frame_alloc();

                int width = tempCodecCtx->width;
                int height = tempCodecCtx->height;

                rgbFrame->format = AV_PIX_FMT_RGB24;
                rgbFrame->width = width;
                rgbFrame->height = height;

                if (av_frame_get_buffer(rgbFrame, 0) < 0) {
                    av_frame_free(&rgbFrame);
                    if (tempCodecCtx) avcodec_free_context(&tempCodecCtx);
                    if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                }

                struct SwsContext* swsCtx = sws_getContext(
                    width, height, tempCodecCtx->pix_fmt,
                    width, height, AV_PIX_FMT_RGB24,
                    SWS_BICUBIC, nullptr, nullptr, nullptr);

                int tempCurrentFrame = 0;
                bool gotFrame = false;
                int packetCount = 0;
                const int maxPackets = 500;

                while (av_read_frame(tempFormatCtx, packet) == 0)
                {
                    if (++packetCount > maxPackets) break;

                    if (packet->stream_index == tempVStreamIdx)
                    {
                        if (avcodec_send_packet(tempCodecCtx, packet) == 0)
                        {
                            while (avcodec_receive_frame(tempCodecCtx, frame) == 0)
                            {
                                if (tempCurrentFrame == 0)
                                {
                                    sws_scale(
                                        swsCtx,
                                        frame->data,
                                        frame->linesize,
                                        0,
                                        height,
                                        rgbFrame->data,
                                        rgbFrame->linesize);
                                    imgThumb = wxImage(width, height, true);

                                    unsigned char* dst = imgThumb.GetData();
                                    unsigned char* src = rgbFrame->data[0];
                                    int srcStride = rgbFrame->linesize[0];

                                    for (int y = 0; y < height; y++) {
                                        memcpy(dst + y * width * 3, src + y * srcStride, width * 3);
                                    }

                                    gotFrame = true;
                                    break;
                                }
                                tempCurrentFrame++;
                            }
                        }
                    }
                    av_packet_unref(packet);
                    if (gotFrame) break;
                }

                if (rgbFrame) av_frame_free(&rgbFrame);
                if (frame) av_frame_free(&frame);
                if (tempFormatCtx) avformat_close_input(&tempFormatCtx);
                tempFormatCtx = nullptr;
                if (tempCodecCtx) avcodec_free_context(&tempCodecCtx);
                tempCodecCtx = nullptr;
                if (swsCtx) sws_freeContext(swsCtx);
            }
            else
            {
                continue;
            }

            if (imgThumb.IsOk())
            {
                imgThumb.Rescale(72, 72, wxIMAGE_QUALITY_HIGH);
                FileLoadingProgress(1);
            }
            wxTheApp->CallAfter([this, imgThumb, fname]() {
                wxBitmapButton* thumbBtn = new wxBitmapButton(scrolledWindowFiles, wxID_ANY, wxBitmap(imgThumb), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
                thumbBtn->Bind(wxEVT_LEFT_DOWN, &MyFrame::BtnFileThumb, this);
                thumbBtn->SetClientData(new wxString(fname));
                thumbBtn->SetBackgroundColour(wxColor(0, 0, 0));
                this->gridSizerFiles->Add(thumbBtn, 0, wxTOP, 4);
                });
        }
        FileChunksLoaded();
    }

    void BufferFrames()
    {

    }

    void PlayBack()
    {
        playbackThreadRunning.store(true);

        // Declare and initialize packets and video frames.
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
            if (rgbFrame) av_frame_free(&rgbFrame);
            if (vCodecCtx) avcodec_free_context(&vCodecCtx);
            if (formatCtx) avformat_close_input(&formatCtx);
        }

        // Declare and initialize a software scaling context.
        struct SwsContext* swsCtx = sws_getContext(
            width, height, vCodecCtx->pix_fmt,
            width, height, AV_PIX_FMT_RGB24,
            SWS_BICUBIC, nullptr, nullptr, nullptr);

        // Initialize the SDL audio subsystem.
        SDL_Init(SDL_INIT_AUDIO);

        //Declare a software resampling context.
        struct SwrContext* swrCtx = nullptr;

        AVChannelLayout outChLayout = {};
        av_channel_layout_default(&outChLayout, 2);

        AVFrame* aFrame = nullptr;

        SDL_AudioSpec want, have;
        SDL_AudioDeviceID dev = NULL;

        // If there's an audiostream, allocate
        // the software resampling context.
        if (aStreamIdx != -1)
        {
            int ret = swr_alloc_set_opts2(
                &swrCtx,
                &outChLayout,
                AV_SAMPLE_FMT_S16,
                44100,
                &aCodecCtx->ch_layout,
                aCodecCtx->sample_fmt,
                aCodecCtx->sample_rate,
                0, nullptr);

            if (ret == 0) {
                swr_init(swrCtx);

                aFrame = av_frame_alloc();

                SDL_zero(want);
                want.freq = 44100;
                want.format = AUDIO_S16SYS;
                want.channels = 2;
                want.samples = 4096;
                want.callback = NULL;

                dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
                if (dev == 0)
                {
                    std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
                }
                else
                {
                    SDL_PauseAudioDevice(dev, 0);
                }
            }
        }
        
        /******************************************************
        * Retrieve packets, display if frames, play if audio. *
        ******************************************************/
        for (int i = 0; i < 1; i++) {
            auto startTime = std::chrono::steady_clock::now();

            int64_t seekTarget = targetSec / av_q2d(formatCtx->streams[vStreamIdx]->time_base);

            av_seek_frame(formatCtx, vStreamIdx, seekTarget, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(vCodecCtx);

            double vPts = 0.0;
            double audioBasePts = -1.0;
            std::chrono::steady_clock::time_point audioWallClockStart;

            while (av_read_frame(formatCtx, packet) == 0)
            {
                if (packet->stream_index == vStreamIdx)
                {
                    if (avcodec_send_packet(vCodecCtx, packet) == 0)
                    {
                        while (avcodec_receive_frame(vCodecCtx, frame) == 0)
                        {
                            sws_scale(
                                swsCtx,
                                frame->data,
                                frame->linesize,
                                0,
                                frame->height,
                                rgbFrame->data,
                                rgbFrame->linesize);

                            wxImage img = wxImage(frame->width, frame->height, true);

                            unsigned char* dst = img.GetData();
                            for (int y = 0; y < frame->height; ++y) {
                                memcpy(dst + y * frame->width * 3,
                                    rgbFrame->data[0] + y * rgbFrame->linesize[0],
                                    frame->width * 3);
                            }

                            {
                                std::lock_guard<std::mutex> lock(imgMutex);
                                srcImg = img;
                            }

                            vPts = frame->pts * av_q2d(vStream->time_base);

                            if (aStreamIdx != -1) {
                                double audioClock = 0.0;

                                if (audioBasePts >= 0) {
                                    auto now = std::chrono::steady_clock::now();
                                    double wallClock = std::chrono::duration<double>(now - audioWallClockStart).count();
                                    audioClock = audioBasePts + wallClock;
                                }
                                else {
                                    audioClock = vPts;  // fallback if no audio yet
                                }

                                double diff = vPts - audioClock;

                                if (diff > 0)
                                {
                                    std::this_thread::sleep_for(std::chrono::duration<double>(diff));
                                }
                                else if (diff < -0.1)
                                {
                                    continue; // drop late frame
                                }
                            }
                            else
                            {
                                if (vPts <= targetSec) continue;
                                double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
                                double delay = (vPts - targetSec) - elapsed;
                                if (delay > 0) std::this_thread::sleep_for(std::chrono::duration<double>(delay));
                            }

                            int sliderValue = (vPts / vStreamDuration) * 1000;

                            if (!isPlaying) break;

                            wxTheApp->CallAfter([this, sliderValue]() {
                                mediaSlider->SetValue(sliderValue);
                                panelMedia->Refresh();
                                });
                        }
                    }
                }
                else if (packet->stream_index == aStreamIdx)
                {
                    if (avcodec_send_packet(aCodecCtx, packet) == 0)
                    {
                        while (avcodec_receive_frame(aCodecCtx, aFrame) == 0)
                        {
                            uint8_t** outData;
                            int outLinesize;
                            av_samples_alloc_array_and_samples(
                                &outData, &outLinesize,
                                2, aFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);

                            int nbOutSamples = swr_convert(
                                swrCtx,
                                outData,
                                aFrame->nb_samples,
                                (const uint8_t**)aFrame->data, aFrame->nb_samples);

                            int outBufferSize = av_samples_get_buffer_size(
                                nullptr, 2, nbOutSamples, AV_SAMPLE_FMT_S16, 1);

                            double framePts = aFrame->pts * av_q2d(formatCtx->streams[aStreamIdx]->time_base);

                            if (audioBasePts < 0) {
                                audioBasePts = framePts;
                                audioWallClockStart = std::chrono::steady_clock::now();
                                SDL_PauseAudioDevice(dev, 0);
                            }
                            
                            int16_t* samples = reinterpret_cast<int16_t*>(outData[0]);
                            int numSamples = nbOutSamples * 2;

                            for (int j = 0; j < numSamples; j++)
                            {
                                samples[j] = static_cast<int16_t>(samples[j] * volume);
                            }

                            SDL_QueueAudio(dev, outData[0], outBufferSize);

                            av_freep(&outData[0]);
                            av_freep(&outData);

                            if (!isPlaying) break;
                        }
                    }
                }

                av_packet_unref(packet);
                if (!isPlaying) break;
            }

            if (aStreamIdx != -1) SDL_PauseAudioDevice(dev, 0);

            targetSec = vPts;
            if (!isPlaying) break;
            if (loop) {
                i -= 1;
                targetSec = 0.0;
            }
        }

        if (&frame) av_frame_free(&frame);
        if (&rgbFrame) av_frame_free(&rgbFrame);
        if (swsCtx) sws_freeContext(swsCtx);

        isPlaying = false;
        playbackThreadRunning.store(false);
    }

    /***************************
    *       Button events      *
    ***************************/

    void BtnLoadFiles(wxCommandEvent& event)
    {
        isPlaying = false;

        while (playbackThreadRunning.load()) {
            continue;
        }
        
        this->Disable();

        wxString folderPath = wxDirSelector("Select folder");
        wxDir dir(folderPath);

        files.resize(0);
        dirs.resize(0);

        if (!fileSearchRecursive)
            wxDir::GetAllFiles(folderPath, &files, wxEmptyString, wxDIR_FILES);
        else {
            // Recursive search (needs ALL folders, not just one level further)
            if (dir.IsOpened())
            {
                wxString filename;
                bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
                while (cont)
                {
                    wxString path = folderPath + "/" + filename;
                    if (wxDirExists(path))
                    {
                        dirs.Add(path);

                        wxDir subdir(path);
                        if (subdir.IsOpened())
                        {
                            wxString subfilename;
                            bool subcont = subdir.GetFirst(&subfilename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
                            while (subcont)
                            {
                                wxString subpath = path + "/" + subfilename;
                                if (wxDirExists(subpath))
                                {
                                    dirs.Add(subpath);
                                    // You could recurse deeper here as needed
                                }
                                else
                                {
                                    files.Add(subpath);
                                }
                                subcont = subdir.GetNext(&subfilename);
                            }
                        }
                    }
                    else
                    {
                        files.Add(path);
                    }
                    cont = dir.GetNext(&filename);
                }
            }
        }

        fileCount = files.GetCount();

        if (fileCount == 0)
        {
            this->Enable();
            return;
        }

        gridSizerFiles->Clear(true);
        chunksCompleted = 0;
        loadingProgress = 0;

        // Multi-threaded file loading
        threadCount = 10;
        int chunkSize = fileCount / threadCount;
        int remainder = fileCount % threadCount;
        int chunkStart = 0;
        std::vector<std::thread> threads;

        for (int i = 0; i < threadCount; i++)
        {
            int chunkEnd = chunkStart + chunkSize;
            if (i == threadCount - 1) {
                chunkEnd += remainder;
            }
            threads.emplace_back(&MyFrame::ThreadLoadFiles, this, chunkStart, chunkEnd);
            chunkStart = chunkEnd;
        }

        // Detach threads
        for (int i = 0; i < threadCount; i++)
        {
            threads[i].detach();
        }
        event.Skip();
    }

    void BtnMediaPrevious(wxCommandEvent& event)
    {
        event.Skip();
    }

    void BtnMediaBack(wxCommandEvent& event)
    {
        targetSec -= 5.0;
        // Seek function here
        event.Skip();
    }

    void BtnMediaStop(wxCommandEvent& event)
    {
        isPlaying = false;
        event.Skip();
    }

    void BtnMediaPlay(wxCommandEvent& event)
    {
        isPlaying = false;

        if (isPlaying) return;

        if (isSeeking) return;

        if (playbackThreadRunning.load()) return;

        isPlaying = true;

        threadPlayBack = new std::thread(&MyFrame::PlayBack, this);
        threadPlayBack->detach();
        event.Skip();
    }

    void BtnMediaForward(wxCommandEvent& event)
    {
        targetSec += 5.0;
        // Seek function here
        event.Skip();
    }

    void BtnMediaNext(wxCommandEvent& event)
    {
        event.Skip();
    }

    void BtnMediaLoop(wxCommandEvent& event)
    {
        if (loop)
        {
            loop = false;
            btnMediaLoop->SetBackgroundColour(wxColor(240, 240, 240));
        }
        else
        {
            loop = true;
            btnMediaLoop->SetBackgroundColour(wxColor(128, 240, 128));
        }
        event.Skip();
    }

    void BtnMediaAutoplay(wxCommandEvent& event)
    {
        if (autoplay)
        {
            autoplay = false;
            btnMediaAutoplay->SetBackgroundColour(wxColor(240, 240, 240));
        }
        else
        {
            autoplay = true;
            btnMediaAutoplay->SetBackgroundColour(wxColor(128, 240, 128));
        }
        event.Skip();
    }

    void BtnFileThumb(wxMouseEvent& event)
    {
        isPlaying = false;

        while (playbackThreadRunning.load()) {
            continue;
        }

        if (isSeeking) {
            return;
        }

        // Free previous file
        avcodec_free_context(&vCodecCtx);
        if (aStreamIdx != -1) avcodec_free_context(&aCodecCtx);
        avformat_close_input(&formatCtx);

        // Retrieve path to file
        wxBitmapButton* thumb = dynamic_cast<wxBitmapButton*>(event.GetEventObject());
        wxString* tempSrcPath = static_cast<wxString*>(thumb->GetClientData());

        targetSec = 0;
        mediaSlider->SetValue(0);

        /*******************
        * Try to open file *
        *******************/
        if (avformat_open_input(&formatCtx, tempSrcPath->ToUTF8().data(), nullptr, nullptr) < 0)
        {
            avformat_close_input(&formatCtx);
            return;
        }

        if (avformat_find_stream_info(formatCtx, nullptr) < 0)
        {
            avformat_close_input(&formatCtx);
            return;
        }

        /******************************
        * Find the video stream index *
        *******************************/
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

        /******************************
        * Find the audio stream index *
        *******************************/
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
            std::cout << "No audio stream found." << std::endl;
        }
        /*********************************
        * Find the video codec & decoder *
        **********************************/
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

        /*********************************
        * Find the audio codec & decoder *
        *********************************/
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

        // Containers for raw data & decoded data
        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();
        AVFrame* rgbFrame = av_frame_alloc();

        // Store width and height of stream
        int width = vCodecCtx->width;
        int height = vCodecCtx->height;

        rgbFrame->format = AV_PIX_FMT_RGB24;
        rgbFrame->width = width;
        rgbFrame->height = height;

        if (av_frame_get_buffer(rgbFrame, 0) < 0) {
            av_frame_free(&rgbFrame);
            avcodec_free_context(&vCodecCtx);
            if (aStreamIdx != -1) avcodec_free_context(&aCodecCtx);
            avformat_close_input(&formatCtx);
        }

        /*****************
        * Resample video *
        ******************/
        struct SwsContext* swsCtx = sws_getContext(
            width, height, vCodecCtx->pix_fmt,
            width, height, AV_PIX_FMT_RGB24,
            SWS_BICUBIC, nullptr, nullptr, nullptr);

        int currFrame = 0;

        vStream = formatCtx->streams[vStreamIdx];

        // Find stream framerate
        AVRational framerate = av_guess_frame_rate(formatCtx, vStream, nullptr);
        double fps = av_q2d(framerate);
        if (fps <= 0) fps = 30.0; // Framerate fallback

        // Find stream duration
        double duration = 0;
        if (formatCtx->duration != AV_NOPTS_VALUE)
        {
            duration = formatCtx->duration / (double)AV_TIME_BASE;
            vStreamDuration = duration;
        }
        else if (vStream->duration != AV_NOPTS_VALUE)
        {
            duration = vStream->duration * av_q2d(vStream->time_base);
            vStreamDuration = duration;
        }

        // Find first frame and put it on screen
        while (av_read_frame(formatCtx, packet) >= 0)
        {
            if (packet->stream_index == vStreamIdx)
            {
                if (avcodec_send_packet(vCodecCtx, packet) == 0)
                {
                    while (avcodec_receive_frame(vCodecCtx, frame) == 0)
                    {
                        if (currFrame == 0)
                        {
                            sws_scale(
                                swsCtx,
                                frame->data,
                                frame->linesize,
                                0,
                                height,
                                rgbFrame->data,
                                rgbFrame->linesize);
                            wxImage img = wxImage(width, height, true);

                            //memcpy(img.GetData(), rgbFrame->data[0], width * height * 3);

                            unsigned char* dst = img.GetData();
                            for (int y = 0; y < frame->height; ++y) {
                                memcpy(dst + y * frame->width * 3,
                                    rgbFrame->data[0] + y * rgbFrame->linesize[0],
                                    frame->width * 3);
                            }

                            srcImgX = width;
                            srcImgY = height;
                            srcImg = img;
                            img = ResizeFit(img);
                            currentFrame = img;
                            panelMedia->Refresh();
                            goto done;
                        }
                        currFrame++;
                    }
                }
            }
            av_packet_unref(packet);
        }

    done:
        av_packet_unref(packet);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        av_packet_free(&packet);
        sws_freeContext(swsCtx);

        wxTheApp->CallAfter([this, tempSrcPath]() {
            SetStatusText(tempSrcPath->ToUTF8().data());
            if (autoplay) {
                wxCommandEvent dummyEvent;
                BtnMediaPlay(dummyEvent);
            }
            });
        event.Skip();
    }

    // Menubar events
    void OnQuit(wxCommandEvent& event)
    {
        event.Skip();
    }

    void OnAbout(wxCommandEvent& event)
    {
        event.Skip();
    }

    /****************************
    *       Slider events       *
    ****************************/

    void MediaSlider(wxCommandEvent& event)
    {
        if (!formatCtx) return;

        isPlaying = false;

        if (playbackThreadRunning.load()) {
            return;
        }

        if (isSeeking) {
            return;
        }

        isSeeking = true;

        std::thread([this]() {
            int sliderVal = mediaSlider->GetValue();
            targetSec = (static_cast<double>(sliderVal) / 1000) * vStreamDuration;

            int64_t targetTs = static_cast<int64_t>(targetSec / av_q2d(vStream->time_base));

            if (av_seek_frame(formatCtx, vStreamIdx, targetTs, AVSEEK_FLAG_BACKWARD) < 0) return;

            avcodec_flush_buffers(vCodecCtx);

            AVPacket* packet = av_packet_alloc();
            AVFrame* frame = av_frame_alloc();
            AVFrame* rgbFrame = av_frame_alloc();

            int width = vCodecCtx->width;
            int height = vCodecCtx->height;

            //int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
            //uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
            //av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, width, height, 1);

            rgbFrame->format = AV_PIX_FMT_RGB24;
            rgbFrame->width = width;
            rgbFrame->height = height;

            if (av_frame_get_buffer(rgbFrame, 0) < 0) {
                av_frame_free(&rgbFrame);
                if (vCodecCtx) avcodec_free_context(&vCodecCtx);
                if (formatCtx) avformat_close_input(&formatCtx);
            }

            struct SwsContext* swsCtx = sws_getContext(
                width, height, vCodecCtx->pix_fmt,
                width, height, AV_PIX_FMT_RGB24,
                SWS_BICUBIC, nullptr, nullptr, nullptr);

            bool frameDecoded = false;
            while (!frameDecoded && av_read_frame(formatCtx, packet) >= 0)
            {
                if (packet->stream_index == vStreamIdx)
                {
                    int ret = avcodec_send_packet(vCodecCtx, packet);
                    if (ret >= 0)
                    {
                        while (avcodec_receive_frame(vCodecCtx, frame) == 0)
                        {
                            if (frame->pts >= targetTs)
                            {
                                sws_scale(
                                    swsCtx,
                                    frame->data,
                                    frame->linesize,
                                    0,
                                    height,
                                    rgbFrame->data,
                                    rgbFrame->linesize);
                                wxImage img = wxImage(width, height, true);

                                unsigned char* dst = img.GetData();
                                for (int y = 0; y < frame->height; ++y) {
                                    memcpy(dst + y * frame->width * 3,
                                        rgbFrame->data[0] + y * rgbFrame->linesize[0],
                                        frame->width * 3);
                                }

                                //memcpy(img.GetData(), rgbFrame->data[0], width * height * 3);

                                std::lock_guard<std::mutex> lock(imgMutex);
                                srcImg = img;

                                wxTheApp->CallAfter([this]() {
                                    panelMedia->Refresh();
                                    });
                                frameDecoded = true;
                                break;
                            }
                        }
                    }
                }
                av_packet_unref(packet);
            }

            if (frame) av_frame_free(&frame);
            if (rgbFrame) av_frame_free(&rgbFrame);
            //if (buffer) av_free(buffer);
            if (swsCtx) sws_freeContext(swsCtx);
            isSeeking = false;
            }).detach();
        event.Skip();
    }

    void VolumeSlider(wxCommandEvent& event)
    {
        volume = static_cast<float>(volumeSlider->GetValue()) / 100.0;
        event.Skip();
    }

    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
    /*                                      */
    /*              MODULES                 */
    /*                                      */
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

    /***************************
    *       Face swapper       *
    ***************************/
    
    void AddFaceSwapperModule()
    {

    }

    void FindSourceFaces()
    {

    }

    void FindTargetFaces()
    {

    }

    void CoupleFaces()
    {

    }

    void Swap()
    {

    }
    
    /*******************************
    *       Frame interpolator     *
    *******************************/
    
    void AddInterpolateModule()
    {

    }

    wxImage Interpolate(wxImage frame1, wxImage frame2)
    {

    }
    
    /***********************
    *       Upscaler        *
    ***********************/
    
    void AddUpscalerModule()
    {

    }

    wxImage Upscale(wxImage frame)
    {

    }
    
    /************************
    *       Stylizer        *
    ************************/

    void AddStylizerModule()
    {

    }

    void Stylizer()
    {

    }

};

enum
{
    Menubar_Quit = wxID_EXIT,
    Menubar_About = wxID_ABOUT
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Menubar_Quit, MyFrame::OnQuit)
    EVT_MENU(Menubar_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
    //freopen("CONOUT$", "w", stderr);

    if (!wxApp::OnInit()) return false;

    MyFrame* frame = new MyFrame("Barfograph");
    frame->SetInitialSize(wxSize(1100, 670));
    frame->Show(true);

    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(300, 150))
{
    wxInitAllImageHandlers();

    // Menubar
    wxMenu* fileMenu = new wxMenu;
    wxMenu* helpMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "&Exit\tAlt-X", "Quit this program");
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
    
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // Boxsizers
    wxBoxSizer* sizerFrame = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizerMaster = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizerLeft = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizerMiddle = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* sizerFiles = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizerMedia = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizerMediaCtrls = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizerMediaSlider = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizerMediaButtons = new wxBoxSizer(wxHORIZONTAL);

    // Gridsizers
    gridSizerFiles = new wxGridSizer(2, 0, 0);

    // Panels
    wxPanel* panelMaster = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED);
    wxPanel* panelLeft = new wxPanel(panelMaster, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxBORDER_SUNKEN);
    wxPanel* panelMiddle = new wxPanel(panelMaster, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxBORDER_SUNKEN);
    wxPanel* panelRight = new wxPanel(panelMaster, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxBORDER_SUNKEN);

    wxPanel* panelFiles = new wxPanel(panelLeft, wxID_ANY, wxDefaultPosition, wxSize(179, 200), wxBORDER_RAISED);
    panelMedia = new wxPanel(panelMiddle, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    wxPanel* panelMediaCtrls = new wxPanel(panelMiddle, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxBORDER_NONE);
    wxPanel* panelMediaSlider = new wxPanel(panelMediaCtrls, wxID_ANY, wxDefaultPosition, wxSize(32, 32), wxBORDER_RAISED);
    wxPanel* panelMediaButtons = new wxPanel(panelMediaCtrls, wxID_ANY, wxDefaultPosition, wxSize(32, 52), wxBORDER_RAISED);
    
    wxColor grey = wxColor(92, 92, 92);
    wxColor greyDark = wxColor(32, 32, 32);
    wxColor black = wxColor(0, 0, 0);
    wxColor* panelColor1 = &grey;
    wxColor* panelColor2 = &greyDark;

    panelMaster->SetBackgroundColour(*panelColor2);
    panelLeft->SetBackgroundColour(*panelColor2);
    panelMiddle->SetBackgroundColour(*panelColor2);
    panelRight->SetBackgroundColour(*panelColor2);

    panelFiles->SetBackgroundColour(*panelColor1);
    panelMedia->SetBackgroundColour(black);
    panelMedia->SetBackgroundStyle(wxBG_STYLE_PAINT);
    panelMediaCtrls->SetBackgroundColour(*panelColor2);
    panelMediaSlider->SetBackgroundColour(*panelColor1);
    panelMediaButtons->SetBackgroundColour(*panelColor1);

    // Scrolled windows
    scrolledWindowFiles = new wxScrolledWindow(panelFiles, wxID_ANY, wxDefaultPosition, wxSize(179, 200), wxBORDER_NONE);
    
    scrolledWindowFiles->SetScrollRate(10, 10);
    scrolledWindowFiles->SetBackgroundColour(*panelColor1);

    // Buttons
    wxButton* btnLoadFiles = new wxButton(panelLeft, wxID_ANY, "Load files", wxDefaultPosition, wxSize(128, 32));

    wxButton* btnMediaPrevious = new wxButton(panelMediaButtons, wxID_ANY, "Previous", wxDefaultPosition, wxSize(64, 48));
    wxButton* btnMediaBack = new wxButton(panelMediaButtons, wxID_ANY, "Back", wxDefaultPosition, wxSize(64, 48));
    wxButton* btnMediaStop = new wxButton(panelMediaButtons, wxID_ANY, "Stop", wxDefaultPosition, wxSize(64, 48));
    wxButton* btnMediaPlay = new wxButton(panelMediaButtons, wxID_ANY, "Play", wxDefaultPosition, wxSize(64, 48));
    wxButton* btnMediaForward = new wxButton(panelMediaButtons, wxID_ANY, "Forward", wxDefaultPosition, wxSize(64, 48));
    wxButton* btnMediaNext = new wxButton(panelMediaButtons, wxID_ANY, "Next", wxDefaultPosition, wxSize(64, 48));
    btnMediaLoop = new wxButton(panelMediaButtons, wxID_ANY, "Loop", wxDefaultPosition, wxSize(64, 48));
    btnMediaAutoplay = new wxButton(panelMediaButtons, wxID_ANY, "Autoplay", wxDefaultPosition, wxSize(64, 48));

    btnLoadFiles->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaPrevious->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaBack->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaStop->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaPlay->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaForward->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaNext->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaLoop->SetBackgroundColour(wxColor(240, 240, 240));
    btnMediaAutoplay->SetBackgroundColour(wxColor(240, 240, 240));

    // Sliders
    mediaSlider = new wxSlider(panelMediaSlider, wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(128, 32));
    volumeSlider = new wxSlider(panelMediaButtons, wxID_ANY, 100, 0, 100, wxDefaultPosition, wxSize(96, 32));

    // Set top/frame sizer
    this->SetSizer(sizerFrame);
    sizerFrame->Add(panelMaster, 1, wxEXPAND);

    // Set master panel sizer
    panelMaster->SetSizer(sizerMaster);
    sizerMaster->Add(panelLeft, 0, wxEXPAND);
    sizerMaster->Add(panelMiddle, 1, wxEXPAND);
    sizerMaster->Add(panelRight, 0, wxEXPAND);

    // Set left panel sizers
    panelLeft->SetSizer(sizerLeft);
    sizerLeft->Add(btnLoadFiles, 0, wxALIGN_CENTER);
    sizerLeft->Add(panelFiles, 1, wxALIGN_CENTER | wxBOTTOM, 9);

    panelFiles->SetSizer(sizerFiles);
    sizerFiles->Add(scrolledWindowFiles, 0, wxEXPAND | wxLEFT, 4);
    
    scrolledWindowFiles->SetSizerAndFit(gridSizerFiles);

    // Set middle panel sizers
    panelMiddle->SetSizer(sizerMiddle);
    sizerMiddle->Add(panelMedia, 1, wxEXPAND);
    sizerMiddle->Add(panelMediaCtrls, 0, wxEXPAND);

    srcPath = new wxString("noise.png");
    srcImg.LoadFile(*srcPath);

    panelMedia->SetSizer(sizerMedia);

    panelMediaCtrls->SetSizer(sizerMediaCtrls);
    sizerMediaCtrls->Add(panelMediaSlider, 0, wxEXPAND);
    sizerMediaCtrls->Add(panelMediaButtons, 0, wxEXPAND);

    panelMediaSlider->SetSizer(sizerMediaSlider);
    sizerMediaSlider->Add(mediaSlider, 1);

    panelMediaButtons->SetSizer(sizerMediaButtons);
    sizerMediaButtons->AddStretchSpacer();
    sizerMediaButtons->Add(volumeSlider, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    sizerMediaButtons->Add(btnMediaPrevious, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaBack, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaStop, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaPlay, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaForward, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaNext, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaLoop, 0, wxALIGN_CENTER);
    sizerMediaButtons->Add(btnMediaAutoplay, 0, wxALIGN_CENTER);
    sizerMediaButtons->AddStretchSpacer();

    // Bindings
    Bind(wxEVT_SIZE, &MyFrame::OnResize, this);
    panelMedia->Bind(wxEVT_PAINT, &MyFrame::PanelMediaPaint, this);
    //mediaSlider->Bind(wxEVT_SLIDER, &MyFrame::MediaSlider, this);
    mediaSlider->Bind(wxEVT_SCROLL_THUMBTRACK, &MyFrame::MediaSlider, this);
    mediaSlider->Bind(wxEVT_SCROLL_PAGEUP, &MyFrame::MediaSlider, this);
    mediaSlider->Bind(wxEVT_SCROLL_PAGEDOWN, &MyFrame::MediaSlider, this);
    volumeSlider->Bind(wxEVT_SCROLL_THUMBTRACK, &MyFrame::VolumeSlider, this);
    volumeSlider->Bind(wxEVT_SCROLL_PAGEUP, &MyFrame::VolumeSlider, this);
    volumeSlider->Bind(wxEVT_SCROLL_PAGEDOWN, &MyFrame::VolumeSlider, this);
    btnLoadFiles->Bind(wxEVT_BUTTON, &MyFrame::BtnLoadFiles, this);
    btnMediaPrevious->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaPrevious, this);
    btnMediaBack->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaBack, this);
    btnMediaStop->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaStop, this);
    btnMediaPlay->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaPlay, this);
    btnMediaForward->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaForward, this);
    btnMediaNext->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaNext, this);
    btnMediaLoop->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaLoop, this);
    btnMediaAutoplay->Bind(wxEVT_BUTTON, &MyFrame::BtnMediaAutoplay, this);

    // Extras
    SetMenuBar(menuBar);
    CreateStatusBar(1);
    
    SetStatusText("Hello!");
}
