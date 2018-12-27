//progC.c
#include "syscall.h"

int
main()
{
	int buffer;

        buffer= SendMessage("progD","Message from C to D",-1);
	WaitAnswer("progD","progC",buffer);

	Exit(0);
}


