#define HEADER_SIZE 12
#define _POSIX_C_SOURCE 200112L

#include "helper1.h"
#include <stdio.h> 
#include <time.h>
#include <string.h>

// this function will read the input from the client/upstream server
// and return 1 if the input is valid
// while other int if the input is invalid(i.e. not implemented)
int read_input(unsigned char* buffer, int size){
    printf("read_input very first\n");
    FILE *fp;
    fp  = fopen ("dns_svr.log", "a");
    char domain_name[256], ipv6_s[256];
    int domain_name_size = 0;
    int qr = 0, aa = 0, ra = 0, z = 0, ad = 0, cd = 0, qdcount = 0;
    int qtype = 0, qclass = 0, atype = 0, aclass = 0, rdlength = 0;
    int buffer_temp = 0, ancount = 0, nscount = 0, arcount = 0;
    int ans_name[2], TTL[4], header_id[2];
    // int rcode[4];

    // for time
    struct tm *info;
    time_t raw_time;
    char time_buffer[256];

    printf("first read_input\n");

    // memset variables and arrays
    memset(time_buffer, 0, 256);
    memset(header_id,0,2*sizeof(int));
    memset(ans_name,0,2*sizeof(int));
    memset(TTL,0,4*sizeof(int));
    memset(domain_name,0,256);
    memset(ipv6_s,0,256);

    printf("read_input: before read hearder\n");
    // header read start here
    for (int count = 0; count < HEADER_SIZE; count++){
        if (count == 0){
            header_id[0] = buffer[count];
        }
        else if(count == 1){
            header_id[1] = buffer[count];
        }
        else if (count == 2){
            qr = (buffer[count]>>7 & 1);
            aa = (buffer[count]>>3 & 1);

        }
        else if (count == 3){
            ra = (buffer[count]>>7 & 1);
            z = (buffer[count]>>6 & 1);
            ad = (buffer[count]>>5 & 1);
            cd = (buffer[count]>>4 & 1);
            // rcode[0] = (buffer[count]>>3 & 1);
            // rcode[1] = (buffer[count]>>2 & 1);
            // rcode[2] = (buffer[count]>>1 & 1);
            // rcode[3] = (buffer[count]>>0 & 1);
        }
        else if (count == 4){
            qdcount += ((int)buffer[count])*16*16;
        }
        else if (count == 5){
            qdcount += (int)buffer[count];
        }
        else if (count == 6){
            ancount += ((int)buffer[count])*16*16;
        }
        else if (count == 7){
            ancount += (int)buffer[count];
        }
        else if (count == 8){
            nscount += ((int)buffer[count])*16*16;
        }
        else if (count == 9){
            nscount += (int)buffer[count];
        }
        else if (count == 10){
            arcount += ((int)buffer[count])*16*16;
        }
        else if (count == 11){
            arcount += (int)buffer[count];
        }
    }

    printf("id: %x %x, QR= %d, aa= %d, ra= %d, z= %d, ad= %d, cd= %d, qcount= %d, ancount= %d, nscount= %d, arcount =%d\n",
    header_id[0], header_id[1], qr, aa, ra,z,ad,cd,qdcount,ancount,nscount,arcount);


    
    buffer_temp = HEADER_SIZE;
    printf("read_input: before read question\n");


    // question read start here
    int label_size = 0;
    int qname_size = 0;
    for (int count = 0; count < size-HEADER_SIZE; count ++){
        qname_size++;
        if (buffer[buffer_temp] == 0){
            break;
        }
        if (label_size == 0){
            label_size = (int)buffer[buffer_temp++];
            if (count == 0){
                continue;
            }
            else{
                domain_name[domain_name_size++] = '.';
                printf("here\n");
                continue;
            }
        }
        domain_name[domain_name_size++] = (char)buffer[buffer_temp++];

        label_size-- ;
    }
    buffer_temp++;
    printf("domain_name_size = %d\n",domain_name_size);
    printf("domain_name: %s\n",domain_name);
    // qtype
    qtype = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
    buffer_temp+=2;
    // qclass
    qclass = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
    buffer_temp+=2;
    printf("qtype = %d, qclass = %d\n",qtype,qclass);

    printf("read_input: before put into log question part\n");
    // if the input is question
    if (qr == 0){
        printf("qr == 0\n");
        memset(time_buffer,0,256);
        // time
        time(&raw_time); 
        info = localtime( &raw_time );
        // put time into the dns_svr.log
        strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
        printf("time_buffer = %s\n",time_buffer);
        fprintf(fp, "%s requested %s\n",time_buffer, domain_name);
        fflush(fp);
    }
    // when the request is not AAAA(IPV6)
    if (qtype != 28){
        // time
        memset(time_buffer,0,256);
        info = localtime( &raw_time );
        strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
        // put time into the dns_svr.log
        fprintf(fp, "%s unimplemented request\n",time_buffer);
        int code;
        code = ((int)buffer[3]) + 4;
        fflush(fp);
        return code;
    }

    printf("read_input: before read answer\n");
    // answer part read start here
    if (ancount > 0){
        ans_name[0] = buffer[buffer_temp++];
        ans_name[1] = buffer[buffer_temp++];
        atype = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
        buffer_temp+=2;
        aclass = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
        buffer_temp+=2;
        TTL[0] = buffer[buffer_temp++];
        TTL[1] = buffer[buffer_temp++];
        TTL[2] = buffer[buffer_temp++];
        TTL[3] = buffer[buffer_temp++];
        rdlength = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
        buffer_temp+=2;

        // ipv6 read start here(rdata in answer part in dns)
        int rdata[rdlength];
        memset(rdata, 0, rdlength);
        int zero_time = 0;
        // make the ipv6 string and also save in rdata
        printf("read_input: before ipv6\n");
        for (int i = 0; i< rdlength; i++){
            char temp[256];
            memset(temp,0,256);
            int iszero = 0;
            if (i%2 == 0){
                int temp_int = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
                if (temp_int == 0){
                    iszero = 1;
                    if (zero_time == 0){
                        if (i != 0){
                            char *a = ":";
                            strcat(ipv6_s,a);
                            zero_time++;
                        }
                        else if(i == 0){
                            char *a = "::";
                            strcat(ipv6_s,a);
                            zero_time++;
                        }
                    }
                }
                else{
                    sprintf(temp, "%x", temp_int);
                    strcat(ipv6_s,temp);
                }
                if (i != (rdlength -2)){
                    if (!iszero){
                        char *a = ":";
                        strcat(ipv6_s,a);
                    }
                }
            }
            rdata[i] = buffer[buffer_temp];
            buffer_temp++;
        }

        printf("ans_name = %x %x\n", ans_name[0],ans_name[1]);
        printf("atype = %d, aclass = %d\n",atype,aclass);
        printf("TTL = %x %x %x %x\n", TTL[0],TTL[1],TTL[2],TTL[3]);
        printf("ipv6_s = %s\n", ipv6_s);
        printf("read_input: before put log answer part\n");

        // when the input is answer and type is AAAA(IPV6)
        if (atype == 28 && qr==1){
            memset(time_buffer,0,256);
            time(&raw_time);
            info = localtime( &raw_time );
            strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
            // printf("time_buffer = %s\n",time_buffer);
            fprintf(fp, "%s %s is at %s\n",time_buffer, domain_name,ipv6_s);
            fflush(fp);
        }      
    }

    return 1;
}
