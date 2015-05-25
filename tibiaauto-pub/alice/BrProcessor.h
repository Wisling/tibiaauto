/**
 * BrProcessor - Breaks output into multiple lines
 *
 * @author	Jonathan Roewen
 */
#ifndef BR_PROCESSOR_H
#define BR_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class BrProcessor : public AimlProcessor
{
public:
	~BrProcessor() {
	}

	string process(Match *, PElement, Responder *, const string &) {
		return "\n";
	}
};

#endif // ifndef BR_PROCESSOR_H
