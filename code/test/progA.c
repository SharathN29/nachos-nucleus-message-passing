//progA.c
#include "syscall.h"

int
main()
{
        int buffer;   
	int buffer2;	
    	
	buffer = SendMessage("progB","Message1 from A to B",-1);	//dest,msg,buffer
  	buffer = SendMessage("progB","Message2 from A to B",buffer);
        
	WaitAnswer("progB","progA",buffer);			//dest,src,buffer
	WaitAnswer("progB","progA",buffer);			//dest,src,buffer

	Exit(0);
}

