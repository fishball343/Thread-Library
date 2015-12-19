
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "1t.h"
using namespace std;



void child(void * args){
char * id = (char *) args;

if(id=="thread1"){
dthreads_semdown(1);
}
if(id=="master"){
dthreads_semdown(1);
}

if(id=="thread2"){

dthreads_semup(1);
dthreads_semdown(2);
}
cout << "method ended with " << id << endl;
}


void master(void * args){
cout << "master thread started\n";
dthreads_seminit(1,0);
dthreads_seminit(2,0);
dthreads_start((dthreads_func_t) child, (void *) "thread1");
dthreads_start((dthreads_func_t) child, (void *) "thread2");
child_thread( (void *) "master");
cout << "end process\n";
}



int main(int argc, char *argv[]){
dthreads_init((dthreads_func_t) master, (void *) 100);

}