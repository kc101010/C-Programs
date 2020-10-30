//pre-processor directives
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>


//global variables
FILE *fsys;
char doc_fname[40];
HANDLE hConsole;


//method declarations
void cmd_proc();
void testFile();
void readFile();
void writeFile();
void openFile();
void progHelp();

//start of code
int main(void){
	//handle used by windows in order to edit console colours
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle("CText - 0.3");
	
	//print title
	printf("### CText Editor ### \n\n");
	
	cmd_proc(hConsole);
	
	return 0;
	
}

void cmd_proc(HANDLE hConsole){
	
	char f_command[20] = {};
	SetConsoleTextAttribute(hConsole, 7);
	
	printf("\n Waiting for command... \n");
	scanf("%s", f_command);
	
	if(strcmp(f_command,"READ") == 0){
		readFile();
		//readFile();
	}else if(strcmp(f_command, "WRITE") == 0){
		writeFile();
	}else if(strcmp(f_command, "CHECK") == 0){
		testFile();
	}else if(strcmp(f_command, "OPEN") == 0){
		openFile();
	}else if(strcmp(f_command, "CLEAR") == 0 || strcmp(f_command, "CLS") == 0 ){ //added shorthand to command
		system("cls");
		cmd_proc(hConsole);
	}else if(strcmp(f_command, "HELP") == 0){
		progHelp();
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
		printf("%s did not pass test - please check file \n", &doc_fname);
		
	}else{
		SetConsoleTextAttribute(hConsole, 2);
		printf("%s passed test \n", &doc_fname);
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
		SetConsoleTextAttribute(hConsole, 12);
		printf("ERROR: %S NOT FOUND \n", &doc_fname);
		cmd_proc(hConsole);
	}
	
	Sleep(1000);
	while(fgets(inp_buf, 400, fsys)){ 
		//13/10/20 -- Fixed bug where first line of file would always display twice. 
		// Next time just use a loop like this instead of whatever was attempted before
		Sleep(100);
		printf(inp_buf);	
		
	}

	
	fclose(fsys);
	printf("\nFile read complete! \n");
	
	cmd_proc(hConsole);
}

void writeFile(){
	
	fflush(stdout);
	char out_buf[400] = {};
	
	fgets(out_buf, 400, stdin);	//fgets allows the program to successfully take string input with spaces included rather than scanf which stores a string and moves upon the very first space in stdin
	
	fsys = fopen(doc_fname, "a+");
	if(fsys == NULL){
		SetConsoleTextAttribute(hConsole, 12);
		printf("ERROR: %s NOT FOUND \n", &doc_fname);
		cmd_proc(hConsole);
	}
	
	fprintf(fsys, out_buf);
	fprintf(fsys, "\n");
		
		
	fclose(fsys);
	printf("\nFile write to %s complete!\n", &doc_fname);
	
	cmd_proc(hConsole);
}

//13/10/20 - made file opening a command rather than a mandatory action
void openFile(){
	Sleep(100);
	scanf("%s", &doc_fname);
	Sleep(100);

	testFile();
	
	
}

//13/10/20 - Help page seems pretty useful
//26/10/20 - Updated to add clear cmd
//29/10/20 - Update to include shorthand clear
void progHelp(){
	
	printf("\nCText By Kyle Christie -- Help Page\n\n");
	printf("COMMAND LIST: (Syntax must be exact for commands to function) \n");
	printf("==================================================================== \n");
	printf("HELP                > Displays the message you are currently viewing \n");
	printf("OPEN (filename)     > Opens the specified file in the editor \n");
	printf("READ                > Reads file contents into editor \n");
	printf("WRITE (input/text)  > Writes user input to the file \n");
	printf("CLEAR / CLS		    > Clears screen contents");
	

	cmd_proc(hConsole);
}

