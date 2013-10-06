#ifndef NODE_WALKER_H
#define NODE_WALKER_H

/**
 * NodeWalker
 *
 * Walks a Nodemaster tree, visiting every node. This should allow us
 * to remove nodes (or just templates), so that we can erase all nodes
 * and/or templates for a particular file when reloading the file,
 * which should then allow merging to be put back in, as merging is
 * quite important for the learning bot :-)
 *
 * @author	Jonathan Roewen
 */
#include <string>

#include "Nodemaster.h"
#include "Element.h"

using namespace std;

class NodeWalker
{
public:
	static void dumpAiml(Nodemaster *);
	static void dumpAiml(Nodemaster *, const string &, PElement);
};

#endif
