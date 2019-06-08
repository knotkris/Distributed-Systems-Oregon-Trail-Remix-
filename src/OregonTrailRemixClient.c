#include <winsock2.h>
#include <stdio.h>

void InitWinsock()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

struct Message{
	unsigned char code;
	unsigned char input;
	unsigned char state;
	unsigned int timestamp[3];
	unsigned char name[50];
}Message;

int ClientSend(SOCKET socketC, struct Message m, int len, struct sockaddr_in serverInfo, struct Message* Sm);
char Parse(struct Message m);
char checkCharState(struct Message ServerMessage);

int main(void)
{
	SOCKET socketC;
	struct Message m, Sm;
	int saved = 0;
	char state = '1';
	char code = '0';
	m.code = '0';
	m.input = '0';
	InitWinsock();
	struct sockaddr_in serverInfo;
	int len = sizeof(serverInfo);
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(8888);
	serverInfo.sin_addr.s_addr = inet_addr("10.121.126.145");

	while (1)
	{
	    system("cls");
    	if(code == '0' && state == '1'){
        	socketC = socket(AF_INET,SOCK_DGRAM,0);
        	printf("Please input username: ");
        	gets((char*)m.name);
        	m.code = '1';
    	}
    	if(code == '3' && state == '1'){
        	printf("Please enter game input: ");
        	scanf(" %c", &m.input);
        	m.code = '2';
    	}

    	if(state == '0'){
        	printf("You are dead\n");
        	//shutdown(socketC, 1);
        	state = '2';
        	Sleep(3000);
        	break;

    	}

    	int cnt = 0;
    	if (state == '1'){
        	while((saved = ClientSend(socketC, m, len, serverInfo, &Sm)) == 1){//send error check
            	if(cnt == 5){
                	break;
            	}
            	cnt++;
        	}
        	//printf("Receive response from server: %s\n", (char*)Sm.name); //response from client
        	//closesocket(socketC);
        	code = Parse(Sm); //returns the parsed code value
        	printf("code:%c\n",code);
        	switch(code){
            	case('3'):
                	printf("ack\n");
                	break;

            	case('4'):
                	printf("calc result\n");
                	break;

            	case('5'):
                	printf("char state\n");
                	Sleep(3000);
                	state = checkCharState(Sm); //update char state
                	if (state == '2'){
                    	printf("state error");
                	}//error check
                	break;

            	case('6'):
                	printf("game state\n");
                	//update game state
                	break;
            	default:
                	printf("that's not a valid message\n");
                	//send again
                	break;
        	}
    	}
	}
	printf("you dead...fuck\n");
	closesocket(socketC);
	WSACleanup();

	return 0;
}

int ClientSend(SOCKET socketC, struct Message m, int len, struct sockaddr_in serverInfo, struct Message* Sm){
	if (sendto(socketC, (void*)&m, sizeof(m), 0, (struct sockaddr*)&serverInfo, len) != SOCKET_ERROR)
    	{
    	    printf("send good\n");
        	if (recvfrom(socketC, (void*)Sm, sizeof(*Sm), 0, (struct sockaddr*)&serverInfo, &len) != SOCKET_ERROR)
        	{
            	//memcpy(&Sm, buffer, sizeof(buffer));
            	//printf("Receive response from server: %s\n", (char*)Sm.name);
           	// printf("Receive response from server: %c\n", &Sm.input);
            	//printf("Receive response from server: %u\n", &m.input);
            	printf("received\n");
            	return 0;
        	}
        	else{
                printf("receive failed %d\n", WSAGetLastError());
        	}
    	}
    printf("send failed\n");
	return 1;
}

char Parse(struct Message m){
	int i;
	for (i = 51; i < 54; i++){
    	printf("%c\n", i);
    	if(m.code == (char)i){
        	return (m.code);
    	}
	}
	return ('\0');
}

char checkCharState(struct Message ServerMessage){
	if (ServerMessage.state == '0'){
    	return ('0');
	}
	else if(ServerMessage.state == '1'){
    	return ('1');
	}
	else{
    	return ('2');
	}
}
