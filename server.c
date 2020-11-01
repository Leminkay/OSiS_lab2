#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#include "func.h"
#define DEF_PORT 4311
#define DEF_CONS 1
struct package{
	char comm[8]; // command
	char status[12];
	char msg[1400];
};
// available commands: VAR DATA RES RSET QUIT
void shiftArr(struct sockaddr_in * arr, short int cons){
	short int i;
	for(i = 0; i < cons - 1; i++){
		arr[i] = arr[i + 1];
	}
}


int main(int argc, char *argv[]) {

	struct sockaddr_in addr, claddr, cur_claddr;

	short int port;
	short int cons;
	if (argc == 1) {
		port = DEF_PORT;
	} else {
		port = atoi(argv[1]);
		cons = DEF_CONS;
		if(argc == 3){
			cons = atoi(argv[2]);
		}
		
	}
	
	int listner = socket(AF_INET, SOCK_DGRAM, 0);
	if (listner < 0) {
		perror("ERROR: Cannot create socket.\n");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listner, (struct sockaddr *)&addr , sizeof(addr)) < 0){
		perror("ERROR: Cannot connect.\n");
		return 2;
	}
	
	int sock_cl;
	int size_in;
	char buf1[1024], buf2[1024];
	char n[1024], m[1024];
	char resC[1024];
	char dInt [] = "−100000 100000\n";
	struct sockaddr_in cons_arr[cons];
	short int cur_cons = 0;

	struct package pack;

	while(1){
		//connect to client
		socklen_t c_len = sizeof(cur_claddr);
		if(cur_cons > 0){
			cur_claddr = cons_arr[0];
			shiftArr(cons_arr, cur_cons);
			cur_cons--;
		}else{
			if (recvfrom(listner, (struct package *)&pack, sizeof(pack), 0, (struct sockaddr *)&cur_claddr, &c_len) == -1){
				perror("ERROR: Unable to recieve the package");
				break;
			}
		}
		strcpy(pack.status,"CONNECTED\n");
		strcpy(pack.msg,"250: Successful connection\n");
		if (sendto(listner, (struct package *)&pack, sizeof(pack), 0, (struct sockaddr *)&cur_claddr, c_len) == -1){
			perror("ERROR: Unable to send a package");
			break;
		}

		while(1){
			
			socklen_t len = sizeof(claddr);
			if (recvfrom(listner, (struct package *)&pack, sizeof(pack), 0, (struct sockaddr *)&claddr, &len) == -1){
				perror("ERROR: Unable to recieve the package");
				break;
			}
			if(memcmp(&claddr, &cur_claddr, sizeof(struct sockaddr)) != 0){
				if(cur_cons < cons - 1){
					cons_arr[cur_cons] = claddr;
					cur_cons++;
				}
				continue;							
			}
			
			perror(pack.comm);
			perror(pack.msg);
			if( strcmp(pack.comm, "DATA") == 0){
				if(strcmp(n, "") != 0 && strcmp(m, "") != 0){
					strcpy(pack.status, "ERROR");	
					strcpy(pack.msg, "510: Variables already assignet\n");
				}else{
					char* pch = strtok(pack.msg, " =,");
					int cnt = 1;
					
					while(pch != NULL){
						
						if(cnt == 2){
							strcpy(buf1, pch);
						}
						if(cnt == 4){
							strcpy(buf2, pch);
						}
						pch = strtok(NULL, " =,");
						cnt++;
					}
					cnt--;
					if(cnt < 4){
						strcpy(pack.status, "ERROR");	
						strcpy(pack.msg, "530: Not enough variables\n");		
					}else if(cnt > 4){
						strcpy(pack.status, "ERROR");	
						strcpy(pack.msg, "540: Too much variables\n");
						printf("%d\n", cnt);
					}else if(!(-100000 <= atoi(buf1) && atoi(buf1) <= 100000) || !( -100000 <= atoi(buf2) && atoi(buf2) <= 100000) 
						|| (atoi(buf1) == 0 && strcmp(buf1, "0") != 0) || (atoi(buf2) == 0 && strcmp(buf2, "0") != 0)){

						strcpy(pack.status, "ERROR");	
						strcpy(pack.msg, "520: One of the variable doesnt fit into requirements\n");

					}else{
						strcpy(n, buf1);
						strcpy(m, buf2);
						printf("n = %d, m = %d\n", atoi(n), atoi(m));
						strcpy(pack.status, "OK");	
						strcpy(pack.msg, "250: Success\n");											
					}
				}
			}
			if( strcmp(pack.comm, "VAR") == 0){
				strcpy(pack.status, "OK");
				strcpy(pack.msg, "250: recieved\n");
				strcat(pack.msg, "n\n");
				strcat(pack.msg, dInt);
				strcat(pack.msg, "m\n");
				strcat(pack.msg, dInt);
			}
			if( strcmp(pack.comm, "RSET") == 0){
				strcpy(n, "");
				strcpy(m, "");
				strcpy(pack.status, "OK");
				strcpy(pack.msg, "250: values have been reset\n");
			}
			if( strcmp(pack.comm, "RES") == 0){
				if(strcmp(n, "") == 0 || strcmp(m, "") == 0){
					strcpy(pack.status, "ERROR");
					strcpy(pack.msg, "540: No values assigned");
				}else{
					int res = delta(atoi(n), atoi(m));
					sprintf(resC, "%d", res);
					printf("n = %d, m = %d\n", atoi(n), atoi(m));
					perror(resC);

					strcpy(pack.status, "OK");
					strcpy(pack.msg, "250: value is = ");
					strcat(pack.msg, resC);
					strcat(pack.msg, "\n");
				}

			}
			if(strcmp(pack.comm, "QUIT") == 0){

				strcpy(pack.status, "OK");
				strcpy(pack.msg, "250: closing connection\n");
				break;
			}
			if (sendto(listner, (struct package *)&pack, sizeof(pack), 0, (struct sockaddr *)&cur_claddr, c_len) == -1){
				perror("ERROR: Unable to send a package");
				break;
			}
		}
		
	}
	
	return 0;
}
