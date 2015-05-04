#ifndef BAYESIAN_NETWORK_H
#define BAYESIAN_NETWORK_H

#include "dlib/bayes_utils.h""
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"
#include <iostream>

class BayesianNetwork{
	// Use an enum to make some more readable names for our nodes.
//--------------------------DATA MEMBERS-----------------------------
public:
	enum Nodes
	{
		LOC = 0,//location, L(i,j)
		P1 = 1,P2 = 2,P3 = 3,P4 = 4,//PITS
		JC1 = 5,JC2 = 6,JC3 = 7,JC4 = 8,//JOINT CELLS
		NJC1 = 9,NJC2 = 10,NJC3 = 11,NJC4 = 12//NON-JOINT CELLS
	};
private:
	directed_graph<bayes_node>::kernel_1a_c bn;//network

//--------------------------FUNCTIONS-----------------------------
public:
	BayesianNetwork();
	void initialize();	
	void setEvidence();

private:
	void setProbability(Nodes node);
	void setJointProb(Nodes node, Nodes parent_1, Nodes parent_2);
	void setNonJointProb(Nodes node, Nodes parent);
};

#endif

