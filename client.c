#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 2048

SOCKET serverSocket;
struct sockaddr_in serverAddress;
int bytesRead;
char buf[MAX_SIZE] = "ConnectTest";
FILE *file;
WSADATA wsadata;

void ClientSetup(char *ServerIP, char *ServerPort);
void Con2Server();
void CommandRead(char *comread);
void LOGIN();
void CREATE();
void DEPOSIT();
void WITHDRAW();


int main(int argc, char** argv) {

    ClientSetup(argv[1], argv[2]);

	while(1) {
		printf(">>>");
		scanf("%s", buf);
        CommandRead(buf);
	 }

	return 0;
}

void ClientSetup(char *ServerIP,char* ServerPort) {
	
	 if( WSAStartup(MAKEWORD(2,2),(LPWSADATA)&wsadata) != 0) {
        printf("Winsock Error\n");
        system("PAUSE");
        exit(1);
	 }

	 memset(&serverAddress, 0, sizeof(serverAddress));
     serverAddress.sin_family = AF_INET;
     serverAddress.sin_addr.s_addr = inet_addr(ServerIP); 
     serverAddress.sin_port = htons(atoi(ServerPort));
     
     serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	 if(connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0){
            printf("Connect Error\n");
            system("PAUSE");
            exit(1);
	 }   
	 
	 else printf("Connecting to Server successfully.\n");     	 
	 send(serverSocket, buf, sizeof("ConnectTest"), 0);
	 bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	 buf[bytesRead] = '\0';	
	 closesocket(serverSocket);		 		
}

void Con2Server() {
	
	 serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	 if(connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0){
            printf("Connect Error\n");
            system("PAUSE");
            exit(1);
	 }	 
	 send(serverSocket, buf, strlen(buf), 0);
	 bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	 buf[bytesRead] = '\0';			
}

void CommandRead(char *comread) {
	
	 if(!strcmp(comread,"login")) LOGIN();
	 else if (!strcmp(comread,"create")) CREATE();
	 else if (!strcmp(comread,"deposit")) DEPOSIT();
	 else if (!strcmp(comread,"withdraw")) WITHDRAW();
	 else printf("Invalid Command!\n") ;		
}

void LOGIN(){
	
	Con2Server();
	printf("Your account: ");
	scanf("%s", buf);
	send(serverSocket, buf, strlen(buf), 0); // account send
	bytesRead = recv(serverSocket, buf, MAX_SIZE, 0); // If correct, receive "Password Ask". If not correct, receive "Account Error".
	buf[bytesRead] = '\0';
	
	if(!strcmp(buf, "Account Error")){
		fclose(file);
		closesocket(serverSocket);
		printf("This account does not exist! \n");
		printf("Please create one for it. \n");
	}
	
	if(!strcmp(buf, "Password Ask")){
		printf("Please type in your password: ");
		scanf("%s", buf);
		send(serverSocket, buf, strlen(buf), 0);
		bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
		buf[bytesRead] = '\0';
		
		if(!strcmp(buf, "Password Error")){
		fclose(file);
		printf("This password is invalid! \n");
		printf("Please login this account once again! \n");
		closesocket(serverSocket);
		}
		
		else	printf("Login successfully! \n");
	}
	else{
		printf("Login Error! \n");
		printf("Please try again! \n");
	}
	
	return;
}

void CREATE(){
	Con2Server();
	printf("Create an account: ");
	scanf("%s", buf);
	send(serverSocket, buf, strlen(buf), 0);
	bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	
	if(!strcmp(buf, "File Overwrite")){
		printf("This account has already existed! \n");
		printf("Please try another one! \n");
		return;
	}
	else if(!strcmp(buf, "Password")){
		printf("Please type in your desired password: ");
		scanf("%s", buf);
		send(serverSocket, buf, strlen(buf), 0);
		bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
		buf[bytesRead] = '\0';
		if(!strcmp(buf, "Remaining money")){
			printf("Your desired remaining money in this account: ");
			scanf("%s", buf);
			send(serverSocket, buf, strlen(buf), 0);
			bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
			buf[bytesRead] = '\0';
			if(!strcmp(buf, "Finished")){
				printf("Your account is created successfully!\n");
				return; 
			}
		}
	}
	else{
		printf("Failure! Your account cannot be created! \n");
		printf("Please try again!\n");
		return;
	}	
}

void DEPOSIT(){
	
	Con2Server();
	bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	printf("Current money in this account: %s \n", buf);
	printf("Deposit how much money in this account: ");
	scanf("%s", buf);
	send(serverSocket, buf, strlen(buf), 0);
	bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	
	if(!strcmp(buf, "Deposit Error")){
		printf("Deposit Error occurs! \n");
		printf("Please try again!\n");
		return;
	}
	
	printf("The remaining money after depositing in this account: %s \n", buf);
	return;
}

void WITHDRAW(){
	
	Con2Server();
	bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	printf("Current money in this account: %s \n", buf);
	printf("Withdraw how much money in this account: ");
	scanf("%s", buf);
	send(serverSocket, buf, strlen(buf), 0);
	bytesRead = recv(serverSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	
	if(!strcmp(buf, "Withdraw Error")){
		printf("Withdraw Error occurs! \n");
		printf("Please try again!\n");
		return;
	}
	
	printf("The remaining money after withdrawing in this account: %s \n", buf);
	return;
}
