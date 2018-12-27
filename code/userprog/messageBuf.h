#ifndef MESSAGEBUF_H
#define MESSAGEBUF_H

#include "../lib/list.h"
#include "message.h"
#include "stats.h"
#include <string>
using namespace std;


class MessageBuffer{
public:
        string senderName;
        string receiverName;
        int bufferId;
	List<Message *> *listOfMsgs;
        MessageBuffer(string senderName,string receiverName,int bufferId);
	string getReceiverName(){return receiverName;}
	string getSenderName(){return senderName;}
	
};

#endif;

