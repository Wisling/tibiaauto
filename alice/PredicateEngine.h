/*

	File:	PredicateEngine.h
	
	Author:	Jonathan Roewen
	
	Date:	08-December-2002
	
	Description:
			PredicateEngine replaces Memory + Database. The reasons
			behind this is for a (hopefully) simple interface with
			extended capabilities, and configurability.
			
			One such planned extension is to simulate having arrays
			as the predicate types, so that the bot can store info
			as collections. For example: a predicate array, fruit
			basket, could store all the fruit a user wants to buy
			if the bot was a fruit seller.

*/

#ifndef PREDICATE_ENGINE_H
#define PREDICATE_ENGINE_H

#include <string>
#include <vector>
#include <map>

using namespace std;

//	this defines how many history items can be stored
//	before overwriting a value
#define HISTORY_SIZE 5

/*

	Requires a size, index to the 'top' element, and then all the elements

*/
typedef struct {
	int size;
	int top;
	map<int, string> elements;
} HISTORY;

/*

	Requires number stored, size of the array, and then all the elements
	
	Using a map, rather than a vector is actually more useful here

*/
typedef struct {
	int number;
	int size;
	map<int, string> elements;
} ARRAY;

typedef map<pair<string, string>, HISTORY> historyMap;
typedef map<pair<string, string>, ARRAY> uArrayMap;
typedef map<string, ARRAY> gArrayMap;

class PredicateEngine
{
public:
	PredicateEngine();	//	This will load the predicates
	~PredicateEngine();	//	This will save the predicates
	
	//	PREDICATE ARRAYS
	
	//	Adds at ARRAY.size + 1
	void addValue(const string &property, const string &id, const string &value);
	//	Adds at a random position, overwriting any previous value, defaulting to first index
	void setValue(const string &property, const string &id, const string &value, int index = 1);
	//	Sets the value at the specified index to the empty string (no value)
	void clearValue(const string &property, const string &id, int index = 1);
	//	Completely erases all values matching the property & id
	void clearValues(const string &property, const string &id);
	//	Returns the value at the specified index (or empty string if it doesn't exist)
	string getValue(const string &property, const string &id, int index = 1);
	//	Returns all the non-empty values in the array, concatenated with the given conjunction (default is " and ")
	string getValues(const string &property, const string &id, const string &conjunction = " and ");
	
	//	HISTORY PREDICATES
	
	//	Adds a history item for the given property and id
	void addHistory(const string &property, const string &id, const string &value);
	//	Gets a history item for the given property and id (empty string if out of bounds), defaults to most recent
	string getHistory(const string &property, const string &id, int index = 0);
private:
	historyMap history;
	uArrayMap userPredicates;
};

#endif
