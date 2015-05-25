/**
 * Element - An XML Element
 *
 * @author	Jonathan Roewen
 */
#ifndef ELEMENT_H
#define ELEMENT_H

#include <map>
#include <string>
#include <list>

using namespace std;

#include "Pointer.h"

class Element;

typedef Pointer<Element> PElement;
typedef list<PElement> velement;
typedef list<PElement>::iterator velement_it;

class Element
{
public:
	Element(const string &);
	~Element();

	string getTagname() const;
	string getNamespace() const;
	string getText(bool resolve = true) const;
	string getAttribute(const string &) const;

	void setName(const string &);
	void setTagname(const string &);
	void setNamespace(const string &);
	void setText(const string &);
	void setAttribute(const string &, const string &);

	bool hasNamespace() const;
	bool hasAttributes() const;
	bool hasChildren() const;

	PElement getChild() const;
	PElement getChild(const string &) const;
	PElement getNextSibling() const;

	void addChild(PElement);

	void getChildren(velement *);
	void getChildren(const string &, velement *);

	map<string, string> getAttributes() const;
private:
	static string resolveEntities(const string &);

	string text;
	char *tname, *nspace;
	map<string, string> attributes;
	PElement child, next, last;
};

#endif // ifndef ELEMENT_H
