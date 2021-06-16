#pragma once

/* A* pathfinding implementacija bazirana je na implementaciji koju je napravio Javidx9, a.k.a. OneLoneCoder
 * (https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_PathFinding_AStar.cpp).
 * 
 * Njegova je implementacija zasticena GNU GPLv3 licencom, te stoga i ova implentacija mora biti zasticena istom
 * (https://github.com/OneLoneCoder/videos/blob/master/LICENSE).
 */

#include <vector>
#include <stack>
using namespace std;

struct sNode {
	bool bObstacle = false;			// Is the node an obstruction?
	bool bVisited = false;			// Have we searched this node before?
	bool bPath = false;				// Is it part of the path?
	float fGlobalGoal;				// Distance to goal so far
	float fLocalGoal;				// Distance to goal if we took the alternative route
	int x;							// Nodes position in 2D space
	int y;
	vector<sNode*> vecNeighbours;	// Connections to neighbours
	sNode* parent;					// Node connecting to this node that offers shortest parent
};

class Pathfinding {
private:

	int nMapWidth;
	int nMapHeight;

	sNode* nodes = nullptr;
	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;

	bool useDiagonals = true;

public:

	Pathfinding(int width, int height);
	Pathfinding();

	sNode* getNode(int x, int y);

	stack<sNode*> getPath();

	void setStartNode(int x, int y);
	void setEndNode(int x, int y);
	void setObstacle(int x, int y, bool isObstacle);
	void toggleDiagonals();

	bool isStart(int x, int y);
	bool isEnd(int x, int y);
	bool isObstacle(int x, int y);
	bool isPath(int x, int y);
	bool isVisited(int x, int y);

	bool Solve_AStar();
	void InitializeNodes();
	void RefreshNodes();
};

