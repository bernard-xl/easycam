#ifndef COENVIRONMENT_H
#define COENVIRONMENT_H

#include <objbase.h>

namespace ezc
{
	/**
	 * Simple static class for intialize and uninitialize COM.
	 * No matter how many times CoEnvironment::Initialize() is called,
	 * one CoEnvironment::UnInitialize() call effectively uninitialize COM.
	 */
	class CoEnvironment
	{
	public:
		static int Initialize();
		static void UnInitialize();
	};
}

#endif