#include "music.h"
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <mpg123.h>
#define WAVE_BUF_AMOUNT 2
#define BGM_DSP_CHANNEL_ID 0
#define MP3_BUF_SIZE 4096

//liquidfenrir still rocks!

static ndspWaveBuf waveBufs[WAVE_BUF_AMOUNT];
static s16* audioBuffers[WAVE_BUF_AMOUNT] = {0};
static size_t bufferSize;
static mpg123_handle* mpg;

static bool valid = false;
static bool ndspInited = false;

static void readSamples(u8* buf, size_t size)
{
    size_t readSize = 0;
    mpg123_read(mpg, buf, size, &readSize);

    // loop audio
    if(readSize < size)
        mpg123_seek(mpg, 0, SEEK_SET);

    DSP_FlushDataCache(buf, size);
}

void startSong(const char* Thing)
{
    romfsInit();
    ndspInited = R_SUCCEEDED(ndspInit());
    if(ndspInited)
    {
        int initerror = MPG123_OK;
        initerror = mpg123_init();
        if(initerror != MPG123_OK)
        {
            ndspExit();
            ndspInited = false;
            return;
        }
    }

    int mpg123error = MPG123_OK;
    mpg = mpg123_new(NULL, &mpg123error);
    if(mpg123error != MPG123_OK)
    {
        return;
    }

    mpg123error = mpg123_open(mpg, Thing);
    if(mpg123error != MPG123_OK)
    {
        mpg123_delete(mpg);
        return;
    }

    long int samplerate;
    int channels;
    int encoding;
    mpg123error = mpg123_getformat(mpg, &samplerate, &channels, &encoding);
    if(mpg123error != MPG123_OK)
    {
        mpg123_close(mpg);
        mpg123_delete(mpg);
        return;
    }
    else if(channels > 2)
    {
        mpg123_close(mpg);
        mpg123_delete(mpg);
        return;
    }

    ndspChnReset(BGM_DSP_CHANNEL_ID);
    ndspChnWaveBufClear(BGM_DSP_CHANNEL_ID);
    ndspChnSetInterp(BGM_DSP_CHANNEL_ID, NDSP_INTERP_POLYPHASE);

    if(channels == 1)
        ndspChnSetFormat(BGM_DSP_CHANNEL_ID, NDSP_FORMAT_MONO_PCM16);
    else if(channels == 2)
        ndspChnSetFormat(BGM_DSP_CHANNEL_ID, NDSP_FORMAT_STEREO_PCM16);

    ndspChnSetRate(BGM_DSP_CHANNEL_ID, samplerate);
    bufferSize = MP3_BUF_SIZE * channels;
    for(int i = 0; i < WAVE_BUF_AMOUNT; i++)
    {
        ndspWaveBuf* waveBuf = &waveBufs[i];
        memset(waveBuf, 0, sizeof(ndspWaveBuf));

        audioBuffers[i] = (s16*)linearAlloc(bufferSize);

        waveBuf->data_vaddr = audioBuffers[i];
        waveBuf->nsamples = (MP3_BUF_SIZE / channels);

        readSamples((u8*)waveBuf->data_pcm16, bufferSize);
        ndspChnWaveBufAdd(BGM_DSP_CHANNEL_ID, waveBuf);
    }

    valid = true;
}

void exitSong(void)
{
    if(valid)
    {
        mpg123_close(mpg);
        mpg123_delete(mpg);

        ndspChnWaveBufClear(BGM_DSP_CHANNEL_ID);

        for(int i = 0; i < WAVE_BUF_AMOUNT; i++)
        {
            if(audioBuffers[i])
                linearFree(audioBuffers[i]);
        }
    }

    if(ndspInited)
    {
        mpg123_exit();
        ndspExit();
    }
    romfsExit();
}

void advance(void)
{
    if(!valid)
        return;

    for(int i = 0; i < WAVE_BUF_AMOUNT; i++)
    {
        ndspWaveBuf* waveBuf = &waveBufs[i];
        if(waveBuf->status == NDSP_WBUF_DONE || waveBuf->status == NDSP_WBUF_FREE)
        {
            readSamples((u8*)waveBuf->data_pcm16, bufferSize);
            ndspChnWaveBufAdd(BGM_DSP_CHANNEL_ID, waveBuf);
        }
    }
}