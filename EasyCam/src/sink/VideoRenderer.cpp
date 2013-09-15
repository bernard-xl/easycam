#include <dshow.h>
#include "VideoRenderer.h"
#include "..\utils\CoEnvironment.h"
#include "..\err\SystemException.h"

using namespace ezc;

VideoRenderer::VideoRenderer(void)
	: ctrl(NULL)
{
	//Possible point to intialize COM
	CoEnvironment::Initialize();

	HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, 
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&filter); 
	if (FAILED(hr)) throw SystemException(hr);

	IVMRFilterConfig* pConfig; 
    hr = filter->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
	if(FAILED(hr)) throw SystemException(hr);

    hr = pConfig->SetRenderingMode(VMRMode_Windowless); 
    pConfig->Release(); 
	if(FAILED(hr)) throw SystemException(hr);

	hr = filter->QueryInterface(IID_IVMRWindowlessControl, (void**)&ctrl);
	if(FAILED(hr)) throw SystemException(hr);
}

VideoRenderer::~VideoRenderer(void)
{
	if(ctrl != NULL) ctrl->Release();
}

void VideoRenderer::setRenderWindow(HWND hWnd)
{
	HRESULT hr = ctrl->SetVideoClippingWindow(hWnd);
	if(FAILED(hr)) throw SystemException(hr);
	hWindow = hWnd;
}

void VideoRenderer::setVideoPosition(RECT rect)
{
	ctrl->SetVideoPosition(NULL, &rect);
}

IBaseFilter* VideoRenderer::getNativeHandler() const
{
	return filter;
}

void VideoRenderer::repaintVideo(HDC hdc) const
{
	ctrl->RepaintVideo(hWindow, hdc);
}