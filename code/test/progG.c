//progG.c
#include "syscall.h"

int
main()
{
        int buffer;


        buffer = WaitMessage("progE","progG");
        SendAnswer("progG","progE",buffer,"Answer from progG");

        Exit(0);
}


