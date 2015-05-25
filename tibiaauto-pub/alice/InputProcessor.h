/**
 * InputProcessor - Learns AIML files (for now)
 *
 * @author	Jonathan Roewen
 */
#ifndef INPUT_PROCESSOR_H
#define INPUT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>
#include <iostream>

using namespace std;

class InputProcessor : public AimlProcessor
{
public:
	~InputProcessor()
	{
	}

	string process(Match *, PElement e, Responder *, const string &id)
	{
		string index = e->getAttribute("index");
		if (index.empty())
			index = "1";
		int i = atoi(index.c_str());
		return Kernel::predicates->getHistory("input", id, i);
	}
};

#endif // ifndef INPUT_PROCESSOR_H
