#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8000
int main(int argc, char const *argv[])
{
    int server_fd=0,opt;
    struct sockaddr_in address;  
    opt = 1;
    int addrlen;
    addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int new_socket=0;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    else
    {
    
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );// Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian
    }
    int valread;
    
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 5) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    new_socket=0;
    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    for(;1;){
    char * buffer=calloc(256,sizeof(char));
    char *final[50];
    int j=0;
    valread = read(new_socket , buffer, 256);  // read infromation received into the buffer
    char * each = strtok(buffer," \n\t\r");
    while(each != NULL){
        final[j]=each;
        each=strtok(NULL, " \n\t\r");
        j+=1;
    }
    if(strcmp(final[0],"get")==0)
    {
    int i;    
    for(i=1;i<j;){  
        int total=0,sent;
        sent=0;
        char *sending=calloc (256,sizeof(char));
        char *statbar=calloc (256,sizeof(char));
        char *filetransfer=calloc (256,sizeof(char));    
        int fp;
        fp = open(final[i],O_RDONLY);
        if(fp<0){
            total=-1;
            sprintf(sending,"%d",total);
            int temp;
            write(new_socket,sending,strlen(sending));
            read(new_socket,sending,256);
            for(temp=0;temp<strlen(sending);temp++)
                sending[temp]='\0';
            i+=1;
        }
        else {
            total=lseek(fp,0,SEEK_END); //length for percentage
            sprintf(sending,"%d",total);
            write(new_socket,sending,strlen(sending));
            int rchck;
            read(new_socket,sending,256);
             for(int temp=0;temp<strlen(sending);temp++)
                sending[temp]='\0';
            lseek(fp,0,SEEK_SET);
            while(total!=0){
                if((rchck=read(fp,filetransfer,256)) > 0){
                    int temp;
                    write(new_socket,filetransfer,rchck);
                    for(temp=0;temp<strlen(filetransfer);temp++)
                        filetransfer[temp]='\0';
                    if(rchck<256){
                        close(fp);
                        read(new_socket,filetransfer,sizeof(filetransfer));
                        i+=1;
                        for(temp=0;temp<strlen(filetransfer);temp++)
                        filetransfer[temp]='\0';
                        break;
                    }
                }
                else {
                        int temp;
                        close(fp);
                        i+=1;
                        read(new_socket,filetransfer,sizeof(filetransfer));
                        for(temp=0;temp<strlen(filetransfer);temp++)
                        filetransfer[temp]='\0';
                        break;                    
                }
            }
        }  
    }
    }
    else if (!strcmp("exit",final[0])){
        printf("exiting\n");
        break;
    }
    else {
        printf("No valid request \n");
    }
    }
    close(new_socket);
    return 0;
}
