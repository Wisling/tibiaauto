/**
 * Substituter - Performs substitutions :P
 *
 * @author	Jonathan Roewen
 */
#ifndef SUBSTITUTER_H
#define SUBSTITUTER_H

#include "Utils.h"

#include <map>
#include <string>
#include <cctype>

using namespace std;

class Node
{
public:
	Node() {
	}

	Node *getChild(char ch) {
		if (children.find(ch) == children.end())
			return NULL;
		return (*children.find(ch)).second;
	}

	map<char, Node*> children;
	string replace;
};

class SubTree
{
public:
	SubTree() {
		root = new Node();
	}

	void addChild(string find, string replace) {
		char ch;
		Node *n = root;
		for (size_t ix = 0; ix < find.length(); ++ix)
		{
			ch = (char)toupper(find[ix]);
			if (n->children.find(ch) == n->children.end())
				n->children[ch] = new Node();
			n = (*n->children.find(ch)).second;
		}
		n->replace = replace;
	}

	Node *root;
};

class Substituter
{
public:
	static map<string, SubTree> substituters;

	Substituter();

	static string replace(SubTree, const string &);
	static string substitute(const string &, const string &subs = "normal");
	static SubTree getSubstituter(const string &);
	static void addSubstitute(const string &, const string &, const string &);
};

#endif // ifndef SUBSTITUTER_H
