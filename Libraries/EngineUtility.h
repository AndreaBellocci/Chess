#pragma once
#include <string>
#include <memory>
#include <vector>
#include <random>
#include <unordered_map>
#include "ThirdParty\FastDelegate\FastDelegate.h"


// Debug (nonoptimized code, asserts active, PROFILE defined to assist debugging)
// Profile (optimized code, asserts disabled, PROFILE defined to assist optimization)
// Release (optimized code, asserts disabled, PROFILE not defined)
#ifndef DEBUGGING
#define DEBUGGING ((_DEBUG) || (DEBUG)) && !(NDEBUG)
#endif

#ifndef ENGINE_NAMESPACE
#define ENGINE_NAMESPACE GameEngine
#endif

#ifndef BEGIN_ENGINE_NAMESPACE
#define BEGIN_ENGINE_NAMESPACE namespace ENGINE_NAMESPACE {
#endif

#ifndef END_ENGINE_NAMESPACE
#define END_ENGINE_NAMESPACE }
#endif

#if DEBUGGING
#define CHECK_MEMORY_ALLOCATIONS
#endif

#    define VERIFY(...)do{}while(false)
#    define VERIFY_EXPR(...)do{}while(false)


#define ENGINE_NEW new

#ifdef _MSC_VER
// Note that MSVC x86 compiler by default uses __this call for class member functions
#	define ENGINE_CALL_TYPE __cdecl
#else
#	define ENGINE_CALL_TYPE
#endif

BEGIN_ENGINE_NAMESPACE

/// Unique interface identifier
struct INTERFACE_ID
{
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t  Data4[8];

	bool operator==(const INTERFACE_ID& rhs) const
	{
		return Data1 == rhs.Data1 &&
			Data2 == rhs.Data2 &&
			Data3 == rhs.Data3 &&
			memcmp(Data4, rhs.Data4, sizeof(Data4)) == 0;
	}
};

/// Unknown interface
static const INTERFACE_ID IID_Unknown = { 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0} };


/// Base interface for a raw memory allocator
struct IMemoryAllocator
{
	/// Allocates block of memory
	virtual void* Allocate(size_t Size, const char* dbgDescription, const char* dbgFileName, const int32_t dbgLineNumber) = 0;

	/// Releases memory
	virtual void Free(void* Ptr) = 0;
}; // end interface IMemoryAllocator
 
typedef struct INTERFACE_ID INTERFACE_ID;

typedef long long INT64;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long UINT64;
typedef wchar_t WCHAR;
typedef char CHAR;
typedef unsigned char BYTE;

typedef fastdelegate::FastDelegate1<short /*Progress*/, void> NotifyProgressCallback;
typedef fastdelegate::FastDelegate2<short /*Progress*/, bool& /*outCancel*/, void> ManageProgressCallback;

using nanoseconds = __int64;
using milliseconds = float;
constexpr milliseconds NanoToMilli(nanoseconds ns) { return ns / 1000.0f / 1000.0f; }
constexpr nanoseconds MilliToNano(milliseconds ms) { return __int64(ms * 1000.0f) * 1000; }

constexpr unsigned int MAX_STRING = 256;
constexpr __int64 kINFINITE = LLONG_MAX;
constexpr UINT64 KILO_BYTE = 1024;
constexpr UINT64 MEGA_BYTE = KILO_BYTE * 1024;
constexpr UINT64 GIGA_BYTE = MEGA_BYTE * 1024;

milliseconds GetTimeSinceStartExecution();

enum class Supported_OS
{
	OS_WINDOWS
}; // end scoped enum Supported_OS

enum class Supported_Renderers
{
	R_UNKNOWN = -1,
	R_DIRECT3D_11,
	R_DIRECT3D_12
}; // end scoped enum Supported_Renderers

enum class DIRECTORY
{
	D_ROOT = 0,
	D_SETUP,
	D_ASSETS,
	D_LOGGER,
	D_SAVED_GAMES,
	D_SCREENSHOTS,
}; // end scoped enum DIRECTORY

enum class LOGS
{
	L_INFO = -1,
	L_ASSERTION_FAILED,
	L_FUNCTION_SUCCEEDED,
	L_FUNCTION_FAILED,
	L_THREAD_EXCEPTION,
	L_SCRIPT_ERROR,
	L_INVALID_PARAMETER,
	L_INVALID_POINTER,
	L_INVALID_ID,
	L_OBJECT_NOT_INITIALIZED,
	L_OBJECT_ALREADY_INITIALIZED,
	L_ILLEGAL_CALL_TO_FUNCTION,
	L_UNEXPECTED_CALL_TO_FUNCTION,
	L_UNEXPECTED_VALID_POINTER,
	L_OPERATION_CANCELED,
	L_TIME_EXCEEDED,
	L_DATA_CORRUPTED,
	L_OUT_OF_MEMORY,
	L_HARDWARE_ERROR,
	L_NOT_SUPPORTED,
	L_UNKNOWN_CODE,
}; // end scoped enum LOGS

enum class BaseGameState
{
	BGS_Invalid,
	BGS_Initializing,
	BGS_Running,
	BGS_Paused,
	BGS_Quitting,
	BGS_MenuLoop,
	BGS_WaitingForPlayers,
	BGS_LoadingGameEnvironment,
	BGS_WaitingForPlayersToLoadEnvironment,
	BGS_SpawningPlayersActors,
}; // end scoped enum BaseGameState

// The values of this enum are designed to match Windows virtual key codes;
// If other operating systems uses different codes, it must be changed to match them
// NOTE: This enum contains also the codes for mouse input, although this engine 
// handle all mouse inputs in a separate method
enum class Keys : unsigned short
{
#ifdef _WINDOWS
	K_LBUTTON	= 0x01,
	K_RBUTTON	= 0x02,
	K_CANCEL	= 0x03,
	K_MBUTTON	= 0x04, // NOT contiguous with L & RBUTTON
	K_XBUTTON1	= 0x05, // NOT contiguous with L & RBUTTON
	K_XBUTTON2	= 0x06, // NOT contiguous with L & RBUTTON

	// 0x07 : Reserved

	K_BACK		= 0x08,
	K_TAB		= 0x09,

	// 0x0A - 0x0B : Reserved

	K_CLEAR		= 0x0C,
	K_RETURN	= 0x0D, // Enter

	// 0x0E - 0x0F : Unassigned

	K_SHIFT		= 0x10,
	K_CONTROL	= 0x11,
	K_MENU		= 0x12,
	K_PAUSE		= 0x13,
	K_CAPITAL	= 0x14,
	K_KANA		= 0x15,
	K_HANGEUL	= 0x15, // Old name - should be here for compatibility
	K_HANGUL	= 0x15,

	// 0x16 : Unassigned

	K_JUNJA = 0x17,
	K_FINAL = 0x18,
	K_HANJA = 0x19,
	K_KANJI = 0x19,

	// 0x1A : Unassigned

	K_ESCAPE		= 0x1B,
	K_CONVERT		= 0x1C,
	K_NONCONVERT	= 0x1D,
	K_ACCEPT		= 0x1E,
	K_MODECHANGE	= 0x1F,

	K_SPACE		= 0x20,
	K_PRIOR		= 0x21,
	K_NEXT		= 0x22,
	K_END		= 0x23,
	K_HOME		= 0x24,
	K_LEFT		= 0x25,
	K_UP		= 0x26,
	K_RIGHT		= 0x27,
	K_DOWN		= 0x28,
	K_SELECT	= 0x29,
	K_PRINT		= 0x2A,
	K_EXECUTE	= 0x2B,
	K_SNAPSHOT	= 0x2C,
	K_INSERT	= 0x2D,
	K_DELETE	= 0x2E,
	K_HELP		= 0x2F,

	// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	K_0 = 0x30,
	K_1 = 0x31,
	K_2 = 0x32,
	K_3 = 0x33,
	K_4 = 0x34,
	K_5 = 0x35,
	K_6 = 0x36,
	K_7 = 0x37,
	K_8 = 0x38,
	K_9 = 0x39,

	// 0x3A - 0x40 : unassigned

	// VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
	K_A = 0x41,
	K_B = 0x42,
	K_C = 0x43,
	K_D = 0x44,
	K_E = 0x45,
	K_F = 0x46,
	K_G = 0x47,
	K_H = 0x48,
	K_I = 0x49,
	K_J = 0x4A,
	K_K = 0x4B,
	K_L = 0x4C,
	K_M = 0x4D,
	K_N = 0x4E,
	K_O = 0x4F,
	K_P = 0x50,
	K_Q = 0x51,
	K_R = 0x52,
	K_S = 0x53,
	K_T = 0x54,
	K_U = 0x55,
	K_V = 0x56,
	K_W = 0x57,
	K_X = 0x58,
	K_Y = 0x59,
	K_Z = 0x5A,

	K_LWIN = 0x5B,
	K_RWIN = 0x5C,
	K_APPS = 0x5D,

	// 0x5E : Reserved

	K_SLEEP = 0x5F,

	K_NUMPAD0 = 0x60,
	K_NUMPAD1 = 0x61,
	K_NUMPAD2 = 0x62,
	K_NUMPAD3 = 0x63,
	K_NUMPAD4 = 0x64,
	K_NUMPAD5 = 0x65,
	K_NUMPAD6 = 0x66,
	K_NUMPAD7 = 0x67,
	K_NUMPAD8 = 0x68,
	K_NUMPAD9 = 0x69,

	K_MULTIPLY	= 0x6A,
	K_ADD		= 0x6B,
	K_SEPARATOR	= 0x6C,
	K_SUBTRACT	= 0x6D,
	K_DECIMAL	= 0x6E,
	K_DIVIDE	= 0x6F,

	K_F1 = 0x70,
	K_F2 = 0x71,
	K_F3 = 0x72,
	K_F4 = 0x73,
	K_F5 = 0x74,
	K_F6 = 0x75,
	K_F7 = 0x76,
	K_F8 = 0x77,
	K_F9 = 0x78,
	K_F10 = 0x79,
	K_F11 = 0x7A,
	K_F12 = 0x7B,
	K_F13 = 0x7C,
	K_F14 = 0x7D,
	K_F15 = 0x7E,
	K_F16 = 0x7F,
	K_F17 = 0x80,
	K_F18 = 0x81,
	K_F19 = 0x82,
	K_F20 = 0x83,
	K_F21 = 0x84,
	K_F22 = 0x85,
	K_F23 = 0x86,
	K_F24 = 0x87,

	// 0x88 - 0x8F : UI navigation
	K_NAVIGATION_VIEW	= 0x88, // Reserved
	K_NAVIGATION_MENU	= 0x89, // Reserved
	K_NAVIGATION_UP		= 0x8A, // Reserved
	K_NAVIGATION_DOWN	= 0x8B, // Reserved
	K_NAVIGATION_LEFT	= 0x8C, // Reserved
	K_NAVIGATION_RIGHT	= 0x8D, // Reserved
	K_NAVIGATION_ACCEPT	= 0x8E, // Reserved
	K_NAVIGATION_CANCEL	= 0x8F, // Reserved
	K_NUMLOCK			= 0x90,
	K_SCROLL			= 0x91,

	// NEC PC-9800 kbd definitions
	K_OEM_NEC_EQUAL = 0x92,  // '=' key on numpad

	// Fujitsu/OASYS kbd definitions
	K_OEM_FJ_JISHO		= 0x92, // 'Dictionary' key
	K_OEM_FJ_MASSHOU	= 0x93, // 'Unregister word' key
	K_OEM_FJ_TOUROKU	= 0x94, // 'Register word' key
	K_OEM_FJ_LOYA		= 0x95, // 'Left OYAYUBI' key
	K_OEM_FJ_ROYA		= 0x96, // 'Right OYAYUBI' key
	
	// 0x97 - 0x9F : Unassigned
	
	// VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
	// Used only as parameters to GetAsyncKeyState() and GetKeyState().
	// No other API or message will distinguish left and right keys in this way.
	K_LSHIFT	= 0xA0,
	K_RSHIFT	= 0xA1,
	K_LCONTROL	= 0xA2,
	K_RCONTROL	= 0xA3,
	K_LMENU		= 0xA4,
	K_RMENU		= 0xA5,

	K_BROWSER_BACK		= 0xA6,
	K_BROWSER_FORWARD	= 0xA7,
	K_BROWSER_REFRESH	= 0xA8,
	K_BROWSER_STOP		= 0xA9,
	K_BROWSER_SEARCH	= 0xAA,
	K_BROWSER_FAVORITES	= 0xAB,
	K_BROWSER_HOME		= 0xAC,

	K_VOLUME_MUTE			= 0xAD,
	K_VOLUME_DOWN			= 0xAE,
	K_VOLUME_UP				= 0xAF,
	K_MEDIA_NEXT_TRACK		= 0xB0,
	K_MEDIA_PREV_TRACK		= 0xB1,
	K_MEDIA_STOP			= 0xB2,
	K_MEDIA_PLAY_PAUSE		= 0xB3,
	K_LAUNCH_MAIL			= 0xB4,
	K_LAUNCH_MEDIA_SELECT	= 0xB5,
	K_LAUNCH_APP1			= 0xB6,
	K_LAUNCH_APP2			= 0xB7,

	// 0xB8 - 0xB9 : Reserved

	K_OEM_1			= 0xBA,	// ';:' for US
	K_OEM_PLUS		= 0xBB,	// '+' any country
	K_OEM_COMMA		= 0xBC, // ',' any country
	K_OEM_MINUS		= 0xBD, // '-' any country
	K_OEM_PERIOD	= 0xBE, // '.' any country
	K_OEM_2			= 0xBF, // '/?' for US
	K_OEM_3			= 0xC0, // '`~' for US

	// 0xC1 - 0xC2 : Reserved

	// 0xC3 - 0xDA : Gamepad input
	K_GAMEPAD_A							= 0xC3, // Reserved
	K_GAMEPAD_B							= 0xC4, // Reserved
	K_GAMEPAD_X							= 0xC5, // Reserved
	K_GAMEPAD_Y							= 0xC6, // Reserved
	K_GAMEPAD_RIGHT_SHOULDER			= 0xC7, // Reserved
	K_GAMEPAD_LEFT_SHOULDER				= 0xC8, // Reserved
	K_GAMEPAD_LEFT_TRIGGER				= 0xC9, // Reserved
	K_GAMEPAD_RIGHT_TRIGGER				= 0xCA, // Reserved
	K_GAMEPAD_DPAD_UP					= 0xCB, // Reserved
	K_GAMEPAD_DPAD_DOWN					= 0xCC, // Reserved
	K_GAMEPAD_DPAD_LEFT					= 0xCD, // Reserved
	K_GAMEPAD_DPAD_RIGHT				= 0xCE, // Reserved
	K_GAMEPAD_MENU						= 0xCF, // Reserved
	K_GAMEPAD_VIEW						= 0xD0, // Reserved
	K_GAMEPAD_LEFT_THUMBSTICK_BUTTON	= 0xD1, // Reserved
	K_GAMEPAD_RIGHT_THUMBSTICK_BUTTON	= 0xD2, // Reserved
	K_GAMEPAD_LEFT_THUMBSTICK_UP		= 0xD3, // Reserved
	K_GAMEPAD_LEFT_THUMBSTICK_DOWN		= 0xD4, // Reserved
	K_GAMEPAD_LEFT_THUMBSTICK_RIGHT		= 0xD5, // Reserved
	K_GAMEPAD_LEFT_THUMBSTICK_LEFT		= 0xD6, // Reserved
	K_GAMEPAD_RIGHT_THUMBSTICK_UP		= 0xD7, // Reserved
	K_GAMEPAD_RIGHT_THUMBSTICK_DOWN		= 0xD8, // Reserved
	K_GAMEPAD_RIGHT_THUMBSTICK_RIGHT	= 0xD9, // Reserved
	K_GAMEPAD_RIGHT_THUMBSTICK_LEFT		= 0xDA, // Reserved

	K_OEM_4 = 0xDB, //  '[{' for US
	K_OEM_5 = 0xDC, //  '\|' for US
	K_OEM_6 = 0xDD, //  ']}' for US
	K_OEM_7 = 0xDE, //  ''"' for US
	K_OEM_8 = 0xDF,

	// 0xE0 : reserved

	// Various extended or enhanced keyboards
	K_OEM_AX		= 0xE1,	//  'AX' key on Japanese AX kbd
	K_OEM_102		= 0xE2,	//  "<>" or "\|" on RT 102-key kbd.
	K_ICO_HELP		= 0xE3,	//  Help key on ICO
	K_ICO_00		= 0xE4,	//  00 key on ICO
	K_PROCESSKEY	= 0xE5,
	K_ICO_CLEAR		= 0xE6,
	K_PACKET		= 0xE7,

	// 0xE8 : unassigned

	// Nokia/Ericsson definitions
	K_OEM_RESET		= 0xE9,
	K_OEM_JUMP		= 0xEA,
	K_OEM_PA1		= 0xEB,
	K_OEM_PA2		= 0xEC,
	K_OEM_PA3		= 0xED,
	K_OEM_WSCTRL	= 0xEE,
	K_OEM_CUSEL		= 0xEF,
	K_OEM_ATTN		= 0xF0,
	K_OEM_FINISH	= 0xF1,
	K_OEM_COPY		= 0xF2,
	K_OEM_AUTO		= 0xF3,
	K_OEM_ENLW		= 0xF4,
	K_OEM_BACKTAB	= 0xF5,

	K_ATTN			= 0xF6,
	K_CRSEL			= 0xF7,
	K_EXSEL			= 0xF8,
	K_EREOF			= 0xF9,
	K_PLAY			= 0xFA,
	K_ZOOM			= 0xFB,
	K_NONAME		= 0xFC,
	K_PA1			= 0xFD,
	K_OEM_CLEAR		= 0xFE,

	// 0xFF : reserved

	K_TotalKeys = 256

#endif // _WINDOWS
}; // end scoped enumeration Keys

struct AppSettings
{
	AppSettings();

	int		minWidth, minHeight;			// Minimum window's size (in pixels)	
	int		overrideWidth, overrideHeight;	// Force initial window's size (in pixels)
	int		startXpos, startYpos;			// Force initial window's position on screen
	double	timePerFrame;					// Requested refresh time (in ms)		
	bool	forceStartFullscreen;			// Force to start int windowed/fullscreen mode
	bool	forceConstantFrameTime;			// Force, if possible, a constant frame time - specified by timePerFrame
	bool	showMsgBox;						// Show Message Boxes on errors
	bool	showStats;						// Do not display app's stats
	bool	parseCmdLine;					// Do not parse command line if app says not to
	bool	showCursorWhileFullscreen;		// If true, show the cursor while the window is fullscreen
	bool	useJoystickIfAvailable;			// If true, search for attached joysticks and enable them
	bool	mustBeUniqueInstance;			// Set to true if only one instance of the game is allowed to run at a time; checked asap	
	ULONG	diskSpaceNeededMB;				// Minimum space needed by the app on the hard disk
	ULONG	physicalRAMNeededMB;			// RAM space needed by the app (in MegaBytes)
	ULONG	virtualRAMNeededMB;				// V-RAM space needed by the app (in MegaBytes)
	ULONG	minimumCPUSpeedMHz;				// Minimum CPU's speed to run the app (in MegaHertz)		
}; // end structure AppSettings

#pragma region INTERFACES
class IAppWindow
{
public:
	explicit IAppWindow() = default;
	virtual ~IAppWindow() = default;

	virtual void VMoveResizeWindow(int xPos, int yPos, int width, int height) = 0;
	virtual void VToggleFullScreen() = 0;
	virtual void VPause(bool bPause, bool bPauseRendering) = 0;
	virtual void VShutDown(int nExitCode = 0) = 0;

	virtual void VLoadCursor(const WCHAR* const fileName, const WCHAR* const cursorName, bool setNow = true) = 0;
	virtual void VSelectCursor(const std::wstring& cursorName) const = 0;
	virtual void VCaptureMouse() const = 0;
	virtual void VReleaseMouseCapture() const = 0;
	virtual void VSetShowCursor(bool showCursor) const = 0;
	virtual void VSetClipCursor(bool clipCursor) const = 0;
	virtual void VSetAppIcon(const WCHAR* const fileName) = 0;
	virtual void VEnableShortcutKeys(bool bEnable) = 0;

	virtual Supported_OS VGetUnderlyingOS() const = 0;
private:
	IAppWindow(const IAppWindow& copy) = delete;
	IAppWindow& operator=(const IAppWindow& right) = delete;
}; // end interface IAppWindow declaration

class IApp
{
public:
	// Construction
	explicit IApp() = default;
	virtual ~IApp() = default;
	virtual bool VCreate(AppSettings& outInfo) = 0;
	virtual void VDestroy() = 0;

	// Main methods
	virtual void VOnInitialize(IAppWindow* pApp) = 0;
	virtual void VOnCreateWindow(UINT width, UINT height, bool fullscreen) = 0;
	virtual void VOnUpdate(milliseconds deltaMs) = 0;
	virtual void VOnRender(milliseconds deltaMs) = 0;
	virtual bool VIsPaused() const = 0;

	// Input handlers
	virtual void VOnSizeChange(int newWidth, int newHeight) = 0;
	virtual void VOnKeyDown(Keys uKey, bool prevState, bool bAltDown) = 0;
	virtual void VOnKeyUp(Keys uKey, bool prevState, bool bAltDown) = 0;
	virtual void VOnMouseInput(bool left, bool right, bool middle, bool x1, bool x2, int wheel, int xPos, int yPos) = 0;

	// Methods to carry out specific actions
	virtual void VOnTakeScreenShot() const = 0;
	virtual std::wstring VGetAppDataString() const = 0;
	virtual std::wstring VGetProgramTitle() const = 0;
	virtual std::wstring VGetDirectory(DIRECTORY dir) const = 0;

private:
	IApp(const IApp& copy) = delete;
	IApp& operator=(const IApp& right) = delete;
}; // end interface IApp declaration

class IRenderer
{
public:
	explicit IRenderer() = default;
	virtual ~IRenderer() = default;

	virtual bool VInitialize(int startWidth, int startHeight, bool fullScreen) = 0;
	virtual void VRender(milliseconds deltaMs) = 0;
	virtual void VChangeSize(int newWidth, int newHeight) = 0;
	virtual void VTakeScreenShot(const std::wstring& screenName) = 0;
	virtual bool VToggleFullScreenFromRenderer() = 0;
	virtual void VSetBackgroundColor(BYTE bgA, BYTE bgR, BYTE bgG, BYTE bgB) = 0;
	virtual void VHandleDeviceRemoved() = 0;
	virtual void VRelease() = 0;
private:
	IRenderer(const IRenderer& copy) = delete;
	IRenderer& operator=(const IRenderer& right) = delete;
}; // end interface IRenderer declaration

class IDrawable
{
public:
	virtual void VOnDraw() = 0;
}; // end interface IDrawable

typedef unsigned long EventType;
class IEvent
{
public:
	virtual ~IEvent() = default;
	virtual EventType VGetEventType() const = 0;
}; // end interface IEvent declaration
typedef std::shared_ptr<IEvent> IEventPtr;

class Resource;
class ResourceHandle;

class IResourceLoader
{
public:
	virtual std::wstring VGetPattern() const = 0;
	virtual bool VUseRawFile() const = 0;
	virtual bool VDiscardRawBufferAfterLoad() const = 0;
	virtual bool VAddNullZero() const = 0;
	virtual UINT64 VGetLoadedResourceSize(const char* rawBuffer, UINT64 rawSize) const = 0;
	virtual bool VLoadResource(const char* rawBuffer, UINT64 rawSize, const std::shared_ptr<ResourceHandle>& handle) const = 0;
}; // end interface IResourceLoader

class IResourceFile
{
public:
	virtual ~IResourceFile() = default;

	virtual bool VOpen() = 0;
	virtual UINT64 VGetRawResourceSize(const Resource& res) const = 0;
	virtual UINT64 VGetRawResource(const Resource& res, char* buffer) const = 0;
	virtual UINT64 VGetNumResources() const = 0;
	virtual std::wstring VGetResourceName(UINT64 num) const = 0;
	virtual bool VIsUsingDevelopmentDirectories() const = 0;
}; // end interface IResourceFile

class IScriptManager
{
public:
	virtual ~IScriptManager() = default;
	virtual bool VInit() = 0;
	virtual void VExecuteFile(const char* resource) = 0;
	virtual void VExecuteString(const char* str) = 0;
}; // end interface IScriptManager

#pragma endregion INTERFACES

class RandomNumberGenerator
{
public:
	__forceinline RandomNumberGenerator() : m_gen(m_rd())
	{} // end class RandomNumberGenerator constructor

	static __forceinline RandomNumberGenerator& GetGlobalRNG()
	{
		static RandomNumberGenerator global;
		return global;
	} // end static method GetGlobalRNG

	__forceinline void SetSeed(UINT s)
	{
		this->m_gen.seed(s);
	} // end method SetSeed

	// Default int range is [MIN_INT, MAX_INT].  Max value is included.
	__forceinline int32_t NextInt()
	{ 
		return std::uniform_int_distribution<int32_t>(0x80000000, 0x7FFFFFFF)(this->m_gen);
	} // end method NextInt

	__forceinline int32_t NextInt(int32_t MaxVal)
	{
		return std::uniform_int_distribution<int32_t>(0, MaxVal)(this->m_gen);
	} // end method NextInt

	__forceinline int32_t NextInt(int32_t MinVal, int32_t MaxVal)
	{
		return std::uniform_int_distribution<int32_t>(MinVal, MaxVal)(this->m_gen);
	} // end method NextInt

	// Default float range is [0.0f, 1.0f).  Max value is excluded.
	__forceinline double NextFloat(double MaxVal = 1.0f)
	{
		return std::uniform_real_distribution<double>(0.0f, MaxVal)(this->m_gen);
	} // end method NextFloat

	__forceinline double NextFloat(double MinVal, double MaxVal)
	{
		return std::uniform_real_distribution<double>(MinVal, MaxVal)(this->m_gen);
	} // end method NextFloat

private:
	std::random_device m_rd;
	std::minstd_rand m_gen;
}; // end class RandomNumberGenerator definition

#pragma region TEMPLATES
// This class is useful when a list of different closures 
// is required, because when creating a container all elements
// must have the same tipe, which in generally is not true when using
// templates like std::function; however, std::function or fastdelegate
// classes are a better solution to create closures, for they're much faster
class FunctionWrapper
{
public:
	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, FunctionWrapper>, int> = 0>
	constexpr explicit FunctionWrapper(_Fn&& func, _Args&&... args)
		: impl(new details_impl<_Fn, _Args...>(std::forward<_Fn>(func), std::forward<_Args>(args)...))
	{} // end class ThreadWorker constructor 

	explicit FunctionWrapper(const FunctionWrapper& copy)
		: impl(copy.impl)
	{} // end struct details_impl copy constructor 

	//explicit FunctionWrapper(FunctionWrapper&& move) noexcept
	//	: impl(std::move(move.impl))
	//{} // end class ThreadWorker move constructor 

	constexpr void operator()() const { return this->impl->vcall(); }

private:
	struct details_base
	{
		constexpr virtual void vcall() const = 0;
		virtual ~details_base() = default;
	}; // end structure details_base definition

	template <class _Fn, class... _Args>
	struct details_impl : details_base
	{
		using _Tuple = std::tuple<std::decay_t<_Args>...>;

		constexpr explicit details_impl(_Fn&& _Fx, _Args&&... _Ax)
			: m_Args(std::make_shared<_Tuple>(std::forward<_Args>(_Ax)...)),
			m_Fn(std::make_shared<_Fn>(std::forward<_Fn>(_Fx)))
		{} // end struct details_impl constructor 

		//constexpr details_impl(details_impl&& move)
		//	: m_Args(std::move(move.m_Args))
		//{} // end struct details_impl move constructor 

		constexpr explicit details_impl(const details_impl& copy)
			: m_Args(copy.m_Args), m_Fn(copy.m_Fn)
		{} // end struct details_impl copy constructor 

		constexpr virtual void vcall() const override
		{
			constexpr auto _Invoker_proc = FunctionWrapper::_Get_invoke<_Fn, _Tuple>(std::make_index_sequence<sizeof...(_Args)>{});
			_Invoker_proc(this->m_Fn.get(), this->m_Args.get());
		}; // end method vcall

	private:
		const std::shared_ptr<_Fn> m_Fn;
		const std::shared_ptr<_Tuple> m_Args;
	}; // end structure details_impl definition

	template <class _Fn, class _Tuple, size_t... _Indices>
	constexpr static void _Invoke(_Fn* _Fx, _Tuple* _FnVals) noexcept /* terminates */
	{
		const _Tuple& _Tup = *_FnVals;
		(*_Fx)(std::move(std::get<_Indices>(_Tup))...);
	} // end method _Invoke

	template <class _Fn, class _Tuple, size_t... _Indices>
	static constexpr auto _Get_invoke(std::index_sequence<_Indices...>) noexcept
	{
		return &FunctionWrapper::_Invoke<_Fn, _Tuple, _Indices...>;
	} // end method _Get_invoke

	const std::shared_ptr<details_base> impl;
}; // end class FunctionWrapper definition

template <class BaseClass, class IdType>
class ObjectFactory
{
public:
	template <class SubClass>
	bool Register(IdType id)
	{
		auto findIt = this->m_CreationFunctions.find(id);
		if (findIt == this->m_CreationFunctions.end())
		{
			// ObjectCreationFunction<BaseClass, SubClass> f;
			constexpr auto createFunc = [] ()-> BaseClass* { return new SubClass; };
			this->m_CreationFunctions[id] = createFunc;
			return true;
		} // end if

		return false;
	} // end method Register

	BaseClass* Create(IdType id) const
	{
		auto findIt = this->m_CreationFunctions.find(id);
		if (findIt != this->m_CreationFunctions.end())
		{
			ObjectCreationFunction pFunc = findIt->second;
			return pFunc();
		} // end if

		return nullptr;
	} // end method Create

private:
	typedef BaseClass* (*ObjectCreationFunction)();
	std::unordered_map<IdType, ObjectCreationFunction> m_CreationFunctions;
}; // end class template ObjectFactory definition
#pragma endregion TEMPLATES

#pragma region FREE_FUNCTIONS

void* StringHashName(const char* pIdentStr);
bool WildcardMatch(const wchar_t* pat, const wchar_t* str);
void Split(const std::string& str, std::vector<std::string>& vec, char delimiter);


__forceinline std::wstring ToWideString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
} // end function ToWideString

#pragma endregion FREE_FUNCTIONS
END_ENGINE_NAMESPACE