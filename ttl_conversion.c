/* 
 * conversion.c
 * TTl bytes conversion
 * 
 * Patricia Angelica Budiman (1012861)
 */ 

#include "ttl_conversion.h"

/* Convert ttl Hex bytes in string to hex bytes
 * From: https://stackoverflow.com/questions/3408706/hexadecimal-string-to-byte-array-in-c  
 */
void ttl_hexstr_to_bytes(char* hex_str,unsigned char* hex_bytes) {
    int ttl_bytes_str_len = 8;
    size_t index = 0;
    while (index < ttl_bytes_str_len) {
        char c = hex_str[index];
        int value = 0;
        if(c >= '0' && c <= '9')
          value = (c - '0');
        else if (c >= 'A' && c <= 'F') 
          value = (10 + (c - 'A'));
        else if (c >= 'a' && c <= 'f')
          value = (10 + (c - 'a'));
        

        hex_bytes[(index/2)] += value << (((index + 1) % 2) * 4);

        index++;
    }

}

/* Convert TTL hex bytes into decimal */
int ttl_hex_to_dec(unsigned char *full_bytes, int index){
    char bytes_str[3];
    char ttl_bytes_str[9] = ""; // 4 hex bytes into string
    char *p;
    int decimal_num; 
    /* Parse hex bytes to string */
    for(int i=0;i<4;i++){
        sprintf(bytes_str, "%02x", full_bytes[index+i]);
        strcat(ttl_bytes_str,bytes_str);
    }

    decimal_num = strtol(ttl_bytes_str, &p, 16);
    return decimal_num;
}
