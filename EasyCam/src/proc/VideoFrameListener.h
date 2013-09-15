#ifndef VIDEOFRAMELISTENER_H
#define VIDEOFRAMELISTENER_H

namespace ezc
{
	/**
	 * A functional interface that is used to listen new frame event of VideoSource.
	 * This listener will get called when a new frame is buffered.
	 */
	class VideoFrameListener
	{
	public:
		VideoFrameListener(void);
		virtual ~VideoFrameListener(void);

		virtual void NewFrameSampled(void *buffer, int width, int height, long size) = 0;
	};
}
#endif
