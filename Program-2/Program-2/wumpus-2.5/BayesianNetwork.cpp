#include "BayesianNetwork.h"

using namespace dlib;
using namespace std;
using namespace bayes_node_utils;

BayesianNetwork::BayesianNetwork(){
	// This statement declares a bayesian network called bn.  Note that a bayesian network
	// in the dlib world is just a directed_graph object that contains a special kind 
	// of node called a bayes_node.
	bn = directed_graph<bayes_node>::kernel_1a_c();
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

void BayesianNetwork::initialize(){
	try
	{		
		// The next few blocks of code setup our bayesian network.

		// The first thing we do is tell the bn object how many nodes it has
		// and also add the three edges.  Again, we are using the network
		// shown in ASCII art at the top of this file.
		bn.set_number_of_nodes(13);//our network has 13 nodes
		
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

		bn.add_edge(P3, NJC4);//P4 connections
		bn.add_edge(P3, JC3);
		bn.add_edge(P3, JC4);

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
		

		// We have now finished setting up our bayesian network.  So let's compute some 
		// probability values.  The first thing we will do is compute the prior probability
		// of each node in the network.  To do this we will use the join tree algorithm which
		// is an algorithm for performing exact inference in a bayesian network.   

		// First we need to create an undirected graph which contains set objects at each node and
		// edge.  This long declaration does the trick.
		typedef dlib::set<unsigned long>::compare_1b_c set_type;
		typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;
		join_tree_type join_tree;

		// Now we need to populate the join_tree with data from our bayesian network.  The next  
		// function calls do this.  Explaining exactly what they do is outside the scope of this
		// example.  Just think of them as filling join_tree with information that is useful 
		// later on for dealing with our bayesian network.  
		create_moral_graph(bn, join_tree);
		create_join_tree(join_tree, join_tree);

		// Now that we have a proper join_tree we can use it to obtain a solution to our
		// bayesian network.  Doing this is as simple as declaring an instance of
		// the bayesian_network_join_tree object as follows:
		bayesian_network_join_tree solution(bn, join_tree);


		// now print out the probabilities for each node
		cout << "Using the join tree algorithm:\n";
		cout << "p(P1=1) = " << solution.probability(P1)(1) << endl;
		cout << "p(P1=0) = " << solution.probability(P1)(0) << endl;
		cout << "p(P2=1) = " << solution.probability(P2)(1) << endl;
		cout << "p(P2=0) = " << solution.probability(P2)(0) << endl;
		cout << "p(P3=1) = " << solution.probability(P3)(1) << endl;
		cout << "p(P3=0) = " << solution.probability(P3)(0) << endl;
		cout << "p(P4=1) = " << solution.probability(P4)(1) << endl;
		cout << "p(P4=0) = " << solution.probability(P4)(0) << endl;
		cout << "\n\n\n";


		// Now to make things more interesting let's say that we have discovered that the C 
		// node really has a value of 1.  That is to say, we now have evidence that 
		// C is 1.  We can represent this in the network using the following two function
		// calls.
		set_node_value(bn, C, 1);
		set_node_as_evidence(bn, C);

		// Now we want to compute the probabilities of all the nodes in the network again
		// given that we now know that C is 1.  We can do this as follows:
		bayesian_network_join_tree solution_with_evidence(bn, join_tree);

		// now print out the probabilities for each node
		cout << "Using the join tree algorithm:\n";
		cout << "p(A=1 | C=1) = " << solution_with_evidence.probability(A)(1) << endl;
		cout << "p(A=0 | C=1) = " << solution_with_evidence.probability(A)(0) << endl;
		cout << "p(B=1 | C=1) = " << solution_with_evidence.probability(B)(1) << endl;
		cout << "p(B=0 | C=1) = " << solution_with_evidence.probability(B)(0) << endl;
		cout << "p(C=1 | C=1) = " << solution_with_evidence.probability(C)(1) << endl;
		cout << "p(C=0 | C=1) = " << solution_with_evidence.probability(C)(0) << endl;
		cout << "p(D=1 | C=1) = " << solution_with_evidence.probability(D)(1) << endl;
		cout << "p(D=0 | C=1) = " << solution_with_evidence.probability(D)(0) << endl;
		cout << "\n\n\n";

		// Note that when we made our solution_with_evidence object we reused our join_tree object.
		// This saves us the time it takes to calculate the join_tree object from scratch.  But
		// it is important to note that we can only reuse the join_tree object if we haven't changed
		// the structure of our bayesian network.  That is, if we have added or removed nodes or 
		// edges from our bayesian network then we must recompute our join_tree.  But in this example
		// all we did was change the value of a bayes_node object (we made node C be evidence)
		// so we are ok.





		// Next this example will show you how to use the bayesian_network_gibbs_sampler object
		// to perform approximate inference in a bayesian network.  This is an algorithm 
		// that doesn't give you an exact solution but it may be necessary to use in some 
		// instances.  For example, the join tree algorithm used above, while fast in many
		// instances, has exponential runtime in some cases.  Moreover, inference in bayesian
		// networks is NP-Hard for general networks so sometimes the best you can do is
		// find an approximation.
		// However, it should be noted that the gibbs sampler does not compute the correct
		// probabilities if the network contains a deterministic node.  That is, if any
		// of the conditional probability tables in the bayesian network have a probability
		// of 1.0 for something the gibbs sampler should not be used.


		// This Gibbs sampler algorithm works by randomly sampling possibles values of the
		// network.  So to use it we should set the network to some initial state.  

		set_node_value(bn, A, 0);
		set_node_value(bn, B, 0);
		set_node_value(bn, D, 0);

		// We will leave the C node with a value of 1 and keep it as an evidence node.  


		// First create an instance of the gibbs sampler object
		bayesian_network_gibbs_sampler sampler;


		// To use this algorithm all we do is go into a loop for a certain number of times
		// and each time through we sample the bayesian network.  Then we count how 
		// many times a node has a certain state.  Then the probability of that node
		// having that state is just its count/total times through the loop. 

		// The following code illustrates the general procedure.
		unsigned long A_count = 0;
		unsigned long B_count = 0;
		unsigned long C_count = 0;
		unsigned long D_count = 0;

		// The more times you let the loop run the more accurate the result will be.  Here we loop
		// 2000 times.
		const long rounds = 2000;
		for (long i = 0; i < rounds; ++i)
		{
			sampler.sample_graph(bn);

			if (node_value(bn, A) == 1)
				++A_count;
			if (node_value(bn, B) == 1)
				++B_count;
			if (node_value(bn, C) == 1)
				++C_count;
			if (node_value(bn, D) == 1)
				++D_count;
		}

		cout << "Using the approximate Gibbs Sampler algorithm:\n";
		cout << "p(A=1 | C=1) = " << (double)A_count / (double)rounds << endl;
		cout << "p(B=1 | C=1) = " << (double)B_count / (double)rounds << endl;
		cout << "p(C=1 | C=1) = " << (double)C_count / (double)rounds << endl;
		cout << "p(D=1 | C=1) = " << (double)D_count / (double)rounds << endl;
	}
	catch (std::exception& e)
	{
		cout << "exception thrown: " << endl;
		cout << e.what() << endl;
		cout << "hit enter to terminate" << endl;
		cin.get();
	}

}

