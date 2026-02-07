#pragma once
#include "EngineUtility.h"

BEGIN_ENGINE_NAMESPACE

namespace Events
{
	class Event_Request_New_Actor : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Request_New_Actor::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Request_New_Actor declaration

	class Event_Create_Actor : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Create_Actor::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Create_Actor declaration

	class Event_Move_Actor : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Move_Actor::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Move_Actor declaration

	class Event_Request_Destroy_Actor : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Request_Destroy_Actor::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Request_Destroy_Actor declaration

	class Event_Destroy_Actor : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Destroy_Actor::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Destroy_Actor declaration

	class Event_Network_Player_Actor_Assignment : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Network_Player_Actor_Assignment::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Network_Player_Actor_Assignment declaration

	class Event_New_Render_Component : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_New_Render_Component::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_New_Render_Component declaration

	class Event_Modified_Render_Component : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Modified_Render_Component::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Modified_Render_Component declaration

	class Event_Remote_Client : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Remote_Client::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Remote_Client declaration

	class Event_Request_Start_Game : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Request_Start_Game::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Request_Start_Game declaration

	class Event_Environment_Loaded : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Environment_Loaded::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Environment_Loaded declaration

	class Event_Remote_Environment_Loaded : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Remote_Environment_Loaded::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Remote_Enviroment_Loaded declaration

	class Event_Decompress_Request : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Decompress_Request::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Decompress_Request declaration

	class Event_Decompression_Progress : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Decompression_Progress::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Decompression_Progress declaration

	class Event_Play_Sound : public IEvent
	{
	public:
		virtual EventType VGetEventType() const override
		{ return Event_Play_Sound::sk_EventType; } // end method VGetEventType

		static const EventType sk_EventType;
	}; // end class Event_Play_Sound declaration
} // namespace Events

END_ENGINE_NAMESPACE