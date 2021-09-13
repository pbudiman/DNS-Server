#ifndef __UTILS__
#define __UTILS__  

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

#include "structs.h"

#define _OPEN_SYS_SOCK_IPV6
#define MSG_BYTES_LEN 2
#define HEADER_LEN 12
#define TIMESTAMP_SIZE 25
#define A 1
#define AAAA 28

void put_to_header(header_t *header,unsigned char *full_msg);
void print_header(header_t *header);
void get_message_type(unsigned char flag_bytes[],unsigned char *qr);
int parse_question(question_t *question,unsigned char *full_msg);
int parse_answer(answer_t *answer,int ans_start_index,unsigned char *full_msg);
void ipv6_parsing(unsigned char *full_msg,answer_t *answer,int index);
bool add_colon(int i);
char *get_domain_name(int start_index,int *final_index, unsigned char *full_msg);
int ttl_hex_to_dec(unsigned char *full_bytes, int index);

#endif
