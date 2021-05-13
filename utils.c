#include "utils.h"

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

    if(question->q_type!=AAAA){
        fprintf(log_file, "%s unimplemented request\n",timestamp);
    }

    fflush(log_file);
}

/* Put header's hex bytes to structure*/
void put_to_header(header_t *header,unsigned char *full_msg){
    unsigned char id[2] = {full_msg[0],full_msg[1]};
    unsigned char flags[2] = {full_msg[2],full_msg[3]};
    unsigned char q_count[2] = {full_msg[4],full_msg[5]};
    int ans_RR = full_msg[6]+full_msg[7];
    int auth_RR = full_msg[8]+full_msg[9];
    int add_RR = full_msg[10]+full_msg[11];
    
    memcpy(header->tran_id, id, sizeof(id));
    memcpy(header->flags, flags, sizeof(flags));
    memcpy(header->qcount, q_count, sizeof(q_count));
    header->ans_RR_count = ans_RR;
    header->auth_RR_count = auth_RR;
    header->add_RR_count = add_RR;

}

void print_header(header_t *header){
    printf("Transaction ID %02x %02x \nFlags %02x %02x \nQcount  %02x %02x \nAnswer RR %d\nAuth RR %d\nAdditional RR %d\n",
    header->tran_id[0],header->tran_id[1],
    header->flags[0],header->flags[1],
    header->qcount[0],header->qcount[1],
    header->ans_RR_count,
    header->auth_RR_count,
    header->add_RR_count);
}

/* Get whether message is a request or response */
void get_message_type(unsigned char flag_bytes[], unsigned char *qr){
    char first_byte[3];
    char second_byte[3];

    sprintf(first_byte,"%02x", flag_bytes[0]);
    sprintf(second_byte, "%02x", flag_bytes[1]); 

    /* Right shift to get the first bit */
    (*qr) = (flag_bytes[0]>>7);
}

/* Extract important info from question and put into structure */
int parse_question(question_t *question,unsigned char *full_msg){
    /* Bytes location in the array of the full msg bytes after header */
    int curr_loc = 12;
    char *domain_name;
    /* Bytes location after reading domain name */
    int next_loc;

    domain_name = (char*)malloc(sizeof(char));
    next_loc = get_domain_name(curr_loc,domain_name,full_msg);
    question->q_name = domain_name;

    /* Next two bytes after domain name : Question Type */
    question->q_type = full_msg[next_loc]+full_msg[next_loc+1];
    printf("pasring question type %d\n",question->q_type);
    next_loc+=2;

    /* Next two bytes after domain name : Question Class */
    (question->q_class)[0]=full_msg[next_loc];
    (question->q_class)[1]=full_msg[next_loc+1];
    next_loc+=2;

    return next_loc;
    
}

void parse_answer(answer_t *answer,int ans_start_index,unsigned char *full_msg){
    int loc = ans_start_index;
    answer->name[0]=full_msg[loc];
    answer->name[1]=full_msg[loc+1];
    answer->type=full_msg[loc+2]+full_msg[loc+3];
    printf("ANSWER TYPE IS %d\n",answer->type);
    answer->q_class[0]=full_msg[loc+4];
    answer->q_class[1]=full_msg[loc+5];
    answer->ttl = full_msg[loc+6]+full_msg[loc+7]+full_msg[loc+8]+full_msg[loc+9];
    answer->rd_len=full_msg[loc+10]+full_msg[loc+11];
    handle_ipv6(full_msg,answer,loc+12);
}

/* Get ipv6 address from response into wanted format */
void handle_ipv6(unsigned char *full_msg,answer_t *answer,int index){
    char each_byte[16][2];
    char ipv6_string[INET6_ADDRSTRLEN]="";
    char colon[2]=":";
    unsigned char buffer[sizeof(struct in6_addr)];
    char ipv6[INET6_ADDRSTRLEN];

    /* ipv6 address in Hex bytes to string */
    for(int i=0;i<16;i++){
        sprintf(each_byte[i],"%02x", full_msg[index+i]);
        strcat(ipv6_string,each_byte[i]);
        if(add_colon(i)) strcat(ipv6_string,colon);
    }

    inet_pton(AF_INET6, ipv6_string, buffer);
    inet_ntop(AF_INET6, buffer, ipv6, INET6_ADDRSTRLEN);
    strcpy(answer->ip_add,ipv6);
}

bool add_colon(int index){
    return ((((index+1) % 2)==0)&&(index!=0) && (index!=15));
}

/* Log response 
 * Timestamp based on https://www.geeksforgeeks.org/strftime-function-in-c/ */
void log_res(question_t *question, answer_t *answer, header_t *header){
    FILE *log_file; 
    time_t t ;
    struct tm *tmp ;
    char timestamp[TIMESTAMP_SIZE];
    time( &t );
    tmp = localtime( &t );
    log_file  = fopen ("dns_svr.log", "a");
    
    strftime(timestamp, sizeof(timestamp), "%FT%T%z", tmp);

    if(header->ans_RR_count!=0 && (answer->type==AAAA)){
        fprintf(log_file, "%s %s is at %s\n",timestamp,question->q_name,answer->ip_add);
    }

    fflush(log_file);
}


bool is_a_response(unsigned char *qr,header_t *header){
    int q_type = *qr ;
    return (q_type == RESPONSE);
}

bool is_a_request(unsigned char*qr){

    int q_type = *qr ;
    return (q_type == REQUEST);
}

int get_domain_name(int start_index, char *domain_name, unsigned char *full_msg){
    /* Index to read next section of domain name */
    int section_start_i = 1;
    /* Total length of domain name */
    int domain_name_len = 0;
    size_t len = 0;
    /* Length of domain name stored */
    size_t size = 1;
    char dot ='.';
    /* Length of first label of domain name */
    int label_len =  full_msg[start_index];

    /* While there's next label/section to be read */
    while (label_len){
        domain_name_len += label_len;
        printf("Start Loc: %d, Local Loc: %d\n",start_index,section_start_i);
        /* Index of the next section of domain name to be read*/
        int new_sec_index = start_index+section_start_i;

        /* Read each character in the section */
        for(int x=0; x<label_len; x++){
            char temp = (char)full_msg[new_sec_index+x];
            domain_name[len++]=temp;
            if(len==size){
                domain_name = realloc(domain_name, sizeof(*domain_name)*(size+=1));
            }
        }

        /* Move to next section */
        domain_name_len+=1;
        /* Next section starting index, skip the bytes for section length */
        section_start_i +=(label_len+1);
        /* Length of next section */
        label_len = full_msg[start_index + domain_name_len];
        
        /* Add dot '.' if not last section*/
        if(label_len){
            domain_name[len++]=dot;
            if(len==size){
                domain_name = realloc(domain_name, sizeof(*domain_name)*(size+=1));
            }
        }
    }
    domain_name[len++]='\0';

    printf("domain name is %s\n",domain_name);
    printf("domain name Len is %zu\n",len);
    start_index += domain_name_len;
    start_index += 1;

    return start_index;
}

void log_message(unsigned char *qr,header_t *header,int next_loc, unsigned char *full_msg, question_t *question){
    /* Check whether message is a question/request or response & log */
    if(is_a_response(qr,header)){
        /* Parse and store answer */
        answer_t *answer = (answer_t*)malloc(sizeof(answer_t));
        parse_answer(answer,next_loc,full_msg);
        log_res(question,answer,header);
    }else if( is_a_request(qr)){
        log_req(question);
    }

}

void rcode_four_error(unsigned char *query_msg){
    /* 2nd byte of the Two Bytes Flag always ending with 0 
     * as RCODE will always be 0(no error cond) for query from client 
     * Not Implemented RCODE : 4 */
    printf("query msg 3 is %02x\n",query_msg[3]);
    unsigned char temp=(query_msg[3]+NOT_IMPL_RCODE);
    
    query_msg[3] = temp;
    printf("NON VALID QUERY CHANGED RCODE TO 4 - %02x\n",query_msg[3]);
}
