#include <stdio.h>
#include <stdlib.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <tr1/tuple>

#include <string>
#include <vector>
#include <utility>

//typedef std::tr1::unordered_map<int,int> Map;
//typedef std::tr1::unordered_map<int, Map> Graph;

//typedef std::tr1::unordered_map<std::string, int> Map;
typedef std::tr1::unordered_map<std::string, std::tr1::unordered_set<int> > Map;
typedef std::tr1::unordered_map<std::string, Map> Graph;

typedef std::tr1::tuple<std::string,std::string,bool> Transistor;
typedef std::vector< std::tr1::tuple<std::string,std::string,bool> > TxVec;

void buildGraph(const TxVec& txVec, Graph& graph)
{
	if(txVec.empty()) return;
	
	int idx = 0;
	for( TxVec::const_iterator it = txVec.begin();  it != txVec.end(); ++it )
	{
		//graph[std::tr1::get<0>((*it))][std::tr1::get<1>((*it))] = idx;
		//graph[std::tr1::get<1>((*it))][std::tr1::get<0>((*it))] = idx;
		graph[std::tr1::get<0>((*it))][std::tr1::get<1>((*it))].insert(idx);
		graph[std::tr1::get<1>((*it))][std::tr1::get<0>((*it))].insert(idx);
		++idx;
	}
}

int getEdgeCount(const std::string& node, Graph& graph)
{
    if( graph.find(node) == graph.end() ) return -1;	
    
    int total = 0;
    const Map& mymap = graph[node];
    for( Map::const_iterator iter = mymap.begin(); iter != mymap.end(); ++iter )
    {
        total += (*iter).second.size();	
    }
    
    return total;
}

void searchEulerPath(const TxVec& txVec, Graph& graph)
{
	if( txVec.empty() ) return;
	
	//record if the transistors are visited or not.
	std::vector<bool> visited(txVec.size(), false);
	std::vector<std::string> netOrdering;
	std::vector<Transistor>  txOrient;
	
	//setting the initial status. Chose the left net of the first transistor with its orientation set to R0	
	//std::string node = std::tr1::get<0>(txVec.front());
	//netOrdering.push_back(node);		
	std::string node;
	for(int idx = 0; idx < txVec.size(); ++idx)
	{
		if( visited[idx] ) continue;
				
		int curIdx = idx;
		//clean up and setting up
		txOrient.clear();
	    netOrdering.clear();
	    
	    //choose a odd-edge side as starting point. if none, choose arbitrarily.
	    int leftEdgeCnt = getEdgeCount(std::tr1::get<0>( txVec[curIdx] ), graph);
	    int rightEdgeCnt = getEdgeCount(std::tr1::get<1>( txVec[curIdx] ), graph);
	    if( 1 == (leftEdgeCnt & 1) || (0 == (leftEdgeCnt & 1) && 0 == (rightEdgeCnt & 1) ) )
	        node = std::tr1::get<0>( txVec[curIdx] );
	    else if( 1 == (rightEdgeCnt & 1) )    
	        node = std::tr1::get<1>( txVec[curIdx] );
	    netOrdering.push_back( node );
	        
		while( curIdx < txVec.size() )
		{		
		    const Map& mymap = graph[node];		    
		    Map::const_iterator iter = mymap.begin();
		    for( ; iter != mymap.end(); ++iter )
		    {		    				    
				if( (*iter).second.end() != (*iter).second.find(curIdx)  )
			    {
				    //push into the tmp vector
				    netOrdering.push_back((*iter).first);
				    txOrient.push_back(txVec[curIdx]);
				    if(node != std::tr1::get<0>(txOrient.back()) )
				        std::tr1::get<2>(txOrient.back()) = true;
				        
				    node = (*iter).first;
				    visited[curIdx] = true;
				    ++curIdx;
				    break;
			    }			    			    
		    }//======for		
		    if( !(iter != mymap.end() ))
		        break;
	    }//inner while
	    
	    //print the sub-result
	    for(int idy = 0; idy < netOrdering.size() ; ++idy)
	    {
	    	printf("%s--", netOrdering[idy].c_str());
	    }
	    printf("\n");
	    for(int idy = 0; idy < txOrient.size() ; ++idy)
	    {
	    	printf("(%s,%s,%d)==", std::tr1::get<0>(txOrient[idy]).c_str(), std::tr1::get<1>(txOrient[idy]).c_str(), std::tr1::get<2>(txOrient[idy]) );
	    }
	    printf("\n");
	    
	    	    
	}
}

int main(int argc, char** argv)
{	
	TxVec txVec;
	#if 0  //test case 1
	txVec.push_back( Transistor("A","B", false) );
	txVec.push_back( Transistor("A","B", false) );
	txVec.push_back( Transistor("A","C", false) );
	
	txVec.push_back( Transistor("C","D", false) );
	//below is the experimental transistor
	txVec.push_back( Transistor("C","D", false) );
	txVec.push_back( Transistor("G","K", false) );
	#endif

    #if 0 //test case2 
    txVec.push_back( Transistor("A","B", false) );
	txVec.push_back( Transistor("A","B", false) );
	txVec.push_back( Transistor("A","C", false) );
	
	txVec.push_back( Transistor("B","C", false) );
	txVec.push_back( Transistor("B","C", false) );
	#endif
	
	//test case 3
	txVec.push_back( Transistor("B","A", false) );
	txVec.push_back( Transistor("B","A", false) );
	txVec.push_back( Transistor("B","A", false) );
	
	txVec.push_back( Transistor("B","C", false) );
	txVec.push_back( Transistor("B","C", false) );
	txVec.push_back( Transistor("B","C", false) );
	
	
    //
    // Single Row Chaining with transistors ordering are fixed.
    //
    Graph graph;
    buildGraph(txVec, graph);
	searchEulerPath(txVec, graph);
		
	return EXIT_SUCCESS;
}
