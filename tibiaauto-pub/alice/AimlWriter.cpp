#include <stdafx.h>

#include "AimlWriter.h"

#include <map>
#include <iostream>

using namespace std;

string AimlWriter::parse(PElement tree, bool pretty, const string &rootName, bool addRoot)
{
	if (rootName.empty())
		addRoot = false;
	string result = "";
	if (addRoot)
		result += "<" + rootName + ">";
	if (pretty)
		result += prettyAiml(tree);
	else
		result += recurse(tree);
	if (addRoot)
		result += "</" + rootName + ">";
	return result;
}

//	Produces a formatted string of the AIML
string AimlWriter::prettyAiml(PElement element)
{
	if (element == NULL)
		return "";
	string s = "<aiml>\n\n";
	if (element->getTagname() != "aiml")
	{
		cerr << "Error: Document root is not 'aiml'" << endl;
		return "";
	}
	velement array;
	velement_it ix;
	//	Process <topic> first
	element->getChildren("topic", &array);
	//	All the <topic> nodes...
	for (ix = array.begin(); ix != array.end(); ++ix)
	{
		velement categories;
		(*ix)->getChildren("category", &categories);
		//	All of the <category> nodes per <topic>
		string topic = (*ix)->getAttribute("name");
		s += "<topic name=\"" + topic + "\">\n\n";
		for (velement_it iy = categories.begin(); iy != categories.end(); ++iy)
		{
			//	Get the <that> <pattern> and <template>
			string Pattern  = recurse((*iy)->getChild("pattern"));
			string That     = recurse((*iy)->getChild("that"));
			string Template = recurse((*iy)->getChild("template"));

			s += "<category>\n";
			if (!That.empty())
				s += "<that>" + That + "</that>\n";
			s += "<pattern>" + Pattern + "</pattern>\n";
			s += "<template>\n" + Template + "\n</template>\n";
			s += "</category>\n\n";
		}
		s += "</topic>\n\n";
	}
	array.clear();

	element->getChildren("category", &array);
	//	All the <category> nodes without a <topic>
	for (ix = array.begin(); ix != array.end(); ++ix)
	{
		//	Get the <that> <pattern> and <template>
		string Pattern  = recurse((*ix)->getChild("pattern"));
		string That     = recurse((*ix)->getChild("that"));
		string Template = recurse((*ix)->getChild("template"));

		s += "<category>\n";
		if (!That.empty())
			s += "<that>" + That + "</that>\n";
		s += "<pattern>" + Pattern + "</pattern>\n";
		s += "<template>\n" + Template + "\n</template>\n";
		s += "</category>\n\n";
	}

	s += "</aiml>\n";

	return s;
}

string AimlWriter::recurse(PElement element)
{
	if (element == NULL)
		return "";
	string s       = "";
	PElement child = element->getChild();
	while (child != NULL)
	{
		if (child->getTagname() == "#text")
		{
			s += child->getText(false);
		}
		else if (child->getTagname() == "#cdata")
		{
			s += child->getText(false);
		}
		else
		{
			string tag = "<";
			if (child->hasNamespace())
				tag += child->getNamespace() + ":";
			tag += child->getTagname();
			if (child->hasAttributes())
			{
				map<string, string> attr                = child->getAttributes();
				map<string, string>::const_iterator itr = attr.begin();
				while (itr != attr.end())
				{
					tag += " " + (*itr).first + "=\"" + (*itr).second + "\"";
					itr++;
				}
			}
			if (child->hasChildren())
			{
				tag += ">" + recurse(child) + "</";
				if (child->hasNamespace())
					tag += child->getNamespace() + ":";
				tag += child->getTagname() + ">";
			}
			else
			{
				tag += "/>";
			}
			s += tag;
		}
		child = child->getNextSibling();
	}
	return s;
}
