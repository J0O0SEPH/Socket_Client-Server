#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <time.h>

#define CALL(x) if(!(x)&&count++)error(#x,errno,count)

int count=2;

void error(char *er, int erno, int cnt){
	fprintf(stderr,"%s error No:%d\n",er, erno);
	exit(cnt);
}

int main(int argc, char **argv){
	if(argc!=2){
		printf("usage error: %s\n",*argv);
		exit(1);
	}

	struct protoent *protoStruct;
	CALL((protoStruct=getprotobyname("tcp"))!=NULL);	
	int protoNo=protoStruct->p_proto;	

	
	int listenSocket=0;
	CALL((listenSocket=socket(AF_INET, SOCK_STREAM,protoNo))>0);
	
	int optval=1;
	CALL((setsockopt(listenSocket,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(optval)))==0);

	int portNo=0;
	CALL((sscanf(argv[1],"%d",&portNo))==1&&portNo>0&&portNo<64000);
	
	struct sockaddr_in sokStruct;
	sokStruct.sin_family=AF_INET;
	sokStruct.sin_port=htons(portNo);
	sokStruct.sin_addr.s_addr=htonl(INADDR_ANY);
	memset(sokStruct.sin_zero, 0,8);

	CALL((bind(listenSocket,(struct sockaddr *)&sokStruct, sizeof(sokStruct)))==0);
	
	CALL((listen(listenSocket, 5))==0);

	
	for(;;){
		struct sockaddr_in acceptStruct;
		int addrlen=sizeof(acceptStruct);
		int sd=0;
		CALL((sd=accept(listenSocket, (struct sockaddr *)&acceptStruct, &addrlen))>0);
		printf("socket descriptor:--%d\n", sd);
		char *cmd_draw="DRAW\r\n", *cmd_exit="EXIT\r\n",
		 *cmd_quit="QUIT\r\n", *ans="OK\r\n", *err_ans="BigError\r\n";
		srand(time(NULL));

		for(;;){
		char buff[16];
		int inread=0;
		inread=read(sd, buff, sizeof(buff)-1);
		buff[inread]='\0';

			if(!strcmp(buff,cmd_exit)){
				write(sd,ans,strlen(ans));
				printf("exitted\n");
				close(sd);close(listenSocket);
				return 0;
			}
			else if(!strcmp(buff,cmd_quit)){
				write(sd,ans,strlen(ans));
				printf("quitted!\n");
				break;
			}
			else if(!strcmp(buff,cmd_draw)){
				char answer[100];
				sprintf(answer,"%s\n%d\n",ans,rand()%10000);
				write(sd,answer,strlen(answer));
				printf("drawn!\n");
			}
			else{
				write(sd,err_ans, strlen(err_ans));
				printf("bad command %s\n", buff);
			}
		}
	}
}
