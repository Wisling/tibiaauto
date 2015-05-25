#include <stdafx.h>

/**
 * StringTokenizer - Tokenizes Strings
 *
 * Modified from Java StringTokenizer
 */
#include "StringTokenizer.h"

bool StringTokenizer::hasMoreTokens()
{
	skipDelimiters();
	return (currentPosition < maxPosition);
}

string StringTokenizer::nextToken()
{
	skipDelimiters();

	if (currentPosition >= maxPosition)
		return "";

	int start = currentPosition;
	while ((currentPosition < maxPosition) &&
	       (delim.find(str[currentPosition]) == string::npos))
	{
		currentPosition++;
	}
	if (retTok && (start == currentPosition) &&
	    (delim.find(str[currentPosition]) != string::npos))
		currentPosition++;
	return str.substr(start, currentPosition - start);
}

string StringTokenizer::nextToken(const string &d)
{
	delim = d;
	return nextToken();
}

int StringTokenizer::countTokens()
{
	int count   = 0;
	int currpos = currentPosition;

	while (currpos < maxPosition)
	{
		while (!retTok &&
		       (currpos < maxPosition) &&
		       (delim.find(str[currpos]) != string::npos))
		{
			currpos++;
		}
		if (currpos >= maxPosition)
			break;
		int start = currpos;
		while ((currpos < maxPosition) &&
		       (delim.find(str[currpos]) == string::npos))
		{
			currpos++;
		}
		if (retTok && (start == currpos) &&
		    (delim.find(str[currpos]) != string::npos))
			currpos++;
		count++;
	}
	return count;
}

void StringTokenizer::skipDelimiters()
{
	while (!retTok &&
	       (currentPosition < maxPosition) &&
	       (delim.find(str[currentPosition]) != string::npos))
	{
		currentPosition++;
	}
}
