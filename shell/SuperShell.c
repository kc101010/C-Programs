#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "get_user.c"

//3/1/21 -- add username to shell, added "cls" cmd, added colouring to shell loop

#define LSH_RL_BUFSIZE 1024

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

//https://brennan.io/2015/01/16/write-a-shell-in-c/

#define uname getUser() //store username as macro to avoid it disappearing when using clear/cls

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_cls(char **args);
int lsh_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"cls",
	"exit"	

};

int(*builtin_func[]) (char **) = {
	&lsh_cd,
	&lsh_help,
	&lsh_cls,
	&lsh_exit	
		
};

int lsh_num_builtins(){
	return sizeof(builtin_str) / sizeof(char *);
}


int lsh_cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr, "SuperShell: expected argument to \"cd\"\n");
	}else{
		if(chdir(args[1]) != 0){
			perror("lsh");
		}
	}
	system("dir");
	return 1;
}


int lsh_help(char **args){

	int i;

	printf("\n#### kc's SuperShell ####\n");
	printf("Based on LSH by Stephan Brennan \n");
	printf("Type program names & arguments and then hit enter\n");
	printf("The following are built in: \n");
	
	for(i = 0; i < lsh_num_builtins(); i++){
		printf(" %s\n", builtin_str[i]);
	}
	
	printf("Use the man command for info on other programs.\n");
	return 1;
}

int lsh_cls(char **args){ //implemented windows cls for easy term. clearing
	system("clear");
	return 1;
}

int lsh_exit(char **args){
	return 0;
}

int lsh_launch(char **args){

	pid_t pid, wpid;
	int status;

	pid = fork();
	
	if(pid == 0){
		//Child process
		if(execvp(args[0], args) == -1  ){
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	}else if(pid < 0){
		//Error forking
		perror("lsh");
	}else{
		//Parent process
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status) );
	}


	return 1;
}

int lsh_execute(char **args){
	int i;

	if(args[0] == NULL){
	
		return 1;
	}

	for(i = 0; i < lsh_num_builtins(); i++){
		if(strcmp(args[0], builtin_str[i]) == 0 ){
			return (*builtin_func[i])(args);
		}

	}
	
	return lsh_launch(args);
}



char *lsh_read_line(void){
	
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize );
	int c;
	
	if(!buffer){
		fprintf(stderr, "SuperShell: alloc error\n");
		exit(EXIT_FAILURE);	
	}	

	while(1){
		//read char
		c = getchar();

		//if EOF replace it with a null char and return
		if(c == EOF || c == '\n'){
			buffer[position] = '\0';
			return buffer;
		}else{
			buffer[position] = c;
		}

		position++;

		if(position >= bufsize){
			bufsize += LSH_RL_BUFSIZE;
			buffer += LSH_RL_BUFSIZE;

			if(!buffer){
				fprintf(stderr, "SuperShell: alloc error \n");
				exit(EXIT_FAILURE);
			}
		}

	}

}


char **lsh_split_line(char *line){

	int  bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr, "SuperShell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	
	while(token != NULL){
		tokens[position] = token;
		position++;

		if(position >= bufsize){
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char));

			if(!tokens){
				fprintf(stderr, "SuperShell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;

}

void lsh_loop(void){
	char *line;
	char **args;
	int status;


	do{
		
		printf("\033[0;34m%s\033[0;36m@SuperShell ::> ", uname); // \033 is escape character, with the colour coming after -- https://en.wikipedia.org/wiki/ANSI_escape_code#Colors | https://www.theurbanpenguin.com/4184-2/
		printf("\033[0m");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);
		
		free(line);
		free(args);
	}while(status);

}


int main(int argc, char **argv){
	//Load config
	

	//cmd proc
	lsh_loop();


	return EXIT_SUCCESS;
}
