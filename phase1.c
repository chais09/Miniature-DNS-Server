#define _POSIX_C_SOURCE 200112L
#define HEADER_SIZE 12

#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main(int argc, char* argv[]) {
    int n, domain_name_size = 0;
    int header_id[2], rcode[4];
	unsigned char buffer[256];
    int qr = 0, aa = 0, ra = 0, z = 0, ad = 0, cd = 0, qdcount = 0, ancount = 0, nscount = 0, arcount = 0;
    char domain_name[256];
    int qtype = 0, qclass = 0;
    int buffer_temp = 0;
    int ans_name[2], TTL[4];
    int atype = 0, aclass = 0, rdlength = 0;
    char ipv6_s[256];
    struct tm *info;
    time_t raw_time;
    char time_buffer[256];

    // time(&raw_time);
    FILE *fp;
    fp  = fopen ("dns_svr.log", "w");
    
    int size;
    // Read message from server
    memset(buffer, 0, 256);
    memset(time_buffer, 0, 256);
    memset(header_id,0,2);
    memset(ans_name,0,2);
    memset(TTL,0,4);
    memset(domain_name,0,256);
    memset(ipv6_s,0,256);
    // while(1){
        n = read(0, buffer, 2);
        if (n < 0) {
            // break;
            perror("read");
            exit(EXIT_FAILURE);
        }
       
        size = (((int)buffer[0])*16*16) + (int)buffer[1];
        printf("size = %d\n",size);

        n = read(0, buffer, size);



        // header read start here
        for (int count = 0; count < HEADER_SIZE; count++){
            n = read(0, buffer, 1);
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
                rcode[0] = (buffer[count]>>3 & 1);
                rcode[1] = (buffer[count]>>2 & 1);
                rcode[2] = (buffer[count]>>1 & 1);
                rcode[3] = (buffer[count]>>0 & 1);
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


            if (n < 0) {
                // break;
                perror("read");
                exit(EXIT_FAILURE);
            }

            // printf("buffer = %x\n",buffer[0]);
        }

        printf("id: %x %x, QR= %d, aa= %d, ra= %d, z= %d, ad= %d, cd= %d, qcount= %d, ancount= %d, nscount= %d, arcount =%d\n",
        header_id[0], header_id[1], qr, aa, ra,z,ad,cd,qdcount,ancount,nscount,arcount);


        

        buffer_temp = HEADER_SIZE;
        // printf("buffer[buffer_temp] = %d\n",buffer[buffer_temp]);

        // question read start here
        int label_size = 0;
        int qname_size = 0;
        for (int count = 0; count < size-HEADER_SIZE; count ++){
            // n = read(0, buffer, 1);
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
        
            // domain_name_size++;
            label_size-- ;
        }
        
        buffer_temp++;
        printf("domain_name_size = %d\n",domain_name_size);
        printf("domain_name: %s\n",domain_name);
        // printf("buffer[buffer_temp] = %d\n",buffer[buffer_temp]);
        qtype = (((int)buffer[buffer_temp++])*16*16) + (int)buffer[buffer_temp+1];
        buffer_temp++;
        qclass = (((int)buffer[buffer_temp++])*16*16) + (int)buffer[buffer_temp+1];
        buffer_temp++;
        printf("qtype = %d, qclass = %d\n",qtype,qclass);
        if (qr == 0){
            memset(time_buffer,0,256);
            time(&raw_time);
            info = localtime( &raw_time );
            strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
            // printf("time_buffer = %s\n",time_buffer);
            fprintf(fp, "%s requested %s\n",time_buffer, domain_name);
        }
        if (qtype != 28){
            memset(time_buffer,0,256);
            info = localtime( &raw_time );
            strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
            // printf("time_buffer = %s\n",time_buffer);
            fprintf(fp, "%s unimplemented request\n",time_buffer);
        }

        // answer read start here
        if (ancount > 0){
            ans_name[0] = buffer[buffer_temp++];
            ans_name[1] = buffer[buffer_temp++];
            atype = (((int)buffer[buffer_temp++])*16*16) + (int)buffer[buffer_temp+1];
            buffer_temp++;
            aclass = (((int)buffer[buffer_temp++])*16*16) + (int)buffer[buffer_temp+1];
            buffer_temp++;
            TTL[0] = buffer[buffer_temp++];
            TTL[1] = buffer[buffer_temp++];
            TTL[2] = buffer[buffer_temp++];
            TTL[3] = buffer[buffer_temp++];
            rdlength = (((int)buffer[buffer_temp++])*16*16) + (int)buffer[buffer_temp+1];
            buffer_temp++;


            int rdata[rdlength];
            memset(rdata, 0, rdlength);
            int zero_time = 0;
            // make the ipv6 string and also save in rdata
            for (int i = 0; i< rdlength; i++){
                char temp[256];
                memset(temp,0,256);
                int iszero = 0;
                if (i%2 == 0){
                    int temp_int = (((int)buffer[buffer_temp])*16*16) + (int)buffer[buffer_temp+1];
                    if (temp_int == 0){
                        iszero = 1;
                        if (zero_time == 0){
                            char *a = ":";
                            strcat(ipv6_s,a);
                            zero_time++;
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
            // for (int i = 0; i< rdlength; i++){
            //     printf("RDATA[%d] = %x\n", rdlength,rdata[i]);
            // }

            printf("ipv6_s = %s\n", ipv6_s);
            
            if (atype == 28 && qr==1){
                memset(time_buffer,0,256);
                time(&raw_time);
                info = localtime( &raw_time );
                strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
                // printf("time_buffer = %s\n",time_buffer);
                fprintf(fp, "%s %s is at %s\n",time_buffer, domain_name,ipv6_s);
            }
            else if(atype != 28){
                memset(time_buffer,0,256);
                time(&raw_time);
                info = localtime( &raw_time );
                strftime(time_buffer, sizeof(time_buffer), "%FT%T%z", info);
                // printf("time_buffer = %s\n",time_buffer);
                fprintf(fp, "%s unimplemented request\n",time_buffer);
            }
        }




    return 0;
}
