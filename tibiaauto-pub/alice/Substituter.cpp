#include <stdafx.h>

/**
 * Substituter - This is fucked
 *
 * @author	Jonathan Roewen
 */
#include "Substituter.h"

#include "Utils.h"
#include "StringTokenizer.h"

#include <vector>
#include <iostream>

using namespace std;

/*

        An Alternative Method

        - Store substitutions in a tree structure, similar
          to the Nodemaster.

        - Look for the longest substitution at the current
          index. If no substitution, move to next character.

        - Once found a substitution, make the replacement,
          then move to the next character. This will prevent
          possible infinite loops.

        Benefits:

        This should result in faster substituting, esp with
        a large number of substitutions, as we will lessen
        the need for string comparisons, using character
        comparisons most of the time, which is faster, and
        we will no longer be looking through a list of
        substitutions that won't apply.

 */

map<string, SubTree> Substituter::substituters;

Substituter::Substituter()
{
	//	Init the output substitutions (internal entitities)
	addSubstitute("output", "&lt;", "<");
	addSubstitute("output", "&gt;", ">");
	addSubstitute("output", "&amp;", "&");
	addSubstitute("output", "&apos;", "'");
	addSubstitute("output", "&quot;", "\"");
}

//	It could possibly still be slightly more efficient
//	However, it does work :) wiki
string Substituter::replace(SubTree tree, const string &input)
{
//	cerr << "start: '" << input << "'" << endl;
	string result = input;
	char ch;
	Node *n;
	int len = result.length();
	for (int ix = 0; ix < len; ++ix)
	{
		n = tree.root;
		int iy = ix;
		for (; iy < len; ++iy)
		{
			ch = toupper(result[iy]);
			if (n->getChild(ch) != NULL)
				n = n->getChild(ch);
			else
				break;
		}
		if (n != tree.root && !n->replace.empty())
		{
			string head = result.substr(0, ix);
			string tail = result.substr(iy);
//			cerr << "before: '" << result << "'" << endl;
			result = head + toLower(n->replace) + tail;
//			cerr << "after: '" << result << "'" << endl;
			len = result.length();
		}
	}
//	cerr << "final: '" << result << "'" << endl;
	return result;
}

string Substituter::substitute(const string &input, const string &subs)
{
	string result = Substituter::replace(getSubstituter(subs), " " + input + " ").substr(1);
	if (!result.empty())
	{
		StringTokenizer st(result, " ");
		string str = st.nextToken();
		while (st.hasMoreTokens())
		{
			str += " " + trim(st.nextToken(), " ");
		}
		return str;
	}
	else
	{
		return "";
	}
}

SubTree Substituter::getSubstituter(const string &name)
{
	if (substituters.find(name) == substituters.end())
		substituters[name] = SubTree();
	return (*substituters.find(name)).second;
}

void Substituter::addSubstitute(const string &name, const string &find, const string &replace)
{
	SubTree tree = getSubstituter(name);
	tree.addChild(find, replace);
	substituters[name] = tree;
}
