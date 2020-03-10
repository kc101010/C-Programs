//pre-processor directives
#include <stdio.h>
#include <windows.h>



int main(){
	
	
	HANDLE console;
	SetConsoleTitle("Click - A Clicker!");
	
	//declare variables
	char input;	//character to recognise input
	int counter = 0; //int ot hold number
	
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 5);
	
	//print menu/title
    printf("CLICKER GAME 3000\n");
	printf("Written by Kyle Christie - for practise\n");
	
		//infinite loop
		while(1){
		
			//get character from console
			input = fgetc(stdin);
		
			
			if(input == 10 || input == 0x0D ){						//if the input is 'ENTER' - denoted by: 10 in decimal, 0x0D(13) in hex
				++counter;											//increment counter
				printf("You pressed enter %d times", counter);		//then print message with counter
				
			}else{
				printf("You didn't press the right button!\n");		//if any other input is entered then print message
			}
			
			//achievement logic
			if(counter >= 100){
				printf("\n ACHIEVEMENT - CLICKS I \n");
				SetConsoleTextAttribute(console, 1);



				if(counter >=500){
					printf("\n ACHIEVEMENT - CLICKS II \n");
					SetConsoleTextAttribute(console, 2);
				}
				if(counter >= 1000){
					printf("\n ACHIEVEMENT - CLICKS III \n");
					SetConsoleTextAttribute(console, 3);
				}
				
				if(counter >= 2000){
					printf("\n ACHIEVEMENT - CLICKS IV \n");
					SetConsoleTextAttribute(console, 4);
				}
				
				if(counter >= 4000){
					printf("\n ACHIEVEMENT - CLICKS V \n");
					SetConsoleTextAttribute(console, 6);
				}
				
				if(counter >= 6000){
					printf("\n ACHIEVEMENT - CLICKS VI \n");
					SetConsoleTextAttribute(console, 7);
				}
				
				if(counter >= 8000){
					printf("\n ACHIEVEMENT - CLICKS VII \n");
					SetConsoleTextAttribute(console, 8);
				}
				
				if(counter >= 10000){
					printf("\n ACHIEVEMENT - CLICKS VIII \n");
					SetConsoleTextAttribute(console, 9);
				}
				
				if(counter >= 20000){
					printf("\n ACHIEVEMENT - CLICKS IX \n");
					SetConsoleTextAttribute(console, 10);
				}
				
			}	
	}
	
	return 0;
}