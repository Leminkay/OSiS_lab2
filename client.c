#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#define DEF_PORT 4311




struct package{
	char comm[8]; // command
	char status[12];
	char msg[1400];
};
// available commands: VAR DATA RES RSET QUIT
int main(int argc, char *argv[]) {

	char msg[1024] = "QQ\n", buf[1024];
	struct sockaddr_in addr;

	short int port;

	if (argc == 1) {
		port = DEF_PORT;
	} else {
		port = atoi(argv[1]);
	}
	
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("ERROR: Cannot create socket.\n");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


	
	// sendto("desc, mes, sizemes, flags, sockaddr*, int size_of_addr)
	// recvfrom("desc, mes, sizemes, flags, sockaddr*, int* size_of_addr)
	char in_comm[8];
	char in_msg[1400];
	socklen_t len = sizeof(addr);
	struct package pack;
	//connecting
	strcpy(pack.comm, "");
	strcpy(pack.msg, "");
	strcpy(pack.status, "");

	if (sendto(sock, (struct package *)&pack, sizeof(pack), 0, (struct sockaddr *)&addr , len) == -1){
		perror("ERROR: Unable to send the package");
	}


	if (recvfrom(sock, (struct package *)&pack, sizeof(pack), 0 ,(struct sockaddr *)&addr , &len) == -1){
		perror("ERROR: Unable to recieve the package");
		return 3;
	}


	printf("%s \n", pack.status);
	printf("%s \n", pack.msg);
	printf("==========\n");

	while(1){
		
		strcpy(pack.msg, "");
		strcpy(pack.status, "");
		scanf("%s", in_comm);
		strcpy(pack.comm, in_comm);
		if (strcmp(pack.comm, "DATA") == 0){
			scanf("%s", in_msg);
			strcpy(pack.msg, in_msg);
		}
		else if(!(strcmp(pack.comm, "QUIT") == 0 || strcmp(pack.comm, "VAR") == 0 || strcmp(pack.comm, "RES")  == 0 || strcmp(pack.comm, "RSET") == 0)){
			perror("ERROR: unavailable command");			
			continue;		
		}
		if (sendto(sock, (struct package *)&pack, sizeof(pack), 0, (struct sockaddr *)&addr , len) == -1){
			perror("ERROR: Unable to send the package");
			return 2;
		}
		if(strcmp(pack.comm, "QUIT") == 0){
			break;		
		} 

		if (recvfrom(sock, (struct package *)&pack, sizeof(pack), 0 ,(struct sockaddr *)&addr , &len) == -1){
			perror("ERROR: Unable to recieve the package");
			return 3;
		}
		printf("%s \n", pack.status);
		printf("%s \n", pack.msg);

		printf("==========\n");
	}
	close(sock);
	return 0;
}
