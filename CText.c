//pre-processor directives
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>




/* 
			~::COMMAND LIST::~ 
	
			READ <file directory/name> - Reads contents into the editor
			WRITE _SPACE_ <file directory/name> - Uses append to edit files
			CHECK <file directory/name> - Performs a read operation to file for validation
			EXIT - Closes program

		
*/



//global variables
FILE *fsys;
char doc_fname[40];
HANDLE hConsole;



//method declarations
void cmd_proc();
void testFile();
void readFile();
void writeFile();

//start of code
int main(){
	
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle("CText - 0.1");
	
	//print title
	printf("### CText Editor ### \n\n");
	
	printf("Please enter the file path you wish to edit. \nIf you wish to create a new file then please enter its path and name. \n");
	Sleep(100);
	scanf("%s", &doc_fname);
	Sleep(100);

	testFile();
	
	return 0;
	
}

void cmd_proc(HANDLE hConsole){
	
	char f_command[20] = {};
	SetConsoleTextAttribute(hConsole, 7);
	
	printf("Waiting for command... \n");
	scanf("%s", f_command);
	
	if(strcmp(f_command,"READ") == 0){
		readFile();
		//readFile();
	}else if(strcmp(f_command, "WRITE") == 0){
		writeFile();
	}else if(strcmp(f_command, "CHECK") == 0){
		testFile();
	}else if(strcmp(f_command, "EXIT") == 0){
		exit(1);
	}else{
		SetConsoleTextAttribute(hConsole, 12);
		printf("ERROR:INVALID COMMAND\n");
		cmd_proc(hConsole);
	}
	
}

void testFile(){
	
	fsys = fopen(doc_fname,"a");	//this allows the program to create programs of any type but can not generate folders. Rather than simply reading a file, it tries to append to the file in order to create the file (if it doesn't already exist)	
	
	if( (fsys = fopen(doc_fname,"a") ) == NULL ){
		SetConsoleTextAttribute(hConsole, 12);
		printf("File did not pass test - please check file \n");
		
	}else{
		SetConsoleTextAttribute(hConsole, 2);
		printf("File passed test \n");
	}
		
	cmd_proc(hConsole); 
	
}

void readFile(){
	
	fflush(stdin);
	char inp_buf[400] = {};
	 
	printf("Reading %s... \n", &doc_fname);
	printf("CONTENTS: \n\n");
	
	fsys = fopen(doc_fname, "r");
	if(fsys == NULL){
		printf("ERROR: FILE NOT FOUND \n");
		cmd_proc(hConsole);
	}
	
	Sleep(1000);
	while((fgetc(fsys)) != EOF){ 
		
		Sleep(100);
		fgets(inp_buf, 400 ,fsys);
		printf(inp_buf);	
		
	}

	
	fclose(fsys);
	printf("\nFile read complete! \n");
	
	cmd_proc(hConsole);
}

void writeFile(){
	
	fflush(stdout);
	char out_buf[400] = {};
	
	printf("%s input written \n", &doc_fname);
	
	fgets(out_buf, 400, stdin);	//fgets allows the program to successfully take string input with spaces included rather than scanf which stores a string and moves upon the very first space in stdin
	
	fsys = fopen(doc_fname, "a+");
	
	fprintf(fsys, out_buf);
	fprintf(fsys, "\n");
		
		
	fclose(fsys);
	printf("\nFile write complete!\n");
	
	cmd_proc(hConsole);
}




