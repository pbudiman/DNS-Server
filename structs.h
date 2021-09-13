#ifndef __STRUCTS__
#define __STRUCTS__
#include <arpa/inet.h>

typedef struct {
    unsigned char tran_id[2];
    unsigned char flags[2];
    unsigned char qcount[2];
    int ans_RR_count;
    int auth_RR_count;
    int add_RR_count;
} header_t;

typedef struct {
    char *q_name;
    unsigned char q_type;
    unsigned char q_class[2];
} question_t;

typedef struct {
    unsigned char name[2];
    unsigned char type;
    unsigned char q_class[2];
    int ttl;
    unsigned char rd_len;
    char ip_add[INET6_ADDRSTRLEN];
} answer_t;

typedef struct{
    char *domain_name;
    unsigned char *full_response;
    int ttl;
    int msg_len;
    time_t time_added;
    char time_expire[25];
    int ttl_original;
} cache_entry_t;

#endif
