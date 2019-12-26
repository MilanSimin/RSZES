#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>           
//sve potrebne biblioteke za rad

#define DISPLAY_X 640 
#define DISPLAY_Y 480
#define MAX_PKT_SIZE (640*480*4)


#define BLUE 0x001F
#define RED 0xF800
#define BLACK 0x0000

using namespace std;

//funkcija za dobijanje boja
/*unsigned int get_colour(string colour_s)
{
	if (colour_s=="BLACK")
		return BLACK;

	else if (colour_s=="RED")
		return RED;

	else if (colour_s=="BLUE")
		return BLUE;

	return BLACK;
}*/

//funkcija za bojenje pozadine
void colour_bckg(int colour, int *img)
{
	for(int x=0; x<DISPLAY_X; x++){
		for(int y=0; y<DISPLAY_Y; y++){
			img[y*DISPLAY_X +x]= colour;
		}
	}

}

//funkcija za bojenje vertikalne linije
void colour_vertical(int colour, int *img, int x, int y1, int y2)
{
		for(int y=y1; y<y2; y++){
			img[y*DISPLAY_X +x]= colour;
		}

}

//funkcija za bojenje horizontalne linije
void colour_horizontal(int colour, int *img, int y, int x1, int x2)
{
		for(int x=x1; x<x2; x++){
			img[y*DISPLAY_X +x]= colour;
		}
}

//funkcija za bojenje kvadrata
void colour_rect(int colour, int *img, int x1, int x2, int y1, int y2)
{
	for(int x=x1; x<x2; x++){
		for(int y=y1; y<y2; y++){
			img[y*DISPLAY_X +x]= colour;
		}
	}
}

int first_free_square(int *pids, int n)
{
	for (int i = 0; i < n; i++){
		if(pids[i] == 0)
			return i;	
	}
	return -1;
}

int take_section(int *pids, int pid)
{
	for(int i = 0; i < 4; i++){
		if(pids[i] == 0){
			pids[i]=pid;
			return i;
		}
	}
	return -1;

}

int free_section(int *pids,int pid)
{
	for(int i = 0; i < 4; i++){
		if(pids[i] == pid){
			pids[i]=0;
			return i;		
		}	
	}
	return -1;
}

void print_list(int *pids, int n)
{
	cout << "Client list: "<<endl;
	for (int i = 0; i < n; i++){
		cout << pids[i] << " ";
		cout << "\n";
	}
}

string sec_n_to_str(int n) { //broj kvadranta u string
	if(n == 0)
		return "Top left";
	if(n == 1)
		return "Top right";
	if(n == 2)
		return "Bottom left";
	if(n == 3)
		return "Bottom right";

	return "Undefined";
}


void sock_write(int sockfd, string s) {
	write(sockfd, s.c_str(), s.size());
}

string sock_read(int sockfd) {
	const int MAX = 256;
	char buff[MAX];
	bzero(buff, MAX);

	read(sockfd, buff, sizeof(buff));
	string message(buff);

	return message;
}

void clear_section(int *img, int clinum) {
	if (clinum == 0) {
		colour_rect(BLACK, img, 0, DISPLAY_X / 2 - 1, 0, DISPLAY_Y / 2 - 1);
	} else if (clinum == 1) {
		colour_rect(BLACK, img, DISPLAY_X / 2 + 1, DISPLAY_X - 1, 0, DISPLAY_Y / 2 - 1);
	} else if (clinum == 2) {
		colour_rect(BLACK, img, 0, DISPLAY_X / 2 - 1, DISPLAY_Y / 2 + 1, DISPLAY_Y - 1);
	} else if (clinum == 3) {
		colour_rect(BLACK, img, DISPLAY_X / 2 + 1, DISPLAY_X - 1, DISPLAY_Y / 2 + 1, DISPLAY_Y - 1);
	}
}

void draw_rect(int *img, int clinum, int x_off, int y_off) { 
	if (clinum == 0) {
		colour_rect(RED, img, DISPLAY_X / 4 - 20 + x_off, DISPLAY_X / 4 + 20 + x_off,
			DISPLAY_Y / 4 - 20 + y_off, DISPLAY_Y / 4 + 20 + y_off);
	} else if (clinum == 1) {
		colour_rect(RED, img, (3 * DISPLAY_X / 4) - 20 + x_off, (3 * DISPLAY_X / 4) + 20 + x_off,
			DISPLAY_Y / 4 - 20 + y_off, DISPLAY_Y / 4 + 20 + y_off);
	} else if (clinum == 2) {
		colour_rect(RED, img, DISPLAY_X / 4 - 20 + x_off, DISPLAY_X / 4 + 20 + x_off,
			(3 * DISPLAY_Y / 4) - 20 + y_off, (3 * DISPLAY_Y / 4) + 20 + y_off);
	} else if (clinum == 3) {
		colour_rect(RED, img, (3 * DISPLAY_X / 4) - 20 + x_off, (3 * DISPLAY_X / 4) + 20 + x_off,
			(3 * DISPLAY_Y / 4) - 20 + y_off, (3 * DISPLAY_Y / 4) + 20 + y_off);
	}
}



void saturate_section(int *x_off, int *y_off) { 

	if (*x_off < -DISPLAY_X / 4 + 21) 
		*x_off = -DISPLAY_X / 4 + 21;

	if (*x_off > DISPLAY_X / 4 - 21)
		*x_off = DISPLAY_X / 4 - 21;

	if (*y_off < -DISPLAY_Y / 4 + 21)
		*y_off = -DISPLAY_Y / 4 + 21;

	if (*y_off > DISPLAY_Y / 4 - 21)
		*y_off = DISPLAY_Y / 4 - 21;
}

void move_rect(int *img, int clinum, string cmd, int *x_off, int *y_off) {
	clear_section(img, clinum);

	if (cmd == "w\n")
		*y_off = *y_off - 10;
	else if (cmd == "s\n")
		*y_off = *y_off + 10;
	else if (cmd == "d\n")
		*x_off = *x_off + 10;
	else if (cmd == "a\n")
		*x_off = *x_off - 10;

	saturate_section(x_off, y_off); 
	draw_rect(img, clinum, (*x_off), (*y_off));
}



int main(){
	//povezivanje sa VGA kao u proslom domacem
	int *buffer;
	int fd;
	int klijent = 0;
	int nizid[4]={0}; 
	int id;
	int pid;

	fd = open("/dev/vga_dma", O_RDWR | O_NDELAY);

	if (fd < 0)
	{
		cout<<"Cannot open vga_buffer\n"<<endl;
		exit(EXIT_FAILURE);
	}
	else
		buffer = (int *)mmap(0, MAX_PKT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	//definisanje socketa i potrebnih promenljivih
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;    
	
	/*prvo pozivamo socket() da bi se registrovao socket:
	  AF_INET je neophodan kada se zahteva komunikacija bilo koja dva hosta na internetu;
	  drugi argument definise tip socketa, SOCK_STREAM ili SOCK_DGRAM;
	  treci argument je zapravo protokol koji se koristi, najcesce stavljamo 0 sto znaci da OS sam odabere
	  podrazumevane protokole za dati tip socketa (TCP ZA SOCK_STREAM)
	*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	} 

	//bojenje pozadine i pravljenje cetiri kvadranta
	colour_bckg(BLACK,buffer);
	colour_horizontal(BLUE, buffer, 319, 0,479);
	colour_vertical(BLUE, buffer, 239, 0, 639);

	/* Inicijalizacija strukture socket-a */    
	bzero((char *) &serv_addr, sizeof(serv_addr));     
	portno = 5001;     
	serv_addr.sin_family = AF_INET; //mora biti AF_INET    

	/* ip adresa host-a. INADDR_ANY vraca ip adresu masine na kojoj se startovao server */    
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	/* broj porta-ne sme se staviti kao broj vec se mora konvertovati u tzv. network byte order funkcijom htons*/  
  	serv_addr.sin_port = htons(portno);

	/* Sada bind-ujemo adresu sa prethodno kreiranim socket-om */    
	if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0) 
	{          
		perror("ERROR on binding");          
		exit(1);     
	}
	
	cout<<"Server started.. waiting for clients ...\n"<<endl;

	listen(sockfd,4); //maksimalno 4 klijenata moze da koristi     
	clilen = sizeof(cli_addr);

	while(1){
		/*ovde ce cekati sve dok ne stigne zahtev za konekcijom od prvog klijenta*/
		if(nizid[0]==0 || nizid[1]==0 || nizid[2]==0 || nizid[3]==0)
        	{
			int flags = fcntl(sockfd, F_GETFL, 0);	
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
			newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);//blokira program i ceka klijenta
			
			cout<<"Client connected... "<<endl;
			if (newsockfd < 0)  
			{        
				if (errno == EWOULDBLOCK) {
					usleep(15000);//sleep 15ms
				} else {    
					perror("ERROR on accept"); 
					exit(1);      
				}
			} else {

				string cli_ip_str = inet_ntoa(cli_addr.sin_addr);
				/* Revert socket config to BLOCKING */
				int flags = fcntl(newsockfd, F_GETFL, 0);
				fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK));

				int first_free = first_free_square(nizid, 4);
				cout << "Client with IP: " << cli_ip_str
				<< " connected to section " << sec_n_to_str(first_free)
				<< "\n";

				pid = fork();
				if (pid < 0) {
					cerr << "ERROR while forking\n";
					exit(1);
				}

				if (pid == 0) {
					int x_off = 0;
					int y_off = 0;
					clear_section(buffer, first_free);
					draw_rect(buffer, first_free, 0, 0);
					close(sockfd);
					sock_write(newsockfd, "Controlling: " + sec_n_to_str(first_free)); 
									// da bi vratili string, zato sock_write

					string command;

						do {
						command = sock_read(newsockfd); //da bi vratili string,  zato sock_read
						
						move_rect(buffer, first_free, command, &x_off, &y_off);
						} while (msg != "q\n" && msg != "");
					
					clear_section(buffer, first_free);
					close(newsockfd);

					exit(0);
				}
				
				else {
					close(newsockfd);
					zauzmi_kvadrant(nizid, pid);
					ispisivanje_liste(nizid, 4);
				}
			
				

				/* Parrent process
				Checks if any of the child processes has terminated (client disconnected)
				On client disconnect release entry in PID list
				*/
				for (int a = 0; a < 4; a++) {
					if (nizid[a] != 0) {
						int status;
						if (waitpid(nizid[a], &status, WNOHANG)) {
							cout << "Client disconnected\n"<<endl;
							oslobadjanje_kvadranta(nizid, nizid[a]);
							ispisivanje_liste(nizid, 4);
						}
					}
				}

			}//kraj of if(newsockfd) petlje

		}//kraj prvobitne if petlje

	} //kraj while petlje

	munmap(buffer, MAX_PKT_SIZE);
	close(fd);

	if (fd < 0)
		printf("Cannot close vga_buffer\n");

		close(sockfd);
	return 0;
}
