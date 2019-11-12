#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <regex>
#include <iostream>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>
//sve potrebne biblioteke za rad

#define DISPLAY_X 640 
#define DISPLAY_Y 480
#define MAX_PKT_SIZE (640*480*4)


string black ="BLACK";
string red ="RED";
string green ="GREEN";
string blue ="BLUE";
string yellow ="YELLOW";


using namespace std;


//funkcija za dobijanje boja
unsigned int get_colour(string colour_s)
{
	unsigned int black_colour = 0;
	unsigned int red_colour   = (1<<15) | (1<<14) | (1<<13) | (1<<12) | (1<<11);
	unsigned int green_colour = (1<<10) | (1<<9)  | (1<<8)  | (1<<7)  | (1<<6)  | (1<<5);
	unsigned int blue_colour  = (1<<4)  | (1<3)   | (1<<2)  | (1<<1)  | (1<<0);
	unsigned int yellow_colour = red_colour | green_colour; 

	cout << "colour = " << colour_s << endl;

	if (colour_s==black)
		return black_colour;

	else if (colour_s==red)
		return red_colour;

	else if (colour_s==green)
		return green_colour;

	else if (colour_s==blue)
		return blue_colour;

	else if (colour_s==yellow)
		return yellow_colour;

	cout<<"colour invalid. "<<endl;
	exit(3);
}




void regex_line (string line, int *image)
{

	regex find_cmd (" (BCKG | LINE_V | LINE_H | RECT): ");
	smatch find_match;

	if(regex_search (line, find_match ,find_cmd)){

		if (find_match[1]=="BCKG"){

			regex find_colour ("(BLACK | RED | GREEN | BLUE | YELLOW)");

			if(regex_search(line,find_match,find_colour)){

				cout<<"Colour: "<<find_match[1]<<endl;
				colour_bckg(get_colour(find_match[1]), image)
			
			}

		}
		
		if(find_match[1]== "LINE_V"){
			
			regex vertical_line("(\\d+), (\\d+), (\\d+);(BLACK | RED | GREEN | BLUE | YELLOW) ");

			if(regex_search(line, find_match, vertical_line)){

			cout<<"LINE_V je: "<<find_match[1]<<" "<<find_match[2]<<" "<<find_match[3]<<" "<<find_match[4]<<endl;
		colour_vertical(get_colour(find_match[4]), image, stoi find_match[1], stoi find_match[2], stoi find_match[3]);
			
			
			}

		}

		if(find_match[1]== "LINE_H"){
			
			regex horizontal_line("(\\d+), (\\d+), (\\d+);(BLACK | RED | GREEN | BLUE | YELLOW) ");

			if(regex_search(line, find_match, horizontal_line)){

			cout<<"LINE_H je: "<<find_match[1]<<" "<<find_match[2]<<" "<<find_match[3]<<" "<<find_match[4]<<endl;
		colour_horizontal(get_colour(find_match[4]), image, stoi find_match[1], stoi find_match[2], stoi find_match[3]);
			
			
			}

		}


		if(find_match[1]== "RECT"){
			
			regex rectangle("(\\d+), (\\d+), (\\d+), (\\d+) ;(BLACK | RED | GREEN | BLUE | YELLOW) ");

			if(regex_search(line, find_match, rectangle)){

cout<<"RECT je: "<<find_match[1]<<" "<<find_match[2]<<" "<<find_match[3]<<" "<<find_match[4]<<" "<<find_match[5]<<endl;
colour_rect(get_colour(find_match[5]), image, stoi find_match[1], stoi find_match[2], stoi find_match[3], stoi find_match[4]);
			
			
			}

		}



	
	}
	
	else {
		
		cout<<"ERROR, no matches found "<<endl;
	
	}

}

void colour_bckg(int colour, int *img)
{
	for(x=0; x<DISPLAY_X; x++){

		for(y=0; y<DISPLAY_Y; y++){
		
			img[x*DISPLAY_Y +y]= colour;
		}
	}

}

void colour_vertical(int colour, int *img, int x, int y1, int y2)
{
	
		for(y=y1; y<y2; y++){
		
			img[x*DISPLAY_Y +y]= colour;
		}

}

void colour_horizontal(int colour, int *img, int y, int x1, int x2)
{
	
		for(x=x1; x<x2; x++){
		
			img[x*DISPLAY_Y +y]= colour;
		}

}


void colour_rect(int colour, int *img, int x1, int x2, int y1, int y2)
{
	for(x=x1; x<x2; x++){

		for(y=y1; y<y2; y++){
		
			img[x*DISPLAY_Y +y]= colour;
		}
	}

}


int main(int argc, char *argv[])
{
	FILE *infile; 
	char *line_buff; 
	size_t line_buff_size = 0; 

	if (argc < 2) {
		cout << "Please enter config file path\n";
		return -1;
	} else {
		infile = fopen (argv[1], "r"); 
	}

	if (infile == NULL)
	{
		printf("Failed to open\n");
		exit(1);
	}

	int *image;
	int fd;

	fd = open("/dev/vga_dma", O_RDWR | O_NDELAY);
	if (fd < 0)
	{
		cout << "Cannot open /dev/vga_dma" << "\n";
		exit(EXIT_FAILURE);
	}
	else
		buffer = (int *)mmap(0, MAX_PKT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	while(getline(&line_buff, &line_buff_size, infile) >= 0)
	{
		regex_line(line_buff, image);
	}

	fclose(infile); //Zatvaranje text fajla
	munmap(image, MAX_MMAP_SIZE);
	close(fd);
	if (fd < 0)
		cout << "Cannot close " << "/dev/vga_dma" << "\n";

	delete [] image;

}






