#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>


#define CALL(x) if(!(x)&& count++)error(#x,errno,count)

int count=2;

void error(char *x, int erno, int cnt){
	fprintf(stderr,"%s error No:%d\n",x, erno);
	exit(cnt);
}

int main(int argc, char ** argv){
	
	if(argc!=4){
		printf("usage: %s not enough argument\n",*argv);
		exit(1);
	}
		
	struct protoent	*protocol;
	CALL((protocol=getprotobyname("tcp"))!=NULL);
	int proNo=protocol->p_proto;

	int sokNo=0;
	CALL((sokNo=socket(AF_INET, SOCK_STREAM, proNo))>0);

	struct hostent *IPstruct;
	CALL((IPstruct=gethostbyname(argv[1]))!=NULL);
	

	int PORTaddr=0;
	CALL((sscanf(argv[2],"%d",&PORTaddr))==1&&PORTaddr>0&&PORTaddr<64000);	
	
	int iterate=0;
	CALL((sscanf(argv[3],"%d",&iterate))==1&&iterate>=0&&iterate<10);	
	
	
	struct sockaddr_in PORTnIP;
	
	PORTnIP.sin_family=AF_INET;	
	PORTnIP.sin_port=htons(PORTaddr);
	PORTnIP.sin_addr=*((struct in_addr*)IPstruct->h_addr);
	memset(PORTnIP.sin_zero, 0, 8);	
	
	CALL((connect(sokNo, (struct sockaddr *)&PORTnIP,sizeof(PORTnIP)))==0);
	
	char *cmd_quit="QUIT\r\n", *cmd_draw="DRAW\r\n";
	
	if(iterate==0){
		char *cmd_exit="EXIT\r\n";
		CALL((write(sokNo,cmd_exit,strlen(cmd_exit)))==strlen(cmd_exit));
		close(sokNo);
	}
	while(iterate--){
		CALL((write(sokNo,cmd_draw,strlen(cmd_draw)))==strlen(cmd_draw));
		char ans[16];
		int readin=0;
		CALL((readin=read(sokNo, ans, sizeof(ans)))>0);
		ans[readin]='\0';
		printf("re: %s\n",ans);
		if(iterate){sleep(1);}
	}
	CALL(((write(sokNo,cmd_quit,strlen(cmd_quit)))==strlen(cmd_quit)));
	close(sokNo);
}
