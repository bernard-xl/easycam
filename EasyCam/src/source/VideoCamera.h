#ifndef VIDEOCAMERA_H
#define VIDEOCAMERA_H

#include <dshow.h>

#include <string>
#include <list>

#include "..\utils\IteratorWrapper.h"

using namespace std;

namespace ezc
{
	/**
	 * This class abstracts the cameras installed on the system.
	 * It provides basic identification about the device, works together with VideoSource.
	 */
	class VideoCamera
	{
	public:
		typedef list<VideoCamera>::const_iterator _ConstIterator;
		typedef IteratorWrapper<_ConstIterator> Iterator;

		static Iterator enumerateDevices();
		static VideoCamera getInstance(int index);

		VideoCamera(int index);
		VideoCamera(const VideoCamera &camera);
		~VideoCamera(void);

		string getFriendlyName() const;
		string getDescription() const;
		string getDevicePath() const;

		IBaseFilter* getNativeHandler() const;

	private:
		VideoCamera(IMoniker *moniker);

	private:
		string friendlyName;
		string description;
		string devicePath;
		IBaseFilter *filter;
	};
}
#endif
