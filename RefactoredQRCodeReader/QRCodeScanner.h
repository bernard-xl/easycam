#ifndef QRCODESCANNER_H
#define QRCODESCANNER_H

#include <string>

#include <shellapi.h>
#include <zbar.h>

#include "EasyCam.h"

using namespace std;
using namespace zbar;
using namespace ezc;

class QRCodeScanner : public VideoFrameListener
{ 
public:
	QRCodeScanner(void);
	virtual ~QRCodeScanner(void);

	virtual void NewFrameSampled(void *buffer, int width, int height, long size);

private:
	ImageScanner *scanner;
	string lastResult;
};

#endif