#include "EngineUtility.h"
#include <chrono>

BEGIN_ENGINE_NAMESPACE

static const std::chrono::high_resolution_clock::time_point g_TimeAtStartExecution = std::chrono::high_resolution_clock::now();
milliseconds GetTimeSinceStartExecution()
{
	return NanoToMilli((std::chrono::high_resolution_clock::now() - g_TimeAtStartExecution).count());
} // end function GetTimeSinceStartExecution

AppSettings::AppSettings()
	: minWidth(200), minHeight(125),
	timePerFrame(1000.0 / 60.0), forceConstantFrameTime(true),
	overrideWidth(720), overrideHeight(640),
	startXpos(0), startYpos(0),
	showMsgBox(true),
	showStats(true),
	parseCmdLine(false),
	forceStartFullscreen(false),
	showCursorWhileFullscreen(true),
	mustBeUniqueInstance(true),
	useJoystickIfAvailable(false),
	diskSpaceNeededMB(10),
	physicalRAMNeededMB(512),
	virtualRAMNeededMB(1024),
	minimumCPUSpeedMHz(1300)
{} // end struct CREATE_APP_INFO constructor

void* StringHashName(const char* pIdentStr)
{
	// Largest prime smaller than 65536
	const unsigned long BASE = 65521L;

	// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
	const unsigned long NMAX = 5552;

#define DO1(buf,i)  {s1 += tolower(buf[i]); s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

	if (pIdentStr)
	{
		unsigned long long s1 = 0;
		unsigned long long s2 = 0;

		for (size_t len = strlen(pIdentStr); len > 0; )
		{
			auto k = len < NMAX ? len : NMAX;

			len -= k;

			while (k >= 16)
			{
				DO16(pIdentStr);
				pIdentStr += 16;
				k -= 16;
			} // end while

			if (k != 0)
			{
				do
				{
					s1 += tolower(*pIdentStr++);
					s2 += s1;
				} while (--k);
			} // end if

			s1 %= BASE;
			s2 %= BASE;
		} // end for

		return reinterpret_cast<void*>((s2 << 16) | s1);
	} // end if

#undef DO1
#undef DO2
#undef DO4
#undef DO8
#undef DO16
	return nullptr;
} // end function  string_hash_name

// The following function was found on http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html
bool WildcardMatch(const wchar_t* pat, const wchar_t* str)
{
	const wchar_t* s;
	const wchar_t* p;
	bool star = false;

loopStart:
	for (s = str, p = pat; *s; ++s, ++p)
	{
		switch (*p)
		{
		case '?':
			if (*s == '.')
				goto starCheck;
			break;

		case '*':
			star = true;
			str = s, pat = p;
			do { ++pat; } while (*pat == '*');

			if (!*pat)
				return true;
			goto loopStart;

		default:
			if (*s != *p) //if (mapCaseTable[*s] != mapCaseTable[*p])
				goto starCheck;
			break;
		} // endswitch
	} // endfor

	while (*p == '*') ++p;
	return (!*p);

starCheck:
	if (!star)
		return false;
	str++;
	goto loopStart;
} // end function WildcardMatch


// This is basically like the Perl split() function.  It splits str into substrings by cutting it at each delimiter.  
// The result is stored in vec.
void Split(const std::string& str, std::vector<std::string>& vec, char delimiter)
{
	vec.clear();
	size_t strLen = str.size();
	if (strLen == 0)
		return;

	size_t startIndex = 0;
	size_t indexOfDel = str.find_first_of(delimiter, startIndex);
	while (indexOfDel != std::string::npos)
	{
		vec.push_back(str.substr(startIndex, indexOfDel - startIndex));
		startIndex = indexOfDel + 1;
		if (startIndex >= strLen)
			break;
		indexOfDel = str.find_first_of(delimiter, startIndex);
	} // end while
	if (startIndex < strLen)
		vec.push_back(str.substr(startIndex));
} // end function Split
END_ENGINE_NAMESPACE

#if 0
int i, star;

new_segment:

star = 0;
if (*pat == L'*')
{
	star = 1;
	do { pat++; } while (*pat == L'*'); /* enddo */
} /* endif */

test_match:

for (i = 0; pat[i] && (pat[i] != L'*'); i++)
{
	//if (mapCaseTable[str[i]] != mapCaseTable[pat[i]]) {
	if (str[i] != pat[i])
	{
		if (!str[i]) return false;
		if ((pat[i] == L'?') && (str[i] != L'.')) continue;
		if (!star) return false;
		str++;
		goto test_match;
	} // end if
} // end for

if (pat[i] == L'*')
{
	str += i;
	pat += i;
	goto new_segment;
} // end if

if (!str[i]) return true;
if (i && pat[i - 1] == L'*') return true;
if (!star) return false;
str++;
goto test_match;
#endif