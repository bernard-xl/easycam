#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <dshow.h>
#include "../patch/qedit.h"

#include <list>

#include "../source/VideoCamera.h"
#include "../sink/VideoRenderer.h"
#include "VideoFormat.h"
#include "VideoFrameListener.h"

using namespace std;

namespace ezc
{
	//Forward declaration of internally used class.
	class VideoCallback;

	/**
	 * The core class that stream the video from input to output.
	 */
	class VideoSource
	{
	public:
		VideoSource(VideoCamera *camera);
		~VideoSource(void);

		VideoFormat::Iterator getSupportedFormats() const;
		VideoFormat* getFormat() const;
		void setFormat(VideoFormat *format);

		VideoFrameListener* getNewFrameListener() const;
		void setNewFrameListener(VideoFrameListener *listener);

		void start(VideoRenderer *renderer = NULL);
		void stop();

	private:
		mutable list<VideoFormat> fmtList;
		mutable VideoFormat *format;
		VideoCamera *input;
		VideoFrameListener *listener;
		ICaptureGraphBuilder2 *captureBuilder;
		IGraphBuilder *graphBuilder;
		IMediaControl *mediaCtrl;
		ISampleGrabber *grabber;
		IBaseFilter *grabberFilter;
		IBaseFilter *renderFilter;
		VideoCallback *callbackWrapper;
	};

	/**
	 * This class isolates the event listener of VideoSource from COM.
	 */
	class VideoCallback : public ISampleGrabberCB 
	{
	public:
		//some variables and stuff... 
		STDMETHODIMP_(ULONG) AddRef()  { return 2; }
		STDMETHODIMP_(ULONG) Release() { return 1; }

		STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
		{
			//CheckPointer(ppv, E_POINTER);

			if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) 
			{
				*ppv = (void *) static_cast<ISampleGrabberCB *>(this);
				return NOERROR;
			}    
			return E_NOINTERFACE;
		}

		STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
		{ 
			return 0;
		}

		STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferLen )
		{
			listener->NewFrameSampled(pBuffer, videoWidth, videoHeight, BufferLen);
			return 0;
		}

		VideoFrameListener *listener;
		int videoWidth;
		int videoHeight;
	};
}
#endif