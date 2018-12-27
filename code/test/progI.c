//progI.c
#include "syscall.h"

int
main()
{
        int buffer;

        buffer = WaitMessage("progH","progI");
        buffer = WaitMessage("progH","progI");
		
	SendAnswer("progI","progH",buffer,"Answer1 from I");
	Exit(0);
	SendAnswer("progI","progH",buffer,"Answer2 from I");
        
}


