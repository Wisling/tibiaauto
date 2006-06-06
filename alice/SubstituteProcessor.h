/**
 * SubstituteProcessor - Used to apply custom substitutions
 *
 * @author	Jonathan Roewen
 */
#ifndef SUBSTITUTE_PROCESSOR_H
#define SUBSTITUTE_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>

using namespace std;

class SubstituteProcessor : public AimlProcessor
{
public:
	~SubstituteProcessor() { }
	
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string value = Kernel::process(m, e, r, id);
		string sub = e->getAttribute("sub");
		return Substituter::substitute(value, sub);
	}
};

#endif
