#include <cstdlib>
#include <ucontext.h>
#include <deque>
#include "interrupt.h"
#include "1t.h"
#include <iostream>
#include <map>

using namespace std;
struct Thread{
	
	char* stack;
	ucontext_t* ucontext_ptr;
	bool check;
};

typedef map<unsigned int, deque<Thread*>*>::const_iterator threadmap;
typedef map<unsigned int, int>::const_iterator valuemap;
static map<unsigned int, deque<Thread*>*> semathreadmap	;
static map<unsigned int, int> semavaluemap;
typedef void (*thread_startfunc_t)(void *);
bool started =false;
static deque<Thread*> readyqueue;
static deque<Thread*> waitqueue;
static Thread* current;
static ucontext_t* context;





static int start(thread_startfunc_t func, void *arg2) {
	interrupt_enable();
	func(arg2);
	interrupt_disable();
	current->check = true;
	swapcontext(current->ucontext_ptr, context);
	return 0;
}

int dthreads_init(dthreads_func_t func, void *arg){
	if (started) {
		return -1;
	}
	started = true;
	if(dthreads_start(func, arg)<0){
		return -1;
	}
	Thread* first = readyqueue.front();
	readyqueue.pop_front();
	
	try {
		context = new ucontext_t;
		getcontext(context);
	} 
	catch (std::bad_alloc b) {
		delete context;
		return -1;
	}

	current = first;

	interrupt_disable();
	swapcontext(context, current->ucontext_ptr);
	while (readyqueue.size() > 0) {
		if (current->check == true){
				delete current->stack;
				current->ucontext_ptr->uc_stack.ss_sp = NULL;
				current->ucontext_ptr->uc_stack.ss_size = 0;
				current->ucontext_ptr->uc_stack.ss_flags = 0;
				current->ucontext_ptr->uc_link = NULL;
				delete current->ucontext_ptr;
				delete current;
				current = NULL;
		}
			
		Thread* next = readyqueue.front();
		readyqueue.pop_front();
		current = next;
		swapcontext(context, current->ucontext_ptr);
	}
	interrupt_enable();

	cout << "Thread library exiting.\n"<<endl;
	exit(0);
}

int dthreads_start(dthreads_func_t func, void *arg){
	if(!started){
		return -1;
	}
	interrupt_disable();
	Thread* thd;
	try{
		thd = new Thread;
		thd->ucontext_ptr = new ucontext_t;
		getcontext(thd->ucontext_ptr);
		thd->ucontext_ptr->uc_stack.ss_sp = new char[STACK_SIZE];
		thd->ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
		thd->ucontext_ptr->uc_stack.ss_flags = 0;
		thd->ucontext_ptr->uc_link = NULL;
		makecontext(thd->ucontext_ptr, (void (*)())start, 2, func, arg);
		
	}
	catch (std::bad_alloc b) {

		delete thd;
		interrupt_enable();
		return -1;
	}
	thd->check = false;
	readyqueue.push_back(thd);
	interrupt_enable();
	return 0;
}




int dthreads_yield(void) {
	if (!started) {
		return -1;
	}
	interrupt_disable();
	readyqueue.push_back(current);
	swapcontext(current->ucontext_ptr, context);
	interrupt_enable();
	return 0;
}




int dthreads_seminit(unsigned int sem, unsigned int value){
if(semathreadmap.find(sem)!= semathreadmap.end()){
	return -1;
}
interupt_disable();
semathreadmap.insert(make_pair(sem,new deque<Thread*>));
semavaluemap.insert(make_pair(sem,value));
interupt_enable();
return sem;
}


int dthreads_semup(unsigned int sem){
if(!started){
return -1;
}interrupt_disable();

if(semavaluemap.find(sem)==semavaluemap.end()){
	return -1;
}
std:: map<unsigned int, int>:: iterator yo = semavaluemap.find(sem);
yo->second = yo->second+1;
cout<<yo->second<<endl;
 threadmap cond_iter = semathreadmap.find(sem);
 deque<Thread*>* c = (*cond_iter).second;
if (!c->empty()) { //Threads waiting
Thread* t = c->front();
c->pop_front();
readyqueue.push_back(t);
}
 
interrupt_enable();
return 0;

}

int dthreads_semdown(unsigned int sem){
if(!started){
return -1;
}
interrupt_disable();
std:: map<unsigned int, int>:: iterator yo = semavaluemap.find(sem);

 if(yo->second ==0){
	threadmap cond_iter = semathreadmap.find(sem);
	deque<Thread*>* c = (*cond_iter).second;

	(*cond_iter).second->push_back(current);
	
}
else{
	yo->second = yo->second-1;

}
interrupt_enable();
return 0;




deque<Thread*>* c;
c= new deque<Thread*>
c.push_back(current);
}