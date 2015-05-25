/**
 * ConditionProcessor - Conditional selection of choices
 *
 * @author	Jonathan Roewen
 */
#ifndef CONDITION_PROCESSOR_H
#define CONDITION_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>

using namespace std;

class ConditionProcessor : public AimlProcessor
{
public:
	~ConditionProcessor() {
	}

	string process(Match *m, PElement e, Responder *r, const string &id) {
		string cond = e->getAttribute("name");
		if (cond.empty())
			return "";
		string value = e->getAttribute("value");
		velement li;
		e->getChildren("li", &li);
		for (velement_it ix = li.begin(); ix != li.end(); ++ix)
		{
			string c = (*ix)->getAttribute("name");
			if (c.empty())
				c = cond;
			string v   = (*ix)->getAttribute("value");
			string ct  = (*ix)->getAttribute("contains");
			string bw  = (*ix)->getAttribute("beginswith");
			string ew  = (*ix)->getAttribute("endswith");
			string ex  = (*ix)->getAttribute("exists");
			string mem = Kernel::predicates->getValue(c, id);
			if (!v.empty())
				if (v == mem)
					return Kernel::process(m, *ix, r, id);
			if (!ct.empty())
				if (mem.find(ct) != string::npos)
					return Kernel::process(m, *ix, r, id);
			if (!bw.empty())
				if (mem.find(bw) == 0)
					return Kernel::process(m, *ix, r, id);
			if (!ew.empty())
				if (mem.length() >= ew.length() && mem.rfind(ew) == mem.length() - ew.length())
					return Kernel::process(m, *ix, r, id);
			if (!ex.empty())
			{
				if (ex == "false" && mem.length() == 0)
					return Kernel::process(m, *ix, r, id);
				if (ex == "true" && mem.length() > 0)
					return Kernel::process(m, *ix, r, id);
			}
			if (ct.empty() && v.empty() && bw.empty() && ew.empty() && ex.empty())
				return Kernel::process(m, *ix, r, id);
		}
		if (!cond.empty() && !value.empty())
		{
			string mem = Kernel::predicates->getValue(cond, id);
			if (value == mem)
				return Kernel::process(m, e, r, id);
		}
		return "";
	}
};

#endif // ifndef CONDITION_PROCESSOR_H
