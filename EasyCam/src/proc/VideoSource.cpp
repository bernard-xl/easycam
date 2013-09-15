#include "VideoSource.h"
#include "..\utils\CoEnvironment.h"
#include "..\err\SystemException.h"

using namespace ezc;

static inline void releaseIfNotNull(IUnknown *comPtr)
{
	if(comPtr != NULL) comPtr->Release();
}

VideoSource::VideoSource(VideoCamera *camera)
	: input(camera), listener(NULL), captureBuilder(NULL), graphBuilder(NULL), mediaCtrl(NULL), 
	grabber(NULL), grabberFilter(NULL), renderFilter(NULL), format(NULL), callbackWrapper(NULL)
{
	//possible point to initialize COM
	CoEnvironment::Initialize();

	HRESULT hr;
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&captureBuilder);
	if(FAILED(hr)) throw SystemException(hr);
	hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&graphBuilder);
	if(FAILED(hr)) throw SystemException(hr);
	captureBuilder->SetFiltergraph(graphBuilder);
	hr = graphBuilder->QueryInterface(IID_IMediaControl, (void**)&mediaCtrl);
	if(FAILED(hr)) throw SystemException(hr);
	hr = graphBuilder->AddFilter(camera->getNativeHandler(), L"Video input");
	if(FAILED(hr)) throw SystemException(hr);
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&grabberFilter));
	hr = graphBuilder->AddFilter(grabberFilter, L"Sample Grabber");
	hr = grabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&grabber);
	if(FAILED(hr)) throw SystemException(hr);
	grabber->SetOneShot(FALSE);
	grabber->SetBufferSamples(TRUE);

}

VideoSource::~VideoSource(void)
{
	if(format != NULL) delete format;
	releaseIfNotNull(grabber);
	releaseIfNotNull(grabberFilter);
	releaseIfNotNull(mediaCtrl);
	releaseIfNotNull(graphBuilder);
	releaseIfNotNull(captureBuilder);
}

VideoFormat::Iterator VideoSource::getSupportedFormats() const
{
	if(fmtList.size() != 0) return VideoFormat::Iterator(fmtList.begin(), fmtList.end());

	IAMStreamConfig *pConfig;
	HRESULT hr = captureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0, input->getNativeHandler(), IID_IAMStreamConfig, (void**)&pConfig);

	int iCount = 0, iSize = 0;
	hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		for (int iFormat = 0; iFormat < iCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;
			hr = pConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
			if (SUCCEEDED(hr))
			{
				if(pmtConfig->majortype == MEDIATYPE_Video)
				{
					fmtList.push_back(VideoFormat(pmtConfig));
				}
			}
		}
	}
	return VideoFormat::Iterator(fmtList.begin(), fmtList.end());
}

VideoFormat* VideoSource::getFormat() const
{
	if(format != NULL) 
	{
		delete format;
		format = NULL;
	}

	AM_MEDIA_TYPE *mediaType;
	IAMStreamConfig *pConfig;
	HRESULT hr = captureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0, input->getNativeHandler(), IID_IAMStreamConfig, (void**)&pConfig);
	hr = pConfig->GetFormat(&mediaType);
	if(FAILED(hr)) throw SystemException(hr);
	format = new VideoFormat(mediaType);
	return format;
}

void VideoSource::setFormat(VideoFormat *format)
{
	IAMStreamConfig *pConfig;
	HRESULT hr = captureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0, input->getNativeHandler(), IID_IAMStreamConfig, (void**)&pConfig);
	hr =pConfig->SetFormat(format->getNativeHandler());
	if(FAILED(hr)) throw SystemException(hr);

	if(callbackWrapper != NULL)
	{
		callbackWrapper->videoWidth = format->getWidth();
		callbackWrapper->videoHeight = format->getHeight();
	}
}

VideoFrameListener* VideoSource::getNewFrameListener() const
{
	return listener;
}

void VideoSource::setNewFrameListener(VideoFrameListener *listener)
{
	this->listener = listener;
	if(callbackWrapper != NULL) callbackWrapper->listener = listener;
}

void VideoSource::start(VideoRenderer *renderer)
{
	if(renderFilter != NULL) throw SystemException(0, string("Another stream is running."));

	renderFilter = (renderer == NULL)? NULL : renderer->getNativeHandler();
	if(renderFilter != NULL) graphBuilder->AddFilter(renderFilter, L"Video renderer");
	HRESULT hr = captureBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, input->getNativeHandler(), NULL, renderFilter);
	if(FAILED(hr)) throw SystemException(hr);

	if(listener != NULL)
	{
		getFormat();
		callbackWrapper = new VideoCallback();
		callbackWrapper->listener = listener;
		callbackWrapper->videoWidth = format->getWidth();
		callbackWrapper->videoHeight = format->getHeight();
		grabber->SetCallback(callbackWrapper, 1);
	}
	mediaCtrl->Run();
}

void VideoSource::stop()
{
	mediaCtrl->Stop();

	if(callbackWrapper != NULL)
	{
		delete callbackWrapper;
		callbackWrapper = NULL;
	}

	if(renderFilter != NULL) {
		graphBuilder->RemoveFilter(renderFilter);
		renderFilter = NULL;
	}
}