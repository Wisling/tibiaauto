/**
 * TopicstarProcessor - Retrieves nth * of <topic>
 *
 * @author	Jonathan Roewen
 */
#ifndef TOPICSTAR_PROCESSOR_H
#define TOPICSTAR_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class TopicstarProcessor : public AimlProcessor
{
public:
	~TopicstarProcessor() { }
	
	string process(Match *m, PElement e, Responder *, const string &) {
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		return m->getTopicStar(atoi(index.c_str()));
	}
};

#endif
