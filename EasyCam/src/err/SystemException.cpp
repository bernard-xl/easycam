#include "SystemException.h"

using namespace ezc;

SystemException::SystemException(int errCode)
	: errCode(errCode), description(lookupErrorMessage(errCode)), runtime_error("")
{
}

SystemException::SystemException(int errCode, string &description)
	: errCode(errCode), description(description), runtime_error("")
{
}

SystemException::~SystemException(void)
{
}

const char* SystemException::what() const
{
	return description.c_str();
}

int SystemException::code() const
{
	return errCode;
}

string SystemException::lookupErrorMessage(int errorCode) const
{
	void* p_text;
	DWORD count = ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
									FORMAT_MESSAGE_FROM_SYSTEM |
									FORMAT_MESSAGE_MAX_WIDTH_MASK,
									NULL,
									errCode,
									MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
									(LPTSTR) &p_text,
									0,
									NULL);
	string message((char*)p_text);
	::LocalFree (p_text);
	return message;
}
