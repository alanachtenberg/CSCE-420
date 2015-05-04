#ifndef BAYESIAN_NETWORK_H
#define BAYESIAN_NETWORK_H

#define NUM_NODES 13

#include "dlib/bayes_utils.h""
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"
#include <iostream>
using namespace dlib;
using namespace std;
using namespace bayes_node_utils;
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
	bool hasEvidence;
	
	typedef dlib::set<unsigned long>::compare_1b_c set_type;
	typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;
	join_tree_type join_tree;
//--------------------------FUNCTIONS-----------------------------
public:
	BayesianNetwork();
	void initialize();	
	void setEvidence(Nodes node, int value);
	void clearEvidence();
	bayesian_network_join_tree getSolution();

private:
	void setProbability(Nodes node);
	void setJointProb(Nodes node, Nodes parent_1, Nodes parent_2);
	void setNonJointProb(Nodes node, Nodes parent);
	void setLOCProb(int p1, int p2, int p3, int p4);
	
};

#endif

