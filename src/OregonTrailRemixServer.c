#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#define MAX 4
struct M{
	unsigned char code;
	unsigned char input;
	unsigned char state;
	unsigned int timestamp[3];
	unsigned char name[50];
}M;

struct Player{
	unsigned char name[50];
	unsigned char input;
	unsigned char state;
	struct sockaddr_in IP;
};

struct GM{
	int cruiser; //100 w 100m
	int bullets; //0.1 w 1m
	int food; //0.5 w 1m
	int money;
	int survivors;
	int dead;
	int days;
	int miles;
	float weight;
}GM;

void InitWinsock()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}
void printTable(struct Player players[20]);
void addVote(struct Player players[20], struct sockaddr_in from, struct M m);
char ChooseHighVote(int votes[MAX]);
char tallyvotes(struct Player players[20]);
char parseMsg(struct M m);
void CheckCharState();
char whatMessage(struct M);
void returnSend(struct Player players[20], struct M m, SOCKET socketS);
int checkTimer(time_t start, double limit);
void Voting(SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int);
int AddingPlayers(SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int);
void establishValues(struct GM* inv);
void Store(struct GM* inv, SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int);
int Bridge(struct GM* inv, SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int); //the river of this game
void GatherResources(struct GM* inv); //robbing a general store +
void getAdvice(); //randomly chooses advice to give player
void Cruisin(SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int, int); //run the game
void updateInvBridge(struct GM* inv, int days,int length);
void Death(struct Player players[20], struct GM* gm);
void theWay(char p[50]);
//


int main(void)
{
	int i = 0;
	int test = 0;
	int test2 = 0;
	int ret;
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	time_t startTimer;
	SOCKET socketS;
	struct Player players[20];

	for (i = 0; i < 20; i++){
    	players[i].IP.sin_addr.s_addr = inet_addr("0.0.0.0");
	}

	struct M m;
	struct M ms;

	struct sockaddr_in local;
	struct sockaddr_in from;

	InitWinsock();

	int fromlen = sizeof(from);
	//char client_reply[2000];
	//char server_buffer[2000];
	local.sin_family = AF_INET;time(&startTimer);
	local.sin_port = htons(8888);
	local.sin_addr.s_addr = INADDR_ANY;

	socketS = socket(AF_INET,SOCK_DGRAM,0);

	bind(socketS,(struct sockaddr *)&local, sizeof(local));
	printf("START Server\n");
	getch();
	i = 0;
	int rt = 0;
	unsigned long nonblock = 1;
	ioctlsocket(socketS, FIONBIO, &nonblock);
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(socketS, &fd);
	ret = AddingPlayers(socketS, fd, &tv, &m, players, &startTimer, &from, fromlen);
	printTable(players);
	Cruisin(socketS, fd, &tv, &m, players, &startTimer, &from, fromlen, ret);
	printf("end program\n");
	getch();
	closesocket(socketS);
	WSACleanup();
	return 0;
}

void printTable(struct Player players[20]){
	int i;
	printf("==================TABLE==================\n");
	for(i = 0; i < 20; i++){
    	if(players[i].IP.sin_addr.s_addr != inet_addr("0.0.0.0")){
        	printf("Name: %s | IP: ", players[i].name);
        	printf("%s\n", inet_ntoa(players[i].IP.sin_addr));
    	}
	}
}
void addVote(struct Player players[20], struct sockaddr_in from, struct M m){
	int i;
	for (i = 0; i < 20; i++){
    	if(players[i].IP.sin_addr.s_addr == from.sin_addr.s_addr){
            	players[i].input = m.input;
    	}
	}
}

void returnSend(struct Player players[], struct M m, SOCKET socketS){
	int i;
	m.code = whatMessage(m);
	for(i = 0; i < 20; i++){
    	if(players[i].IP.sin_addr.s_addr != inet_addr("0.0.0.0")){
        	if(players[i].state == '0'){
            	m.code = '5';
        	}
        	else{
            	m.code = '3';
        	}
    	if(sendto(socketS, (void*)&m, sizeof(m), 0, (struct sockaddr*)&players[i].IP, sizeof(players[i].IP)) < 0){
        	printf("Send Fail\n");
        	}
    	}
	}
}
char whatMessage(struct M msg){
	char code;
	code = parseMsg(msg);
	switch(code){
    	case('1'):
        	return '3';
    	case('2'):
        	return '3';
    	default:
        	printf("Where's Doc? : %s : %c\n", msg.name, code);
        	return '6';
	}
}

char tallyvotes(struct Player players[20]){
	int i;
	//int high;
	//int match;
	//match = 0;
	int votes[4];
	for(i = 0; i < 4; i++){
    	votes[i] = 0;
	}
	for(i = 0; i < 20; i++){
    	if(players[i].IP.sin_addr.s_addr != inet_addr("0.0.0.0")){
        	if(players[i].input == 'A' || players[i].input == 'a'){
            	votes[0]++;
        	}
        	if(players[i].input == 'B' || players[i].input == 'b'){
            	votes[1]++;
        	}
        	if(players[i].input == 'C' || players[i].input == 'c'){
            	votes[2]++;
        	}
        	if(players[i].input == 'D' || players[i].input == 'd'){
            	votes[3]++;
        	}
        	if(players[i].input == 'E' || players[i].input == 'e'){
        	}
    	}
	}
	return(ChooseHighVote(votes));

}

char ChooseHighVote(int votes[MAX]){
	int i, high, highInd;
	int match[MAX];
	high = votes[0];
	highInd = 0;
	for (i = 0; i < MAX; i++){
    	if(votes[i] >= high){
        	high = votes[i];
        	highInd = i;
    	}
	}
	int matches = 0;
	for (i = 0; i < MAX; i++){
    	if (votes[i] == high){
        	match[matches] = i;
        	matches++;
    	}
	}

	if(matches > 1){
    	srand(time(NULL));
    	int r = rand() % (matches);
    	return ((char)(match[r] + 65));
	}
	return ((char)(highInd + 65));
}

char parseMsg(struct M m){
	int i;
	for (i = 49; i < 51; i++){//check if code is 1 or 2
    	if(m.code == (char)i){
        	return (m.code);
    	}
	}
	return ('\0');
}

int checkTimer(time_t start, double limit){
	time_t end;
	time(&end);
	if(difftime(end, start) >= limit){
    	return 1;
	}
	else{
    	return 0;
	}
}

void Voting(SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int fromlen){
	srand(time(NULL));
	tv->tv_sec = 10;
	tv->tv_usec = 0;
	time(startTimer);
	while (select(socketS+1, &fd, NULL, NULL, tv) == 1 || (checkTimer(*startTimer, 10.00) != 1)){
    	if (recvfrom(socketS,(void*)m, sizeof(*m), 0,(struct sockaddr*)from, &fromlen)!=SOCKET_ERROR){
        	if(whatMessage(*m) == '3'){
            	addVote(players, *from, *m);
        	}
    	}
    	fd_set fd;
    	FD_ZERO(&fd);
    	FD_SET(socketS, &fd);
	}
}

int AddingPlayers(SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int fromlen){
	time(startTimer);
	int count = 0;
	int i = 0;
	while (select(socketS+1, &fd, NULL, NULL, tv) == 1 || (checkTimer(*startTimer, 10.00) != 1)){
    	if (recvfrom(socketS,(void*)m,sizeof(*m),0,(struct sockaddr*)from,&fromlen)!=SOCKET_ERROR){
        	while (i < 20){
            	printf("%d\n",i);
            	if((from->sin_addr.s_addr != players[i].IP.sin_addr.s_addr) && (players[i].IP.sin_addr.s_addr != inet_addr("0.0.0.0"))){
                	i++;
            	}
            	else{
                	count++;
                	players[i].IP = *from;
                	players[i].state = m->state;
                	strcpy((char*)players[i].name, (char*)m->name);
                	break;
            	}
        	}
    	}
    	fd_set fd;
    	FD_ZERO(&fd);
    	FD_SET(socketS, &fd);
	}
	return count;
}


void establishValues(struct GM* inv){
	inv->cruiser = 0;
	inv->bullets = 0;
	inv->days = 0;
	inv->money = 0;
	inv->dead = 0;
	inv->food = 0;
	inv->survivors = 0;
	inv->weight = 0;
	inv->miles = 0;
}
int costOver(int money, int cost){
	if((money - cost) < 0){
    	return 1;
	}
	return 0;
}

int Bridge(struct GM* inv, SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int fromlen){//, struct Player players){
	srand(time(NULL));
	int r = rand();
	int length = (r % 50) + 30; //30 - 80 l
	r = rand();
	float weightLim = (r % 750) + 500 + (inv->survivors * 50); //500 - 1250 w
	r = rand();
	int luck = (r % 20) - 10;

	double x;
	float chance = 100 - (inv->weight / (weightLim/length));
	if(chance < 0){
    	chance = 0;
	}
	chance += luck;
	char choice;
	printf("The crew comes across da old bridge\n");
	printf("Would you like to cross?\n"
       	"A. Yes\n"
       	"B. No\n"
       	);
	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
	choice = tallyvotes(players);
	if(choice == 'a' || choice == 'A'){
    	if(chance >= 60){
        	//good
        	printf("You did it!!!!! WOOOOOOOO!!!!!\n");
        	updateInvBridge(inv, 1, length);
        	return -1;
    	}
    	else {
        	printf("The LOND'N Bridge has fallen down!!!!! AAAAAAAAAAAAHHHHHH!!!!\n");
        	int ind;
        	//bad
        	//one person dies
        	//check to see if they are in array if not choose again

        	inv->dead += 1;
        	inv->survivors -= 1;
        	updateInvBridge(inv, 1, length);
        	ind = rand() % 20;
        	while (players[ind].state == 0){
            	ind = rand() % 20;
        	}
        	players[ind].state = 0;
        	return ind;
    	}
	}
	else{
    	int delay = (rand() % 3) + 2; //delay created from 2 to 4 days
    	printf("The crew decided to find another way around the bridge...took %d days\n", delay);
    	inv->days += delay;
    	updateInvBridge(inv, delay, length);
    	return -1;
	}
}

void printInv(struct GM inv){
	printf("CRUISERS: %d\n", inv.cruiser);
	printf("BULLETS: %d\n", inv.bullets);
	printf("FOOD: %d\n", inv.food);
	printf("WEIGHT: %0.3f\n",inv.weight);
}

void getAdvice(){
	srand(time(NULL));
	int r = rand()%6;
	int secr;
	switch(r){
    	case(1):
        	//advice about food
        	printf(
        	"about yo food storage."
        	"\nThe more people there is, the food is consumed."
        	"\nWith that in mind, make sure you have enough for\n"
        	"everyone. I recommend at least 3 food per person \n"
        	"per day...but that's just my opinion\n");
        	break;
    	case(2):
        	//advice about cruiser
        	printf(
        	"The cruiser is your group's vehicle.\n"
        	"I would suggest at least 1 cruiser \n"
        	"for every 3 people in your group.\n");
        	break;
    	case(3):
        	//advice about money
        	printf(
        	"If you plan on not buying a crap ton\n"
        	"of food, you're gonna need some bullets.\n"
        	"A lot of rich guys are just waiting to\n"
        	"give out resources to convincing crews\n");
        	break;
    	case(4):
        	//advice about weight
        	printf(
        	"Make sure you keep your weight at\n"
        	"a relatively small number since\n"
        	"there may or may not be weight related\n"
        	"obstacles in this game. If you ignore\n"
        	"this advice...welp...good luck\n");
        	break;
        	default:
        	//random bad advice
        	secr = (rand()%3)+1;
        	switch(secr){
            	case(1):
                	printf("You probably should not buy food...\n"
                       	"there's always hunting\n");
                	break;
            	case(2):
                	printf("Save, save, save your money\n"
                       	"It's really important...super\n"
                       	"super important\n");
                	break;
            	case(3):
                	printf("I think that you should not\n"
                       	"buy any bullets...stealing\n"
                       	"is wrong\n");
                	break;
        	}
        	break;
	}
}

void Store(struct GM* inv, SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int fromlen){
	int cont, amt;
	char amnt, choice;
	double cost, addweight;
	cont = 1;
	while(cont == 1){
    	returnSend(players, *m, socketS);
    	printf("Money: %d\n",inv->money);
    	printf("Enter the option you'd like to buy...\n");
    	printf("========STORE========\n");
    	printf("A.Cruisers   	100m\n");
    	printf("B.Food         	1m\n");
    	printf("C.Bullets      	1m\n");
    	printf("D.Leave         	\n");
    	printf("=====================\n");
    	printf("Tallying votes\n");
    	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
    	choice = tallyvotes(players);
    	int good = 1;
    	system("cls");
    	switch(choice){

        	case 'A':
            	while(good == 1){
                	printInv(*inv);
                	returnSend(players, *m, socketS);
                	printf("Money: %d\n",inv->money);
                	printf("How many cruisers would you like to buy...\n");
                	printf("========STORE=========\n");
                	printf("A.1          	100m\n");
                	printf("B.3          	300m\n");
                	printf("C.5          	500m\n");
                	printf("D.Go Back         	\n");
                	printf("======================\n");
                	printf("================Choice: \n");
                	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
                	amnt = tallyvotes(players);
                	switch(amnt){
                    	case('A'):
                        	amt = 1;
                        	break;
                    	case('B'):
                        	amt = 3;
                        	break;
                    	case('C'):
                        	amt = 5;
                        	break;
                    	case('D'):
                        	good = 0;
                        	break;
                    	}
                	if(amnt != 'D'){
                    	cost = amt * 100;
                    	if(costOver(inv->money, cost) == 0){
                        	addweight = amt * 100;
                        	inv->cruiser += amt;
                        	inv->weight += addweight;
                        	inv->money = inv->money - cost;
                        	good = 1;
                    	}
                    	else{
                        	printf("You don't have enough money\n");
                        	Sleep(300);
                    	}
                	}
                	system("cls");
            	}
            	break;

        	case 'B':
            	while(good == 1){
                	returnSend(players, *m, socketS);
                	printInv(*inv);
                	printf("Money: %d\n",inv->money);
                	printf("How much food would like you to buy...\n");
                	printf("========STORE=========\n");
                	printf("A.100         	100m\n");
                	printf("B.300         	300m\n");
                	printf("C.500         	500m\n");
                	printf("D.Go Back         	\n");
                	printf("======================\n");
                	printf("================Choice: \n");
                	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
                	amnt = tallyvotes(players);
                	switch(amnt){
                    	case('A'):
                        	amt = 100;
                        	break;
                    	case('B'):
                        	amt = 300;
                        	break;
                    	case('C'):
                        	amt = 500;
                        	break;
                    	case('D'):
                        	good = 0;
                        	break;
                    	}
                	if(amnt != 'D'){
                    	cost = amnt * 1;
                    	if(costOver(inv->money, cost) == 0){
                        	addweight = amt * 0.5;
                        	inv->food += amt;
                        	inv->weight += addweight;
                        	inv->money -= cost;
                    	}
                    	else{
                        	printf("You don't have enough money...choose again\n");
                        	Sleep(300);
                    	}
                	}
                	system("cls");
            	}
            	break;

        	case 'C':
            	while(good == 1){
                	returnSend(players, *m, socketS);
                	printInv(*inv);
                	printf("Money: %d\n",inv->money);
                	printf("How many bullets would you like to buy...\n");
                	printf("========STORE=========\n");
                	printf("A.30           	30m\n");
                	printf("B.75           	75m\n");
                	printf("C.100         	100m\n");
                	printf("D.Go Back         	\n");
                	printf("======================\n");
                	printf("================Choice: \n");
                	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
                	amnt = tallyvotes(players);
                	switch(amnt){
                    	case('A'):
                        	amnt = 30;
                        	break;
                    	case('B'):
                        	amnt = 75;
                        	break;
                    	case('C'):
                        	amnt = 100;
                        	break;
                    	case('D'):
                        	good = 0;
                        	break;
                    	}
                	if(amnt != 'D'){
                    	cost = amnt * 1;
                    	if (costOver(inv->money, cost) == 0){
                        	addweight = amt * 0.1;
                        	inv->bullets += amt;
                        	inv->weight += addweight;
                        	inv->money -= cost;
                    	}
                    	else{
                        	printf("You don't have enough money...choose again\n");
                        	Sleep(300);
                    	}
                	}
                	system("cls");
            	}
            	break;

        	default:
            	printf("Leaving store...Thank you for coming\n");
            	cont = 0;
            	break;
    	}
    	system("cls");
	}
}

void updateInv(struct GM* inv){
	//ADD CHECK IF ZERO FOR THE SUBTRACTION
	float decWeight = (3*inv->survivors)/2;
	int decFood = (3*inv->survivors);
	inv->miles += 15;
	inv->days += 1;
	if(inv->food - decFood <= 0){
    	inv->food = 0;
    	return;
	}
	if(inv->weight - decWeight <= 0){
    	inv->weight = 0;
    	return;
	}
	inv->weight -= decWeight;
	inv->food -= decFood;

}

void updateInvBridge(struct GM* inv, int days,int length){
	float decW = days * ((3*inv->survivors)/2);
	int decf = days * (3*inv->survivors);
	inv->miles += length;
	inv->days += days;
	if(inv->food - decf <= 0){
    	inv->food = 0;
    	return;
	}
	if(inv->weight - decW <= 0){
    	inv->weight = 0;
    	return;
	}
	inv->weight -= decW;
	inv->food -= decf;
}

void printStats(struct GM inv){
	printf("\n\n=====Your==Stuff=====\n");
	printf("Days:      	%d\n", inv.days);
	printf("Miles:     	%d\n", inv.miles);
	printf("Food:      	%d\n", inv.food);
	printf("Bullets:   	%d\n", inv.bullets);
	printf("Dead:      	%d\n", inv.dead);
	printf("Alive:     	%d\n", inv.survivors);
	printf("Money:     	%dM\n",inv.money);
	printf("Weight:    	%0.2fW\n", inv.weight);
	printf("=====================\n\n");
}

int checkStore(int atMiles, int storeone, int storetwo, int storethree, int storefour){
	int stores;
	if  ((atMiles <= storeone + 7.5) ||
    	(atMiles >= storetwo - 7.5 && atMiles <= storetwo + 7.5 )||
    	(atMiles >= storethree - 7.5 && atMiles <= storethree + 7.5) ||
    	(atMiles >= storethree - 7.5 && atMiles <= storethree + 7.5)){
    	return stores = 1;
	}else{
    	return stores = 0;
	}
}

void GatherResources(struct GM* inv){
	int r = (rand() % 8) +1 ;   	///random number 1-8 switch
	int food = rand() % 200;	/// random foods 1-100
	int money = rand() % 300;   /// random moneys 1-300
	int chance = (rand() % 10) + 1; ///random chance variable 1-10 depending on pass or fail
	int bulletsUsed =((rand() % 50) +1);
	if(inv->bullets > bulletsUsed){
    	switch(r){
        	case 1:
            	printf("The crew comes across Rich Dawg,\nowner of the local MacRib franchise.\n");
            	break;
        	case 2:
            	printf("The crew comes across Shades-Of-Grey,\nlocal hip-hop star.\n");
            	break;
        	case 3:
            	printf("The crew comes across Hugh,\na vertically challenged kangaroo\t\t\t\t\t\t\t\t\t bro!\n");
            	break;
        	case 4:
            	printf("The crew comes across BIG Gelow,\nBIGGEST of Gelow.\n");
            	break;
        	case 5:
            	printf("The crew comes across Evan,\nan Evil Wizard.\n");
            	break;
        	case 6:
            	printf("The crew comes across the Sushi Master.\n");
            	break;
        	case 7:
            	printf("The crew comes across the Milk Man.\n");
            	break;
        	default:
            	printf("The crew comes across Colonel Coup.\nHe sells chicken.\n");
            	break;
    	}
    	if(r <= chance){
        	printf("The crew robbed successfully!\nThey got away with %d foods and %d moneys!\n", food, money);
    	}
    	else if(r > chance){
        	food = 0;
        	money = 0;
        	printf("The crew was caught trying to steal!\nThey ran away with %d foods and %d moneys :(\n", food, money);
    	}
    	printf("Bullets Used: %d\n", bulletsUsed);
    	inv->bullets -=bulletsUsed;
    	inv->food += food;
    	inv->money += money;
    	inv->days += 1;
	}
	else{
    	printf("You don't have enough bullets to hunt.\n");
	}
}

int checkCruiser(struct GM inv){
	if (inv.cruiser >= (inv.survivors / 3)){
    	printf("Cruiser BONUS!!!! +5 MILES\n");
    	return 5;
	}
	else{
    	return 0;
	}
}

void Cruisin(SOCKET socketS, fd_set fd, struct timeval* tv, struct M* m, struct Player players[20], time_t* startTimer, struct sockaddr_in* from, int fromlen, int ret){
	srand(time(NULL));
	int ran = rand();
	char amt;
	int bRet = 0;
	int s;
	struct GM inv;
	establishValues(&inv);
	int gameEnd = 2;
	inv.survivors = ret;
	int r = ((inv.survivors*3) * 0.45);
	int minDays = (inv.survivors * 3) - r;
	int minFood = 3 * inv.survivors;
	int miles = minDays * 15;
	int atMiles = 0;
	int store = 1;
	int distBetween = miles/4;
	int storeone, storetwo, storethree, storefour;
	int mod = 5;
	int daysLeft = inv.survivors * 3;
	inv.money = inv.survivors * 100;

	storeone = 0;
	storetwo = miles * 0.25;
	storethree= miles * 0.50;
	storefour = miles * 0.75;

	while(!(gameEnd < 2)){
    	//gameEnd check
    	if(inv.survivors <= 0 || daysLeft <= inv.days){
        	printf("You lost. Sorry!\n");
        	gameEnd = 0;
        	break;
    	}
    	else if(inv.miles >= miles){
        	printf("You won!!!\n");
        	gameEnd = 1;
        	break;
    	}
    	if(checkStore(atMiles, storeone, storetwo, storethree, storefour) == 1){
        	returnSend(players, *m, socketS);
        	printStats(inv);
        	printf("\n========MENU=========\n");
        	printf("A. 	Cruise\n");
        	printf("B. 	Mug\n");
        	printf("C. 	Store\n");
        	printf("===============Choice: \n");
        	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
        	amt = tallyvotes(players);
        	//amnt = (int)(amt) - 64;
        	switch(amt){
            	case 'A': //cruise
                	s = ran % mod;
                	switch(s){
                    	case 1:
                        	returnSend(players, *m, socketS);
                        	Bridge(&inv, socketS, fd, tv, m, players, startTimer, from, fromlen);
                        	atMiles += 15;
                        	updateInv(&inv);
                        	break;
                    	case 2:
                        	printf("\nHere's some advice...\n");
                        	getAdvice();
                        	atMiles += 15;
                        	inv.miles += checkCruiser(inv);
                        	updateInv(&inv);
                        	break;
                    	case 3:
                        	Death(players, &inv);
                        	inv.dead += 1;
                        	inv.survivors -= 1;
                        	atMiles +=15;
                        	inv.miles += checkCruiser(inv);
                        	updateInv(&inv);
                        	break;
                    	default:
                        	printf("The crew just cruised today\n");
                        	atMiles +=15;
                        	inv.miles += checkCruiser(inv);
                        	updateInv(&inv);
                        	break;
                	}
                    	break;
            	case 'B': //hunt
                	returnSend(players, *m, socketS);
                	GatherResources(&inv);
                	break;
            	case 'C': //store
                	returnSend(players, *m, socketS);
                	Store(&inv, socketS, fd, tv, m, players, startTimer, from, fromlen);
                	store = 0;
                	break;
        	}
    	}
    	else{
        	if(inv.food < (inv.survivors * 3)){
            	Death(players, &inv);
            	inv.survivors -= 1;
            	inv.dead += 1;
            	if(inv.survivors <= 0 || (r + minDays) <= inv.days){
                	printf("You lost...sorry\n");
                	gameEnd = 0;
                	break;
            	}
        	}
        	printStats(inv);
        	returnSend(players, *m, socketS);
        	printf("========MENU=========\n");
        	printf("A. 	Cruise\n");
        	printf("B. 	Mug\n");
        	printf("================Choice: \n");
        	Voting(socketS, fd, tv, m, players, startTimer, from, fromlen);
        	amt = tallyvotes(players);
        	switch(amt){
            	case 'A': //cruise
                	s = rand() % 5;
                	switch(s){
                    	case 1:
                        	returnSend(players, *m, socketS);
                        	Bridge(&inv, socketS, fd, tv, m, players, startTimer, from, fromlen);
                        	atMiles +=15;
                        	break;
                    	case 2:
                        	printf("\nHere's some advice...\n");
                        	getAdvice();
                        	atMiles +=15;
                        	inv.miles += checkCruiser(inv);
                        	updateInv(&inv);
                        	break;
                    	case 3:
                        	Death(players, &inv);
                        	printf("Death\n");
                        	inv.dead += 1;
                        	inv.survivors -= 1;
                        	atMiles +=15;
                        	inv.miles += checkCruiser(inv);
                        	updateInv(&inv);
                        	break;
                    	default:
                        	printf("The crew cruised 15 miles today\n");
                        	atMiles +=15;
                        	inv.miles += checkCruiser(inv);
                        	updateInv(&inv);
                        	break;
                    	}
                    	break;

            	case 'B':
                	returnSend(players, *m, socketS);
                	GatherResources(&inv);
                	break;
        	}

    	}

	}
}

void Death(struct Player players[20], struct GM* gm) {
	int r;
	int i;
	/// pick random
	int ind, nr, cnt;
	ind = rand() % gm->survivors;// picks a random player index using r
	while (players[ind].state == 0){
    	ind = rand() % gm->survivors;// picks a random player index using r
	}
	players[ind].state = '0';
	players[ind].input = 'e';
	theWay(players[ind].name);
	printf("\n\n");
}

void theWay(char p[50]) {
	int r;
	r = rand() % 8 +1;

	switch(r) {
    	case 1:
        	printf("Better luck next time, %s was killed by dire wolves.\n", p);
        	break;
    	case 2:
        	printf("Watch your step, %s tripped on a rock and died.\n", p);
        	break;
    	case 3:
        	printf("Wash your hands, %s died of street dysentery.\n", p);
        	break;
    	case 4:
        	printf("WHAM!! %s got shot by the Blips.\n", p);
        	break;
    	case 5:
        	printf("Cover your ears, %s bled to death after listening to Shades-Of-Gray.\n", p);
        	break;
    	case 6:
        	printf("RUN! %s died thanks to global warming.\n", p);
        	break;
    	case 7:
        	printf("%s died from death.\n", p);
        	break;
    	case 8:
        	printf("YEET. %s died.\n", p);
        	break;
    	default:
        	printf("Sucks to suck. %s has died.", p);
	}
}
