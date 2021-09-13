/* 
 * cache.c
 * Functions related to caching
 * 
 * Patricia Angelica Budiman (1012861)
 */
#include "caching.h"
#include "parsing_utils.h"
#include "client.h"
#include "ttl_conversion.h"

/* Create Empty Cache */
cache_entry_t *create_empty_cache(){
    cache_entry_t *cache;

    cache = (cache_entry_t*)malloc(sizeof(*cache)*CACHE_SIZE);

    for (int i = 0; i < CACHE_SIZE; i++){
        cache[i].domain_name="";
        cache[i].full_response=NULL;
        cache[i].ttl=-1;
        cache[i].ttl_original=-1;
        cache[i].msg_len=-1;
        cache[i].time_added=0;
        strcpy(cache[i].time_expire,"");
    }

    return cache;
}

/* Add a record to Cache */
void add_to_cache(cache_entry_t *cache,char *domain_name, int ttl, unsigned char *full_msg, int msg_len){
    time_t time_added;
    time_t time_exp;
    struct tm *tmp ;
    char timestamp_exp[TIMESTAMP_LEN];
    int i = 0;

    /* Find empty space in cache */
    for (i = 0; i < CACHE_SIZE; i++)
    {
        if(cache[i].ttl<1){
            break;
        }
    }

    /* Store domain name */
    cache[i].domain_name = malloc(sizeof(char)*strlen(domain_name));
    strcpy(cache[i].domain_name,domain_name);

    /* Store full record bytes*/
    cache[i].full_response = malloc(sizeof(unsigned char)*msg_len);
    for(int x = 0; x < msg_len ; x++){
        (cache[i].full_response)[x]=full_msg[x];
    }

    /* Store ttl */
    cache[i].ttl = ttl;
    cache[i].ttl_original = ttl;

    /* Store length of message from Two header bytes */
    cache[i].msg_len=msg_len;

    /* Store time of expire */
    time(&time_added);
    cache[i].time_added = time_added;
    time_exp = time_added + ttl;
    tmp = localtime( &time_exp);
    strftime(timestamp_exp, sizeof(timestamp_exp), "%FT%T%z", tmp);
    strcpy(cache[i].time_expire,timestamp_exp);

}

/* Check whether there's empty space in cache */
bool check_empty_cache(cache_entry_t *cache){
    printf("checking\n");
    
    bool empty_space = false;
    int i;

    for(i=0; i<CACHE_SIZE; i++){
        
        if(cache[i].ttl<1){
            cache[i].ttl=-1;
            empty_space = true;
        }
    }
    return empty_space;

}

/* Check whether a domain name information is in cache
 * If yes, location in cache is returned
 */
int in_cache(cache_entry_t *cache, char *domain_name){
    int record_index = -1;

    for(int i = 0; i<CACHE_SIZE; i++){
        if(strcmp(cache[i].domain_name,domain_name)==0){
            record_index=i;
            break;
        }
    }
    return record_index;
}

/* Get domain name's record from cache 
 * Returns indication whether record retrival from cache is successful/fail 
 */
bool get_from_cache(cache_entry_t *cache, char *domain_name,cache_entry_t *record){
    
    for(int i = 0 ; i < CACHE_SIZE ; i++){

        update_cache_ttl(cache);

        /* When record in cache is found and is not expired */
        if(strcmp(cache[i].domain_name,domain_name)==0 && cache[i].ttl>0){
            
            /* Get cache's record  */
            record->domain_name = malloc(sizeof(char)*strlen(domain_name));
            strcpy(record->domain_name,domain_name);

            record->full_response=malloc(sizeof(unsigned char)*cache[i].msg_len);
            for(int x=0;x<cache[i].msg_len;x++){
                record->full_response[x] = cache[i].full_response[x];
            }

            record->msg_len=cache[i].msg_len;
            record->time_added=cache[i].time_added;

            record->ttl=cache[i].ttl;
            record->ttl_original=cache[i].ttl_original;

            strcpy(record->time_expire,cache[i].time_expire);

            return true;
        }
    }
    /* Record not found/expired */
    record = NULL;
 
    return false;
}

/* Edit Transaction ID to match those from client */
void edit_trans_id(unsigned char *full_msg, unsigned char client_trans_id[2]){
    int loc_after_header = 2;

    full_msg[loc_after_header]=client_trans_id[0];
    full_msg[loc_after_header+1]=client_trans_id[1];
}

/* Reduce ttl of record in cache */
void reduce_ttl(cache_entry_t *cache_record){
    int time_elapsed, updated_ttl, ans_loc, ttl_loc;
    unsigned char *qr;
    header_t *header;
    question_t *question;
    answer_t *answer;
    unsigned char *temp_res_msg;
    time_t time_now;
    unsigned char *hex_bytes;
    char ttl_hex_str[9];
  
    /* Parse full response stored in cache to get to ttl's bytes location */
    header = (header_t*)malloc(sizeof(header));
    question = (question_t*)malloc(sizeof(question));
   
    /* Get message without two headr bytes */
    temp_res_msg = get_res_msg(cache_record->full_response,(cache_record->msg_len-2));
  
	/* Put header to structure */
    put_to_header(header,temp_res_msg);
	
    /* Find message type - QR : message is a question/request or response */
    qr = (unsigned char*)malloc(sizeof(*qr));
    get_message_type(header->flags,qr);
	
	/* Parse question into structure and get next index to read next section */
    ans_loc = parse_question(question,temp_res_msg);

    /* Parse answer & get ttl's bytes location */
	answer = (answer_t*)malloc(sizeof(answer_t));
    ttl_loc = parse_answer(answer,ans_loc,temp_res_msg);

    /* Update ttl in cache record */
    time(&time_now);
    time_elapsed = (time_now - cache_record->time_added);
    updated_ttl = (cache_record->ttl_original - time_elapsed); 
    cache_record->ttl = updated_ttl;

    /* Parse updated ttl decimal to hex bytes in string */
    sprintf(ttl_hex_str, "%08x", cache_record->ttl);
    
    /* Parse updated ttl hex bytes string to hex bytes */
    hex_bytes = (unsigned char*)malloc(sizeof(hex_bytes)*4);
    ttl_hexstr_to_bytes(ttl_hex_str, hex_bytes);

    /* Update ttl's hex bytes in cache full reponse's bytes */
    for(int i=0;i<4;i++){
        cache_record->full_response[i+ttl_loc+2]=hex_bytes[i];
    }

    free(header);
    free(question);
    free(answer);
    free(hex_bytes);


}

/* Renew/replace cache record */
void renew_cache_record(cache_entry_t *cache,char *domain_name, int ttl, unsigned char *full_msg, int msg_len, int existing){
    time_t time_added;
    time_t time_exp;
    struct tm *tmp ;
    char timestamp_exp[25];
    int i = 0;
    
    /* If renewing an existing cache record */
    if(existing){
        for (i = 0; i < CACHE_SIZE; i++){
            if(strcmp(cache[i].domain_name,domain_name)==0) break;
        }
    }/* If replacing a cache record with another new domain name's record */
    else{
        for (i = 0; i < CACHE_SIZE; i++){
            if(cache[i].ttl<1 && (strcmp(cache[i].domain_name,"")!=0))break;
        }
    }

    /* Store new record's information */
    cache[i].domain_name = realloc(cache[i].domain_name,sizeof(domain_name));
    strcpy(cache[i].domain_name,domain_name);

    cache[i].full_response = malloc(sizeof(unsigned char)*msg_len);
    for(int x=0;x<msg_len;x++){
        (cache[i].full_response)[x]=full_msg[x];
    }

    cache[i].ttl_original = ttl;
    cache[i].ttl = ttl;

    cache[i].msg_len = msg_len;

    time(&time_added);
    cache[i].time_added = time_added;

    time_exp = time_added + ttl;
    tmp = localtime( &time_exp);
    strftime(timestamp_exp, sizeof(timestamp_exp), "%FT%T%z", tmp);
    strcpy(cache[i].time_expire,timestamp_exp);

}

/* Get expired cache record */
cache_entry_t *get_expired_rec(cache_entry_t *cache){
    cache_entry_t *rec=  NULL;

    for(int i = 0 ; i < CACHE_SIZE ; i++){

        /* Update each record's ttl before referencing any */
        update_cache_ttl(cache);

        /* If expired */
        if(cache[i].ttl<1 && cache[i].ttl_original>0){
            rec = &cache[i];
            break;
        }
    }
    
    return rec;

}

/* Update each record's ttl in cache */
void update_cache_ttl(cache_entry_t *cache){

    for(int i = 0 ; i < CACHE_SIZE ; i++){
        if(cache[i].ttl>0){
            reduce_ttl(&cache[i]);
        }
    }
}

void print_cache_record(cache_entry_t *one_record){
    printf("+ domain name is %s\n+ ",one_record->domain_name);
    print_bytes_cache(one_record->full_response,one_record->msg_len);
    printf("+ msg len is is %d\n",one_record->msg_len);
    printf("+ time added is %ld\n",one_record->time_added);
    printf("+ time expire is %s\n",one_record->time_expire);
    printf("+ ttl is %d\n",one_record->ttl);
}

void print_bytes_cache(unsigned char *bytes, int len){
	for (int i=0;i<len;i++){
		printf("%02x - ",bytes[i]);
	}
	printf("\n");
}

void print_cache(cache_entry_t *cache){
    printf("CACHE\n");
    for(int i = 0 ; i < CACHE_SIZE ; i++){
       
        printf("index: %d\n",i);
        printf("%s\n",cache[i].domain_name);
        print_bytes_cache(cache[i].full_response,cache[i].msg_len);
        printf("msg len %d\n",cache[i].msg_len);
        printf("ttl %d\n",cache[i].ttl);
        printf("expires at %s\n", cache[i].time_expire);
        
    }

}
