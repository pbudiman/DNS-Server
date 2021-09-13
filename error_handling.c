/* 
 * error_handling.c 
 * Functions handling error non-AAAA record request
 * Respond with RCODE 4
 * 
 * Patricia Angelica Budiman (1012861)
 */

#include "error_handling.h"

/* Handle RCODE 4 error 
 * Assuming: Opcode 0 TC 0 RD 1*/
void rcode_four_error(unsigned char *query_msg){
    /* 2nd byte of the Two Bytes Flag always ending with 0 
     * as RCODE will always be 0(no error cond) in the query from client 
     * Not Implemented RCODE : 4 */
    unsigned char rcode4 = (query_msg[3] + NOT_IMPL_RCODE);
    query_msg[3] = rcode4;

    /* Handle Recursion Available to 1 manually as req not forwarded upstream 
     * [qr opcode aa tc rd]
     * [1  0000   1   0  1] = 85 in hex = 133 in decimal
     */
    query_msg[2] = HEX_EIGHTYFOUR;

}
