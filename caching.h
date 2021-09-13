#ifndef __CACHING__
#define __CACHING__ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "structs.h"

#define CACHE_SIZE 5
#define TIMESTAMP_LEN 25

cache_entry_t *create_empty_cache();
void add_to_cache(cache_entry_t *cache,char *domain_name, int ttl, unsigned char *full_msg, int msg_len);
bool check_empty_cache(cache_entry_t *cache);
int in_cache(cache_entry_t *cache, char *domain_name);
bool get_from_cache(cache_entry_t *cache, char *domain_name,cache_entry_t *record);
void edit_trans_id(unsigned char *full_msg,unsigned char trans_id[2]);
void reduce_ttl(cache_entry_t *cache_record);
void renew_cache_record(cache_entry_t *cache,char *domain_name, int ttl, unsigned char *full_msg, int msg_len, int existing);
cache_entry_t *get_expired_rec(cache_entry_t *cache);
void update_cache_ttl(cache_entry_t *cache);
void print_cache_record(cache_entry_t *one_record);
void print_cache(cache_entry_t *cache);
void print_bytes_cache(unsigned char *bytes, int len);

#endif
