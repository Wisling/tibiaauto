#include <stdafx.h>

/**
 * TemplateProcessor - The big dude on campus
 *
 * @author	Jonathan Roewen
 */
#include "TemplateProcessor.h"
#include "Responder.h"
#include "Handler.h"
#include "StringTokenizer.h"
#include <iostream>

using namespace std;

AimlProcessor *TemplateProcessor::hook = new TemplateProcessor();

TemplateProcessor::~TemplateProcessor() {
	map<string, AimlProcessor *>::iterator itr = Handler::processors.begin();
	while (itr != Handler::processors.end()) {
		delete (*itr).second;
		*itr++;
	}
}

string TemplateProcessor::process(Match *m, PElement e, Responder *r, const string &id) {
	string buffer = "";
	if (e == NULL) {
		return buffer;
	}
	try {
		PElement child = e->getChild();
		while (child != NULL) {
			if (child->getTagname() == "#text") {
				buffer += child->getText();
			} else if (child->getTagname() == "#cdata") {
				buffer += child->getText(false);
			} else {
				if (child->hasNamespace()) {
					if (r == NULL) {
						;
					} else {
						buffer += r->respond(m, child, id);
					}
				} else {
					child = checkCompatibility(child);

					AimlProcessor *p = TemplateProcessor::getProcessor(child->getTagname());
					if (p == NULL) {
						//	Then nothing to process this tag with
						buffer += process(m, child, r, id);
					} else {
						buffer += p->process(m, child, r, id);
					}
				}
			}
			child = child->getNextSibling();
		}
	} catch (int &ex) {
		ex = ex; //force referencing
	}
	return buffer;
}

AimlProcessor *TemplateProcessor::getProcessor(const string &name) {
	if(Handler::hasProcessor(name)) {
		return (*Handler::processors.find(name)).second;
	} else {
		return NULL;
	}
}

string TemplateProcessor::processTemplate(Match *m, PElement e, Responder *r, const string &id) {
	string temp = hook->process(m, e, r, id);
	return temp;
}

PElement TemplateProcessor::checkCompatibility(PElement e) {
	string elementName = e->getTagname();
	if (elementName.find("_") != string::npos) {
		StringTokenizer st(elementName, "_");
		string action = st.nextToken();
		string property = st.nextToken();
		e->setTagname(action);
		e->setAttribute("name", property);
	}
	return e;
}
	
