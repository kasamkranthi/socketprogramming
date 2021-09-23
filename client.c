
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include  <unistd.h>
#define PORT 8000



int main(int argc, char const *argv[])
{
    struct sockaddr_in address,serv_addr;
    int sock = 0;
    char array[1024],buffer[1024]={0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    int temp;

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    int valread;
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    else
    {
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    }
    for(;1;){
    printf("To get files use get filename1 filename2... and exit to disconnect from server");
    printf("\nclient>");
    fgets(array,1024,stdin);
    if(array[strlen(array)-1]=='\n')
    {
        array[strlen(array)-1]='\0';
    }
    char *parts[50];
    int j=0;
    write(sock,array,strlen(array));
    char * each = strtok(array," \n\t\r");
    char * receive = (char *) calloc(256,sizeof(char)),*ftrans=(char *) calloc(256,sizeof(char));
    while(each != NULL){
        parts[j]=each;
        j+=1;
        each=strtok(NULL, " \n\t\r");
    }
    if(!strcmp(parts[0],"get"))
    {
        for(int i=1;i<j;){
            int tempcnt,count=0,total;
            read(sock,receive,256);
            total=atoi(receive);
            write(sock,receive,strlen(receive));
            for(int temp=0;temp<sizeof(receive);temp++)
                receive[temp]='\0';
                if(total >= 0){
                    int fp;
                    fp = open(parts[i],O_WRONLY | O_TRUNC | O_CREAT,0644);
                   if(fp < 0){
                    printf("Download error exiting\n");
                    return 0;
                   }
                   if(total!=0){
                    while(1){
                    tempcnt = read(sock,ftrans,256);
                    if(tempcnt>0)
                    {
                    write(fp,ftrans,tempcnt);
                    count=count+tempcnt;
                    }
                    for(int temp;temp<strlen(ftrans);temp++)
                    {
                        ftrans[temp]='\0';
                    }
                    float statusn;
                    statusn= (((float) count )/total)*100;
                    printf("\rDownloading %.5f%%",statusn);
                    if(tempcnt<256 || tempcnt==0 ||total == count){
                        
                        printf("\nDownloaded\n");
                        write(sock,ftrans,sizeof(ftrans));
                        close(fp);
                        i++;
                        break;
                    }
                    }
                    }
                    else {
                        printf("\nDownloaded it's an empty file\n");

                        
                        close(fp);
                        i+=1;
                    }
                }
                else {
                    printf("\nFile doesnt exist %s\n",parts[i]);
                    i+=1;
                }
            }
    }
    else if (strcmp(parts[0],"exit")==0){
        printf("exiting\n");
        break;
    }
    else
    {
        printf("[-]Invalid input\n");
    }
    
    }
    return 0;
}