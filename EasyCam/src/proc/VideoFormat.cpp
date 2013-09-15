#include "VideoFormat.h"

using namespace ezc;

static void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        // pUnk should not be used.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

static void _DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
    if (pmt != NULL)
    {
        _FreeMediaType(*pmt); 
        CoTaskMemFree(pmt);
    }
}

VideoFormat::VideoFormat(AM_MEDIA_TYPE *mediaType)
	: mediaType(mediaType)
{
	videoHeader = (VIDEOINFOHEADER*)mediaType->pbFormat;
}

VideoFormat::VideoFormat(const VideoFormat &videoFmt)
{
	if(this != &videoFmt)
	{
		this->mediaType = videoFmt.mediaType;
		this->videoHeader = videoFmt.videoHeader;
	}
}

VideoFormat::~VideoFormat(void)
{
	_DeleteMediaType(mediaType);
}

int VideoFormat::getWidth() const
{
	return videoHeader->bmiHeader.biWidth;
}

int VideoFormat::getHeight() const
{
	return videoHeader->bmiHeader.biHeight;
}

int VideoFormat::getType() const
{
	const GUID typeGuid = mediaType->subtype;
	if(typeGuid == MEDIASUBTYPE_YUY2) return VideoFormat::YUY2;
	if(typeGuid == MEDIASUBTYPE_RGB32) return VideoFormat::RGB32;
	if(typeGuid == MEDIASUBTYPE_RGB24) return VideoFormat::RGB24;
	if(typeGuid == MEDIASUBTYPE_RGB8) return VideoFormat::RGB8;
	return VideoFormat::UNKNOWN;
}

int VideoFormat::getFramePerSecond() const
{
	return (int)(10000000LL / videoHeader->AvgTimePerFrame);
}

int VideoFormat::getBitRate() const
{
	return videoHeader->dwBitRate;
}

AM_MEDIA_TYPE* VideoFormat::getNativeHandler() const
{
	return mediaType;
}