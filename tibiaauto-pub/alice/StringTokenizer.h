/**
 * StringTokenizer - Tokenizes Strings
 *
 * Modified from Java StringTokenizer
 */
#ifndef STRING_TOKENIZER_H
#define STRING_TOKENIZER_H

#include <string>

using namespace std;

class StringTokenizer
{
public:
	StringTokenizer(const string &s, const string &d = " \t\n\r", bool rt = false) :
		currentPosition(0), str(s), delim(d), retTok(rt) {
		maxPosition = str.length();
	}

	bool hasMoreTokens();
	string nextToken();
	string nextToken(const string &d);
	int countTokens();
	void reset() {
		currentPosition = 0;
	}
	string tail() {
		skipDelimiters();
		int cp = currentPosition;
		currentPosition = maxPosition;
		return str.substr(cp);
	}
private:
	int currentPosition;
	string str;
	string delim;
	bool retTok;
	int maxPosition;

	void skipDelimiters();
};

#endif // ifndef STRING_TOKENIZER_H
