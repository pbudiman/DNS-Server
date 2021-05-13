#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#define _OPEN_SYS_SOCK_IPV6
#include <arpa/inet.h>

#include "structs.h"

#define MSG_BYTES_LEN 2
#define HEADER_LEN 12
#define TIMESTAMP_SIZE 25
#define A 1
#define AAAA 28
#define REQUEST 0
#define RESPONSE 1
#define NOT_IMPL_RCODE 4
#define HEX_EIGHTY 128


void log_req(question_t *question);
void log_res(question_t *question, answer_t *answer,header_t *header);
void put_to_header(header_t *header,unsigned char *full_msg);
void print_header(header_t *header);
void get_message_type(unsigned char flag_bytes[],unsigned char *qr);
int parse_question(question_t *question,unsigned char *full_msg);
void parse_answer(answer_t *answer,int ans_start_index,unsigned char *full_msg);
void handle_ipv6(unsigned char *full_msg,answer_t *answer,int index);
bool add_colon(int i);
bool is_a_response(unsigned char *qr,header_t *header);
bool is_a_request(unsigned char *qr);
int get_domain_name(int start_index, char *domain_name, unsigned char *full_msg);
void log_message(unsigned char *qr,header_t *header,int next_loc, unsigned char *full_msg, question_t *question);
void rcode_four_error(unsigned char *flag_bytes);
