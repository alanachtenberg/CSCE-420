#include "BayesianNetwork.h"

using namespace dlib;
using namespace std;
using namespace bayes_node_utils;

BayesianNetwork::BayesianNetwork(){
	// This statement declares a bayesian network called bn.  Note that a bayesian network
	// in the dlib world is just a directed_graph object that contains a special kind 
	// of node called a bayes_node.
//	bn = directed_graph<bayes_node>::kernel_1a_c(); had to comment out, because library declared bn as a noncopyable class
}

void BayesianNetwork::setJointProb(Nodes node, Nodes parent_1, Nodes parent_2){
	assignment parent_state;
	parent_state.add(parent_1, 1);
	parent_state.add(parent_2, 1);
	set_node_probability(bn, node, 1, parent_state, 1);//there will always be a breeze if either p1 or p2 is true
	set_node_probability(bn, node, 0, parent_state, 0);
	parent_state[parent_1] = 1;
	parent_state[parent_2] = 0;
	set_node_probability(bn, node, 1, parent_state, 1);
	set_node_probability(bn, node, 0, parent_state, 0);
	parent_state[parent_1] = 0;
	parent_state[parent_2] = 1;
	set_node_probability(bn, node, 1, parent_state, 1);
	set_node_probability(bn, node, 0, parent_state, 0);
	parent_state[parent_1] = 0;//this one is a little more tricky, no certainty,we know at least 2 of the 4 adjacent cells do not contain a pit								
	parent_state[parent_2] = 0;//we know the probability of a breeze is the probability that at least 1 of the other 2 cells is a pit(.2 chance for every cell)
	//P(A|B)=P(A)+P(B)-P(A&B), .2+.2-.04=.36 , this is the prob that at least one of the other adjacent cells is a pit
	set_node_probability(bn, node, 1, parent_state, .36);
	set_node_probability(bn, node, 0, parent_state, 1-.36);
}

void BayesianNetwork::setNonJointProb(Nodes node, Nodes parent){
	assignment parent_state;
	parent_state.add(parent, 1);	
	set_node_probability(bn, node, 1, parent_state, 1);//there will always be a breeze if parent is a pit
	set_node_probability(bn, node, 0, parent_state, 0);
	parent_state[parent] = 0;//if parent is 0 then the probability of a breeze is the probaility that at least one of the other 3 adjacent cells is a pit
	//to determine this we will take the probability that no pits occur and subtract that from 1
	//1-.8*.8*.8=.488
	set_node_probability(bn, node, 1, parent_state, .488);
	set_node_probability(bn, node, 0, parent_state, 1-.488);
}
void BayesianNetwork::setLOCProb(int p1, int p2, int p3, int p4){
	assignment parent_state;
	parent_state.add(P1, p1);
	parent_state.add(P2, p2);
	parent_state.add(P3, p3);
	parent_state.add(P4, p4);

	if (p1 == 1 || p2 == 1 || p3 == 1 || p4 == 1){
		set_node_probability(bn, LOC, 1, parent_state, 1);
		set_node_probability(bn, LOC, 0, parent_state, 0);
	}
	else{
		set_node_probability(bn, LOC, 1, parent_state, 0);
		set_node_probability(bn, LOC, 0, parent_state, 1);
	}


}

void BayesianNetwork::setProbability(BayesianNetwork::Nodes node){
	
	switch (node){
	case JC1:
		setJointProb(node, P1, P2);
		break;
	case JC2:
		setJointProb(node, P2, P3);
		break;
	case JC3:
		setJointProb(node, P3, P4);
		break;
	case JC4:
		setJointProb(node, P4, P1);
		break;
	case NJC1:
		setNonJointProb(node, P1);
		break;
	case NJC2:
		setNonJointProb(node, P2);
		break;
	case NJC3:
		setNonJointProb(node, P3);
		break;
	case NJC4:
		setNonJointProb(node, P4);
		break;
	}
}
void BayesianNetwork::setEvidence(Nodes node, int value){
	set_node_value(bn, node, value);
	set_node_as_evidence(bn, node);
}
void BayesianNetwork::clearEvidence(){
	for (int i = 0; i < NUM_NODES; ++i){
		Nodes node=(Nodes)i;//cast integer into enum
		set_node_as_nonevidence(bn, node);//remove from evidence
	}
}

bayesian_network_join_tree BayesianNetwork::getSolution(){
	
	
	return bayesian_network_join_tree(bn, join_tree);
}


void BayesianNetwork::initialize(){
	try
	{		
		// The next few blocks of code setup our bayesian network.

		// The first thing we do is tell the bn object how many nodes it has
		// and also add the three edges.  Again, we are using the network
		// shown in ASCII art at the top of this file.
		bn.set_number_of_nodes(NUM_NODES);//our network has 13 nodes
		
		bn.add_edge(P1, LOC);//pits adjacent to location
		bn.add_edge(P2, LOC);
		bn.add_edge(P3, LOC);
		bn.add_edge(P4, LOC);

		bn.add_edge(P1, NJC1);//P1 connections
		bn.add_edge(P1, JC4);
		bn.add_edge(P1, JC1);

		bn.add_edge(P2, NJC2);//P2 connections
		bn.add_edge(P2, JC1);
		bn.add_edge(P2, JC2);

		bn.add_edge(P3, NJC3);//P3 connections
		bn.add_edge(P3, JC2);
		bn.add_edge(P3, JC3);

		bn.add_edge(P4, NJC4);//P4 connections
		bn.add_edge(P4, JC3);
		bn.add_edge(P4, JC4);

		// Now we inform all the nodes in the network that they are binary
		// nodes.  That is, they only have two possible values.  
		set_node_num_values(bn, LOC, 2);
		set_node_num_values(bn, P1, 2);
		set_node_num_values(bn, P2, 2);
		set_node_num_values(bn, P3, 2);
		set_node_num_values(bn, P4, 2);
		set_node_num_values(bn, JC1, 2);
		set_node_num_values(bn, JC2, 2);
		set_node_num_values(bn, JC3, 2);
		set_node_num_values(bn, JC4, 2);
		set_node_num_values(bn, NJC1, 2);
		set_node_num_values(bn, NJC2, 2);
		set_node_num_values(bn, NJC3, 2);
		set_node_num_values(bn, NJC4, 2);

		assignment parent_state;
		// Now we will enter all the conditional probability information for each node.
		// Each node's conditional probability is dependent on the state of its parents.  
		// To specify this state we need to use the assignment object.  This assignment 
		// object allows us to specify the state of each nodes parents. 

		// Here we specify that p(B=1) = 0.01
		// parent_state is empty in this case since P1 is a root node. 
		set_node_probability(bn, P1, 1, parent_state, 0.2);//probability of a pit is .2
		// Here we specify that p(P1=0) = 1-0.01
		set_node_probability(bn, P1, 0, parent_state, 1 - 0.2);
		set_node_probability(bn, P2, 1, parent_state, 0.2);//PIT 2
		set_node_probability(bn, P2, 0, parent_state, 1 - 0.2);
		set_node_probability(bn, P3, 1, parent_state, 0.2);//PIT 3
		set_node_probability(bn, P3, 0, parent_state, 1 - 0.2);
		set_node_probability(bn, P4, 1, parent_state, 0.2);//PIT 4
		set_node_probability(bn, P4, 0, parent_state, 1 - 0.2);
		
		setProbability(JC1);
		setProbability(JC2);
		setProbability(JC3);
		setProbability(JC4);
		setProbability(NJC1);
		setProbability(NJC2);
		setProbability(NJC3);
		setProbability(NJC4);

		//time for the largest table, LOC
		for (int i = 0; i < 2; ++i){
			for (int j = 0; j < 2; ++j){
				for (int k = 0; k < 2; ++k){
					for (int l = 0; l < 2; ++l){
						setLOCProb(i, j, k, l);//set probability for each case
					}
				}
			}
		}
		
		create_moral_graph(bn, join_tree);
		create_join_tree(join_tree, join_tree);

	}
	catch (std::exception& e)
	{
		cout << "exception thrown: " << endl;
		cout << e.what() << endl;
		cout << "hit enter to terminate" << endl;
		cin.get();
	}

	


}

