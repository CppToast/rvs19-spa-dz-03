#include "Pathfinding.h"

#include <list>
#include <stack>
#include <iostream>

Pathfinding::Pathfinding(int width, int height)
{
	nMapWidth = width;
	nMapHeight = height;
	InitializeNodes();
}

Pathfinding::Pathfinding() {
	nMapWidth = 0;
	nMapHeight = 0;
}

sNode* Pathfinding::getNode(int x, int y)
{
	return &nodes[y * nMapWidth + x];
}

stack<sNode*> Pathfinding::getPath()
{
	stack<sNode*> path;
	if (nodeEnd != nullptr)
	{
		sNode* p = nodeEnd;
		while (p->parent != nullptr)
		{
			path.push(p);
			// Set next node to this node's parent
			p = p->parent;
		}
	}
	path.push(nodeStart);
	return path;
}

void Pathfinding::setStartNode(int x, int y)
{
	nodeStart = &nodes[y * nMapWidth + x];
	nodes[y * nMapWidth + x].bObstacle = false;
}

void Pathfinding::setEndNode(int x, int y)
{
	nodeEnd = &nodes[y * nMapWidth + x];
	nodes[y * nMapWidth + x].bObstacle = false;
}

void Pathfinding::setObstacle(int x, int y, bool isObstacle)
{
	if (!isStart(x, y) and !isEnd(x, y)) { nodes[y * nMapWidth + x].bObstacle = isObstacle; }
}

void Pathfinding::toggleDiagonals()
{
	useDiagonals = !useDiagonals;
	RefreshNodes();
}

bool Pathfinding::isStart(int x, int y)
{
	return nodes[y * nMapWidth + x].x == nodeStart->x and
		   nodes[y * nMapWidth + x].y == nodeStart->y;
}

bool Pathfinding::isEnd(int x, int y)
{
	return nodes[y * nMapWidth + x].x == nodeEnd->x and
		   nodes[y * nMapWidth + x].y == nodeEnd->y;
}

bool Pathfinding::isObstacle(int x, int y)
{
	return nodes[y * nMapWidth + x].bObstacle;
}

bool Pathfinding::isPath(int x, int y)
{
	return nodes[y * nMapWidth + x].bPath;
}

bool Pathfinding::isVisited(int x, int y)
{
	return nodes[y * nMapWidth + x].bVisited;
}

bool Pathfinding::Solve_AStar()
{
	// Reset Navigation Graph - default all node states
	for (int x = 0; x < nMapWidth; x++)
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y * nMapWidth + x].bVisited = false;
			nodes[y * nMapWidth + x].bPath = false;
			nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;
			nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
			nodes[y * nMapWidth + x].parent = nullptr;	// No parents
		}

	auto distance = [](sNode* a, sNode* b) // For convenience
	{
		return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
	};

	auto heuristic = [distance](sNode* a, sNode* b) // So we can experiment with heuristic
	{
		return distance(a, b);
	};

	// Setup starting conditions
	sNode* nodeCurrent = nodeStart;
	nodeStart->fLocalGoal = 0.0f;
	nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

	// Add start node to not tested list - this will ensure it gets tested.
	// As the algorithm progresses, newly discovered nodes get added to this
	// list, and will themselves be tested later
	list<sNode*> listNotTestedNodes;
	listNotTestedNodes.push_back(nodeStart);

	// if the not tested list contains nodes, there may be better paths
	// which have not yet been explored. However, we will also stop 
	// searching when we reach the target - there may well be better
	// paths but this one will do - it wont be the longest.
	while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)// Find absolutely shortest path // && nodeCurrent != nodeEnd)
	{
		// Sort Untested nodes by global goal, so lowest is first
		listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

		// Front of listNotTestedNodes is potentially the lowest distance node. Our
		// list may also contain nodes that have been visited, so ditch these...
		while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
			listNotTestedNodes.pop_front();

		// ...or abort because there are no valid nodes left to test
		if (listNotTestedNodes.empty())
			break;

		nodeCurrent = listNotTestedNodes.front();
		nodeCurrent->bVisited = true; // We only explore a node once


		// Check each of this node's neighbours...
		for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
		{
			// ... and only if the neighbour is not visited and is 
			// not an obstacle, add it to NotTested List
			if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
				listNotTestedNodes.push_back(nodeNeighbour);

			// Calculate the neighbours potential lowest parent distance
			float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

			// If choosing to path through this node is a lower distance than what 
			// the neighbour currently has set, update the neighbour to use this node
			// as the path source, and set its distance scores as necessary
			if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
			{
				nodeNeighbour->parent = nodeCurrent;
				nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

				// The best path length to the neighbour being tested has changed, so
				// update the neighbour's score. The heuristic is used to globally bias
				// the path algorithm, so it knows if its getting better or worse. At some
				// point the algo will realise this path is worse and abandon it, and then go
				// and search along the next best path.
				nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
			}
		}
	}

	// Mark the nodes that are part of the path for easier drawing
	if (nodeEnd != nullptr)
	{
		sNode* p = nodeEnd;
		while (p->parent != nullptr)
		{
			p->bPath = true;

			// Set next node to this node's parent
			p = p->parent;
		}
	}

	return true;
}

void Pathfinding::InitializeNodes()
{
	// Create a 2D array of nodes - this is for convenience of rendering and construction
	// and is not required for the algorithm to work - the nodes could be placed anywhere
	// in any space, in multiple dimensions...

	int start_x, start_y, end_x, end_y;

	if (nodeStart != nullptr) {
		start_x = nodeStart->x;
		start_y = nodeStart->y;
		end_x = nodeEnd->x;
		end_y = nodeEnd->y;
	}

	nodes = new sNode[nMapWidth * nMapHeight];
	for (int x = 0; x < nMapWidth; x++)
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y * nMapWidth + x].x = x; // ...because we give each node its own coordinates
			nodes[y * nMapWidth + x].y = y;
			nodes[y * nMapWidth + x].bObstacle = false;
			nodes[y * nMapWidth + x].parent = nullptr;
			nodes[y * nMapWidth + x].bVisited = false;
		}

	RefreshNodes();

	if (nodeStart == nullptr) {
		// Manually position the start and end markers so they are not nullptr
		nodeStart = &nodes[(nMapHeight / 2) * nMapWidth + 1];
		nodeEnd = &nodes[(nMapHeight / 2) * nMapWidth + nMapWidth - 2];
	}
	else {
		nodeStart = &nodes[start_y * nMapWidth + start_x];
		nodeEnd = &nodes[end_y * nMapWidth + end_x];
	}
}

void Pathfinding::RefreshNodes()
{
	// Create connections - in this case nodes are on a regular grid
	for (int x = 0; x < nMapWidth; x++)
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y * nMapWidth + x].vecNeighbours = {};
			if (y > 0)
				nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 0)]);
			if (y < nMapHeight - 1)
				nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 0)]);
			if (x > 0)
				nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x - 1)]);
			if (x < nMapWidth - 1)
				nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x + 1)]);

			if (useDiagonals) {
				// We can also connect diagonally
				if (y > 0 && x > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x - 1)]);
				if (y < nMapHeight - 1 && x>0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x - 1)]);
				if (y > 0 && x < nMapWidth - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 1)]);
				if (y < nMapHeight - 1 && x < nMapWidth - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 1)]);
			}

		}
}
