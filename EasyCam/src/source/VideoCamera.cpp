#include "VideoCamera.h"
#include "../utils/CoEnvironment.h"
#include "../err/SystemException.h"

using namespace ezc;

//The list must be here to makes the compiler happy.
static list<VideoCamera> devList;

//COM string to std::string conversion helper
static string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
    int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

    std::string dblstr(len, '\0');
    len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
                                pstr, wslen /* not necessary NULL-terminated */,
                                &dblstr[0], len,
                                NULL, NULL /* no default char */);

    return dblstr;
}

//COM string to std::string conversion helper
static string ConvertBSTRToMBS(BSTR bstr)
{
    int wslen = ::SysStringLen(bstr);
    return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

VideoCamera::Iterator VideoCamera::enumerateDevices()
{
	//Possible point to initialize COM
	CoEnvironment::Initialize();

	if(devList.size() != 0) return Iterator(devList.begin(), devList.end());

	// Create the System Device Enumerator.
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr)) throw SystemException(hr);

	// Obtain a class enumerator for the video input category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			devList.push_front(VideoCamera(pMoniker));
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();

	return Iterator(devList.begin(), devList.end());
}

VideoCamera::VideoCamera(int index)
{
	Iterator iter = VideoCamera::enumerateDevices();
	for(_ConstIterator i = iter.begin(); i != iter.end(); i++)
	{
		if(index-- == 0)
		{
			this->friendlyName = i->friendlyName;
			this->description = i->description;
			this->devicePath = i->devicePath;
			this->filter = i->filter;
			filter->AddRef();
			return;
		}
	}
	throw SystemException(ERROR_APP_INIT_FAILURE);
}

VideoCamera::VideoCamera(const VideoCamera &camera)
{
	if(this != &camera)
	{
		this->friendlyName = camera.friendlyName;
		this->description = camera.description;
		this->devicePath = camera.devicePath;
		this->filter = camera.filter;
		filter->AddRef();
	}
}

VideoCamera::VideoCamera(IMoniker *pMoniker)
{
	IPropertyBag *pPropBag;
    HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
    if (hr != S_OK)
    {
        pMoniker->Release();
		throw SystemException(hr);
    } 

    VARIANT desc;
    VariantInit(&desc);

    // Get description.
    hr = pPropBag->Read(L"Description", &desc, 0);
	if (hr != S_OK) description = "Not available.";
	else description = ConvertBSTRToMBS(desc.bstrVal);

	VariantClear(&desc); 

	VARIANT name;
    VariantInit(&name);

    // Get friendly name.
    hr = pPropBag->Read(L"FriendlyName", &name, 0);
	if (hr != S_OK) friendlyName = "Unknown Device";
	else friendlyName = ConvertBSTRToMBS(name.bstrVal);

	VariantClear(&name);

	VARIANT path;
    VariantInit(&path);

    // Get device path.
    hr = pPropBag->Read(L"DevicePath", &path, 0);
	if (hr != S_OK) devicePath = "";
	else devicePath = ConvertBSTRToMBS(path.bstrVal);
	VariantClear(&path);

    pPropBag->Release();
	
	hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&filter);
	if(FAILED(hr)) throw SystemException(hr);
	return;
}

VideoCamera::~VideoCamera(void)
{
	filter->Release();
}

string VideoCamera::getFriendlyName() const
{
	return friendlyName;
}

string VideoCamera::getDescription() const
{
	return description;
}

string VideoCamera::getDevicePath() const
{
	return devicePath;
}

IBaseFilter* VideoCamera::getNativeHandler() const
{
	return filter;
}