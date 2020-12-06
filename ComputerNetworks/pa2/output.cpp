#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <queue>
#include <limits>

using namespace std;

//This type is to avoid writing pair<int, int> multiple times
typedef pair<int, int> pairs;

//These prototypes are from the functions used to make the code work
vector<int> dijkstraAlgo(vector<pairs>*, int, int, int*);
void printResult(vector<int>, int, int*);
void nextHopFunc(int, int*, int*);

int main(){
	//Parse Adj.txt file
	//Create object of fstream class
	fstream file;

	//Open file in read mode
	file.open("Adj.txt", ios::in);

	//Check if the file was not opened
	if(!file){
		cout<<"Error in opening file!"<<endl;
		return 0;
	}

	//Read file while end of file is not found
	//Read to single character
	string input;

	//Variables used for reading information
	int numNodes = 0;
	int fromNode = 0;
	int toNode = 0;
	int distance = 0;

	//Get number of nodes
	getline(file,input);
	//Creating a stringstream object
	stringstream lineNumNodes(input);
	//saving the information to an integer variable
	lineNumNodes>>numNodes;

	//Array of vectors with pairs inside
	vector<pairs> adjacent[numNodes];

	//Reading the file and creating the graph
	while(getline(file,input)){
		//Read line in file
		stringstream line(input);

		//Saving values to the variables
		line>>fromNode>>toNode>>distance;

		//Creating the edges in the graph
		//Adding both directions of the edge
		adjacent[fromNode].push_back(make_pair(toNode,distance));
		adjacent[toNode].push_back(make_pair(fromNode,distance));
	}

	//Initialize all the array to zero
	int parentNode[numNodes] = {0,0,0,0,0,0};

	//Vector with distance to print results
	vector<int> distanceVector = dijkstraAlgo(adjacent, numNodes, 0, parentNode);

	//Print the restults to the user
	printResult(distanceVector, numNodes, parentNode);

	//Close file
	file.close();

	return 0;
}

//Do Dijkstra's Algorithm
vector<int> dijkstraAlgo(vector<pairs>* adjacent, int numNodes, int startNode, int* parentNode){
	//Using a priority queue as a min heap for this problem
	priority_queue<pairs, vector<pairs>, greater<pairs> > queue;

	//Get the maximum value as infinity
	int infinity = numeric_limits<int>::max();

	//Initialize all verticies to infinity
	vector<int> distance(numNodes, infinity);

	//Adding starting node to the priority queue
	queue.push(make_pair(0,startNode));

	//d(s) = 0
	distance[startNode] = 0;

	//Run loop while queue is not empty
	while(!queue.empty()){

		//Save node
		int u = queue.top().second;
		queue.pop();

		//loop through every adjacent node of u
		for(int i = 0; i < adjacent[u].size(); ++i){
			//Getting the node adjacent to u
			int v = adjacent[u][i].first;
			//Getting the distance from the adjacent node to u
			int dist = adjacent[u][i].second;

			//Compare if the newly calculated distance is less
			if(distance[v] > distance[u] + dist){
				//Update the distance value
				distance[v] = distance[u] + dist;
				//push the new distance and vector to the queue
				queue.push(make_pair(distance[v],v));
				//Update the value of the parent node of v
				parentNode[v] = u;
			}
		}
	}

	//return the vector with the distances
	return distance;
}

//Function to print out the result to the user
void printResult(vector<int> distance, int numNodes, int* parentNode){

	//Creating and initializing the array to "0"
	int nextHop[numNodes] = {0,0,0,0,0,0};

	//Look for the next hop to print it out to the user
	nextHopFunc(numNodes, parentNode, nextHop);

	//Printing results to user
	cout<<"Routing Table (<Destination> <Cost> <Next Hop>):"<<endl;
	for(int i = 1; i < numNodes; ++i){
		cout<<i<<" "<<distance[i]<<" "<<nextHop[i]<<endl;
	}

	//Printing out Spanning Tree to user
	cout<<"Output Spanning Tree"<<endl;
	for(int i = 1; i < numNodes; ++i){
		cout<<parentNode[i]<< " "<<i<<endl;
	}

}

//Function to find the next hop
void nextHopFunc(int numNodes, int* parentNode, int* futureHop){

	for(int i = 1; i < numNodes; i++){
		bool flag = false;
		int j = i;
		//Loop while parent is not 0
		while(!flag){
			//If parent is zero, then enter this if statement
			if(parentNode[j] == 0){
				//Save the value of j to futureHop[i]
				futureHop[i] = j;
				//Set the flag to true
				flag = true;
			}
			//If parent is not zero, then enter the else statement
			else{
				//Save the value of parentNode[j] to futureHop[i]
				futureHop[i] = parentNode[j];
			}
			//Update the value of j to futureHop[i] to continue iterating
			j = futureHop[i];
		}
	}
}