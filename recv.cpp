#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */
#include <iostream>
#include <string>

using namespace std;

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	         2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		 3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V object (i.e. message queues, shared memory, and sempahores)
		    is unique system-wide among all System V objects. Two objects, on the other hand,
		    may have the same key.
	 */


	 key_t key = ftok("keyfile.txt", 'a');

	/* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	shmid = shmget(key,SHARED_MEMORY_CHUNK_SIZE,0666|IPC_CREAT);

	/* TODO: Attach to the shared memory */
	sharedMemPtr = shmat(shmid, (void *)0, 0);
	if(sharedMemPtr == (void*) (-1))
	{
		perror("shmat");
	}
	/* TODO: Create a message queue */
	msqid = msgget(key,0666|IPC_CREAT);
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
}


/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 0;

	/* A buffer to store message we will send to the sender. */
	message sndMsg;

	/* A buffer to store message received from the sender. */
	message rcvMsg;

	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

    /* TODO: Receive the message and get the message size. The message will
     * contain regular information. The message will be of SENDER_DATA_TYPE
     * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
     * of the message is not 0, then we copy that many bytes from the shared
     * memory region to the file. Otherwise, if 0, then we close the file and
     * exit.
     *
     * NOTE: the received file will always be saved into the file called
     * "recvfile"
     */
     //msgrcv(msqid,&rcvMsg,rcvMsg.info.size, SENDER_DATA_TYPE, 0);

		 //cout << rcvMsg.msg[8] << endl;

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */
	 int test = 0;
	 do{
		 cout << "Waiting to receive message" << endl;
		 msgrcv(msqid,&rcvMsg,sizeof(rcvMsg.info.msg), SENDER_DATA_TYPE, 0);
		 cout << "Message recieved" << endl;
		 //rcvMsg.size = 10;
		 std::cout << "Message size is: " << rcvMsg.info.size << "\n";
		 msgSize = rcvMsg.info.size;
		 cout << "Output of the message is: " << rcvMsg.info.msg << endl;
		 if(msgSize != 0)
		{
			cout << "Message is not zero" << endl;
			//Save the shared memory to file
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			rcvMsg.print(fp);

			/* TODO: Tell the sender that we are ready for the next file chunk.
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case).
 			 */
			 sndMsg.mtype = RECV_DONE_TYPE;
			 msgsnd(msqid, &sndMsg, sizeof(sndMsg.info.msg), 0);
			 cout << "Reciever sent messsage notifying the sender thats its ready for next file chunk" << endl;
			 ++test;
			 cout << "Loop iterated " << test << " times" << endl;
		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
   }while(msgSize != 0);

	 cout << "EXITED LOOOP!!!" << endl;
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	shmdt(sharedMemPtr);

	/* TODO: Deallocate the shared memory chunk */
	shmctl(shmid,IPC_RMID,NULL);

	/* TODO: Deallocate the message queue */
	msgctl(msqid,IPC_RMID,NULL);
}
/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
	cout << "Exiting\n";
	exit(0);
}

int main(int argc, char** argv)
{
	/* TODO: Install a signal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
	cout << "initialize method finished " << endl;

	/* Go to the main loop */
	mainLoop();
	cout << "mainLoop Method called " <<  endl;
	cout << "cleanUp Method called " <<  endl;

	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
	signal(SIGINT, ctrlCSignal);
	for(;;)
	{
		sleep(1);
	}

	return 0;
}
