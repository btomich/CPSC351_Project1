
/* The information type */

#define SENDER_DATA_TYPE 1

/* The done message */
#define RECV_DONE_TYPE 2

/**
 * The message structure
 */


struct message
{
	/* The message type */
	long mtype;

	struct message_info
	{
		/* How many bytes in the message */
		int size;

		// The message written in the text file
		char msg[100];
	};
	message_info info;



	/**
 	 * Prints the structure
 	 * @param fp - the file stream to print to
 	 */

	void print(FILE* fp)
	{
		fprintf(fp, "%ld %d", mtype, info.size);
	}
};
