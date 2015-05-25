/**
 * StarProcessor - Retrieves value of the wildcard in the
 *		input pattern
 *
 * @author	Jonathan Roewen
 */
#ifndef STAR_PROCESSOR_H
#define STAR_PROCESSOR_H

#include "AimlProcessor.h"

#include <string>

using namespace std;

#include "Utils.h"

class StarProcessor : public AimlProcessor
{
public:
	~StarProcessor()
	{
	}

	string process(Match *m, PElement e, Responder *, const string &)
	{
		string index = e->getAttribute("index");
		if (index.empty())
			index = "1";
		return m->getInputStar(atoi(index.c_str()));
	}
};

#endif // ifndef STAR_PROCESSOR_H
