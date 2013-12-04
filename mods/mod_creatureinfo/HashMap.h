
#include <vector>
#include <map>

/*
HashMap class template; A usage example for vector, map, and function objects.
Written by Rodney Myers, 3/4/00
Description:
  
	This is an implementation of a hash map, which maps values
	of a key type to values of a value type, using a hash function object
	to speed up the search process.  HashMap elements must satisfy all
	the requirements that map elements must satisfy (see the documentation
	for map).  In addition, the client of HashMap must supply a suitable
	function object for the key type as a template parameter.
	
	The hash table uses chaining to resolve collisions.  However, instead of
	using a linked list to store values that hash to the same address as
	many hash table implementations	do, this implementation uses an STL
	map container to store these elements.  Because of the logarithmic
	search time behavior exhibited by map, worst case search time complexity
	is O(log n).  The worst case scenario is that all keys hash to the same
	address.

	As with any hash table implementation, when combined with a hash function
	that distributes elements equally throughout the table, and when the
	loading factor is sufficiently small, average case search time complexity
	is O(1).

	Hash tables represent a space for time tradeoff.  Do not use a hash
	table if space is more important than search time.

	As a safety measure, HashMap's operator[] and Find() methods throw an
	exception of class out_of_range if the hash function object returns
	a value that is not a valid hash table index.

	You must pass the maximum value that will be returned by the hash
	function object to the constructor for HashMap.  The
	values returned by the hash function are assumed to be in the
	range [0,hashFunctionUpperBound]

Notes on adapting this class template to work with your system:
	This code adheres to Standard C++.  It uses certain language features
	that are not available or do not work correctly with older C++ compilers.

	Here are some tips to get it working with older compilers:
	1.  Remove the namespace definition if namespaces are unsupported.
	2.  Remove default template parameters if unsupported.
	3.  Remove all occurences of "std::" if the standard library is not
		properly declared within the std namespace.
	4.  Add .h to the #include'd files
	5.  Remove the explicit keyword from the constructor declaration
		if not supported.
	6.  Configure your compiler so that exception handling is enabled.
*/

namespace Containers
{

/*
Notes:
The underlying map container requires a binary predicate function object,
which I have given a default value of the function object std::less<>
provided by the standard library.  std::less<> compares two objects by
using the less than (<) operator.

The "class HashFunc" template parameter specifies a hash function object.
*/
template <class Key, class Value, class HashFunc, class Pred=std::less<Key> >
class HashMap: private std::vector<std::map<Key, Value, Pred> >
{
public:

	// keyword explicit is used to prevent implicit type conversion from int
	explicit HashMap(int hashFunctionUpperBound)
	{
		// store an instance of the hash function object
		hashFunc = HashFunc();
		// resize the vector to be large enough for the hash table
		resize(hashFunctionUpperBound+1);
	}

	// As with map, the HashMap bracket operator always creates the key/value
	// association if it does not exist.
	Value& operator[](Key& key)
	{
		// returns a reference to the value associated with the requested key
		return at(hashFunc(key))[key];
	}

	// Searches for a key, but does not create a key/value association
	// if it does not exist.  Value is copied into val if the key is found.
	// Returns true if the key is found.
	bool Find(const Key &key, Value &val) const
	{
		std::map<Key, Value, Pred>::const_iterator itr;
		int index = hashFunc(key);

		// search for the key value
		if ((itr = at(index).find(key)) == at(index).end())
			return false; // key not found

		// the key was found, copy the associated value
		val = itr->second;
		return true;
	}

	HashFunc hashFunc;	// used to store an instance of the hash function object
};

}