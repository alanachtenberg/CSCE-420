// Agent.h

#ifndef AGENT_H
#define AGENT_H

#define WORLD_WIDTH 4

#include "Action.h"
#include "Percept.h"
#include "BayesianNetwork.h"
#include "Orientation.h"
#include <stack>
#include <queue>
class Agent
{
private: 	
	struct Position{
		int x;
		int y;
	};
	BayesianNetwork network;//bayesian network to determine where pits may be
	bool stenchMemory[WORLD_WIDTH][WORLD_WIDTH];//use these arrays to remember where things occured
	bool breezeMemory[WORLD_WIDTH][WORLD_WIDTH];
	bool exploredMemory[WORLD_WIDTH][WORLD_WIDTH];//if explored is true then we have been there, note if we have been there and havent died, cell must not be a pit or wumpus
	
	Position position;
	Orientation orientation;
	bool goldFound;
	bool wumpusKilled;

	stack<Action> actionStack;//stack to keep track of position
	queue<Action> backTrackSequence;//sequence generated when we find gold, to climb back out
	void backTrack();//generate the sequence

	BayesianNetwork::Nodes getNode(Position p);
	void supplyEvidence();
	queue<Action> actionSequence;//use to generate a sequence that moves to a sqaure
	void generateActions();
	void updateOrientation(Action left_right);
	bool updatePosition();//we assume we are moving forward, returns true if it is in range
	bool isPositionValid(Position p);
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
};

#endif // AGENT_H
