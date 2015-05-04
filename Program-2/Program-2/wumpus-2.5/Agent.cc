// Agent.cc

#include <iostream>
#include "Agent.h"
#include "BayesianNetwork.h"
using namespace std;


Agent::Agent ()
{
	for (int i = 0; i < WORLD_WIDTH; ++i){//initialize memory
		for (int j = 0; j < WORLD_WIDTH; ++j){
			stenchMemory[i][j] = false;
			breezeMemory[i][j] = false;
			exploredMemory[i][j] = false;
		}
	}
	position.x = 0;
	position.y = 0;
	orientation = RIGHT;
	bool goldFound=false;
}

Agent::~Agent ()
{

}


void Agent::Initialize ()
{
	network.initialize();
}

void Agent::backTrack(){//populate backTrackSequence from actionStack
	Action stackAction;
	for (int i = 0; i < actionStack.size(); ++i){
		stackAction = actionStack.top();
		actionStack.pop();
		switch (stackAction){
		case FORWARD://the inverse of moving forward is to turn around, move forward, turn back around
			backTrackSequence.push(TURNLEFT);
			backTrackSequence.push(TURNLEFT);
			backTrackSequence.push(FORWARD);
			backTrackSequence.push(TURNLEFT);
			backTrackSequence.push(TURNLEFT);
			break;
		case TURNLEFT:
			backTrackSequence.push(TURNRIGHT);
			break;
		case TURNRIGHT:
			backTrackSequence.push(TURNLEFT);
			break;
		default://default is a shoot climb or grab action, we dont care about those since they do not affect position
			break;
		}
	}	
	backTrackSequence.push(Action(CLIMB));
}

BayesianNetwork::Nodes Agent::getNode(Position p){
	int rel_x = p.x - position.x;
	int rel_y = p.y - position.y;
	switch (rel_x){
		case -2:
			return BayesianNetwork::NJC4;
		case -1:
			switch (rel_y){
				case -1:
					return BayesianNetwork::JC3;
				case 0:
					return BayesianNetwork::P4;
				case 1:
					return BayesianNetwork::JC4;
			}
		case 0:
			switch (rel_y){
				case -2:
					return BayesianNetwork::NJC3;
				case -1:
					return BayesianNetwork::P3;
				case 0:
					return BayesianNetwork::LOC;
				case 1:
					return BayesianNetwork::P1;
				case 2:
					return BayesianNetwork::NJC1;
			}
		case 1:
			switch (rel_y){
				case -1:
					return BayesianNetwork::JC2;
				case 0:
					return BayesianNetwork::P2;
				case 1:
					return BayesianNetwork::JC1;
			}
		case 2:
			return BayesianNetwork::NJC2;
	}
}

void Agent::supplyEvidence(){//gives evidence to our 
	network.clearEvidence();
	std::vector<Position> cells;
	//we will handle breezes first and then pits
	cells.push_back(position);
	cells.push_back(Position{ position.x-2, position.y });//NJC4
	
	cells.push_back(Position{ position.x -1, position.y-1 });//JC3
	cells.push_back(Position{ position.x - 1, position.y + 1 });//JC4

	cells.push_back(Position{ position.x, position.y - 2 });//NJC3
	cells.push_back(Position{ position.x, position.y + 2 });//NJC1

	cells.push_back(Position{ position.x+1, position.y - 1 });//JC1
	cells.push_back(Position{ position.x+1, position.y + 1 });//JC2
	
	cells.push_back(Position{ position.x + 2, position.y });//NJC2
	
	for (int i = 0; i < cells.size(); ++i){
		Position cell = cells[i];
		if (isPositionValid(cell)){//if cell is a valid position			
				if (breezeMemory[cell.x][cell.y])
					network.setEvidence(getNode(cell), 1);//if there is a breeze set the value to 1
				else if (exploredMemory[cell.x][cell.y]){
					network.setEvidence(getNode(cell), 0);//if we explored the cell and there was no breeze set to 0
				}
		}		
	}
	cells.clear();//remove cells
		
	cells.push_back(Position{ position.x, position.y+1 });//P1
	cells.push_back(Position{ position.x, position.y -1 });//P3
	cells.push_back(Position{ position.x+1, position.y });//P2
	cells.push_back(Position{ position.x-1, position.y });//P4

	for (int i = 0; i < cells.size(); ++i){
		Position cell = cells[i];
		if (isPositionValid(cell)){//if cell is a valid position			
			if (exploredMemory[cell.x][cell.y])
				network.setEvidence(getNode(cell), 0);//if we have been to this cell without dying, then it is not a pit
		}
	}
}

void Agent::generateActions(){
	if (actionSequence.size() == 0){
		bayesian_network_join_tree solution=network.getSolution();
		std::vector<Position> cells;
		
		Position P1{ position.x, position.y + 1 };//P1
		Position P2{ position.x + 1, position.y };//P2		
		Position P3{ position.x, position.y - 1 };//P3	
		Position P4{ position.x - 1, position.y };//P4
		if (isPositionValid(P1)){			
			if (!exploredMemory[P1.x][P1.y])//Only consider non explored cells
				cells.push_back(P1);
		}
		if (isPositionValid(P2)){
			if (!exploredMemory[P2.x][P2.y])
				cells.push_back(P2);
		}
		if (isPositionValid(P3)){
			if (!exploredMemory[P3.x][P3.y])
				cells.push_back(P3);
		}
		if (isPositionValid(P4)){
			if (!exploredMemory[P4.x][P4.y])
				cells.push_back(P4);
		}
		
		double lowest_prob = INT_MAX;
		int index;
		for (int i = 0; i < cells.size(); ++i){
			double prob=solution.probability(getNode(cells[i]))(0);//probability that cell is not a pit
			if (prob < lowest_prob){
				index = i;
				lowest_prob = prob;
			}
			if (prob == lowest_prob){
				if (std::rand()%2==1);
					index = i;
			}
		}
		
		if (lowest_prob>.75){//lets consider the explored set again
			cells.clear();
			if (isPositionValid(P1)){			
					cells.push_back(P1);
			}
			if (isPositionValid(P2)){				
					cells.push_back(P2);
			}
			if (isPositionValid(P3)){			
					cells.push_back(P3);
			}
			if (isPositionValid(P4)){			
					cells.push_back(P4);
			}
			lowest_prob = INT_MAX;
			for (int i = 0; i < cells.size(); ++i){
				double prob = solution.probability(getNode(cells[i]))(0);//probability that cell is not a pit
				if (prob < lowest_prob){
					index = i;
					lowest_prob = prob;
				}
			}			
		}
		BayesianNetwork::Nodes desiredNode = getNode(cells[index]);
		switch (desiredNode){
			case BayesianNetwork::P1:
				while (orientation != UP){
					actionSequence.push(TURNRIGHT);//just keep turning right until we are in the correct position
					updateOrientation(TURNRIGHT);
				}
				actionSequence.push(FORWARD);
				break;
			case BayesianNetwork::P2:
				while (orientation != RIGHT){
					actionSequence.push(TURNRIGHT);
					updateOrientation(TURNRIGHT);
				}
				actionSequence.push(FORWARD);
				break;
			case BayesianNetwork::P3:
				while (orientation != DOWN){
					actionSequence.push(TURNRIGHT);
					updateOrientation(TURNRIGHT);
				}
				actionSequence.push(FORWARD);
				break;
			case BayesianNetwork::P4:
				while (orientation != LEFT){
					actionSequence.push(TURNRIGHT);
					updateOrientation(TURNRIGHT);
				}
				actionSequence.push(FORWARD);
				break;
		}
	}
}
void Agent::updateOrientation(Action left_right){
	if (left_right == TURNLEFT){
		switch (orientation){
		case UP:
			orientation = LEFT;
			break;
		case RIGHT:
			orientation = UP;
			break;
		case DOWN:
			orientation = RIGHT;
			break;
		case LEFT:
			orientation = DOWN;
			break;
		}		
	}
	if (left_right == TURNRIGHT){
		switch (orientation){
		case UP:
			orientation = RIGHT;
			break;
		case RIGHT:
			orientation = DOWN;
			break;
		case DOWN:
			orientation = LEFT;
			break;
		case LEFT:
			orientation = UP;
			break;
		}
	}
}
bool Agent::updatePosition(){//we assume we are moving forward, returns true if it is in range
	Position newPosition;
	switch (orientation){
		case UP:
			newPosition.x = position.x;
			newPosition.y = position.y+1;
			break;
		case RIGHT:
			newPosition.x = position.x+1;
			newPosition.y = position.y;
			break;
		case DOWN:
			newPosition.x = position.x;
			newPosition.y = position.y-1;
			break;
		case LEFT:
			newPosition.x = position.x-1;
			newPosition.y = position.y;
			break;
	}
	if (isPositionValid(newPosition)){
		position = newPosition;//we are moving into a valid position
		return true;
	}
	else
		return false;
}
bool Agent::isPositionValid(Position p){
	if (0 <= p.x && p.x < WORLD_WIDTH && 0 <= p.y && p.y < WORLD_WIDTH)
		return true;
	else
		return false;
}

Action Agent::Process (Percept& percept)
{
	
	exploredMemory[position.x][position.y] = true;//set this area to explored
	Action action=TURNRIGHT;//give a default action
	if (goldFound && backTrackSequence.size()==0)
		backTrack();
	if (backTrackSequence.size() != 0){
		action = backTrackSequence.front();//get action from sequence
		backTrackSequence.pop();//remove from sequence
		return action;
	}	
	if (percept.Glitter){
		action = GRAB;
		goldFound = true;
		return action;
	}
	if (percept.Bump){
		action = TURNRIGHT;
		actionStack.pop();//remove action from stack, since we couldnt actually move forward
	}
	if (percept.Breeze){
		breezeMemory[position.x][position.y] = true;
	}
	if (percept.Stench){
		breezeMemory[position.x][position.y] = true;
	}
	supplyEvidence();//update bayesian network
	generateActions();
	if (actionSequence.size() != 0){
		action = actionSequence.front();//get action
		actionSequence.pop();//pop off the queue, actions we want to take
	}
	actionStack.push(action);//push our action on our history stack, ie actions we have taken	
	if (action == FORWARD){
		updatePosition();		
	}

	return action;
}

void Agent::GameOver (int score)
{

}

