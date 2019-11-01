#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>

#define CALL(x) if(!(x)&&count++)error(#x,errno,count)

int count=2;

void error(char *er, int erno, int cnt){
	fprintf(stderr,"%s error No:%d\n",er, erno);
	exit(cnt);
}
char *cmd_draw="DRAW\r\n", *cmd_exit="EXIT\r\n",*cmd_quit="QUIT\r\n", *ans="OK\r\n", *err_ans="BigError\r\n";

void* thread(void *data){
	int sd=*((int *)data);
	for(;;){
		char buff[16];
		int inread=0;
		inread=read(sd, buff, sizeof(buff)-1);
		buff[inread]='\0';
		pthread_t thrdNo=0;
		thrdNo=pthread_self();
		if(!strcmp(buff,cmd_exit)){
			write(sd,ans,strlen(ans));
			printf("thread No:%d, exitted\n",(int)thrdNo);
		close(sd);
			return 0;
		}
		else if(!strcmp(buff,cmd_quit)){
			write(sd,ans,strlen(ans));
			printf("thread No:%d, quitted!\n", (int)thrdNo);
		break;
		}
		else if(!strcmp(buff,cmd_draw)){
			char answer[100];
			sprintf(answer,"%s\n%d\n",ans,rand()%10000);
			write(sd,answer,strlen(answer));
			printf("thread No:%d, drawn!\n", (int)thrdNo);
		}
		else{
			write(sd,err_ans, strlen(err_ans));
			printf("thread No:%d, bad command %s\n", (int)thrdNo, buff);
		}
	}
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
		srand(time(NULL));
		struct sockaddr_in acceptStruct;
		int addrlen=sizeof(acceptStruct);
		int sd=1;
		CALL((sd=accept(listenSocket, (struct sockaddr *)&acceptStruct, &addrlen))>0);
		printf("socket descriptor--:%d\n", sd);
	 	
		int *data=malloc(sizeof(int));		
		*data=sd;
		pthread_t thrd;
		pthread_create(&thrd, NULL, thread, (void *)data);
	}
	
}
