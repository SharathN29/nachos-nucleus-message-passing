//progJ.c
#include "syscall.h"

int
main()
{
	int buffer;

        buffer= SendMessage("progH","Message from J to H",-1);
	WaitAnswer("progH","progJ",buffer);

	Exit(0);
}


