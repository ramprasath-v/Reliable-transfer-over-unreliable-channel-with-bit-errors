#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<netdb.h>
#include<stdlib.h>
#include<time.h>
#include <unistd.h>

unsigned short csum(unsigned short *buf, int nwords)
{       
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned char)(~sum);
}

struct udpheader {
 unsigned short int srcport;
 unsigned short int destport;
 unsigned short int len;
 unsigned short int chksum;
};

int main(int argc,char* argv[])
{
    char inputFilename[50],outputFilename[50], buffer[50],msg[50];    
    struct sockaddr_in serveraddr;
    struct udpheader udp;
    int clientsock,n,portno,m;
    socklen_t len;
    unsigned short int iph_chksum;
    bzero((char*)&serveraddr,sizeof(serveraddr));
    
    portno = atoi(argv[1]); 
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port= htons(portno);
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    clientsock=socket(AF_INET,SOCK_DGRAM,0);

    udp.srcport = htons(portno);
    udp.destport = htons(portno);
    udp.len = htons (sizeof(struct udpheader));
    
    FILE *fp;
    printf("Enter the name of the input file = ");
    scanf("%s",inputFilename);
    printf("Enter the name of the output file = ");
    scanf("%s",outputFilename);
    printf("\nFilename =  %s\n", inputFilename);
    sendto(clientsock, outputFilename, strlen(outputFilename), 0,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr));
    fp=fopen(inputFilename,"rb");
    len= sizeof(serveraddr);
    
    int flag=0;
    char seq0[50] ="seq0";
    char seq1[50] ="seq1";
    time_t u;
    srand((unsigned) time(&u));
    int k = rand() % 10;
    k=1;
    if(fp==NULL)
    {
        perror("Error in opening file");
        return(-1);
    }
    
    else
    {
        while(!feof(fp))
        {
            bzero(buffer,50);
            n = fread(buffer, 10, 1, fp);            
            if(n>0)
            {
                //printf("k value =%d\n", k);
                if (k==1)
                {
                    udp.chksum = 0;
                    k=0;
                }
                else
                {

                udp.chksum = csum((unsigned short *)buffer, sizeof(struct udpheader));
                }
                sendto(clientsock,(struct udpheader *)&udp, sizeof(struct udpheader), 0,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr));
                printf("\nAppended check sum in client = %hu\n", udp.chksum);                
                if (flag%2 == 0)
                {
                    
                    sendto(clientsock, seq0, strlen(seq0) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
                    printf("Sequence number = %s\n", seq0);

                    if(sendto(clientsock, buffer, strlen(buffer) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
                    
                    {
                        perror("sendto() error");
                        exit(-1);
                    }
                    else
                    {
                        printf("Data sent from client = %s \n",buffer);
                        m = recvfrom(clientsock,msg,50,0,(struct sockaddr *)&serveraddr, &len);
                        while(strcmp(msg,"ack0") !=0 || strcmp(msg,"error") ==0)
                        {
                            printf("error in check sum or ack 0 not received\n");
                            printf("Resending seq 0 packets\n");
                            udp.chksum = csum((unsigned short *)buffer, sizeof(struct udpheader));
                            sendto(clientsock,(struct udpheader *)&udp, sizeof(struct udpheader), 0,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr));
                            sendto(clientsock, seq0, strlen(seq0) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
                            sendto(clientsock, buffer, strlen(buffer) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) ;
                            printf("\nAppended check sum in client = %hu\n", udp.chksum); 
                            printf("Sequence number = %s\n", seq0);
                            printf("Data sent from client = %s \n",buffer);
                            m = recvfrom(clientsock,msg,50,0,(struct sockaddr *)&serveraddr, &len);
                            if(strcmp(msg,"ack0") == 0)
                            {
                                printf("Acknowledgement from server for seq 0 packets = %s \n",msg);
                            }
                        }
                        
                        
                        printf("Acknowledgement from server for seq 0 packets = %s \n",msg);
                        
                    }

                }
                else
                {
                    
                    sendto(clientsock, seq1, strlen(seq1) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
                    printf("Sequence number = %s\n", seq1);
                    if(sendto(clientsock, buffer, strlen(buffer) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
                    {
                        perror("sendto() error");
                        exit(-1);
                    }
                    else
                    {
                        printf("Data sent from client = %s \n",buffer);
                        m = recvfrom(clientsock,msg,50,0,(struct sockaddr *)&serveraddr, &len);
                        while(strcmp(msg,"ack1") !=0 || strcmp(msg,"error") ==0)
                        {
                            printf("error in check sum or ack 1 not received \n");
                            printf("Resending seq 1 packets\n");
                            udp.chksum = csum((unsigned short *)buffer, sizeof(struct udpheader));
                            sendto(clientsock,(struct udpheader *)&udp, sizeof(struct udpheader), 0,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr));
                            sendto(clientsock, seq1, strlen(seq1) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
                            sendto(clientsock, buffer, strlen(buffer) , 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) ;
                            printf("\nAppended check sum in client = %hu\n", udp.chksum); 
                            printf("Sequence number = %s\n", seq1);
                            printf("Data sent from client = %s \n",buffer);
                            m = recvfrom(clientsock,msg,50,0,(struct sockaddr *)&serveraddr, &len);
                            if(strcmp(msg,"ack1") == 0)
                            {
                                printf("Acknowledgement from server for seq 0 packets = %s \n",msg);
                            }
                        }
                        printf("Acknowledgement from server for seq 0 packets = %s \n",msg);
                        
                    }
                }
                
           }
            else
            {
                break;
            }
            flag++;
        }
    }
    
    printf("\nFile was sent successfully\n");
    close(clientsock);
    fclose(fp);
    return 0;
        
}
