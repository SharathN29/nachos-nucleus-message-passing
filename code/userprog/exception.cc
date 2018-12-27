#include "../lib/list.h"
#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include <map>
#include "message.h"
#include <iostream>
using namespace std;
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
List<int> *bufferIds = new List<int>();
string parseInput(int startLocation){
        int k;
        string str = "";
	while(true){
	bool flag;
	flag = kernel->machine->ReadMem(startLocation,1,&k);
	char c;
	c = (char) k;
	if(flag){
	      if(c!='\0'){	
		startLocation++;
                 str+=c;}
               else
                 break;
	       }
      
	}
    return str;
}
void
removeBuffers(int id){
	ListIterator<MessageBuffer *> *it = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
	//MessageBuffer *mb;
	while(!it -> IsDone()){
                MessageBuffer *mb = it->Item();
               if(mb->bufferId == id){
		     cout<<"Returning the buffer "<<id<<" to the pool of buffers\n";
		     kernel->listOfBuffers->Remove(mb);
	             return;
               }
               it->Next();
        }	
	return;
}

/*


sendanswers with result as dummy  to all the buffers of the messages present in the current threads message queue and to append the answer to
the queue of 'senders of the messages' present in the current threads message queue


*/
void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;
     
     case SC_Exit:
     {
	ListIterator<Message *> *iter = new ListIterator<Message *>(kernel->removedMessages);
	while(!iter -> IsDone()){
		Message *msg = iter->Item();
        	if(msg->receiverName == kernel->currentThread->getName()){		//send dummy Answer
			string str = "Dummy answer recieved due to " +kernel->currentThread->getName()+" exiting";
			string receiverName = msg->senderName; 
			string senderName = msg->receiverName;
			int bufferId = msg->bufferId;
	   		ListIterator<MessageBuffer *> *it = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
			MessageBuffer *mb;
                	while(!it -> IsDone()){
                        	mb = it->Item();
                        	if(mb->bufferId == bufferId){
					Message *ans = new Message(senderName,receiverName,bufferId,str);
					 mb->listOfMsgs->Append(ans);
					if(kernel->processListMap.find(receiverName) != kernel->processListMap.end()){
						kernel->processListMap[receiverName]->Append(ans);
				
					}
					else{
						List<Message *> *msglist = new List<Message *>(); 
						msglist->Append(ans);
						kernel->processListMap[receiverName] = msglist;
					}						
				}
			it->Next();
			}
		}		
		iter->Next();
	}
	ListIterator<MessageBuffer *> *it1 = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
	//MessageBuffer *mb;
	while(!it1 -> IsDone()){
                MessageBuffer *mb = it1->Item();
               if(mb->senderName == kernel->currentThread->getName()){
		      Kernel::bufferPool->Clear(mb->bufferId);
                      bufferIds->Append(mb->bufferId); 
		     // kernel->listOfBuffers->Remove(mb);
               }
               it1->Next();
        }	
	ListIterator<int> *it2 = new ListIterator<int>(bufferIds);
	//MessageBuffer *mb;
	while(!it2 -> IsDone()){
               removeBuffers(it2->Item());
		it2->Next();
        }
	//cout<<"number of buffers: "<<kernel->listOfBuffers->NumInList()<<endl;
	//kernel::bufferPool->Print();
	cout<<kernel->currentThread->getName()<<" EXITING\n\n";
	kernel->currentThread->Finish();
        
	{
          /* set previous programm counter (debugging only)*/
          kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

          /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
          kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

          /* set next programm counter for brach execution */
          kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }

	return;

        ASSERTNOTREACHED();

        break;

     }
     case SC_SendMessage:{
        int receiverNamePtr = (int)kernel->machine->ReadRegister(4);
	int msgPtr = (int)kernel->machine->ReadRegister(5); 
	int bufferId = (int)kernel->machine->ReadRegister(6);
	
	string senderName = kernel->currentThread->getName();
	string receiverName = parseInput(receiverNamePtr);
	string mesg = parseInput(msgPtr);
	/*
             check if bufferId is -1, if so, 
					find the available buffer from the bitmap
					create a buffer witht the id,insert the buffer into the list of buffers, insert the message into the buffers 
					list of messages 
					and also into the queue of the receiver( check if the receiver exists in the map, if so append the message to the
					quueu, if not create a new queue and push it to the map)
                                        write the buffer id into the register
                                    if not
                                         find the buffer from the list of buffers based on the buffer id
					 and check to see that the buffers sender and reciver is same as currenthread name and receiver name respectively
                                         append the message into the buffers list of messages and also into the queue of the receiver
                                         write the buffer id into the register

        */
	MessageBuffer *msgbuf;
	Message *msg = new Message(kernel->currentThread->getName(),receiverName,bufferId,mesg);
	if(bufferId == -1){
		int buffer  = Kernel::bufferPool->FindAndSet();
		if(buffer!=-1){
			cout<<"\n\nBuffer "<<buffer<<" allocated to "<<senderName<<" and "<<receiverName<<" for communication\n";
			cout<<senderName<<" sending message: "<<mesg<<endl;
			msgbuf = new MessageBuffer(kernel->currentThread->getName(),receiverName,buffer);
			msg->bufferId = buffer;
			msgbuf->listOfMsgs->Append(msg);
			kernel->listOfBuffers->Append(msgbuf);
			if(kernel->processListMap.find(receiverName) != kernel->processListMap.end()){
				kernel->processListMap[receiverName]->Append(msg);
		                msgbuf->listOfMsgs->Remove(msg);
	          		}
			else{
				List<Message *> *msglist = new List<Message *>(); 
				msglist->Append(msg);
		                msgbuf->listOfMsgs->Remove(msg); 
				kernel->processListMap[receiverName] = msglist;
			}
		}
		else{
			cout<<"No available buffer. Message will be dropped\n"; //no available buffer
			kernel->currentThread->Finish();
		}
	}
	else{
        	ListIterator<MessageBuffer *> *it = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
		cout<<senderName<<" sending message: "<<mesg<<endl;
		while(!it -> IsDone()){
			MessageBuffer *mb = it->Item();
			if(mb->bufferId == bufferId){
				mb->listOfMsgs->Append(msg);
                                 //cout<<mb->listOfMsgs->NumInList()<<endl;
				kernel->processListMap[receiverName]->Append(msg);
                                 
		                mb->listOfMsgs->Remove(msg);
                                 //cout<<kernel->processListMap[receiverName]->NumInList()<<endl;
                                break;
			}
		}		

	}
		
        kernel->machine->WriteRegister(2, (int)msg->bufferId);

        {
          /* set previous programm counter (debugging only)*/
          kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

          /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
          kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

          /* set next programm counter for brach execution */
          kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }

        return;

        ASSERTNOTREACHED();

        break;
      }

    case SC_WaitMessage:
    {
        int senderPtr = (int)kernel->machine->ReadRegister(4);
	int rcvrPtr = (int)kernel->machine->ReadRegister(5); 
	//int bufferId = (int)kernel->machine->ReadRegister(6);
	
	string receiverName = parseInput(rcvrPtr);
	string senderName = parseInput(senderPtr);
        
	if(kernel->processListMap.find(kernel->currentThread->getName()) !=  kernel->processListMap.end()){	
		List<Message *> *msglist = kernel->processListMap[kernel->currentThread->getName()];
		while(true){
		
                    bool flag = false;
		    ListIterator<Message *> *iter = new ListIterator<Message *>(msglist);
                    while(!iter -> IsDone()){
                    	if(iter->Item()->senderName == senderName && iter->Item()->receiverName == receiverName){
		        	flag = true;
				kernel->removedMessages->Append(iter->Item());
				cout<<"Message received: "<<iter->Item()->msg<<endl;
				msglist->Remove(iter->Item());
				kernel->machine->WriteRegister(2, (int)iter->Item()->bufferId);
				break;	
		 	}
			iter->Next();
                    }
                    if(flag)
                        break;
		    kernel->interrupt->SetLevel(IntOff);
	   	    kernel->currentThread->Yield();
	   	    kernel->interrupt->SetLevel(IntOn);
		}	

        }
        
            /*
              get the receiver queue from the map based on the current threads name, 
              put a infinite while loop
               if the contents of the queue are not empty
               remove front from the queue,print the contents of the message and other parms like bufferid,sender id etc, and sendAnswer to the buffer 
		
               
              and continue

             */
 

 
	{
          /* set previous programm counter (debugging only)*/
          kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

          /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
          kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

          /* set next programm counter for brach execution */
          kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }

        return;

        ASSERTNOTREACHED();

        break;
       }

	case SC_WaitAnswer:
        {
        int senderPtr = (int)kernel->machine->ReadRegister(4);
	int rcvrPtr = (int)kernel->machine->ReadRegister(5); 
	int bufferId = (int)kernel->machine->ReadRegister(6);

	string receiverName = parseInput(rcvrPtr);
	string senderName = parseInput(senderPtr);
	cout<<receiverName<<" waiting an answer from "<<senderName<<"\n\n";
        MessageBuffer *mb;
        //ListIterator<MessageBuffer *> *it = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
        bool isAnswer = false;
        while(true){
        	ListIterator<MessageBuffer *> *it = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
		while(!it -> IsDone()){
                	mb = it->Item();
                        if(mb->bufferId == bufferId){
                                List<Message *> *Messages = mb->listOfMsgs;
				ListIterator<Message *> *iter = new ListIterator<Message *>(Messages);
                		while(!iter -> IsDone()){
                        		if(iter->Item()->senderName == senderName && iter->Item()->receiverName == receiverName){
						isAnswer = true;
						cout<<"Answer received at "<<receiverName<<": "<<iter->Item()->msg<<"\n";
					//	List<Message *> *queueSender = kernel->processListMap[senderName];
						List<Message *> *queueReceiver = kernel->processListMap[receiverName];
						queueReceiver->Remove(iter->Item());
                                                Messages->Remove(iter->Item());
					//	if(queueSender->NumInList()<=0){
						//	Kernel::bufferPool->Clear(bufferId);
						//	kernel->listOfBuffers->Remove(mb);
					
					//	}
					 	break;	
					}
					iter->Next();
				}
                               if(isAnswer)
                                   break; 
                               else{
	   			kernel->interrupt->SetLevel(IntOff);
	   			kernel->currentThread->Yield();
	   			kernel->interrupt->SetLevel(IntOn);
				}
                        }
			it->Next();
                }
                  if(isAnswer)
                    break;
	}
             /* read the buffer id from the register 4
               wait on the buffers messages for a answer and result,
                   once an answer arrives ,print the answer and result, reset the buffer id in the bitmap, and remove the buffer from the list of buffers         
            */



        {	
          /* set previous programm counter (debugging only)*/
          kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

          /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
          kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

          /* set next programm counter for brach execution */
          kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }

        return;

        ASSERTNOTREACHED();

        break;
         }
	case SC_SendAnswer:
        {
        int senderPtr = (int)kernel->machine->ReadRegister(4);
	int rcvrPtr = (int)kernel->machine->ReadRegister(5); 
	int bufferId = (int)kernel->machine->ReadRegister(6);
	int ansPtr = (int)kernel->machine->ReadRegister(7);

	string receiverName = parseInput(rcvrPtr);
	string senderName = parseInput(senderPtr);
	string str = parseInput(ansPtr);
	cout<<"Sending an answer from "<<senderName<<" to "<<receiverName<<"\n\n";
	MessageBuffer *mb;
	   ListIterator<MessageBuffer *> *it = new ListIterator<MessageBuffer *>(kernel->listOfBuffers);
                while(!it -> IsDone()){
                        mb = it->Item();
                        if(mb->bufferId == bufferId){
				Message *ans = new Message(senderName,receiverName,bufferId,str);
				
				mb->listOfMsgs->Append(ans);
                               //  cout<<mb->listOfMsgs->NumInList()<<endl;
                                //kernel->processListMap[senderName]->Append(ans);

				if(kernel->processListMap.find(receiverName) != kernel->processListMap.end()){
					kernel->processListMap[receiverName]->Append(ans);						
				//	List<Message *> *msglist = kernel->processListMap[kernel->currentThread->getName()];
				}
				else{
					List<Message *> *msglist = new List<Message *>(); 
					msglist->Append(ans);
					kernel->processListMap[receiverName] = msglist;
				}
                    		ListIterator<Message *> *iter = new ListIterator<Message *>(kernel->removedMessages);
		                while(!iter -> IsDone()){
                			if(iter->Item()->senderName == receiverName && iter->Item()->receiverName == senderName){
						kernel->removedMessages->Remove(iter->Item());
                        			}
                        		iter->Next();
                    		}

                                break;
                        }
			it->Next();
                }
                  
	   			kernel->interrupt->SetLevel(IntOff);
	   			kernel->currentThread->Yield();
	   			kernel->interrupt->SetLevel(IntOn);

            /*
              read the buffer id from the register 4, read the sender and reciver names
               append the answer to the buffer id,  and also put it into the message queue of the receiver queue from the map			

		 */



        {
          /* set previous programm counter (debugging only)*/
          kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

          /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
          kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

          /* set next programm counter for brach execution */
          kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }

        return;

        ASSERTNOTREACHED();

        break;
         }

      
      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}



