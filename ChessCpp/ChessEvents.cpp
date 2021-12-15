// Milan, 12th December 2021
//	Completed on December, 12
//

#include "ChessEvents.h"
using namespace ENGINE_NAMESPACE;

const EventType Event_RightButtonDown::sk_EventType	(0xa3814ac1);
const EventType Event_LeftButtonDown::sk_EventType	(0xa3814ac2);
const EventType Event_SelectSquare::sk_EventType	(0xa3814ac3);
const EventType Event_SelectionReset::sk_EventType	(0xa3814ac4);
const EventType Event_PieceSelected::sk_EventType	(0xa3814ac5);
const EventType Event_IllegalMove::sk_EventType		(0xa3814ac6);
const EventType Event_StartMovePiece::sk_EventType	(0xa3814ac7);
const EventType Event_Castle::sk_EventType			(0xa3814ac8);
const EventType Event_EndMovePiece::sk_EventType	(0xa3814ac9);
const EventType Event_Check::sk_EventType			(0xa3814aca);
const EventType Event_EndTurn::sk_EventType			(0xa3814acb);
const EventType Event_PieceEaten::sk_EventType		(0xa3814acc);
const EventType Event_PawnPromotion::sk_EventType	(0xa3814acd);
const EventType Event_EndMatch::sk_EventType		(0xa3814ace);