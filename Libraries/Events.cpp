#include "Events.h"
#include <guiddef.h>

BEGIN_ENGINE_NAMESPACE

namespace Events
{
	// Each event type must be unique: in order to achieve that,
	// create a GUID (globally Unique ID) for each type, then use
	// only the first number; that's should still be enough
	const EventType Event_Environment_Loaded::sk_EventType(0xa3814acd);
	const EventType Event_Remote_Environment_Loaded::sk_EventType(0x8E2AD6E6);
	const EventType Event_Create_Actor::sk_EventType(0xe86c7c31);
	const EventType Event_Move_Actor::sk_EventType(0xeeaa0a40);
	const EventType Event_Destroy_Actor::sk_EventType(0x77dd2b3a);
	const EventType Event_New_Render_Component::sk_EventType(0xaf4aff75);
	const EventType Event_Modified_Render_Component::sk_EventType(0x80fe9766);
	const EventType Event_Request_Start_Game::sk_EventType(0x11f2b19d);
	const EventType Event_Remote_Client::sk_EventType(0x301693d5);
	const EventType Event_Network_Player_Actor_Assignment::sk_EventType(0xa7c92f11);
	const EventType Event_Decompress_Request::sk_EventType(0xc128a129);
	const EventType Event_Decompression_Progress::sk_EventType(0x68de1f28);
	const EventType Event_Request_New_Actor::sk_EventType(0x40378c64);
	const EventType Event_Request_Destroy_Actor::sk_EventType(0xf5395770);
	const EventType Event_Play_Sound::sk_EventType(0x3d8118ee);

} // namespace Events
END_ENGINE_NAMESPACE