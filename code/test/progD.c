//progD.c
#include "syscall.h"

int
main()
{
        int buffer;


        buffer = WaitMessage("progC","progD");
        SendAnswer("progD","progC",buffer,"Answer1 from progD");

        Exit(0);
}


