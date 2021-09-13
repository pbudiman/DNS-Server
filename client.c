/* 
 * client.c 
 * Functions related to local server acting as client for upstream
 * 
 * Patricia Angelica Budiman (1012861)
 */

#include "client.h"

/* Local server acting as client for upstream
 * Based on week 9 practical: server.c & client.c */
unsigned char *client_for_upstream( unsigned char* client_msg, int msg_len, char *port, char *ipv4, int *total_msg_len){
	printf("Local Mock Server as client START here\n ");
	unsigned char res_len_bytes[2], *res_msg, *original_res;
	int upstream_sockfd, num_bytes, res_msg_len;
	struct addrinfo hints, *servinfo, *rp;

	/* Create address */
	memset(&hints, 0, sizeof hints);
	printf("memset hints ");
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Get addrinfo of server.  */
	if ((getaddrinfo(ipv4, port, &hints, &servinfo)) < 0) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}

	/* Connect to first valid result */
	for (rp = servinfo; rp != NULL; rp = rp->ai_next) {
		upstream_sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (upstream_sockfd == -1)
			continue;
		if (connect(upstream_sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break; 

		close(upstream_sockfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(EXIT_FAILURE);
	}

	/* Forward request to upstream */
	printf("sending to upstream\n");
	num_bytes = send(upstream_sockfd,client_msg,msg_len+2,0);
	if (num_bytes < 0) {
		perror("send");
		exit(EXIT_FAILURE);
	}
	printf("length of data sent to upstream %d\n",num_bytes);

	/* Receive the length of response message from upstream - Two header bytes */
	num_bytes = recv(upstream_sockfd,res_len_bytes,TWO_BYTES_HEADER,0);
	if (num_bytes < 0) {
		perror("receive");
		exit(EXIT_FAILURE);
	}

	printf("length of data recv from upstream %02x %02x\n",(res_len_bytes[0]),(res_len_bytes[1]));
	printf("length of data recv from upstream %d\n",(res_len_bytes[0]+res_len_bytes[1]));
	
	/* Length of the response message from the Two bytes header */
	res_msg_len = res_len_bytes[0]+res_len_bytes[1];

	/* Read the rest of the response bases of the length */
	res_msg=(unsigned char*)malloc(sizeof(*res_msg)*res_msg_len);
	num_bytes = recv(upstream_sockfd,res_msg,res_msg_len,0);
	if (num_bytes < 0) {
		perror("receive");
		exit(EXIT_FAILURE);
	}

	print_bytes(res_msg,num_bytes);

	/* Update length of response length for use in local server*/
	(*total_msg_len) = res_msg_len + TWO_BYTES_HEADER;

	/* Combine response's header and the rest of the message*/
	original_res = combine_header_msg(res_len_bytes,res_msg,res_msg_len);

	free(res_msg);
	close(upstream_sockfd);
	freeaddrinfo(servinfo);

	printf("Local Mock Server as client is CLOSED here\n");

	return original_res;
}

/* Combine msg length bytes with the rest of the message */
unsigned char *combine_header_msg(unsigned char *len_bytes, unsigned char *msg, int msg_len){
	unsigned char *whole_msg;
	whole_msg = (unsigned char*)malloc(sizeof(*whole_msg)*(msg_len+2));
	whole_msg[0] = len_bytes[0];
	whole_msg[1] = len_bytes[1];
	for(int i=0;i<msg_len;i++){
		whole_msg[i+2]=msg[i];
	}
	return whole_msg;
}

void print_bytes(unsigned char *bytes, int len){
	for (int i=0;i<len;i++){
		printf("%02x - ",bytes[i]);
	}
	printf("\n");
}

/* Handle query from client/server */
answer_t *handle_query(unsigned char *full_msg, header_t *header,question_t *question,cache_entry_t *record, bool cache_renewal){
    unsigned char *qr;
	int next_loc;
	printf("CACHING\n");
	/* Put header to structure */
    put_to_header(header,full_msg);
	
    /* Find message type - QR : message is a question/request or response */
    qr = (unsigned char*)malloc(sizeof(*qr));
	assert(qr!=NULL);
    get_message_type(header->flags,qr);
	
	/* Parse question into structure and get next index to read next section */
    next_loc = parse_question(question,full_msg);
	printf("error her %s\n",question->q_name);
    /* Log query based on whether it's a request or response */
	answer_t *answer = (answer_t*)malloc(sizeof(answer_t));
    log_message(qr,header,next_loc,full_msg,question,answer,record,cache_renewal);

	return answer;

}


/* Get the message part of the query without the Two header bytes */
unsigned char *get_res_msg(unsigned char* original_res,int msg_len){

	unsigned char *res_msg;
	res_msg = (unsigned char*)malloc(sizeof(*res_msg)*msg_len);
	for(int i=0;i<msg_len;i++){
		res_msg[i]=original_res[i+2];
	}
	return res_msg;
}

/* Check whether question is of type AAAA */
bool is_non_AAAA(question_t *question){
	return (question->q_type!=AAAA);
}
