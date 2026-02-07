#pragma once
#include <mutex>
#include <cassert>
#include "EngineUtility.h"
#include "ThirdParty/TinyXML2/tinyxml2.h"

#if DEBUGGING
#define LOG_MESSAGE(code, message)	do { ENGINE_NAMESPACE::g_pLogger->Message	(code, message, __FILE__, __FUNCTION__, __LINE__);	} while (0);
#define LOG_WARNING(code, warning)	do { ENGINE_NAMESPACE::g_pLogger->Warning	(code, #warning, __FILE__, __FUNCTION__, __LINE__);	} while (0);
#define LOG_ERROR(code, error)		do { ENGINE_NAMESPACE::g_pLogger->Error		(code, #error,	__FILE__, __FUNCTION__, __LINE__);	} while (0);
#define LOG_EXCEPTION(code, excep)	do { ENGINE_NAMESPACE::g_pLogger->Exception	(code, #excep, __FILE__, __FUNCTION__, __LINE__);	} while (0);
#define LOG_ASSERTION(assert)		do { ENGINE_NAMESPACE::g_pLogger->Assertion	(#assert, __FILE__, __FUNCTION__, __LINE__);		} while (0);
#else
#define LOG_MESSAGE(code, message)	do { void(0); } while (0);
#define LOG_WARNING(code, warning)	do { void(0); } while (0);
#define LOG_ERROR(code, error)		do { void(0); } while (0);
#define LOG_EXCEPTION(code, excep)	do { void(0); } while (0);
#define LOG_ASSERTION(assert)		do { void(0); } while (0);

#endif

BEGIN_ENGINE_NAMESPACE
class Logger
{
public:
	static void CreateLoggingSystem(const WCHAR* const fileName);
	static void DestroyLoggingSystem();

	void Message(LOGS log, const CHAR* const message, const CHAR* const file, const CHAR* const caller, int line);
	void Warning(LOGS log, const CHAR* const warning, const CHAR* const file, const CHAR* const caller, int line);
	void Error(LOGS log, const CHAR* const error, const CHAR* const file, const CHAR* const caller, int line);
	void Exception(LOGS log, const CHAR* const exception, const CHAR* const file, const CHAR* const caller, int line);
	void Assertion(const CHAR* const exception, const CHAR* const file, const CHAR* const caller, int line);

protected:
	Logger(const WCHAR* const fileName);
	~Logger();

private:
	void TranslateLogCode(LOGS log, CHAR* pOutString) const;
	void ExtractFileName(const CHAR* const fullPath, CHAR* file) const;

	tinyxml2::XMLPrinter* m_XMLPrinter;
	std::mutex m_Mutex;
	FILE* m_File;

	Logger(const Logger& copy) = delete;
	Logger& operator=(const Logger& right) = delete;
}; // end class Logger declaration

extern Logger* g_pLogger;

class LoggerWrapper
{
public:
	explicit LoggerWrapper(const WCHAR* const fileName);
	~LoggerWrapper();
}; // end helper class LoggerWrapper

class Exception
{
public:
	Exception(const CHAR* const file, const CHAR* const function, int line, bool fatal = true);
	const CHAR* const What() const;
	bool IsFatal() const;

private:
	CHAR m_Exception[MAX_STRING];
	const bool m_Fatal;
}; // end class Exception declaration

inline void ThrowIfNullptr(const void* ptr, LOGS log, const CHAR* const message,
	const CHAR* const file, const CHAR* const func, int line, bool fatal)
{
	if (ptr);
	else
	{
		g_pLogger->Exception(log, message, file, func, line);
		throw Exception(file, func, line, fatal);
	} // end else
} // end function ThrowIfNullptr

inline void ThrowIfFalse(bool value, LOGS log, const CHAR* const message,
	const CHAR* const file, const CHAR* const func, int line, bool fatal)
{
	if (value);
	else
	{
		g_pLogger->Exception(log, message, file, func, line);
		throw Exception(file, func, line, fatal);
	} // end else
} // end function ThrowIfFailed
END_ENGINE_NAMESPACE


#define ThrowIfNullptr(ptr, log, fatal)		ThrowIfNullptr(ptr, log, #ptr, __FILE__, __FUNCTION__, __LINE__, fatal);
#define ThrowIfFalse(boolean, log, fatal)	ThrowIfFalse(boolean, log, #boolean, __FILE__, __FUNCTION__, __LINE__, fatal);
#define Assert(expr)						(void)((!!(expr))																			\
												|| (ENGINE_NAMESPACE::g_pLogger->Assertion(#expr, __FILE__, __FUNCTION__, __LINE__), 0) \
												|| (ENGINE_NAMESPACE::Logger::DestroyLoggingSystem(), 0)								\
												|| (std::terminate(), 0)) // (_wassert(_CRT_WIDE(#expr), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0))
