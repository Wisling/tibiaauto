#include <stdafx.h>

#include "PredicateEngine.h"

#include "Utils.h"

#include <fstream>

using namespace std;

/*****************************************************

   typedef struct {
        int size;
        int top;
        vector<string> elements;
   } HISTORY;

   typedef struct {
        int number;
        int size;
        map<int, string> elements;
   } ARRAY;

   typedef map<pair<string, string>, HISTORY> historyMap;
   typedef map<pair<string, string>, ARRAY> uArrayMap;
   typedef map<string, ARRAY> gArrayMap;

 ******************************************************/

PredicateEngine::PredicateEngine()
{
	ifstream history("history.csv");
	if (history.is_open())
	{
		string property, id, value;
		while (!history.eof())
		{
			getline(history, property, ',');
			getline(history, id, ',');
			getline(history, value);
			if (!property.empty())
				addHistory(property, id, urldecode(value));
		}
		history.close();
	}
	ifstream predicates("predicates.csv");
	if (predicates.is_open())
	{
		string property, id, value, index;
		while (!predicates.eof())
		{
			getline(predicates, property, ',');
			getline(predicates, id, ',');
			getline(predicates, value, ',');
			getline(predicates, index);
			if (!property.empty())
				setValue(property, id, urldecode(value), atoi(index.c_str()));
		}
		predicates.close();
	}
}

PredicateEngine::~PredicateEngine()
{
	//	Saving of the history is incorrect, as when it reloads,
	//	the order will not be the same
	ofstream hout("history.csv", ios::trunc);
	if (hout.is_open())
	{
		map<pair<string, string>, HISTORY>::const_iterator outerItr = history.begin();
		while (outerItr != history.end())
		{
			const HISTORY h                           = (*outerItr).second;
			const string property                     = (*outerItr).first.first;
			const string id                           = (*outerItr).first.second;
			map<int, string>::const_iterator innerItr = h.elements.begin();
			while (innerItr != h.elements.end())
			{
				hout << property << ',' << id << ',' << urlencode((*innerItr).second) << endl;
				++innerItr;
			}
			++outerItr;
		}
		hout.close();
	}
	ofstream pout("predicates.csv", ios::trunc);
	if (pout.is_open())
	{
		map<pair<string, string>, ARRAY>::const_iterator outerItr = userPredicates.begin();
		while (outerItr != userPredicates.end())
		{
			const ARRAY a                             = (*outerItr).second;
			const string property                     = (*outerItr).first.first;
			const string id                           = (*outerItr).first.second;
			map<int, string>::const_iterator innerItr = a.elements.begin();
			while (innerItr != a.elements.end())
			{
				pout << property << ',' << id << ',' << urlencode((*innerItr).second) << ',' << (*innerItr).first << endl;
				++innerItr;
			}
			++outerItr;
		}
		pout.close();
	}
}

void PredicateEngine::addValue(const string &property, const string &id, const string &value)
{
	//typedef map<pair<string, string>, ARRAY> uArrayMap

	map<pair<string, string>, ARRAY>::iterator itr = userPredicates.find(pair<string, string>(property, id));
	ARRAY array;
	if (itr != userPredicates.end())
		array = (*itr).second;
	else
		array = ARRAY();
	array.size++;
	array.number++;
	array.elements[array.size]                             = value;
	userPredicates[pair < string, string > (property, id)] = array;
}

void PredicateEngine::setValue(const string &property, const string &id, const string &value, int index)
{
	map<pair<string, string>, ARRAY>::iterator itr = userPredicates.find(pair<string, string>(property, id));
	ARRAY array;
	if (itr != userPredicates.end())
		array = (*itr).second;
	else
		array = ARRAY();
	if (array.elements.find(index) == array.elements.end())
		array.number++;
	if (index > array.size)
		array.size = index;
	array.elements[index]                                  = value;
	userPredicates[pair < string, string > (property, id)] = array;
}

void PredicateEngine::clearValue(const string &property, const string &id, int index)
{
	string s = property + id;
	int i    = index;
}

void PredicateEngine::clearValues(const string &property, const string &id)
{
	string s = property + id;
}

string PredicateEngine::getValue(const string &property, const string &id, int index)
{
	map<pair<string, string>, ARRAY>::const_iterator itr = userPredicates.find(pair<string, string>(property, id));
	if (itr != userPredicates.end())
	{
		const ARRAY &array                   = (*itr).second;
		map<int, string>::const_iterator itr = array.elements.find(index);
		if (itr != array.elements.end())
			return (*itr).second;
	}
	return "";
}

string PredicateEngine::getValues(const string &property, const string &id, const string &conjunction)
{
	map<pair<string, string>, ARRAY>::const_iterator itr = userPredicates.find(pair<string, string>(property, id));
	if (itr != userPredicates.end())
	{
		const ARRAY &array                   = (*itr).second;
		map<int, string>::const_iterator itr = array.elements.begin();
		string result                        = "";
		while (itr != array.elements.end())
		{
			result += (*itr).second;
			if (++itr == array.elements.end())
				break;
			else
				result += conjunction;
		}
		return result;
	}
	return "";
}

void PredicateEngine::addHistory(const string &property, const string &id, const string &value)
{
	map<pair<string, string>, HISTORY>::iterator itr = history.find(pair<string, string>(property, id));
	HISTORY h;
	if (itr != history.end())
	{
		h = (*itr).second;
	}
	else
	{
		h      = HISTORY();
		h.size = HISTORY_SIZE;
		h.top  = -1;
	}
	h.top++;
	if (h.top == h.size)
		h.top = 0;
	h.elements[h.top]                               = value;
	history[pair < string, string > (property, id)] = h;
}

string PredicateEngine::getHistory(const string &property, const string &id, int index)
{
	if (index > HISTORY_SIZE || index < 1)
		return "";
	map<pair<string, string>, HISTORY>::iterator itr = history.find(pair<string, string>(property, id));
	if (itr != history.end())
	{
		//	Since history doesn't shuffle the data, have to fix the index...
		//	Index values range from 1 to N .. 1 being most recent
		int i = index - 1;
		//	HISTORY.top == most recent (index 1)
		//	HISTORY.top is incremented, so older items are less than HISTORY.top
		i = (*itr).second.top - i;
		if (i < 0)
			i += HISTORY_SIZE;
		map<int, string>::const_iterator citr = (*itr).second.elements.find(i);
		if (citr != (*itr).second.elements.end())
			return (*citr).second;
	}
	return "";
}
