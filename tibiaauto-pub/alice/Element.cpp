#include <stdafx.h>

/**
 * Element - An XML Element
 *
 * @author	Jonathan Roewen
 */
#include "Element.h"
#include "Utils.h"

#include <iostream>
#include <string.h>

using namespace std;

Element::Element(const string &name = "") : text(""), tname(""), nspace("")
{
	if (!name.empty())
		setName(name);
	child = next = last = NULL;
}

Element::~Element()
{
}

string Element::getTagname() const
{
	return tname;
}

string Element::getNamespace() const
{
	return nspace;
}

string Element::getText(bool resolve) const
{
	if (!resolve)
		return text;
	else
		return resolveEntities(text);
}

//	add bool
string Element::getAttribute(const string &key) const
{
	map<string, string>::const_iterator itr = attributes.find(key);
	if (itr == attributes.end())
		return "";
	return resolveEntities((*itr).second);
}

void Element::setName(const string &str)
{
	string::size_type ix = str.find(':');
	if (ix != string::npos)
	{
		setNamespace(str.substr(0, ix));
		setTagname(str.substr(ix + 1));
	}
	else
	{
		setTagname(str);
		setNamespace("");
	}
}

void Element::setTagname(const string &str)
{
	tname = get_string(str);
}

void Element::setNamespace(const string &str)
{
	nspace = get_string(str);
}

void Element::setText(const string &str)
{
	text = str;
}

void Element::setAttribute(const string &key, const string &value)
{
	attributes[key] = value;
}

bool Element::hasNamespace() const
{
	return strlen(nspace) != 0;
}

bool Element::hasAttributes() const
{
	return !attributes.empty();
}

bool Element::hasChildren() const
{
	return !(child == NULL);
}

PElement Element::getChild() const
{
	return child;
}

PElement Element::getChild(const string &tagname) const
{
	PElement ch = child;
	while (ch != NULL)
	{
		if (ch->tname == tagname)
			return ch;
		ch = ch->next;
	}
	return PElement(NULL);
}

PElement Element::getNextSibling() const
{
	return next;
}

void Element::addChild(PElement element)
{
	if (child == NULL)
	{
		child = element;
		last  = child;
	}
	else
	{
		last->next = element;
		last       = last->next;
	}
}

void Element::getChildren(velement *ve)
{
	PElement ch = child;
	while (ch != NULL)
	{
		ve->push_back(ch);
	}
}

void Element::getChildren(const string &tagname, velement *ve)
{
	PElement ch = child;
	while (ch != NULL)
	{
		if (ch->tname == tagname)
			ve->push_back(ch);
		ch = ch->next;
	}
}

map<string, string> Element::getAttributes() const
{
	return attributes;
}

string Element::resolveEntities(const string &input)
{
	string output = input;
	int ix        = output.find("&");
	int iy        = output.find(";", ix);

	while (ix != string::npos && iy != string::npos)
	{
		string head = output.substr(0, ix);
		string tail = output.substr(iy + 1);
		string mid  = output.substr(ix, iy + 1 - ix);
		if (mid == "&lt;")
		{
			output = head + "<" + tail;
			iy    -= 3;
		}
		else if (mid == "&gt;")
		{
			output = head + ">" + tail;
			iy    -= 3;
		}
		else if (mid == "&amp;")
		{
			output = head + "&" + tail;
			iy    -= 4;
		}
		else if (mid == "&quot;")
		{
			output = head + "\"" + tail;
			iy    -= 5;
		}
		else if (mid == "&apos;")
		{
			output = head + "'" + tail;
			iy    -= 5;
		}
		else
		{
			iy = ix + 1;
		}
		ix = output.find("&", iy);
		iy = output.find(";", ix);
	}
	return output;
}
