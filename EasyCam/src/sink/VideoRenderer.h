#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <dshow.h>

namespace ezc
{
	/**
	 * This class abstracts the VMR9 renderer which is used to render the video on a window.
	 * It configures VMR9 to work in windowless mode.
	 */
	class VideoRenderer
	{
	public:
		VideoRenderer(void);
		~VideoRenderer(void);
	
		void setRenderWindow(HWND hWnd);
		void setVideoPosition(RECT rect);

		IBaseFilter* getNativeHandler() const;

		void repaintVideo(HDC hdc) const;

	private:
		IVMRWindowlessControl *ctrl;
		IBaseFilter *filter;
		HWND hWindow;
	};
}
#endif

