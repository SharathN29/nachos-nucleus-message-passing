//progF.c
#include "syscall.h"

int
main()
{
        int buffer;


        buffer = WaitMessage("progE","progF");
        SendAnswer("progF","progE",buffer,"Answer from progF");

        Exit(0);
}


