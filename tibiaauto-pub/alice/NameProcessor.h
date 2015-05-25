/**
 * NameProcessor - Retrieves the bot's name (shortcut)
 *
 * @author	Jonathan Roewen
 */
#ifndef NAME_PROCESSOR_H
#define NAME_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

class NameProcessor : public AimlProcessor
{
public:
	~NameProcessor() {
	}

	string process(Match *, PElement, Responder *, const string &id) {
		return Kernel::respond("BOT NAME", id);
	}
};

#endif // ifndef NAME_PROCESSOR_H
