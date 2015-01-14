#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<stdlib.h>

struct udpheader {
 unsigned short int srcport;
 unsigned short int destport;
 unsigned short int len;
 unsigned short int chksum;
};

unsigned short csum(unsigned short *buf, int nwords)
{       
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned char)(~sum);
}

int main(int argc,char* argv[])
{
    struct sockaddr_in clientaddr,serveraddr;
    struct udpheader udp;
    struct udpheader udp_client;
    int serversock,newserversock,n,portnum,m,x;
    socklen_t clientsize,len;
    unsigned short int server_chksum;
    char buffer[50],outputFilename[50],buff[50],seqArray[50],seqBuff[50];
    fflush(stdin);


    serversock=socket(AF_INET,SOCK_DGRAM,0);

    portnum = atoi(argv[1]);
    bzero((char*)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(portnum);
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bind(serversock,(struct sockaddr*)&serveraddr, sizeof(serveraddr));
    len= sizeof(clientaddr);
    FILE *fp;
    recvfrom(serversock,buff,15,0,(struct sockaddr *)&clientaddr, &len);
    
    strcpy(outputFilename,buff);
    printf("output filename = %s \n",outputFilename);
    fp = fopen(outputFilename,"ab");
    bzero(buffer,50);

    char ack0[10] ="ack0";
    char ack1[10] ="ack1";
    int k =1;
    for(x=0;x<5;x++)
    {
        recvfrom(serversock,(struct udpheader *)&udp_client, sizeof(struct udpheader),0,(struct sockaddr *)&clientaddr, &len);
        printf("\nChecksum value from client= %hu\n", udp_client.chksum);   
        
        recvfrom(serversock,seqBuff,10,0,(struct sockaddr *)&clientaddr, &len);
        strcpy(seqArray,seqBuff);
        printf("Sequence number= %s\n", seqArray);
        
        n=recvfrom(serversock,buffer,10,0,(struct sockaddr *)&clientaddr, &len);
        udp.chksum = csum((unsigned short *)buffer, sizeof(struct udpheader));
        printf("Calculated checksum in server= %hu\n", udp.chksum);
        char seq0[50] ="seq0";
        
        if(n>0)
        {
            if(udp.chksum == udp_client.chksum)
            {
            printf("Received data = %s \n",buffer);
            fwrite(buffer,5,2,fp);
        
            if ((strcmp(seqArray,seq0)) == 0)
            {           
                m = sendto(serversock,ack0,10,0,(struct sockaddr *)&clientaddr, sizeof(struct sockaddr));
                if(m<0)
                {
                    perror("sendto");
                }       
                printf("Acknowledgement for seq0 = %s\n", ack0);

            }
            else
            {
                if (k==1)
                {
                    strcpy(ack1,"ack0");
                    k=0;
                }
                else
                {
                    strcpy(ack1,"ack1");
                }
                m = sendto(serversock,ack1,10,0,(struct sockaddr *)&clientaddr, sizeof(struct sockaddr));
                if(m<0)
                {
                    perror("sendto");
                }  
                printf("Acknowledgement for seq1 = %s\n", ack1);
            }
            }
            else
            {
            printf("error in checksum value\n");
            
            m = sendto(serversock,"errror",10,0,(struct sockaddr *)&clientaddr, sizeof(struct sockaddr));
            
            }
        }
        
    }
    printf("\nFile was received successfully\n");
	printf("New file created is %s\n",outputFilename);        
	close(serversock);
    close(newserversock);
}


