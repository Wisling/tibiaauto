/**
 * SraiProcessor - Symbolic Reduction!
 *
 * @author	Jonathan Roewen
 */
#ifndef SRAI_PROCESSOR_H
#define SRAI_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>
#include <iostream>

using namespace std;

class SraiProcessor : public AimlProcessor
{
public:
	~SraiProcessor()
	{
	}

	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		string srai;
		if (e->hasChildren())
			srai = Kernel::process(m, e, r, id), id;
		else
			srai = e->getText();
		//	Need to lowercase the <srai> input as getting some
		//	funky output (uppercase text in <star/> etc).
		return Kernel::respond(toLower(srai), id, r, true);
	}
};

#endif // ifndef SRAI_PROCESSOR_H
