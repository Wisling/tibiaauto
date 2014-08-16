#include <stdafx.h>

#include "Parser.h"

PElement Parser::getRoot() const {
	return root->getChild();
}

void Parser::elementStarted(const string &name, int) {
	PElement element(new Element(name));
	current->addChild(element);
	current = element;
	recurser.push(current);
}

void Parser::elementStarted(const string &name, attributeMap *am, int) {
	PElement element(new Element(name));
	//	add attributes
	attributeMap::const_iterator itr = am->begin();
	while (itr != am->end()) {
		element->setAttribute((*itr).first, (*itr).second);
		itr++;
	}
	current->addChild(element);
	current = element;
	recurser.push(current);
}

void Parser::elementFinished(const string &, int) {
	if (current != NULL && recurser.size() > 1) {
		recurser.pop();
		current = recurser.top();
	}
}

void Parser::elementText(const string &text, int) {
	PElement element(new Element("#text"));
	element->setText(text);
	current->addChild(element);
}

void Parser::elementCData(const string &cdata, int) {
	PElement element(new Element("#cdata"));
	element->setText(cdata);
	current->addChild(element);
}
