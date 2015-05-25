/**
 * ThinkProcessor - Processes AIML without displaying results
 *
 * @author	Jonathan Roewen
 */
#ifndef THINK_PROCESSOR_H
#define THINK_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class ThinkProcessor : public AimlProcessor
{
public:
	~ThinkProcessor()
	{
	}

	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		Kernel::process(m, e, r, id);
		return "";
	}
};

#endif // ifndef THINK_PROCESSOR_H
