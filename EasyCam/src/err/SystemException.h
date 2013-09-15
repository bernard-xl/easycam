#ifndef SYSTEMEXCEPTION_H
#define SYSTEMEXCEPTION_H

#include <Windows.h>

#include <stdexcept>

using namespace std;

namespace ezc
{
	/**
	 * An exception type that wraps over the system error code
	 * and give simple description about the code.
	 */
	class SystemException : public std::runtime_error
	{
	public:
		SystemException(int errCode);
		SystemException(int errCode, string &description);
		~SystemException(void);

		virtual const char* what() const;
		virtual int code() const;

	protected:
		string lookupErrorMessage(int errCode) const;

	private:
		int errCode;
		string description;
	};
}
#endif

