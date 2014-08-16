/**
 * BotProcessor - Retrieves patterns-side bot property
 *
 * @author	Jonathan Roewen
 */
#ifndef BOT_PROCESSOR_H
#define BOT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

extern string botName;

class BotProcessor : public AimlProcessor
{
public:
	~BotProcessor() { }
	
	string process(Match *, PElement e, Responder *, const string &id) {
		string property = e->getAttribute("name");
		if (property == "name") {
			return botName;
		}
		return Kernel::respond("BOT " + property, id);
	}
};

#endif
