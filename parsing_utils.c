/* 
 * parsing_utils.c 
 * Functions related Parsing query's messages
 * 
 * Patricia Angelica Budiman (1012861)
 */

#include "parsing_utils.h"
#include "ttl_conversion.h"

/* Extract info from question and put into structure */
int parse_question(question_t *question,unsigned char *full_msg){
    /* Question Bytes start location 
     * in the array of the full msg bytes after header */
    int curr_loc = 12;
    char *domain_name;
    /* Bytes location after reading domain name */
    int next_loc;

    /* Get domain name */
    domain_name = get_domain_name(curr_loc,&next_loc,full_msg);
    question->q_name = malloc(sizeof(char)*strlen(domain_name));
    strcpy(question->q_name, domain_name);
    
    /* Next two bytes after domain name : Question Type */
    question->q_type = full_msg[next_loc]+full_msg[next_loc+1];
    next_loc+=2;

    /* Next two bytes after domain name : Question Class */
    (question->q_class)[0]=full_msg[next_loc];
    (question->q_class)[1]=full_msg[next_loc+1];
    next_loc+=2;

    return next_loc;
    
}

/* Extract info from anwswer and put into structure */
int parse_answer(answer_t *answer,int ans_start_index,unsigned char *full_msg){
    int loc = ans_start_index;

    /* Bytes of domain name */
    answer->name[0]=full_msg[loc];
    answer->name[1]=full_msg[loc+1];

    /* domain name type */
    answer->type=full_msg[loc+2]+full_msg[loc+3];

    /* Domain name class */
    answer->q_class[0]=full_msg[loc+4];
    answer->q_class[1]=full_msg[loc+5];

    /* Time to live in second */
    answer->ttl = ttl_hex_to_dec(full_msg,loc+6);

    /* Length of the ip address */
    answer->rd_len=full_msg[loc+10]+full_msg[loc+11];

    /* Parse the ipv6 into correct format */
    ipv6_parsing(full_msg,answer,loc+12);

    return loc+6;
}

/* Read the domain name and return the next index to read next section */
char* get_domain_name(int start_index, int *final_index, unsigned char *full_msg){
    int section_start_i = 1;
    int domain_name_len = 0;
    char *temp_string = (char*) malloc(sizeof(char));
    char dot ='.';
    size_t len = 0;  // Length of domain name 
    size_t size = 1; // Length of domain name stored 
    int label_len =  full_msg[start_index]; // Length of first label of domain name 

    /* While there's next label/section to be read */
    while (label_len){
        domain_name_len += label_len;
        /* Index of the next section of domain name to be read*/
        int new_sec_index = start_index + section_start_i;

        /* Read each character in the section */
        for(int x=0; x<label_len; x++){
            char temp = (char)full_msg[new_sec_index+x];
            temp_string[len++]=temp;
            if(len==size){
                temp_string = realloc(temp_string, sizeof(*temp_string)*(size+=1));
            }
        }

        /* Move to next section */
        domain_name_len+=1;
        /* Next section starting index, skip the bytes for section length */
        section_start_i +=(label_len+1);
        /* Length of next section */
        label_len = full_msg[start_index + domain_name_len];
        
        /* Add dot '.' if not last section */
        if(label_len){
            temp_string[len++]=dot;
            if(len==size){
                temp_string = realloc(temp_string, sizeof(*temp_string)*(size+=1));
            }
        }
    }

    temp_string[len++]='\0';
    start_index += domain_name_len;
    start_index += 1;

    (*final_index) = start_index;

    return temp_string;
}

/* Get ipv6 address from response into wanted format */
void ipv6_parsing(unsigned char *full_msg,answer_t *answer,int index){
    char one_ipv6_byte[3]; //each byte
    char ipv6_string[INET6_ADDRSTRLEN]="";
    char colon[2]=":";
    unsigned char buffer[sizeof(struct in6_addr)];
    char ipv6[INET6_ADDRSTRLEN];

    /* ipv6 address in Hex bytes to string */
    for(int i=0;i<16;i++){
        sprintf(one_ipv6_byte,"%02x", full_msg[index+i]);
        strcat(ipv6_string,one_ipv6_byte);
        if(add_colon(i)) {
            strcat(ipv6_string,colon);
        }
    }

    inet_pton(AF_INET6, ipv6_string, buffer);
    inet_ntop(AF_INET6, buffer, ipv6, INET6_ADDRSTRLEN);
    strcpy(answer->ip_add,ipv6);
}



/* Put header's hex bytes to structure */
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

/* Add colon when 16 bits are read & is not the last 16 bits of ipv6 */ 
bool add_colon(int index){
    return ((((index+1) % 2)==0)&&(index!=0) && (index!=15));
}
