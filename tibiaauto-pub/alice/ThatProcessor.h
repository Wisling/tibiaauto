/**
 * ThatProcessor - Retrieves bot's last response or something
 *
 * @author	Jonathan Roewen
 */
#ifndef THAT_PROCESSOR_H
#define THAT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class ThatProcessor : public AimlProcessor
{
public:
	~ThatProcessor() { }
	
	string process(Match *, PElement e, Responder *, const string &id) {
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		int i = atoi(index.c_str());
		return Kernel::predicates->getHistory("that", id, i);
	}
};

#endif
