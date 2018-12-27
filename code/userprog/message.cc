#include "message.h"

Message:: Message(string sendName,string rcvName,int bufId, string message){
	senderName = sendName;
	receiverName = rcvName;
	bufferId = bufId;
	msg = message;
} 
