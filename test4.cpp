#include <cstdlib>
#include <iostream>
#include "1t.h"
using namespace std;

int num=1;
int semaphore=1;
int semaphore2=7;
int emptybuffer=0;
int fullbuffer=10;
int value=10;

void errors(){
	if(dthreads_semdown==-1 || dthreads_semup== -1|| dthreads_seminit== -1){
		cout<< "error doing wrong thing";

	}
}


void thread1 (void* n){
	cout<<"1 is ready"<<endl;
	while (emptybuffer<fullbuffer){
		while (num!=1){
			dthreads_semdown(semaphore);
		}
		cout<<"thread1 acquires the lock";
		emptybuffer++;
		cout << "current empty: " << emptybuffer << " current full: " << fullbuffer << endl;
		num=2;
		cout<<dthreads_semup(semaphore);
		dthreads_yield();

		}
		cout<<"1finished\n";
	}


void thread2 (void *n){
	cout<<"2 is ready"<<endl;
	while (emptybuffer< fullbuffer){
		while (num!=2){
			dthreads_semdown(semaphore);
		}
		cout<<"thread2 acquires the lock";
		emptybuffer++;
		cout << "current empty: " << emptybuffer << " current full: " << fullbuffer << endl;
		num=2;
		cout<<dthreads_semup(semaphore);
		dthreads_yield();

		}
		cout<<"2finished\n";
	}



void thread3 (void *n){
	cout<<"3 is ready"<<endl;

	while (emptybuffer< fullbuffer){
		while (num!=3){
			dthreads_semdown(semaphore);
		}
		cout<<"thread2 acquires the lock";
		emptybuffer++;
		cout << "current empty: " << emptybuffer << " current full: " << fullbuffer << endl;
		num=1;
		cout<<dthreads_semup(semaphore);
		dthreads_yield();

		}
		cout<<"3finished\n";
	}



void initate_twice(void *n){

	cout<<"initate_twice thats bad"<<endl;
}
void test_initiator() {
    test_init2(&dthreads_seminit);
    test_init1(&dthreads_semup);
    test_init1(&dthreads_semdown);
    test_init0(&dthreads_yield);

}
void test_init0(int (*func)(void)) {
    if (func())
        cout << "thread lib not initialized\n";
    else
        cout << "works with uninitialized library\n";
}

void test_init(int (*func)(unsigned int)) {
    if (func(semaphore))
        cout << "thread lib not initialized\n";
    else
        cout << "works with uninitialized library\n";
}

void test_init2(int (*func)(unsigned int, unsigned int)) {
    if (func(semaphore, semaphore))
        cout << "thread lib not initialized\n";
    else
        cout <<"works with uninitialized library\n";
}



void master(void *n){
	dthreads_start((thread_startfunc_t) thread1, (void*) "n");
    dthreads_start((thread_startfunc_t) thread2, (void*) "n");
    dthreads_start((thread_startfunc_t) thread3, (void*) "n");

    dthreads_init((thread_startfunc_t) initate_twice, (void *) 1);
	
}
int main(){
		test_initiator();
		dthreads_init((thread_startfunc_t) master, (void*) 100);

}