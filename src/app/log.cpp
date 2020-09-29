#include "kkrooo.engine.h"
#include "Common.h"

#include "Functions.h"

#include <cstdarg>

class Log
{
public:
	Log();
	~Log();

	enum E_MessageType
	{
		EMT_INFO,
		EMT_ERROR,
		EMT_WARNING
	};

	void print(E_MessageType mt)
	{
		switch (mt)
		{
		case Log::EMT_INFO:
			fprintf(stdout, "Info: %s", m_buffer);
			break;
		case Log::EMT_ERROR:
			fprintf(stderr, "Error: %s", m_buffer);
			break;
		case Log::EMT_WARNING:
			fprintf(stderr, "Warning: %s", m_buffer);
			break;
		default:
			break;
		}
	}

	char m_buffer[1024];
};

Log::Log()
{
}

Log::~Log()
{
}

Log g_loger;


void kkLogWriteInfo(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(g_loger.m_buffer,1024,format,arg);
	va_end (arg);
	g_loger.print(Log::EMT_INFO);
}
void kkLogWriteError(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(g_loger.m_buffer,1024,format,arg);
	va_end (arg);
	g_loger.print(Log::EMT_ERROR);
}
void kkLogWriteWarning(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(g_loger.m_buffer,1024,format,arg);
	va_end (arg);
	g_loger.print(Log::EMT_WARNING);
}