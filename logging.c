/* 
 * logging.c 
 * Functions related to Logging
 * 
 * Patricia Angelica Budiman (1012861)
 */

#include "logging.h"
#include "parsing_utils.h"

/* Log message based on whether it's a request or response */
void log_message(unsigned char *qr,header_t *header,int next_loc, unsigned char *full_msg, question_t *question,answer_t *answer,cache_entry_t *record, bool cache_renewal){
    /* Check whether message is a question/request or response  log */
    if(is_a_response(qr,header)){
        /* Parse and store answer */
        parse_answer(answer,next_loc,full_msg);
        // if(cache_renewal)log_replacement(question->q_name);
        log_res(question,answer,header,record);
    }else if(is_a_request(qr)){
        log_req(question);
        printf("req printed ok\n");
    }

} 

/* Log request 
 * Timestamp based on https://www.geeksforgeeks.org/strftime-function-in-c/ */
void log_req(question_t *question){
    FILE *log_file;
    time_t t ;
    struct tm *tmp ;
    char timestamp[TIMESTAMP_SIZE];
    time( &t );

    log_file  = fopen ("dns_svr.log", "a");
    tmp = localtime( &t );
    
    /* Use strftime to display time */
    strftime(timestamp, sizeof(timestamp), "%FT%T%z", tmp);
    fprintf(log_file, "%s requested %s\n",timestamp,question->q_name);
     
    /* When request type is not AAAA */
    if(question->q_type!=AAAA){
        fprintf(log_file, "%s unimplemented request\n",timestamp);
    }

    fflush(log_file);
}

/* Log response 
 * Timestamp based on https://www.geeksforgeeks.org/strftime-function-in-c/ */
void log_res(question_t *question, answer_t *answer, header_t *header, cache_entry_t *record){
    FILE *log_file; 
    time_t t ;
    struct tm *tmp ;
    char timestamp[TIMESTAMP_SIZE];
    time( &t );
    tmp = localtime( &t );
    log_file  = fopen ("dns_svr.log", "a");
    
    strftime(timestamp, sizeof(timestamp), "%FT%T%z", tmp);
    
    if(record!=NULL){
        fprintf(log_file, "%s %s expires at %s\n",timestamp,question->q_name,record->time_expire);
    }

    /* When there's answer & the type is AAAA */
    if(header->ans_RR_count!=0 && (answer->type==AAAA)){
        fprintf(log_file, "%s %s is at %s\n",timestamp,question->q_name,answer->ip_add);
    }

    fflush(log_file);
}

/* Log replacement in cache
 * Timestamp based on https://www.geeksforgeeks.org/strftime-function-in-c/ */
void log_replacement(char *prev_name, char *new_name){
    FILE *log_file; 
    time_t t ;
    struct tm *tmp ;
    char timestamp[TIMESTAMP_SIZE];
    time( &t );
    tmp = localtime( &t );
    log_file  = fopen ("dns_svr.log", "a");
    
    strftime(timestamp, sizeof(timestamp), "%FT%T%z", tmp);
    
    fprintf(log_file, "%s replacing %s by %s\n",timestamp,prev_name,new_name);
    
    fflush(log_file);
}


/* Check if query is a reponse */
bool is_a_response(unsigned char *qr,header_t *header){
    int q_type = *qr ;
    return (q_type == RESPONSE);
}

/* Check if query is a request */
bool is_a_request(unsigned char*qr){
    int q_type = *qr ;
    return (q_type == REQUEST);
}

