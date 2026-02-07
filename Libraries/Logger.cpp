#include <share.h>
#include <assert.h>
#include "Logger.h"

BEGIN_ENGINE_NAMESPACE

	Logger* g_pLogger = nullptr;

	LoggerWrapper::LoggerWrapper(const WCHAR* const fileName)
	{
		Logger::CreateLoggingSystem(fileName);
	} // end class LoggerWrapper constructor

	LoggerWrapper::~LoggerWrapper()
	{
		Logger::DestroyLoggingSystem();
	} // end class LoggerWrapper destructor

	void Logger::CreateLoggingSystem(const WCHAR* const fileName)
	{
		if (!g_pLogger)
		{
			g_pLogger = new Logger(fileName);
			if (!g_pLogger)
				throw ENGINE_NAMESPACE::Exception(__FILE__, __FUNCTION__, __LINE__);
		} // end if
	} // end static method CreateLoggingSystem

	void Logger::DestroyLoggingSystem()
	{
		if (g_pLogger)
		{
			delete g_pLogger;
			g_pLogger = nullptr;
		} // end if
	} // end static method DestroyLoggingSystem

	Logger::Logger(const WCHAR* const fileName)
	{
		this->m_File = nullptr;
		std::lock_guard<std::mutex> l(this->m_Mutex);

		if (fileName)
			this->m_File = _wfsopen(fileName, L"wb", _SH_DENYWR);
		else
			this->m_File = _wfsopen(L"Log_File.xml", L"wb", _SH_DENYWR);

		if (this->m_File)
		{
			this->m_XMLPrinter = ENGINE_NEW tinyxml2::XMLPrinter(this->m_File);
			if (this->m_XMLPrinter)
			{
				this->m_XMLPrinter->PushHeader(true, true);
				this->m_XMLPrinter->PushComment("Program-Generated Log File");
				this->m_XMLPrinter->OpenElement("Logs");
			} // end if
			else
				throw GameEngine::Exception(__FILE__, __FUNCTION__, __LINE__);
		} // end if
		else
			throw GameEngine::Exception(__FILE__, __FUNCTION__, __LINE__);
	} // end class Logger constructor

	Logger::~Logger()
	{
		std::lock_guard<std::mutex> l(this->m_Mutex);
		if (this->m_XMLPrinter)
		{
			this->m_XMLPrinter->CloseElement();
			delete this->m_XMLPrinter;
		} // end if

		if (this->m_File)
		{
			fclose(this->m_File);
			this->m_File = nullptr;
		} // end if
	} // end class Logger destructor

	void Logger::Message(LOGS log, const CHAR* const message, const CHAR* const file, const CHAR* const caller, int line)
	{
		CHAR string[MAX_STRING];
		this->TranslateLogCode(log, string);
		//strcat_s(string, " (Message)");

		CHAR f[MAX_STRING];
		this->ExtractFileName(file, f);

		std::lock_guard<std::mutex> l(this->m_Mutex);
		this->m_XMLPrinter->OpenElement(f);
		this->m_XMLPrinter->PushAttribute("Log_Code", string);
		this->m_XMLPrinter->PushAttribute("Message", message);
		this->m_XMLPrinter->PushAttribute("Function", caller);
		this->m_XMLPrinter->PushAttribute("Line", line);
		this->m_XMLPrinter->CloseElement();
	} // end method Message

	void Logger::Warning(LOGS log, const CHAR* const warning, const CHAR* const file, const CHAR* const caller, int line)
	{
		CHAR string[MAX_STRING];
		this->TranslateLogCode(log, string);

		CHAR f[MAX_STRING];
		this->ExtractFileName(file, f);

		std::lock_guard<std::mutex> l(this->m_Mutex);
		this->m_XMLPrinter->OpenElement(f);
		this->m_XMLPrinter->PushAttribute("Log_Code", string);
		this->m_XMLPrinter->PushAttribute("Warning", warning);
		this->m_XMLPrinter->PushAttribute("Function", caller);
		this->m_XMLPrinter->PushAttribute("Line", line);
		this->m_XMLPrinter->CloseElement();
	} // end method Warning

	void Logger::Error(LOGS log, const CHAR* const error, const CHAR* const file, const CHAR* const caller, int line)
	{
		CHAR string[MAX_STRING];
		this->TranslateLogCode(log, string);

		CHAR f[MAX_STRING];
		this->ExtractFileName(file, f);

		std::lock_guard<std::mutex> l(this->m_Mutex);
		this->m_XMLPrinter->OpenElement(f);
		this->m_XMLPrinter->PushAttribute("Log_Code", string);
		this->m_XMLPrinter->PushAttribute("Error", error);
		this->m_XMLPrinter->PushAttribute("Function", caller);
		this->m_XMLPrinter->PushAttribute("Line", line);
		this->m_XMLPrinter->CloseElement();
	} // end method Error

	void Logger::Exception(LOGS log, const CHAR* const exception, const CHAR* const file, const CHAR* const caller, int line)
	{
		CHAR string[MAX_STRING];
		this->TranslateLogCode(log, string);
		
		CHAR f[MAX_STRING];
		this->ExtractFileName(file, f);

		std::lock_guard<std::mutex> l(this->m_Mutex);
		this->m_XMLPrinter->OpenElement(f);
		this->m_XMLPrinter->PushAttribute("Log_Code", string);
		this->m_XMLPrinter->PushAttribute("Exception", exception);
		this->m_XMLPrinter->PushAttribute("Function", caller);
		this->m_XMLPrinter->PushAttribute("Line", line);
		this->m_XMLPrinter->CloseElement();
	} // end method Exception

	void Logger::Assertion(const CHAR* const exception, const CHAR* const file, const CHAR* const caller, int line)
	{
		CHAR string[MAX_STRING];
		this->TranslateLogCode(LOGS::L_ASSERTION_FAILED, string);

		CHAR f[MAX_STRING];
		this->ExtractFileName(file, f);

		std::lock_guard<std::mutex> l(this->m_Mutex);
		this->m_XMLPrinter->OpenElement(f);
		this->m_XMLPrinter->PushAttribute("Log_Code", string);
		this->m_XMLPrinter->PushAttribute("Assertion_Failed", exception);
		this->m_XMLPrinter->PushAttribute("Function", caller);
		this->m_XMLPrinter->PushAttribute("Line", line);
		this->m_XMLPrinter->CloseElement();
	} // end method Assertion

#define CASE_EXCEPTION(code) case code: sprintf_s(pOutString, MAX_STRING, #code); break;
	void Logger::TranslateLogCode(LOGS log, CHAR* pOutString) const
	{
		if (pOutString)
		{
			switch (log)
			{
				CASE_EXCEPTION(LOGS::L_INFO);
				CASE_EXCEPTION(LOGS::L_ASSERTION_FAILED);
				CASE_EXCEPTION(LOGS::L_FUNCTION_SUCCEEDED);
				CASE_EXCEPTION(LOGS::L_FUNCTION_FAILED);
				CASE_EXCEPTION(LOGS::L_THREAD_EXCEPTION);
				CASE_EXCEPTION(LOGS::L_SCRIPT_ERROR);
				CASE_EXCEPTION(LOGS::L_INVALID_POINTER);
				CASE_EXCEPTION(LOGS::L_INVALID_ID);
				CASE_EXCEPTION(LOGS::L_OBJECT_NOT_INITIALIZED);
				CASE_EXCEPTION(LOGS::L_OBJECT_ALREADY_INITIALIZED);
				CASE_EXCEPTION(LOGS::L_ILLEGAL_CALL_TO_FUNCTION);
				CASE_EXCEPTION(LOGS::L_UNEXPECTED_CALL_TO_FUNCTION);
				CASE_EXCEPTION(LOGS::L_UNEXPECTED_VALID_POINTER);
				CASE_EXCEPTION(LOGS::L_OPERATION_CANCELED);
				CASE_EXCEPTION(LOGS::L_TIME_EXCEEDED);
				CASE_EXCEPTION(LOGS::L_DATA_CORRUPTED);
				CASE_EXCEPTION(LOGS::L_OUT_OF_MEMORY);
				CASE_EXCEPTION(LOGS::L_HARDWARE_ERROR);
				CASE_EXCEPTION(LOGS::L_NOT_SUPPORTED);
				CASE_EXCEPTION(LOGS::L_UNKNOWN_CODE);
			default:
				sprintf_s(pOutString, MAX_STRING, "Unrecognised exeption code");
				break;
			} // end switch
		} // end if
	} // end method TranslateLogCode
#undef CASE_EXCEPTION

	void Logger::ExtractFileName(const CHAR* const fullPath, CHAR* file) const
	{
		const auto len = strlen(fullPath);
		size_t i = len;
		for (; i != 0; --i)
			if (fullPath[i - 1] == '\\' || fullPath[i - 1] == '/')
				break;
		strcpy_s(file, MAX_STRING, fullPath + i);
	} // end method ExtractFileName

	Exception::Exception(const CHAR* const file, const CHAR* const function, int line, bool fatal)
		: m_Fatal(fatal)
	{
		sprintf_s(this->m_Exception, MAX_STRING, "Exception encountered while executing"
			" function %s in file %s at line %i.\n", function, file, line);
	} // end class Exception constructor

	const CHAR* const Exception::What() const
	{
		return this->m_Exception;
	} // end method What

	bool Exception::IsFatal() const
	{
		return this->m_Fatal;
	} // end method IsFatal
END_ENGINE_NAMESPACE