#ifndef __CLIENT__
#define __CLIENT__ 

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parsing_utils.h"
#include "logging.h"

#define TWO_BYTES_HEADER 2

unsigned char *client_for_upstream( unsigned char *client_msg, int msg_len, char *port, char *ipv4, int *len);
unsigned char *combine_header_msg(unsigned char *len_bytes, unsigned char* msg,int msg_len);
void print_bytes(unsigned char *bytes, int len);
answer_t *handle_query(unsigned char *full_msg, header_t *header,question_t *question,cache_entry_t *record, bool cache_renewal);
unsigned char *get_res_msg(unsigned char* original_res,int msg_len);
bool is_non_AAAA(question_t *question);

#endif
