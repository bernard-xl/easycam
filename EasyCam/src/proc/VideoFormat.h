#ifndef VIDEOFORMAT_H
#define VIDEOFORMAT_H

#include <dshow.h>

#include <list>

#include "..\utils\IteratorWrapper.h"

using namespace std;

namespace ezc
{
	/**
	 * An entity class that provides information about video stream format.
	 */
	class VideoFormat
	{
	public:
		typedef list<VideoFormat>::const_iterator _ConstIterator;
		typedef IteratorWrapper<_ConstIterator> Iterator;

		static const int UNKNOWN = 0x0;
		static const int RGB32 = 0x1 << 0;
		static const int RGB24 = 0x1 << 1;
		static const int RGB8  = 0x1 << 2;
		static const int YUY2  = 0x1 << 3;

		VideoFormat(AM_MEDIA_TYPE *mediaType);
		VideoFormat(const VideoFormat &videoFmt);
		~VideoFormat(void);

		int getWidth() const;
		int getHeight() const;
		int getType() const;
		int getBitRate() const;
		int getFramePerSecond() const;
		AM_MEDIA_TYPE* getNativeHandler() const;

	private:
		AM_MEDIA_TYPE *mediaType;
		VIDEOINFOHEADER *videoHeader;
	};
}
#endif