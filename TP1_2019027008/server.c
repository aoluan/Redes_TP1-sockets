#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    int eqp_sensor[4][4],i=0,j=0,qnts=0;

    for ( i=0; i<4; i++ ){
        for ( j=0; j<4; j++ ){
        eqp_sensor[i][j]=0;
        }
    }
    
    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);

        while (1) {
            size_t count = 0;
            memset(buf, 0, BUFSZ);

            while(buf[strlen(buf)-1] != '\n'){ 
                count = count + recv(csock, buf + count, BUFSZ - count, 0);
                
            }

            printf("%s", buf);

            //----------------------------------------------

            char *kill = strstr(buf, "kill");

            if (kill!=NULL) {
                logexit("kill");
            } 

            char buf2[BUFSZ];
            int tam_buf=0;
            strcpy(buf2, buf);
            char *cmd = strtok(buf2," ");
            
            while(cmd){
                tam_buf++;
                cmd = strtok(NULL," ");                
            }

            cmd = strtok(buf," ");

            //tratamento da mensagem
            if(strstr(buf, "add")!= NULL){
                int add1=0, add2=0, add3=0, eqp=0;
                cmd = strtok(NULL," ");
                if(tam_buf==5){
                    add1 = atoi(strtok(NULL," "));
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(eqp>4){
                        sprintf(buf, "invalid sensor \n");
                        break;
                    }
                    if(qnts<15){
                        if(eqp_sensor[eqp-1][add1-1]==0){
                            sprintf(buf, "sensor 0%i added \n", add1);
                            eqp_sensor[eqp-1][add1-1] = 1;
                            qnts++;
                        }else{
                            sprintf(buf, "sensor 0%i already exist in 0%i \n", add1, eqp);
                        }
                    }else{
                        sprintf(buf, "limit exceeded\n");
                    }
                }else if (tam_buf==6){
                    add1 = atoi(strtok(NULL," "));
                    add2 = atoi(strtok(NULL," "));
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(qnts<14){
                        if(eqp_sensor[eqp-1][add1-1]==0){
                            if(eqp_sensor[eqp-1][add2-1]==0){
                                sprintf(buf, "sensor 0%i 0%i added \n", add1, add2);
                                eqp_sensor[eqp-1][add1-1] = 1;
                                eqp_sensor[eqp-1][add2-1] = 1;
                                qnts = qnts + 2;
                            }else{
                                sprintf(buf, "sensor 0%i added 0%i already exists in 0%i \n", add1, add2, eqp);
                                eqp_sensor[eqp-1][add1-1] = 1;
                                qnts = qnts + 1;
                            }                            
                        }else{
                            if(eqp_sensor[eqp-1][add2-1]==0){
                                sprintf(buf, "sensor 0%i added 0%i already exists in 0%i \n", add2, add1, eqp);
                                eqp_sensor[eqp-1][add2-1] = 1;
                                qnts = qnts + 1;
                            }else{
                                sprintf(buf, "sensor 0%i 0%i already exists in 0%i \n", add1, add2, eqp);
                            }
                        }
                    }else{
                        sprintf(buf, "limit exceeded\n");
                    }
                } else if (tam_buf==7){
                    add1 = atoi(strtok(NULL," "));
                    add2 = atoi(strtok(NULL," "));
                    add3 = atoi(strtok(NULL," "));   
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(qnts<13){
                        if(eqp_sensor[eqp-1][add1-1]==0){
                            if(eqp_sensor[eqp-1][add2-1]==0){
                                if(eqp_sensor[eqp-1][add3-1]==0){
                                    sprintf(buf, "sensor 0%i 0%i 0%i added \n", add1, add2, add3);
                                    qnts = qnts + 3;
                                    eqp_sensor[eqp-1][add1-1] = 1;
                                    eqp_sensor[eqp-1][add2-1] = 1;
                                    eqp_sensor[eqp-1][add3-1] = 1;
                                } else{
                                    sprintf(buf, "sensor 0%i 0%i added 0%i already exists in 0%i \n", add1, add2, add3, eqp);
                                    eqp_sensor[eqp-1][add1-1] = 1;
                                    eqp_sensor[eqp-1][add2-1] = 1;
                                    qnts = qnts + 2;
                                }
                            }else{
                                if(eqp_sensor[eqp-1][add3-1]==0){
                                    sprintf(buf, "sensor 0%i 0%i added 0%i already exists in 0%i \n", add1, add3, add2, eqp);
                                    eqp_sensor[eqp-1][add1-1] = 1;
                                    eqp_sensor[eqp-1][add3-1] = 1;
                                    qnts = qnts + 2;
                                } else{
                                    sprintf(buf, "sensor 0%i added 0%i 0%i already exists in 0%i \n", add1, add2, add3, eqp);
                                    eqp_sensor[eqp-1][add1-1] = 1;
                                    qnts = qnts + 1;
                                }
                            }                            
                        }else{
                            if(eqp_sensor[eqp-1][add2-1]==0){
                                if(eqp_sensor[eqp-1][add3-1]==0){
                                    sprintf(buf, "sensor 0%i 0%i added 0%i already exists in 0%i \n", add2, add3, add1, eqp);
                                    eqp_sensor[eqp-1][add2-1] = 1;
                                    eqp_sensor[eqp-1][add3-1] = 1;
                                    qnts = qnts + 2;
                                } else{
                                    sprintf(buf, "sensor 0%i added 0%i 0%i already exists in 0%i \n", add2, add1, add3, eqp);
                                    eqp_sensor[eqp-1][add2-1] = 1;
                                    qnts = qnts + 1;
                                }
                            }else{
                                if(eqp_sensor[eqp-1][add3-1]==0){
                                    sprintf(buf, "sensor 0%i added 0%i 0%i already exists in 0%i \n", add3, add1, add2, eqp);
                                    eqp_sensor[eqp-1][add3-1] = 1;
                                    qnts = qnts + 1;
                                } else{
                                    sprintf(buf, "sensor 0%i 0%i 0%i already exists in 0%i \n", add1, add2, add3, eqp);                                    
                                }
                            } 
                        }
                    }else{
                        sprintf(buf, "limit exceeded \n");
                    }               
                }
                count = send(csock, buf, strlen(buf), 0);
                if (count != strlen(buf)) {
                    logexit("send");
                }               
            }else if(strstr(buf, "remove")!= NULL){ // ok
                int sbuf=0, eqp=0;
                cmd = strtok(NULL," ");
                cmd = strtok(NULL," ");
                sbuf = atoi(cmd);
                cmd = strtok(NULL," ");
                cmd = strtok(NULL," ");
                eqp = atoi(cmd);
                if(eqp_sensor[eqp-1][sbuf-1]==1){
                    eqp_sensor[eqp-1][sbuf-1] = 0;
                    qnts--;
                    sprintf(buf, "sensor 0%i removed \n", sbuf);
                } else {
                    sprintf(buf, "sensor 0%i does not exist in 0%i \n", sbuf, eqp);
                }
                count = send(csock, buf, strlen(buf), 0);
                if (count != strlen(buf)) {
                    logexit("send");
                }                
            }else if(strstr(buf, "list")!= NULL){
                cmd = strtok(NULL," ");
                cmd = strtok(NULL," ");
                int eqp = atoi(strtok(NULL," "));
                if(eqp_sensor[eqp-1][0]==1){
                    if(eqp_sensor[eqp-1][1]==1){
                        if(eqp_sensor[eqp-1][2]==1){
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "01 02 03 04 \n");
                            } else {
                                sprintf(buf, "01 02 03 \n");
                            }
                        } else {
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "01 02 04 \n");
                            } else {
                                sprintf(buf, "01 02 \n");
                            }
                        }
                    } else {
                        if(eqp_sensor[eqp-1][2]==1){
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "01 03 04 \n");
                            } else {
                                sprintf(buf, "01 03 \n");
                            }
                        } else {
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "01 04 \n");
                            } else {
                                sprintf(buf, "01 \n");
                            }
                        }
                    }
                } else {
                    if(eqp_sensor[eqp-1][1]==1){
                        if(eqp_sensor[eqp-1][2]==1){
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "02 03 04 \n");
                            } else {
                                sprintf(buf, "02 03 \n");
                            }
                        } else {
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "02 04 \n");
                            } else {
                                sprintf(buf, "02 \n");
                            }
                        }
                    } else {
                        if(eqp_sensor[eqp-1][2]==1){
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "03 04 \n");
                            } else {
                                sprintf(buf, "03 \n");
                            }
                        } else {
                            if(eqp_sensor[eqp-1][3]==1){
                                sprintf(buf, "04 \n");
                            } else {
                                sprintf(buf, "none \n");
                            }
                        }
                    }
                }
                count = send(csock, buf, strlen(buf), 0);
                if (count != strlen(buf)) {
                    logexit("send");
                }
            }else if(strstr(buf, "read")!= NULL){
                float sns1=rand()%1000+1, sns2=rand()%1000+1, sns3=rand()%1000+1, sns4=rand()%1000+1;
                int s1=0, s2=0, s3=0, s4=0, eqp=0;                
                if(tam_buf==4){
                    s1 = atoi(strtok(NULL," "));
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(eqp_sensor[eqp-1][s1-1]==1){
                        sprintf(buf, "%.2f \n", sns1/100);
                    } else{
                        sprintf(buf, "sensor(s) 0%i not installed \n", s1);
                    }
                }else if (tam_buf==5){
                    s1 = atoi(strtok(NULL," "));
                    s2 = atoi(strtok(NULL," "));
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(eqp_sensor[eqp-1][s1-1]==1){
                        if(eqp_sensor[eqp-1][s2-1]==1){
                            sprintf(buf, "%.2f %.2f \n", sns1/100, sns2/100);
                        }else{
                            sprintf(buf, "%.2f sensor(s) 0%i not installed \n", sns1/100, s2);
                        }
                    }else{
                        if(eqp_sensor[eqp-1][s2-1]==1){
                            sprintf(buf, "%.2f sensor(s) 0%i not installed \n", sns2/100, s1);
                        }else{
                            sprintf(buf, "sensor(s) 0%i 0%i not installed \n", s1, s2);
                        }
                    }                    
                } else if (tam_buf==6){
                    s1 = atoi(strtok(NULL," "));
                    s2 = atoi(strtok(NULL," "));
                    s3 = atoi(strtok(NULL," "));
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(eqp_sensor[eqp-1][s1-1]==1){
                        if(eqp_sensor[eqp-1][s2-1]==1){
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                sprintf(buf, "%.2f %.2f %.2f \n", sns1/100, sns2/100, sns3/100);
                            }else{
                                sprintf(buf, "%.2f %.2f sensor(s) 0%i not installed \n", sns1/100, sns2/100, s3);
                            }                            
                        }else{
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                sprintf(buf, "%.2f %.2f sensor(s) 0%i not installed \n", sns1/100, sns3/100, s2);
                            }else{
                                sprintf(buf, "%.2f sensor(s) 0%i 0%i not installed \n", sns1/100, s2, s3);
                            }  
                        }
                    }else{
                        if(eqp_sensor[eqp-1][s2-1]==1){
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                sprintf(buf, "%.2f %.2f sensor(s) 0%i not installed \n", sns2/100, sns3/100, s1);
                            }else{
                                sprintf(buf, "%.2f sensor(s) 0%i 0%i not installed \n", sns2/100, s1, s3);
                            }                            
                        }else{
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                sprintf(buf, "%.2f sensor(s) 0%i 0%i not installed \n", sns3/100, s1, s2);
                            }else{
                                sprintf(buf, "sensor(s) 0%i 0%i 0%i not installed \n", s1, s2, s3);
                            }  
                        }
                    }                
                }else if (tam_buf==7){
                    s1 = atoi(strtok(NULL," "));
                    s2 = atoi(strtok(NULL," "));
                    s3 = atoi(strtok(NULL," "));
                    s4 = atoi(strtok(NULL," "));
                    cmd = strtok(NULL," ");
                    eqp = atoi(strtok(NULL," "));
                    if(eqp_sensor[eqp-1][s1-1]==1){
                        if(eqp_sensor[eqp-1][s2-1]==1){
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f %.2f %.2f \n", sns1/100, sns2/100, sns3/100, sns4/100);
                                }else{
                                    sprintf(buf, "%.2f %.2f %.2f sensor(s) 0%i not installed \n", sns1/100, sns2/100, sns3/100, s4);
                                }
                            }else{
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f %.2f sensor(s) 0%i not installed \n", sns1/100, sns2/100, sns4/100, s3);
                                }else{
                                    sprintf(buf, "%.2f %.2f sensor(s) 0%i 0%i not installed \n", sns1/100, sns2/100, s3, s4);
                                }
                            }                            
                        }else{
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f %.2f sensor(s) 0%i not installed \n", sns1/100, sns3/100, sns4/100, s2);
                                }else{
                                    sprintf(buf, "%.2f %.2f sensor(s) 0%i 0%i not installed \n", sns1/100, sns3/100, s2, s4);
                                }
                            }else{
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f sensor(s) 0%i 0%i not installed \n", sns1/100, sns4/100, s2, s3);
                                }else{
                                    sprintf(buf, "%.2f sensor(s) 0%i 0%i 0%i not installed \n", sns1/100, s2, s3, s4);
                                }
                            }  
                        }
                    }else{
                        if(eqp_sensor[eqp-1][s2-1]==1){
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f %.2f sensor(s) 0%i not installed \n", sns2/100, sns3/100, sns4/100, s1);
                                }else{
                                    sprintf(buf, "%.2f %.2f sensor(s) 0%i 0%i not installed \n", sns2/100, sns3/100, s1, s4);
                                }
                            }else{
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f sensor(s) 0%i 0%i not installed \n", sns2/100, sns4/100, s1, s3);
                                }else{
                                    sprintf(buf, "%.2f sensor(s) 0%i 0%i 0%i not installed \n", sns2/100, s1, s3, s4);
                                }
                            }                            
                        }else{
                            if(eqp_sensor[eqp-1][s3-1]==1){
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f %.2f sensor(s) 0%i 0%i not installed \n", sns3/100, sns4/100, s1, s2);
                                }else{
                                    sprintf(buf, "%.2f sensor(s) 0%i 0%i 0%i not installed \n", sns3/100, s1, s2, s4);
                                }
                            }else{
                                if(eqp_sensor[eqp-1][s4-1]==1){
                                    sprintf(buf, "%.2f sensor(s) 0%i 0%i 0%i not installed \n", sns4/100, s1, s2, s3);
                                }else{
                                    sprintf(buf, "sensor(s) 0%i 0%i 0%i 0%i not installed \n", s1, s2, s3, s4);
                                }
                            }  
                        }
                    }                
                }
                count = send(csock, buf, strlen(buf), 0);
                if (count != strlen(buf)) {
                    logexit("send");
                }
            }else {
                close(csock);

                csock = accept(s, caddr, &caddrlen);
                if (csock == -1) {
                    logexit("accept");
                }
            }

            for ( i=0; i<4; i++ ){ // acompanhamento de sensores e equipamentos
                for ( j=0; j<4; j++ ){
                    printf("%i ", eqp_sensor[i][j]);
                }   
                printf("\n");
            }            
            
        }
    }
    return 0;
}