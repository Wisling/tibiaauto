/**
 * NormalizeProcessor - Normalizes output
 *
 * @author	Jonathan Roewen
 */
#ifndef NORMALIZE_PROCESSOR_H
#define NORMALIZE_PROCESSOR_H

#include "AimlProcessor.h"
#include "Substituter.h"
#include "Kernel.h"

#include <string>

using namespace std;

class NormalizeProcessor : public AimlProcessor
{
public:
	~NormalizeProcessor() {
	}

	string process(Match *m, PElement e, Responder *r, const string &id) {
		return Substituter::substitute(Kernel::process(m, e, r, id));
	}
};

#endif // ifndef NORMALIZE_PROCESSOR_H
