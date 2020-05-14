#include <stdint.h> //uint16_t
#include <stdio.h>	//FILE
#include <signal.h>	//SIGINT
//WINDOWS ONLY
#include <Windows.h>
#include <conio.h>	//_khbit

HANDLE hStdin = INVALID_HANDLE_VALUE;

/*
		PROGRAMMING A VIRTUAL MACHINE 
		https://justinmeiners.github.io/lc3-vm/
	
		VM: LC-3 architecture
		Code originally written by authour ^^
		This code written by Kyle Christie
		
		!LOGIC FOR KEY RECOGNITION AND INPUT BUFFERING UTILISE WINDOWS VERS (SECTION 13 IN ABOVE DOCUMENTATION)
*/

/* MEMORY MAPPED REGISTERS */
enum
{
	MR_KBSR = 0xFE00, 	//keyboard status reg - indicates if a key has been pressed
	MR_KBDR = 0xFE02	//keyboard data reg - indicates which key was pressed
	
	//using these registers allows the program to stay responsive while awaiting input
};


/*TRAP CODES */
enum
{
	TRAP_GETC = 0x20,	//get a character from keyboard, not echoed onto the terminal
	TRAP_OUT = 0x21,	//output a character
	TRAP_PUTS = 0x22, 	//output a word/string 
	TRAP_IN = 0x23,		//get character from keyboard, echo onto terminal
	TRAP_PUTSP = 0x24, 	//output a byte string
	TRAP_HALT = 0x25	//halt the program
						
};

/* MEMORY: 65, 536 memory locations (storage capability: 128kb) */
uint16_t memory[UINT16_MAX];


/* REGISTERS */
enum
{
	R_R0 =0, //R0 through 1 are general purpose registers
	R_R1,
	R_R2,
	R_R3,
	R_R4,
	R_R5,
	R_R6,
	R_R7,
	R_PC, //program counter
	R_COND, //condition flag register
	R_COUNT,
		
};

uint16_t reg[R_COUNT];

/* INSTRUCTION SET */
enum
{
	OP_BR = 0,	//branch
	OP_ADD,		//add
	OP_LD,		//load
	OP_ST,		//store
	OP_JSR,		//jump register
	OP_AND,		//bitwise and
	OP_LDR,		//load register
	OP_STR, 	//store register
	OP_RTI,		//unused
	OP_NOT, 	//bitwise not
	OP_LDI,		//load indirect
	OP_STI,		//store indirect
	OP_JMP,		//jump
	OP_RES,		//reserved (unused)
	OP_LEA, 	//load effective address
	OP_TRAP,	//execute trap
};

/* CONDITION FLAGS */
enum
{
	FL_POS = 1 << 0, // P
	FL_ZRO = 1 << 1, // Z
	FL_NEG = 1 << 2, // N
};

	

//sign extension method
uint16_t sign_extend(uint16_t x, int bit_count)
{
	
	if((x >> (bit_count - 1)) & 1){
		x |= (0xFFFF << bit_count);
	}
	return x;
}

uint16_t swap16(uint16_t x)
{
	return (x << 8) | (x >> 8);
}

//register flag handling
void update_flags(uint16_t r)
{
	if(reg[r] == 0)
	{
		reg[R_COND] = FL_ZRO;
	}
	else if (reg[r] >> 15)
	{
		reg[R_COND] = FL_NEG;
	}
	else
	{
		reg[R_COND] = FL_POS;
	}
	
}

void read_image_file(FILE* file)
{
	//origin tells us where to place image in memory
	uint16_t origin;
	fread(&origin, sizeof(origin), 1, file);
	origin = swap16(origin);
	
	//already know max file size of only 1 fread is needed
	uint16_t max_read = UINT16_MAX - origin;
	uint16_t* p = memory + origin;
	size_t read = fread(p, sizeof(uint16_t), max_read, file);
	
	while(read-- > 0)
	{
		*p = swap16(*p);
		++p;	
	}
	
}

int read_image(const char* image_path)
{
	FILE* file = fopen(image_path, "rb");
	if(!file) { return 0;};
	read_image_file(file);
	fclose(file);
	return 1;
	
}

uint16_t check_key()
{
	return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

void mem_write(uint16_t address, uint16_t val)
{
	memory[address] = val;
}

uint16_t mem_read(uint16_t address)
{
	if(address == MR_KBSR)
	{
		if(check_key())
		{
			memory[MR_KBSR] = (1 << 15);
			memory[MR_KBDR] = getchar();
		}
		else
		{
			memory[MR_KBSR] = 0;
		}
	}
	
	return memory[address];
}


DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hStdin, &fdwOldMode); //save old mode
	
	fdwMode = fdwOldMode
			^ ENABLE_ECHO_INPUT		//no input echo
			^ ENABLE_LINE_INPUT;	//return when 1 or more chars are available
			
	SetConsoleMode(hStdin, fdwMode); //Set new mode
	FlushConsoleInputBuffer(hStdin); //clear buffer
}

void restore_input_buffering()
{
	SetConsoleMode(hStdin, fdwOldMode);
}

//HANDLE INTERRUPT, 12
void handle_interrupt(int signal)
{
	restore_input_buffering();
	printf("\n");
	exit(-2);
}




//START MAIN
//main instruction/cpu cycle logic
int main(int argc, const char* argv[])
{
	//handle cmd line input
	if(argc < 2)
	{
		printf("lc3 [image-file1] ... \n");
		exit(2);
		
	}
	
	for(int j = 1; j < argc; ++j)
	{
		if(!read_image(argv[j]))
		{
			printf("failed to load image: %s \n", argv[j]);
			exit(1); //LAST LINE WRITTEN - JUST STARTING SECTION 6. (implementing instructions)
			
		}
		
	}
	
	//Setup code for windows
	signal(SIGINT, handle_interrupt);
	disable_input_buffering();
	
	//set PC(VM) to start position; address 0x3000 is the default
	enum{ PC_START = 0x3000};
	reg[R_PC] = PC_START;
	
	int running = 1;
	while(running)
	{
		uint16_t instr = mem_read(reg[R_PC]++);
		uint16_t op = instr >> 12;
		
		switch(op)
		{
			case OP_ADD: //Mathematical add instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;	//destination register
					uint16_t r1 = (instr >> 6) & 0x7;	//first operand (SR1)
					uint16_t imm_flag = (instr >> 5) & 0x1; //check for immediate mode
					
					if(imm_flag) //logic for operation in immediate mode
					{
						uint16_t imm5 = sign_extend(instr & 0x1F, 5);
						reg[r0] = reg[r1] + imm5;
					}
					else //logic for operation in register mode
					{
						uint16_t r2 = instr & 0x7;
						reg[r0] = reg[r1] + reg[r2];
					}
					
					update_flags(r0);
				}
				break;
			
			case OP_AND: //Bitwise and instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t r1 = (instr >> 6) & 0x7;
					uint16_t imm_flag = (instr >> 5) & 0x1;
					
					if(imm_flag)
					{
						uint16_t imm5 = sign_extend(instr & 0x1F, 5);
						reg[r0] = reg[r1] & imm5;
					}
					else
					{
						uint16_t r2 = instr & 0x7;
						reg[r0] = reg[r1] & reg[r2];
					}
					
					update_flags(r0); 
				}
				break;
				
			case OP_NOT: //bitwise not instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t r1 = (instr >> 6) & 0x7;
					
					reg[r0] = ~reg[r1];
					update_flags(r0);
				}
				break;
				
			case OP_BR: //branch instruction
				{
					uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
					uint16_t cond_flag = (instr >> 9) & 0x7;
					
					if(cond_flag & reg[R_COND])
					{
						reg[R_PC] += pc_offset;
					}
					
				}
				break;
				
			case OP_JMP: //jump instruction
				{
					//also handles RET
					uint16_t r1 = (instr >> 6) & 0x7;
					reg[R_PC] = reg[r1];
				}
				break;
				
			case OP_JSR: //jump register instruction
				{
					uint16_t long_flag = (instr >> 11) & 1;
					reg[R_R7] = reg[R_PC];
					
					if(long_flag)
					{
						uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11);
						reg[R_PC] += long_pc_offset; //JSR
					}
					else
					{
						uint16_t r1 = (instr >> 6) & 0x7;
						reg[R_PC] = reg[r1]; //JSRR
					}	
					break;
				}
				break;
				
			case OP_LD: //load instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
					
					reg[r0] = mem_read(reg[R_PC] + pc_offset);
					update_flags(r0);
				}
				break;
				
			case OP_LDI: //Load indirect instruction (loads value into register from memory)
				{
					uint16_t r0 = (instr >> 9) & 0x7; //destination register (DR)
					uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //PCoffset 9
					
					//add pc_offset to the current PC, look at that memory location to get the final address
					reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset)); 
					update_flags(r0);
				}
				break;
			
			case OP_LDR: //load register instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t r1 = (instr >> 6) & 0x7;
					uint16_t offset = sign_extend(instr & 0x3F, 6);
					
					reg[r0] = mem_read(reg[r1] + offset);
					update_flags(r0);
					
					
				}
				break;
				
			case OP_LEA: //load effective address instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
					
					reg[r0] = reg[R_PC] + pc_offset;
					update_flags(r0);
					
				}
				break;
				
			case OP_ST: //store instruction 
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
					
					mem_write(reg[R_PC] + pc_offset, reg[r0]);
					
				}
				break;
				
			case OP_STI: //store indirect instruction
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
					
					mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
				}
				break;
				
			case OP_STR: //store register
				{
					uint16_t r0 = (instr >> 9) & 0x7;
					uint16_t r1 = (instr >> 6) & 0x7;
					uint16_t offset = sign_extend(instr & 0x3F, 6);
					
					mem_write(reg[r1] + offset, reg[r0]); 
					
				}
				break;
				
			case OP_TRAP: //TRAP code handling
				{
					
					switch(instr & 0xFF)
					{
						case TRAP_GETC:
							{
								//read a single ASCII char
								reg[R_R0] = (uint16_t)getchar();
							}
							break;
						
						case TRAP_OUT:
							{
								putc((char)reg[R_R0], stdout);
								fflush(stdout);
							}
							break;
							
						case TRAP_PUTS:
							{
								//1 char per word
								uint16_t* c = memory + reg[R_R0]; //start in address specified w/ R0
								while(*c) //output chars from memory while they are available in memory
								{
									putc((char)*c, stdout);
									++c;
									
								}
								fflush(stdout); //terminate writing upon x0000 in memory
								
							}
							break;
						
						case TRAP_IN:
							{
								printf("Enter a character: ");
								char c = getchar();
								putc(c, stdout);
								reg[R_R0] = (uint16_t)c;
							}
							break;
						
						case TRAP_PUTSP:
							{
								//1 char per byte (2 bytes per word)
								
								uint16_t* c = memory + reg[R_R0];
								while(*c)
								{
									char char1 = (*c) & 0xFF;
									putc(char1, stdout);
									char char2 = (*c) >> 8;
									if(char2) putc(char2, stdout);
									++c;
								}
								fflush(stdout);
								
							}
							break;
						
						case TRAP_HALT:
							{
								puts("HALT");
								fflush(stdout);
								running = 0;
							}
							break;
					}
					
				}
				break;
				
			case OP_RES: //UNUSED INSTRUCTION THROWS ERROR
				{abort();}
				break;
				
			case OP_RTI: //UNUSED INSTRUCTION THROWS ERROR
				{abort();}
				break;
			
			default:
				{abort();}
				break;
				
				
				
		}
		
		
		
	}
	
	
	//shutdown code for windows
	restore_input_buffering();
	
	
}//END MAIN 	



