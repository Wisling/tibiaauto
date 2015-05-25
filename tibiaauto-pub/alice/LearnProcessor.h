/**
 * LearnProcessor - Learns AIML files (for now)
 *
 * @author	Jonathan Roewen
 */
#ifndef LEARN_PROCESSOR_H
#define LEARN_PROCESSOR_H

#include "AimlProcessor.h"
#include "Substituter.h"
#include "Parser.h"
#include "Kernel.h"
#include "Nodemaster.h"
#include "Utils.h"

#include <string>
#include <fstream>

using namespace std;

extern int totalCategories;

class LearnProcessor : public AimlProcessor
{
public:
	LearnProcessor() {
		init = false;
	}
	~LearnProcessor() {
	}

	string process(Match *m, PElement e, Responder *r, const string &id) {
		PElement pattern = e->getChild("pattern");
		if (pattern == PElement())
		{
			//	<learn> a file
			string name = Kernel::process(m, e, r, id);
			if (Kernel::load(name))
				return "Loaded '" + name + "'";
			else
				return "Loading '" + name + "' failed";
		}
		else
		{
			openFile();

			if (!init)
				return "";
			//	<learn> new AIML defined in a <pattern>

			//	This needs fixing!
			//	Need to process template, do an output substitution
			//	to change &lt; etc into '<' etc
			//	then parse it...
			//	PElement temp = e->getChild("template");
			/******************************************/
			//	strstream ss;
			string templateText = Kernel::process(m, e->getChild("template"), r, id);
			//	cout << Substituter::output(str) << endl;
			//	ss << Substituter::output(str) << endl;
			//	Parser *p = new Parser();
			//	PElement temp = p->parse(ss, "template");
			/******************************************/
			//	Need to do something similar with <pattern>
			//	strstream s2;
			string contextText = Kernel::process(m, e->getChild("context"), r, id);
			string patternText = Kernel::process(m, e->getChild("pattern"), r, id);
			string thatText    = Kernel::process(m, e->getChild("that"), r, id);
			string topicText   = Kernel::process(m, e->getChild("topic"), r, id);
			if (contextText.empty())
				contextText = "*";
			if (thatText.empty())
				thatText = "*";
			if (topicText.empty())
				topicText = "*";
			//	s2 << str2 << endl;
			//	pattern = p->parse(s2, "template");
			//	PElement that = e->getChild("that");
			//	g-o-d's double learning won't work with this yet
			//	PElement category(new Element("category"));
			//	category->addChild(pattern);
			//	category->addChild(temp);
			//	if (that != PElement()) {
			//		category->addChild(that);
			//	}
			//	PElement root(new Element("aiml"));
			//	root->addChild(category);
			//	Kernel::load10("~temporary memory area~", root);
			//	Now just need to write to a file...

			////    NEW CODE HERE    ////

			templateText = removeNewlines(templateText);

			learnFile << contextText << "," << patternText << "," << thatText << "," << topicText << ",";
			streamsize startIndex = learnFile.tellg();
			learnFile << templateText;
			streamsize endIndex = learnFile.tellg();
			learnFile << endl;
			Nodemaster *node = Kernel::add(contextText, patternText, thatText, topicText);
			if (node != NULL)
			{
				++totalCategories;
				node->addTemplate(
				        new Template("temporary.data", startIndex, endIndex)
				        );
			}

			closeFile();
		}
		return "";
	}
private:
	string removeNewlines(const string &input) {
		string output = "";
		for (size_t ix = 0; ix < input.length(); ++ix)
		{
			if (input[ix] == '\n' || input[ix] == '\r')
				continue;
			output += input[ix];
		}
		return output;
	}

	void openFile() {
		learnFile.open("temporary.data", ios::out | ios::binary | ios::app);
		if (learnFile.is_open())
		{
			learnFile.seekp(0, ios::end);
			init = true;
		}
	}

	void closeFile() {
		if (init)
			learnFile.close();
		init = false;
	}

	fstream learnFile;
	bool init;
};

#endif // ifndef LEARN_PROCESSOR_H
