#include "CoEnvironment.h"

using namespace ezc;

static int initialized = FALSE;

int CoEnvironment::Initialize()
{
	if(initialized == FALSE)
	{
		HRESULT hr = CoInitialize(NULL);
		if(SUCCEEDED(hr)) initialized = TRUE;
	}

	return initialized;
}

void CoEnvironment::UnInitialize()
{
	CoUninitialize();
	initialized = FALSE;
}