//progH.c
#include "syscall.h"

int
main()
{
        int buffer;   
	int buffer2;	
    	
	buffer = SendMessage("progI","Message1 from H to I",-1);	//dest,msg,buffer
  	buffer = SendMessage("progI","Message2 from H to I",buffer);
        
	WaitAnswer("progI","progH",buffer);			//dest,src,buffer
	WaitAnswer("progI","progH",buffer);			//dest,src,buffer

	buffer2=WaitMessage("progJ","progH");	
	Exit(0);
	SendAnswer("progH","progJ",buffer2,"Answer1 from H");	//src,dest,buffer,answer
}

