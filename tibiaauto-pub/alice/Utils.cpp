#include <stdafx.h>

/**
 * Utils - blah blah blah
 *
 * @author	Jonathan Roewen
 */
#include "Utils.h"

#include <set>

using namespace std;

#include <ctype.h>
#include <time.h>	//	Assume works on all platforms .. can fix later


#if __GNUC__ == 3
#define FANCY_COMPILER

#include <ext/hash_set>
using namespace __gnu_cxx;
#endif

typedef set<string> str_set;
static str_set ss;

#ifdef FANCY_COMPILER
//see http://www.sgi.com/tech/stl/hash_set.html
struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

typedef hash_set<const char*, hash<const char*>, eqstr> cstr_hashset;

static cstr_hashset sh;
#endif

//should probly get rid of this function
char *mystrdup(const char *str){
/*#ifdef FANCY_COMPILER
	return strdup(str);
#else*/
	if(!str)
		return NULL;
	char *ret = new char[strlen(str)+1];
	strcpy(ret,str);
	ret[strlen(str)] = '\0';
	return ret;
//#endif
}


char *get_string(const string &str){
#ifdef FANCY_COMPILER
	cstr_hashset::const_iterator itr = sh.find(str.c_str());
	if(itr == sh.end())
	{
		char *c = mystrdup(str.c_str());
		sh.insert(c);
		return c;
	}
	return (char*)*itr;
#else
	str_set::const_iterator itr = ss.insert(ss.begin(), str);
	return (char *) (*itr).c_str();
#endif
}

string trim(const string &text) {
	return trim(text, " \t\n\r");
}

string trim(const string &text, const string &remove) {
	string result = text;
	string::size_type index = result.find_first_not_of(remove);
	if (index > 0)
		result = result.erase(0, index);
	index = result.find_last_not_of(remove);
	if (index != string::npos)
		result = result.erase(index + 1);
	return result;
}

string toLower(const string &text) {
	string result = text;
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		result[ix] = (char)tolower(text[ix]);
	}
	return result;
}

string toUpper(const string &text) {
	string result = text;
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		result[ix] = (char)toupper(text[ix]);
	}
	return result;
}

string replace(const string &text, char a, char b) {
	string result = "";
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		if (text[ix] == a) {
			result += b;
		} else {
			result += text[ix];
		}
	}
	return result;
}

string remove(const string &text, char r) {
	string result = "";
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		if (text[ix] != r) {
			result += text[ix];
		}
	}
	return result;
}

/*
	Maybe it's Win32 specific, not sure yet...
	Windows: result in milliseconds
	Linux: result in microseconds
	Unsure about other platforms
*/
long timerMillis() {
	clock_t time = clock();
#if !defined(WIN32)
	return (long)time / 1000;
#else
	return (long)time;
#endif
}

string intToHex(int);
int hexToInt(char);

string intToHex(int i) {
	string hex = "";
	if (i == 0) {
		return "0";
	}
	while (i != 0) {
		int j = i % 16;
		switch (j) {
			case 15: hex = 'F' + hex; break;
			case 14: hex = 'E' + hex; break;
			case 13: hex = 'D' + hex; break;
			case 12: hex = 'C' + hex; break;
			case 11: hex = 'B' + hex; break;
			case 10: hex = 'A' + hex; break;
			default: hex = ((char)(j + '0')) + hex;
		}
		i /= 16;
	}
	return hex;
}

int hexToInt(char ch) {
	ch = (char)tolower(ch);
	switch (ch) {
		case 'f': return 15;
		case 'e': return 14;
		case 'd': return 13;
		case 'c': return 12;
		case 'b': return 11;
		case 'a': return 10;
		default: return (int)(ch - '0');
	}
}

string urlencode(const string &in) {
	string e;
	char ch;
	for (unsigned int ix = 0; ix < in.length(); ++ix) {
		ch = in[ix];
		if (ch == ' ') {
			e += '+';
		}
		else if (isalpha(ch)) {
			e += ch;
		}
		else {
			e += "%";
			string h = intToHex((int)ch);
			if (h.length() < 2) {
				e += "0";
			}
			e += h;
		}
	}
	return e;
}

string urldecode(const string &in) {
	string d;
	int code = 0;
	char ch;
	for (unsigned int ix = 0; ix < in.length(); ++ix) {
		ch = in[ix];
		switch (ch) {
			case '+':
				d += ' ';
				break;
			case '%':
				code = hexToInt(in[++ix]);
				code *= 16;
				code += hexToInt(in[++ix]);
				d += (char)code;
				break;
			default:
				d += ch;
				break;
		}
	}
	return d;
}
