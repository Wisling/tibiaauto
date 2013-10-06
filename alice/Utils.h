/**
 * Utils - Simple utility functions for strings
 *
 * @author	Jonathan Roewen
 */
#ifndef UTILS_H
#define UTILS_H

#include <string>

using namespace std;

char *mystrdup(const char *str);
char *get_string(const string &str);
string trim(const string &);
string trim(const string &, const string &);
string toLower(const string &);
string toUpper(const string &);
string replace(const string &, char, char);
string remove(const string &, char);
long timerMillis();
string urlencode(const string &);
string urldecode(const string &);

#endif
