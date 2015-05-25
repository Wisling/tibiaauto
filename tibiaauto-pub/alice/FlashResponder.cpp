#include <stdafx.h>

/**
 * Flash Responder - Communicates via Sockets using XML :-)
 *
 * @author	Jonathan Roewen
 */
#include "FlashResponder.h"

FlashResponder::FlashResponder()
{
	//	Create a server socket and start accepting I suppose...
}

string FlashResponder::respond(Match *, PElement, const string &)
{
	//	Process Flash-only commands (<flash:xxx/>)
	return "";
}
