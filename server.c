#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_SIZE 2048

SOCKET serverSocket, clientSocket;
struct sockaddr_in serverAddress, clientAddress;
int clientAddressLen;
int bytesRead;
char buf[MAX_SIZE];
char caccount[MAX_SIZE]; /*caccount represents current running account*/
char password[MAX_SIZE];

FILE *file;
WSADATA wsadata;
HANDLE hFind;
WIN32_FIND_DATA FindData;

void ServerSetup(char* ServerPort);
void CommandRead(char* comread);
int Search_in_File(FILE* fptr, char* str);
void LOGIN();
void CREATE();
void DEPOSIT();
void WITHDRAW();


int main(int argc, char** argv){
	
	ServerSetup(argv[1]);
	printf("Waiting...\n");
	
	while(1){
		clientAddressLen = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);		
		bytesRead = recv(clientSocket, buf, MAX_SIZE, 0); 
		buf[bytesRead] = '\0';
		send(clientSocket, buf, strlen(buf), 0);
		CommandRead(buf);
		closesocket(clientSocket);
	}

	return 0;
}

void ServerSetup(char *ServerPort){
	
	 if(WSAStartup(MAKEWORD(2,2),(LPWSADATA)&wsadata) != 0){ 
        printf("Winsock Error\n");
        system("PAUSE");
        exit(1);
	 }

	 serverSocket = socket(PF_INET, SOCK_STREAM, 0); 

	 memset(&serverAddress, 0, sizeof(serverAddress));
     serverAddress.sin_family = AF_INET;
     serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
     serverAddress.sin_port = htons(atoi(ServerPort));

     if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        printf("Bind Error\n");
        system("PAUSE");
        exit(1);
	 }

	 if(listen(serverSocket, 1) < 0){
		printf("Listen Error\n");
		system("PAUSE");
		exit(1);
	 }		 	 
}

void CommandRead(char *comread){
	 if(!strcmp(comread,"login"))	LOGIN();
	 else if (!strcmp(comread,"create"))	CREATE();
	 else if (!strcmp(comread,"deposit"))	DEPOSIT();
	 else if (!strcmp(comread,"withdraw"))	WITHDRAW();
	 else if (!strcmp(comread, "ConnectTest")){
	 	printf("One client is connecting.\n"); 
	 	printf("Client IP is : %s \n", inet_ntoa(clientAddress.sin_addr));
	 }
	 else	printf("Invalid Command!\n");	
} 

int Search_in_File(FILE* fptr, char* str){
	int find_result = 0;
	char line_temp[100];
	while(fgets(line_temp, 100, fptr) != NULL){
		if(strstr(line_temp, str) != NULL){
			find_result++;
		}
	}
	if(find_result == 0)	return -1;
	return 0;
}

void LOGIN(){
	int result, count = 0, line = 1;
	char temp[50];
	printf("Client is logining...\n");
	bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	strcpy(caccount, buf);
	strcat(caccount, ".txt");
	file = fopen(caccount, "r");
	
	if(file == NULL){
		strcpy(buf, "Account Error");
		send(clientSocket, buf, strlen(buf), 0);
		printf("Login error occurs!\n");
		fclose(file);
		return;
	}
	
	printf("Account matched! Password waiting...\n");
	strcpy(buf, "Password Ask");
	send(clientSocket, buf, strlen(buf), 0);
	bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	strcpy(password, buf);
	result = Search_in_File(file, password);
	
	if(result == -1){
		strcpy(buf, "Password Error");
		send(clientSocket, buf, strlen(buf), 0);
		printf("Password error occurs!\n");
		fclose(file);
		return;
	}
	
	printf("Login completed!\n");
	fclose(file);
	return;
}

void CREATE(){
	char temp[MAX_SIZE];
	printf("Creating an account...\n");
	bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	strcpy(temp, buf);
	strcat(temp, ".txt");
	file = fopen(temp, "r");
	
	if(file != NULL){
		strcpy(buf, "File Overwrite");
		send(clientSocket, buf, MAX_SIZE, 0);
		printf("Creating account error occurs!\n");
		fclose(file);
	}
	else{
		fclose(file);
		file = fopen(temp, "w");
		strcpy(buf, "Password");
		send(clientSocket, buf, strlen(buf), 0);
		bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
		buf[bytesRead] = '\0';
		fprintf(file, "%s\n", buf);
		strcpy(buf, "Remaining money");
		send(clientSocket, buf, strlen(buf), 0);
		bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
		buf[bytesRead] = '\0';
		fprintf(file, "%s\n", buf);
		strcpy(buf, "Finished");
		send(clientSocket, buf, strlen(buf), 0);
		printf("Creating account completed! \n");
		fclose(file);
	}
	
	return;
}

void DEPOSIT(){
	int dnum, cnum; /*dnum represents the depositing number. cnum is the current number in this bank account.*/
	char temp_password[50];
	printf("Client is depositing...\n");
	file = fopen(caccount, "r");
	fgets(temp_password, 50, file);
	fgets(buf, MAX_SIZE, file);
	cnum = atoi(buf);
	sprintf(buf, "%d", cnum);
	send(clientSocket, buf, MAX_SIZE, 0);
	bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	dnum = atoi(buf);
	
	if(dnum < 0){
		strcpy(buf, "Deposit Error");
		send(clientSocket, buf, MAX_SIZE, 0);
		printf("Depositing error occurs!\n");
		fclose(file);
		return;
	}
	
	cnum = cnum + dnum;
	sprintf(buf, "%d", cnum);
	fclose(file);
	file = fopen(caccount, "w+");
	fprintf(file, "%s\n", password);
	fprintf(file, "%s\n", buf);
	fclose(file);
	send(clientSocket, buf, strlen(buf), 0);
	printf("Depositing completed!\n");
	return;
}

void WITHDRAW(){
	int wnum, cnum; /* wnum represents the withdrawing number. cnum is the current number in this bank account.*/
	char temp_password[50];
	printf("Client is withdrawing...\n");
	file = fopen(caccount, "r");
	fgets(temp_password, 50, file);
	fgets(buf, MAX_SIZE, file);
	cnum = atoi(buf);
	sprintf(buf, "%d", cnum);
	send(clientSocket, buf, MAX_SIZE, 0);
	bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
	buf[bytesRead] = '\0';
	wnum = atoi(buf);
	
	if(wnum < 0){
		strcpy(buf, "Withdraw Error");
		send(clientSocket, buf, MAX_SIZE, 0);
		printf("Withdrawing error occurs!\n");
		fclose(file);
		return;
	}
	
	cnum = cnum - wnum;
	
	if(cnum < 0){
		strcpy(buf, "Withdraw Error");
		send(clientSocket, buf, strlen(buf), 0);
		printf("Withdrawing error occurs!\n");
		fclose(file);
		return;
	} 
	
	sprintf(buf, "%d", cnum);
	fclose(file);
	file = fopen(caccount, "w+");
	fprintf(file, "%s\n", password);
	fprintf(file, "%s\n", buf);
	fclose(file);
	send(clientSocket, buf, strlen(buf), 0);
	printf("Withdrawing completed!\n");
	return;
}
