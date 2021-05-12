
typedef struct {
    unsigned char tran_id[2];
    unsigned char flags[2];
    unsigned char qcount[2];
    int ans_RR_count;
    int auth_RR_count;
    int add_RR_count;
} header_t;

typedef struct {
    unsigned char qr;
    unsigned char opcode[4];
    unsigned char aa;
    unsigned char tc;
    unsigned char rd;
    unsigned char ra;
    unsigned char z[3];
    unsigned char rcode[4];
} flag_bin_t;

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

typedef struct {
   header_t header;
   question_t question;
} req_t;

typedef struct {
   header_t header;
   question_t question;
   answer_t answer;
} res_t;

