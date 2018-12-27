#ifndef MESSAGE_H
#define MESSAGE_H

#include "stats.h"
#include <string>
using namespace std;


class Message{
public:
	string senderName;
	string receiverName;
	int bufferId;
	string msg;
	Message(string senderName,string receiverName,int bufferId,string msg);
};

#endif;
