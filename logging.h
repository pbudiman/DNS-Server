#ifndef __LOGGING__
#define __LOGGING__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "structs.h"

#define TIMESTAMP_SIZE 25
#define A 1
#define AAAA 28
#define REQUEST 0
#define RESPONSE 1

void log_message(unsigned char *qr,header_t *header,int next_loc, unsigned char *full_msg, question_t *question,answer_t *answer,cache_entry_t *record, bool cache_renewal);
void log_req(question_t *question);
void log_res(question_t *question, answer_t *answer,header_t *header,cache_entry_t *record);
bool is_a_response(unsigned char *qr,header_t *header);
bool is_a_request(unsigned char *qr); 
void log_replacement(char *prev_name, char *new_name);

#endif


