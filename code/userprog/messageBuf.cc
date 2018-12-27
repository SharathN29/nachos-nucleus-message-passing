#include "messageBuf.h"

MessageBuffer:: MessageBuffer(string sendName, string recvName, int  bufId)
{
	senderName = sendName;
	receiverName = recvName;
	bufferId = bufId;
	listOfMsgs = new List<Message *>();	
}


