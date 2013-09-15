#include "stdafx.h"
#include "QRCodeScanner.h"

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

static const unsigned int Y800 = MAKEFOURCC('Y','8','0','0');

QRCodeScanner::QRCodeScanner(void)
	: scanner(new ImageScanner())
{
}

QRCodeScanner::~QRCodeScanner(void)
{
	delete scanner;
}

void QRCodeScanner::NewFrameSampled(void *buffer, int width, int height, long size)
{
	Image img(width, height, "YUY2", buffer, size);
	Image converted(img.convert(Y800));

	int result = scanner->scan(converted);
	if(result == 0) lastResult = std::string();

	for (Image::SymbolIterator symbol = converted.symbol_begin(); symbol != converted.symbol_end(); ++symbol) {
		if(symbol->get_data() != lastResult)
		{
			lastResult = symbol->get_data();
			ShellExecuteA(NULL, "open", lastResult.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}
