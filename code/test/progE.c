//progE.c
#include "syscall.h"

int
main()
{
	int buffer;
	int buffer2;

        buffer= SendMessage("progF","Message from E to F",-1);
	buffer2= SendMessage("progG","Message from E to G",-1);

	WaitAnswer("progF","progE",buffer);
        WaitAnswer("progG","progE",buffer2);

	Exit(0);
}


