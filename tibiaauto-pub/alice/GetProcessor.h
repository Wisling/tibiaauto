/**
 * GetProcessor - Retrieves value of a user predicate
 *
 * @author	Jonathan Roewen
 */
#ifndef GET_PROCESSOR_H
#define GET_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>
#include <iostream>

using namespace std;

class GetProcessor : public AimlProcessor
{
public:
	~GetProcessor()
	{
	}

	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		string conjunction = e->getAttribute("conjunction");
		bool global        = e->getAttribute("global") == "true";
		if (conjunction.empty())
			conjunction = " and ";
		string result;
		if (global)
			result = Kernel::predicates->getValue(toLower(e->getAttribute("name")), "*");
		else
			result = Kernel::predicates->getValue(toLower(e->getAttribute("name")), id);
		if (result.empty())
			result = Kernel::process(m, e, r, id);
		return result;
	}
};

#endif // ifndef GET_PROCESSOR_H
