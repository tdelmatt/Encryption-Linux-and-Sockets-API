/**********************************************
 *Name: Taylor Del Matto
 *Assignment 4
 *otp_dec.c
 *Operating Systems
 *This component reads in a keyfile and a data file
 *verifies the information and then writes it to the 
 *decryption server to decrypt, it recieves the decrypted information
 *and outputs it to stdout
 * ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <fcntl.h>
char data[50000];
int sockfd, portno, n;
int sentstatus;
int readstatus;
char gbstatus[3];
char good[2];
char bad[3];

void error(const char *msg)
{
    perror(msg);
    exit(0);
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
			//write bad to socket
			//n = write(sockfd1, bad, strlen(bad));
		}
		else{
			//printf("read confirmation....\n");
			//write good to socket
			n = write(sockfd1, gbstatus, 3);
			
			//printf("Here is the message: %s\n", data2);
			readstatus = 1;
			
		}
	}
}

//get data this function gets command line input for testing
void getdata(char data1[], int size){
	printf("Please enter the message: ");
	bzero(data1,size);
    	fgets(data1,size-1,stdin);
}

//wrdata this function writes data over the socket connection
//then waits for a confirmation signal wrdata always writes 
//to a recieving readdata function on the other end
void wrdata(char data1[], int sockfd1){
	//printf("write client begins\n");
	sentstatus = 0;
	while(sentstatus == 0){
		n = write(sockfd1, data1,strlen(data1));
    		if (n < 0) 
        		 error("ERROR writing to socket");
		else{
			n=0;
			n = read(sockfd1, gbstatus, 3);
			if(n > 0){//if nless than or equal to 0, data was not recieved.  
			//if(strcmp(gbstatus, "g") == 0){ 
				//printf("good transmission");
				//printf("write confirmed\n");
				sentstatus = 1;
			//}
			//else{
			//	printf("compare went wrong");
			//	printf("here is gbstatus 0%s0", gbstatus);	
			//	sentstatus = 1;
			}
			else{
				printf("no write confirmation\n");
				sentstatus = 1;
			}
		}	
	}	
}
int readsize;
int file1;
int i;

//readfile this function reads in data from the file
//and checks that there are no bad characters
void readfile(char *filename, char datain1[], int fd){
	//printf("filename %s\n", filename);
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

//verify, this function verifies that there are no bad characters
//and that key file is at least as long as datafile
void verify(char datain2[], char keyin2[]){
	if(strlen(datain2) > strlen(keyin2)){
		//printf("keyin is %s\n", keyin2);
		//printf("datain is %s\n", datain2);
		printf("key not long enough\n");
		exit(1);
	}
	else{
		readsize = (strlen(datain2));
	}
	for(i=0; i < readsize-1; i++){
		if(datain2[i] > 90 || datain2[i] < 65 && datain2[i] != 32){
			
			printf("bad characters in message\n");
			exit(1);
		}
		
		if(keyin2[i] > 90 || keyin2[i] < 65 && keyin2[i] != 32){
			
			printf("bad characters in key\n");
			exit(1);
		}
	}

}

int main(int argc, char *argv[])
{
	int portin, sockfd1;
	//bzero(good, 3);
	strcpy(good, "g");
	bzero(bad, 3);
	strcpy(bad, "b");
	struct sockaddr_in serv_addr, serv_addr1;
	struct hostent *server, *server1;
	char datain[50000];
	char keyin[50000];
	char cipherin[50000];
	int keyfile;
	char verifyised[20];
	char portinstr[20];
	if (argc < 4) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	
	//connect to server	
	portno = atoi(argv[3]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);//define socket
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname("localhost");
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	//set server address information
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	
	//connect
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	
	//verify not dec_d, enc files write a string of eeee
	wrdata("dddddddddd", sockfd);
	//read in response, vefify is not enc_d
	readdata(sockfd, verifyised, 20);

	if(verifyised[0] == 'e'){
		printf("this is enc_d, connection not allowed");
		exit(1);
	}
	else if(verifyised[0] == 'd'){
		//printf("connected to enc_d\n");
	}
	//verify port
	bzero(portinstr, 20);
	readdata(sockfd, portinstr, 15);
	portin = atoi(portinstr);
	
	//read data from file 
	readfile(argv[1], datain, file1);
	readfile(argv[2], keyin, keyfile);
	
	//verify data from files
	verify(datain, keyin);
	//write data to server
	wrdata(datain, sockfd);
	wrdata(keyin, sockfd);
	
	//read decrypted data back from server
	bzero(cipherin,49999);
	readdata(sockfd, cipherin, 50000);
	
	//print data to stdout
	i = 0;
	while(cipherin[i] != '*'){
		printf("%c", cipherin[i]);	
		i++;
	}
	printf("\n");
	close(sockfd);
	return 0;
	}
