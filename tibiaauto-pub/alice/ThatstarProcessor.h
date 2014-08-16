/**
 * ThatstarProcessor - Gets the nth * of <that>
 *
 * @author	Jonathan Roewen
 */
#ifndef THATSTAR_PROCESSOR_H
#define THATSTAR_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class ThatstarProcessor : public AimlProcessor
{
public:
	~ThatstarProcessor() { }
	
	string process(Match *m, PElement e, Responder *, const string &) {
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		return m->getThatStar(atoi(index.c_str()));
	}
};

#endif
