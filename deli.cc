

#include <stdio.h>
#include <stdlib.h>
#include "dthreads.h"
#include "interrupt.h"
#include <deque>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

struct Node{
	int cashierNumber;
	int sandwich;
	bool made;
};

int boardLimit;
int numCashier;
int mutex;
int recentSandwich;
int CVboardFull;
int CVboardOpen;
char** filenames;
string check;
std::deque<Node*> myqueue;



bool sortbool(Node* q1, Node* q2){
	return abs(recentSandwich-q1->sandwich)<abs(recentSandwich-q2->sandwich);
}




bool isBoardFull(){
	if(boardLimit>myqueue.size())
		return false;
	else
		return true;
}





void cook(void* p){
	//cout<<"cooking"<<endl;
	dthreads_lock(mutex);
	while(boardLimit > 0){
	while(boardLimit!=myqueue.size()){
		dthreads_wait(mutex, CVboardFull);
	}
	if(myqueue.size()>0){
		Node* makeorder = myqueue.front();
		cout << "READY: cashier " << makeorder->cashierNumber << " sandwich " << makeorder->sandwich << endl;
		myqueue.pop_front();
		check[makeorder->cashierNumber]='t';
		recentSandwich = makeorder->sandwich;
		

	}
	dthreads_broadcast(mutex, CVboardOpen);

}
dthreads_unlock(mutex);
}

void noMoreCashier(){
	numCashier--;
	if(numCashier<boardLimit){
		boardLimit--;
		dthreads_broadcast(mutex, CVboardFull);
	}

}

void cashier(void* pt){

	int cashierNumber = (int)pt;
	ifstream myfile(filenames[cashierNumber]);
	int orderNumber = 0;
	Node* order;

	while (myfile>>orderNumber){

		order= new Node;
		order->cashierNumber=cashierNumber;
		order-> sandwich= orderNumber;

		dthreads_lock(mutex);
		//cout<<"cashernumber"<<cashierNumber<<check<<endl;
		while(isBoardFull()||check[cashierNumber]=='f'){
			dthreads_wait(mutex, CVboardOpen);
			//cout<<"cashernumber"<<cashierNumber<<endl;
		}
		//cout<<"cashernumber"<<cashierNumber<<"pass"<<endl;
		cout << "POSTED: cashier " << order->cashierNumber << " sandwich " << order->sandwich << endl;
		
		
  

		myqueue.push_back(order);

		sort(myqueue.begin(),myqueue.end(),sortbool);
		check[cashierNumber] = 'f';

		dthreads_broadcast(mutex, CVboardFull);
	}
	myfile.close();

	while (check[cashierNumber]=='f'){
    dthreads_wait(mutex, CVboardOpen);
	}
	noMoreCashier()
	
	dthreads_unlock(mutex);
}



void intitalize(void* nigga) {
	dthreads_start(cook, NULL);

	for(int i=0;i<numCashier;i++){
		dthreads_start(cashier, (void*)i);
	}
}





int main(int argc, char* argv[]){
	if(argc<= 2)
	{
		return 0;
	}
	else{
		boardLimit = atoi(argv[1]);
		numCashier = argc-2;
		mutex = 1;
		recentSandwich = 0;
		CVboardFull = 5;
		CVboardOpen = 6;
		filenames = argv +2;


		for(int i=0;i<numCashier;i++){
			check.append("t");
		}
		dthreads_init(intitalize, NULL);
	}
	return 0;
}
