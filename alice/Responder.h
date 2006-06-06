/**
 * Responder - ABC for all responders to implement
 *
 * @author	Jonathan Roewen
 */
#ifndef RESPONDER_H
#define RESPONDER_H

#include "Element.h"
#include "Match.h"

#include <string>

using namespace std;

extern bool serverRunning;

class Responder {
public:
	virtual ~Responder() { };
	
	/**
	 * Do we need the match element as well?
	 */
	virtual string respond(Match *, PElement, const string &) = 0;
};

#endif
