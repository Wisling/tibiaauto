/**
 * SetProcessor - Used to set value of a user predicate
 *
 * @author	Jonathan Roewen
 */
#ifndef SET_PROCESSOR_H
#define SET_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>

using namespace std;

class SetProcessor : public AimlProcessor
{
public:
	~SetProcessor() {
	}

	string process(Match *m, PElement e, Responder *r, const string &id) {
		string property = toLower(e->getAttribute("name"));
		bool global     = e->getAttribute("global") == "true";
		string value    = "";
		if (!e->hasChildren())
			value = m->getInputStar(1);
		else
			value = Kernel::process(m, e, r, id);
		if (global)
			Kernel::predicates->setValue(property, "*", value);
		else
			Kernel::predicates->setValue(property, id, value);
		return value;
	}
};

#endif // ifndef SET_PROCESSOR_H
