#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <malloc.h>
#include <inttypes.h>
#include "music.h"

Thread threadHandle;
Handle threadRequest;

#define STACKSIZE (4 * 1024)

volatile bool runThread = true;

void threadMain(void *arg) {

	while(runThread) {
		svcWaitSynchronization(threadRequest, U64_MAX);
		svcClearEvent(threadRequest);
		advance();
	}
}

void MakeThread(){
	runThread = true;
	svcCreateEvent(&threadRequest,0);
	threadHandle = threadCreate(threadMain, 0, STACKSIZE, 0x3f, -2, true);
}

void RunThread(){
	svcSignalEvent(threadRequest);
}

void ExitThread(){
	 runThread = false;
	 svcSignalEvent(threadRequest);
	 threadJoin(threadHandle, U64_MAX);
	 svcCloseHandle(threadRequest);
}