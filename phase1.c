/* 
 * phase1.c 
 * Functions to test parsing
 * 
 * Patricia Angelica Budiman (1012861)
 */

#include "parsing_utils.h"
#include "logging.h"

int main(int argc, char* argv[]) {
    unsigned char *msg_len_bytes;
    int msg_len;
    unsigned char *full_msg;
    header_t *header;
    unsigned char *qr;
    question_t *question;

    /* Read the two bytes header for length of message */
    msg_len_bytes = (unsigned char*)malloc(sizeof(*msg_len_bytes)*MSG_BYTES_LEN);
    read(STDIN_FILENO, msg_len_bytes, MSG_BYTES_LEN);
    msg_len = msg_len_bytes[0] + msg_len_bytes[1];
    
    /* Store full message based on message length */
    full_msg = (unsigned char*)malloc(sizeof(*full_msg)*msg_len);
    read(STDIN_FILENO, full_msg, msg_len);

    /* Put header to structure */
    header = (header_t *)malloc(sizeof(header_t));
    put_to_header(header,full_msg);
    print_header(header);

    /* Find message type - QR : message is a question/request or response */
    qr = (unsigned char*)malloc(sizeof(qr));
    get_message_type(header->flags,qr);

    /* Put question to structure */
    question = (question_t*)malloc(sizeof(question));
    int next_loc = parse_question(question,full_msg);

    /* Check whether message is a question/request or response & log */
    if(is_a_response(qr,header)){
        /* Parse and store answer */
        answer_t *answer = (answer_t*)malloc(sizeof(answer_t));
        parse_answer(answer,next_loc,full_msg);
        log_res(question,answer,header,NULL);
    }else if( is_a_request(qr)){
        log_req(question);
    }
    return 0;
}

