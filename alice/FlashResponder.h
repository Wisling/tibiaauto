/**
 * Flash Responder - Communicates via Sockets using XML :-)
 *
 * @author	Jonathan Roewen
 */
#ifndef FLASH_RESPONDER_H
#define FLASH_RESPONDER_H

#include "Responder.h"

class FlashResponder : public Responder
{
public:
	FlashResponder();
	
	string respond(Match *, PElement, const string &);
};

#endif	//	FLASH_RESPONDER_H
