#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <regex>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <iostream>
//sve potrebne biblioteke za rad

#define DISPLAY_X 640 
#define DISPLAY_Y 480
#define MAX_PKT_SIZE (640*480*4)


#define BLUE 0x001F
#define GREEN 0x07E0
#define RED 0xF800
#define BLACK 0x0000
#define YELLOW (RED + GREEN)

using namespace std;


//funkcija za dobijanje boja
unsigned int get_colour(string colour_s)
{
 
	if (colour_s=="BLACK")
		return BLACK;

	else if (colour_s=="RED")
		return RED;

	else if (colour_s=="GREEN")
		return GREEN;

	else if (colour_s=="BLUE")
		return BLUE;

	else if (colour_s=="YELLOW")
		return YELLOW;

	cout<<"colour invalid. "<<endl;
	exit(3);
}





//funkcija za bojenje pozadine
void colour_bckg(int colour, int *img)
{
	for(int x=0; x<DISPLAY_X; x++){

		for(int y=0; y<DISPLAY_Y; y++){
		
			img[x*DISPLAY_Y +y]= colour;
		}
	}

}
//funkcija za bojenje vertikalne linije
void colour_vertical(int colour, int *img, int x, int y1, int y2)
{
	
		for(int y=y1; y<y2; y++){
		
			img[x*DISPLAY_Y +y]= colour;
		}

}
//funkcija za bojenje horizontalne linije
void colour_horizontal(int colour, int *img, int y, int x1, int x2)
{
	
		for(int x=x1; x<x2; x++){
		
			img[x*DISPLAY_Y +y]= colour;
		}

}

//funkcija za bojenje kvadrata
void colour_rect(int colour, int *img, int x1, int x2, int y1, int y2)
{
	for(int x=x1; x<x2; x++){

		for(int y=y1; y<y2; y++){
		
			img[x*DISPLAY_Y +y]= colour;
		}
	}

}

//regex funkcija za pronalazenje kljucnih reci i komandi
void regex_line (string line, int *image)
{

	regex find_cmd ("(BCKG|LINE_V|LINE_H|RECT): ");
	smatch find_match;

	if(regex_search (line, find_match ,find_cmd)){

		if (find_match[1]=="BCKG"){

			regex find_colour ("(BLACK|RED|GREEN|BLUE|YELLOW)");

			if(regex_search(line,find_match,find_colour)){

				cout<<"Colour: "<<find_match[1]<<endl;
				colour_bckg(get_colour(find_match[1]), image);
				
			}

		}
		
		if(find_match[1]== "LINE_V"){
			
			regex vertical_line("(\\d+), (\\d+), (\\d+); (BLACK|RED|GREEN|BLUE|YELLOW)");
			//regex vertical_line("([0-9]+), ([0-9]+), ([0-9]+);(BLACK | RED | GREEN | BLUE | YELLOW) ");

			if(regex_search(line, find_match, vertical_line)){

	cout<<"LINE_V je: "<<find_match[1]<<" "<<find_match[2]<<" "<<find_match[3]<<" "<<find_match[4]<<endl;
	colour_vertical(get_colour(find_match[4]), image, stoi(find_match[1]), stoi(find_match[2]), stoi(find_match[3]));
			
			
			}

		}

		if(find_match[1]== "LINE_H"){
			
			regex horizontal_line("(\\d+), (\\d+), (\\d+); (BLACK|RED|GREEN|BLUE|YELLOW)");

			if(regex_search(line, find_match, horizontal_line)){

	cout<<"LINE_H je: "<<find_match[1]<<" "<<find_match[2]<<" "<<find_match[3]<<" "<<find_match[4]<<endl;
	colour_horizontal(get_colour(find_match[4]), image, stoi(find_match[1]), stoi(find_match[2]), stoi(find_match[3]));
			
			
			}

		}


		if(find_match[1]== "RECT"){
			
			regex rectangle("(\\d+), (\\d+), (\\d+), (\\d+); (BLACK|RED|GREEN|BLUE|YELLOW)");

			if(regex_search(line, find_match, rectangle)){

cout<<"RECT je: "<<find_match[1]<<" "<<find_match[2]<<" "<<find_match[3]<<" "<<find_match[4]<<" "<<find_match[5]<<endl;
colour_rect(get_colour(find_match[5]), image, stoi(find_match[1]), stoi(find_match[2]), stoi(find_match[3]), stoi(find_match[4]));
			
			
			}

		}



	
	}
	
	else {
		
		cout<<"ERROR, no matches found "<<endl;
	
	}

}

int main(int argc, char *argv[])
{
	FILE *infile; 

	if (argc < 2) {
		cout << "Please enter the command file "<<endl;
		return -1;
	} else {
		infile = fopen (argv[1], "r"); 
	}

	if (infile == NULL)
	{
		cout<<"Failed to open file "<<endl;;
		exit(1);
	}



	int *image;
	int fd;
	char *my_line;
	int  my_line_bytes= 150;	
	my_line= (char *) malloc (my_line_bytes+1);

	fd = open("/dev/vga_dma", O_RDWR | O_NDELAY);
	if (fd < 0)
	{
		cout << "error closing /dev/vga_dma for write "<<endl;
		return -1;
	}
	else{
		image = (int *)mmap(0, MAX_PKT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	}

	while( getline(&my_line , &my_line_bytes, infile) >= 0 )
	{
		regex_line(my_line, image);
	}

	fclose(infile); 
	munmap(image, MAX_PKT_SIZE);
	close(fd);
	if (fd < 0){
		cout << "error closing /dev/vga_dma "<<endl;
		return -1;
	}

}






