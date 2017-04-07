/**********************************************
 *Name: Taylor Del Matto
 *Assignment 4
 otp_enc_d.c
 *Operating Systems
 *This component recieves a file and a keyfile
 * from otp_enc.c it encrypts the file and writes the result
 * back to otp_enc.c
 * ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

int n;
int readstatus;
int sentstatus;
char good[3]= "G1";
char bad[3];
char gbstatus[3];
int readsize;
int file1;
int i;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//readfile this function reads in data from the file
//and checks that there are no bad characters
void readfile(char *filename, char datain1[], int fd){
	printf("filename %s\n", filename);
	//READ FILE IN
	fd = open(filename, O_RDONLY);
	if(fd < 0){
		printf("file unreadable");
		exit(1);
	}
	readsize = read(fd, datain1, 50000);
	//printf("readsize, %d", readsize);
	for(i=0; i < readsize-1; i++){
		if(datain1[i] > 90 || datain1[i] < 65 && datain1[i] != 32){
			
			printf("bad characters\n");
			exit(1);
		}
		//else
			//printf("%c", datain1[i]);
	}
	datain1[readsize-1] = '*';
	//printf("this is datain 0%s0", datain1);
}

//this function encrypts data by taking both data 
//and key files as input
void encrypt(char datain[], char keyin[]){
	i=0;
	while(datain[i] != '*'){
		if(datain[i] == 32){
			datain[i] = 64;	
		}
		if(keyin[i] == 32){
			keyin[i] = 64;	
		}

		//subtract 65 from both
		datain[i] = datain[i] - 64;
		keyin[i] = keyin[i] - 64;
		//add together
		datain[i] = datain[i] + keyin[i];
		//If greater than 26, take modulus
		if(datain[i] > 26){
			datain[i] = datain[i] % 27;
		}
		datain[i] = datain[i] + 64;
		if(datain[i] == 64){
		//change back to spaces for encryption
		datain[i] = 32;
		}
		//printf("encrypted dec is %d\n", (int)datain[i]);
	i++;
	}
}

//this function decrypts data by taking both data 
//and key files as input
void decrypt(char datain[], char keyin[]){
	i=0;
	while(datain[i] != '*'){
		if(datain[i] == 32){
			datain[i] = 64;
		}
		datain[i] = datain[i] - 64;
		//subtract key
		datain[i] = datain[i] - keyin[i];
		
		//If less than 0
		if(datain[i] < 0){
			datain[i] = datain[i] + 27;
		}
		//add 65
		datain[i] = datain[i] + 64;
		keyin[i] = keyin[i] + 64;
		if(datain[i] == 64){
			datain[i] = 32;	
		}
	printf("%d\n", datain[i]);
	i++;
	}

}

//readdata this function reads data over the socket connection
//and then sencs a verification signal read data always writes to
//a recieving wrdata function on the other end
void readdata(int sockfd1, char data2[], int size){
	readstatus = 0;
	while(readstatus == 0){
		n = read(sockfd1, data2, size);
		if (n < 0){ 
			error("ERROR reading from socket");
		}
		else{
			//write good to socket
			n = write(sockfd1, "G1", 3);
			readstatus = 1;
			
		}
	}
}

//wrdata this function writes data over the socket connection
//then waits for a confirmation signal wrdata always writes 
//to a recieving readdata function on the other end
void wrdata(char data1[], int sockfd1){
	//printf("write server begins");
	sentstatus = 0;
	while(sentstatus == 0){
		n = write(sockfd1, data1,strlen(data1));
    		if (n < 0) 
        		 error("ERROR writing to socket");
		else{
			n=0;
			n = read(sockfd1, gbstatus, 3);
			if(n > 0){//if nless than or equal to 0, data was not recieved.  
				sentstatus = 1;
			}
			else{
				printf("no write confirmation\n");
				sentstatus = 1;
			}
		}	
	}	
}

//this function opens up a port
void openport(struct sockaddr_in *serv_addr1, int *portno1, int *sockfd1){
	*sockfd1 = socket(AF_INET, SOCK_STREAM, 0); //basic socket with ip, domain, tcp
	if (*sockfd1 < 0) 
		error("ERROR opening socket");
	
	
	bzero((char *) serv_addr1, sizeof(*serv_addr1));
	//portno = atoi(argv[1]);//gets port no from input
	serv_addr1->sin_family = AF_INET;  //filling address struct
	serv_addr1->sin_addr.s_addr = INADDR_ANY;
	serv_addr1->sin_port = htons(*portno1);  //port number
	if (bind(*sockfd1, (struct sockaddr *) serv_addr1, sizeof(*serv_addr1)) < 0) 
		error("ERROR on binding");
}

//this function listens on an opened port
void lisport(struct sockaddr_in *cli_addr2, socklen_t *clilen2, int *portno2, int *sockfd2, int *newsockfd2){
	listen(*sockfd2,5);//if bind works, listen on that port, second param is number of connections to queue
		*clilen2 = sizeof(*cli_addr2);  //sets length of client address struct 
		*newsockfd2 = accept(*sockfd2, (struct sockaddr *) cli_addr2, clilen2);  // gets client connection puts in client address struct via client address pointer

		if (newsockfd2 < 0) 
			error("ERROR on accept");

}

int main(int argc, char *argv[])
{
	//definitions...
	int sockfd1;
	int portnumber;
	char verifyised[20];
	bzero(good, 3);
	strcpy(good, "g");
	bzero(bad, 3);
	strcpy(bad, "b");
	char keyin[50000];
	int ischild = 0;
	int sockfd;
	int  newsockfd, portno, newsockfd1;
	socklen_t clilen, clilen1;
	char buffer[50000]; 
	struct sockaddr_in serv_addr, cli_addr, serv_addr1, cli_addr1;
	char portstr[15];
	pid_t cpid;

	//if not enough args, exit	
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	portno = atoi(argv[1]);
	portnumber = portno;
	openport(&serv_addr, &portno, &sockfd);

	while(ischild == 0){	
		//listen here
		lisport(&cli_addr, &clilen, &portno, &sockfd, &newsockfd);		
		
		//CHILD SHOULDNT BE HERE EVER	
		//server forks
		cpid = fork();
		
		//branch off to child/parent
		if(cpid == -1){
			printf("error child not created");
		}
		//if in child, process data, write data, quit
		else if(cpid == 0){//in child
			ischild = 1;
			
			//move back to parent starts here	
			//loop through letters if ddd exit(1)
			readdata(newsockfd, verifyised, 20);
			wrdata("eeeeeeee", newsockfd);
			if(verifyised[0] == 'd'){
				printf("this is dec, connection not allowed\n");
				continue;
			}
			else if(verifyised[0] == 'e'){
				//printf("connected to enc\n");
			}
			//convert portnumber to string
			sprintf(portstr, "%d", portnumber);
			
			//server sends portnumber
			wrdata(portstr, newsockfd);
			
			//move back to parent ends here
			bzero(buffer,50000);

			//read data
			readdata(newsockfd, buffer, 49999);	
			readdata(newsockfd, keyin, 49999);

			encrypt(buffer, keyin);
			wrdata(buffer, newsockfd);
		}	
		else{//IN PARENT						
		}

		//if in child set ischild to 1, close
		//if in parent, jump back to start of loop
		close(sockfd);
		close(newsockfd);
	} //end while loop
	return 0; 
}
