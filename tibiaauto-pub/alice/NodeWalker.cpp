#include <stdafx.h>

#include "NodeWalker.h"

/**
 * NodeWalker
 *
 * Walks a Nodemaster tree, visiting every node. This should allow us
 * to remove nodes (or just templates), so that we can erase all nodes
 * and/or templates for a particular file when reloading the file,
 * which should then allow merging to be put back in, as merging is
 * quite important for the learning bot :-)
 *
 * Things we should be able to do:
 *
 * - Go through entire loaded AIML, and regenerate our AIML files,
 *   using naming format same as Wallace & Anna AIML Sets (ie: all
 *   patterns starting with A in A.aiml, starting with * in
 *   star.aiml). This is useful for rebuilding the AIML Set with the
 *   temporary (learnt) AIML incorporated into the set. Note that
 *   this should also merge multiple categories into a single
 *   category, using <random> so that it will work with other
 *   interpreters (how dare you!)
 * - Remove all templates that were loaded from a given file
 *
 * @author	Jonathan Roewen
 */

#include "AimlWriter.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
#include <cstring>
#include <cctype>

using namespace std;
typedef pair<char *, Nodemaster *> vecPair;

typedef vector<vecPair> walkingVec;

//
//	Nodemaster children:
//
//	map<char *, Nodemaster *> children
//

//
//	Iterating through a map linearly...
//
//	map<string, string> attr = child->getAttributes();
//	map<string, string>::const_iterator itr = attr.begin();
//	while (itr != attr.end()) {
//		tag += " " + (*itr).first + "=\"" + (*itr).second + "\"";
//		itr++;
//	}
//

struct lessKey : public binary_function<vecPair, vecPair, bool> {
	bool operator() (vecPair lhs, vecPair rhs) {
		return strcmp(lhs.first, rhs.first) < 0;
	}
};

walkingVec getChildren(childrenMap &);

walkingVec getChildren(childrenMap &children) {
	childrenMap::const_iterator itr = children.begin();
	walkingVec vec;
	while (itr != children.end())
	{
		vec.push_back(vecPair((*itr).first, (*itr).second));
		itr++;
	}
	return vec;
}

/**
 * Dumps entire loaded AIML (including temporary) into the \newaiml
 * folder (must previously exist), overwriting any existing files,
 * using naming scheme taken from Wallace & Anna Sets.
 */
/*
   void NodeWalker::dumpAiml(Nodemaster *root) {
        //	This oughta be quite memory intensive...

        //	First get all immediate children, and sort them, then
        //	create output for each letter, digit, and wildcards, one
        //	at a time...

        //	swap uses copy-constructor... (T tmp(a); a = b; b = tmp;)
        //	iter_swap doesn't
        //	sort(RandomAccessIterator f, RandomAccessIterator l);
        //	stable_sort(RandomAccessIterator f, RandomAccessIterator l);
        //	all sorts take optional argument, Compare comp...

        //	define the Compare function though!

        //	Compare has to be defined in the Nodemaster class...

        //	No, not a sorted Nodemaster .. we're just sorting on the
        //	char * values... What is needed is a map/list sorted
        //	according to the key... how to do that? (I need the SGI
        //	site docs!)

        //	SGI SITE:

   //	struct less_mag : public binary_function<double, double, bool> {
   //	bool operator()(double x, double y) { return fabs(x) < fabs(y); }
   //    };
   //
   //    vector<double> V;
   //    ...
   //    sort(V.begin(), V.end(), less_mag());
   //	static int depth = 0;
   //	static int maxDepth = depth;
   //	static map<int, int> depths;
        walkingVec firstChildren = getChildren(root->children);
        //	Doesn't work .. I guess can't sort a map...
        sort (firstChildren.begin(), firstChildren.end(), lessKey());

        vector<PElement> documents;

   //	depths[depth] += firstChildren.size();
        for (int ix = 0; ix < firstChildren.size(); ++ix) {
                Nodemaster *node = firstChildren[ix].second;

   //		for (int z = 0; z < depth; ++z) {
   //			cout << " ";
   //		}
   //		cout << firstChildren[ix].first << endl;

   //		++depth;
   //		if (depth > maxDepth) {
   //			maxDepth = depth;
   //			cout << "Maximum Depth of Graphmaster is now: " << maxDepth << endl;
   //		}
                dumpAiml(node);
   //		--depth;
        }
   //	if (depth == 0) {
   //		for (int p = 0; p < maxDepth; ++p) {
   //			cout << "DEPTH: " << p << "\tCHILDREN: " << depths[p] << endl;
   //		}
   //	}
   //	cout << endl;
   }*/

void NodeWalker::dumpAiml(Nodemaster *root) {
	walkingVec fChildren = getChildren(root->children);
	sort(fChildren.begin(), fChildren.end(), lessKey());

	vector<PElement> documents;     //	star, under, A-Z, 0-9
	for (int iy = 0; iy < 38; ++iy)
	{
		PElement docRoot(new Element("aiml"));
		documents.push_back(docRoot);
	}

	cout << "Created document roots" << endl;

	for (size_t ix = 0; ix < fChildren.size(); ++ix)
	{
		//	Can NEVER have a <template> here :)
		//	non-const reference initialized to temporary...
		string fWord = toUpper(fChildren[ix].first);
//		cout << ":" << fWord << ":" << endl;
		char ch = fWord[0];
		if (ch == '*')          //	Then star
		{//			cout << "Star index: " << 0 << endl;
			dumpAiml(fChildren[ix].second, fWord, documents[0]);
		}
		else if (ch == '_')     //	Then under
		{//			cout << "Under index: " << 1 << endl;
			dumpAiml(fChildren[ix].second, fWord, documents[1]);
		}
		else
		{
			if (isalpha(ch))
			{
				//	Then A-Z
//				cout << "Alpha index: " << (ch - 'A' + 2) << endl;
				dumpAiml(fChildren[ix].second, fWord,
				         documents[ch - 'A' + 2]
				         );
			}
			else
			{
				//	Otherwise 0-9
//				cout << "Int index: " << (ch - '0' + 2) << endl;
				dumpAiml(fChildren[ix].second, fWord,
				         documents[ch - '0' + 28]
				         );
			}
		}
	}

	cout << "Parsed Nodemaster tree" << endl;

	for (int io = 0; io < 38; ++io)
	{
		//	cout << AimlWriter::prettyAiml(documents[io]) << endl;
	}
}

void NodeWalker::dumpAiml(Nodemaster *root, const string &path, PElement doc) {
	walkingVec children = getChildren(root->children);
	sort(children.begin(), children.end(), lessKey());

	for (size_t ix = 0; ix < children.size(); ++ix)
	{
		Nodemaster *node = children[ix].second;

		if (node->hasTemplate())
		{
			//	This is where we want to add a <category>

			//	Right now, we have the path, containing the
			//	<pattern> text, <that> text, and <topic> text.
			//
			//	Also, the <template> is available via the node.
			//
			//	Need the root element to add this to...
			string pattern, that, topic, path2;
			path2 = path + " " + children[ix].first;

			pattern = trim(path2.substr(0, path2.find(" <THAT> ")));
			path2.erase(0, path2.find(" <THAT> ") + 8);
			that = trim(path2.substr(0, path2.find(" <TOPIC> ")));
			path2.erase(0, path2.find(" <TOPIC> ") + 9);
			topic = trim(path2);

			PElement cat(new Element("category"));
			PElement pat(new Element("pattern"));
			PElement text;
			text = new Element("#text");
			text->setText(pattern);
			pat->addChild(text);
			cat->addChild(pat);
			if (that != "*")
			{
				PElement tht(new Element("that"));
				text = new Element("#text");
				text->setText(that);
				tht->addChild(text);
				cat->addChild(tht);
			}
			//	Do template now...
			string templ = node->getTemplate();
			PElement tmp(new Element("template"));
			text = new Element("#text");
			text->setText(templ);
			tmp->addChild(text);
			cat->addChild(tmp);
			if (topic != "*")
			{
				PElement tpc(new Element("topic"));
				tpc->setAttribute("name", topic);
				tpc->addChild(cat);
				//	Add topic
				doc->addChild(tpc);
			}
			else
			{
				//	Add category
				doc->addChild(cat);
			}
		}

		dumpAiml(node, path + " " + children[ix].first, doc);
	}
}
