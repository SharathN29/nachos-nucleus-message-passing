//progB.c
#include "syscall.h"

int
main()
{
        int buffer;

        buffer = WaitMessage("progA","progB");
        buffer = WaitMessage("progA","progB");
		
	SendAnswer("progB","progA",buffer,"Answer1 from B");	//src,dest,buffer,answer
	SendAnswer("progB","progA",buffer,"Answer2 from B");
        
	Exit(0);
}


