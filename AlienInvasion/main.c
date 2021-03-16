#include <stdio.h>
#include <gb/gb.h>
#include "house.c"
#include "brick_tile.c"
#include <gb/font.h>
#include "text.c"
#include "pl.c"
#include "a_ab_splsh_data.c"
#include "a_ab_splsh_map.c"
#include "Text-number.c"

struct G_Character UFO;
struct G_Character HUMAN;
struct G_Character sOne;
struct G_Character sTwo;
struct G_Character sThree;

const UINT8 speed = 2;
UBYTE game_running = 0;
UINT8 score = 0;
UINT8 sec_score = 0;
UINT8 thr_score = 0;
UINT8 h_v = 0;

void drawSplash();
void showSplash();

UBYTE collis(struct G_Character* one, struct G_Character* two){
	
	return (one->x >= two->x && one->x <= two->x + two->width) && (one->y >= two->y && one->y <= two->y + two->height) 
		|| 
		(two->x >= one->x && two->x <= one->x + one->width) && (two->y >= one->y && two->y <= one->y + one->height);
}



void actorMove(struct G_Character* character, UINT8 x, UINT8 y){
	
	move_sprite(character->img, x, y);
}

void setScoreOne(){
	sOne.x = 75;
	sOne.y = 146;
	sOne.width = 6;
	sOne.height = 6;
	

	
	set_sprite_tile(6, score + 10);
	sOne.img = 6;
	if(score > 9){
		
		score = 0;
		set_sprite_tile(6, 10);
		sec_score++;
	}
	
	actorMove(&sOne, sOne.x, sOne.y);
}

void setScoreTwo(){
	sTwo.x = 65;
	sTwo.y = 146;
	sTwo.width = 6;
	sTwo.height = 6;
	
	set_sprite_tile(7, sec_score + 10);
	sTwo.img = 7;
	if(sec_score > 9){
		
		set_sprite_tile(7, 10);
		thr_score++;
	}
	
	actorMove(&sTwo, sTwo.x, sTwo.y);
	
}

void setScoreThree(){
	sThree.x = 55;
	sThree.y = 146;
	sTwo.width = 6;
	sTwo.height = 6;
	
	set_sprite_tile(8, thr_score + 10);
	sThree.img = 8;
	if(thr_score > 9){
		DISPLAY_OFF;
		printf("MAX SCORE REACHED - WELL DONE!");
	}
	actorMove(&sThree, sThree.x, sThree.y);
}

	

void set_ufo(){
	UFO.x = 48;
	UFO.y = 48;
	UFO.width = 6;
	UFO.height = 6;
	
	

	set_sprite_tile(0,6);
	UFO.img = 0;
	
	actorMove(&UFO, UFO.x, UFO.y);
}

void set_human(){
	HUMAN.x = 48; 
	HUMAN.y = 120;
	HUMAN.width = 6;
	HUMAN.height = 6;
	
	set_sprite_tile(2, 8);
	HUMAN.img = 2;
	
	actorMove(&HUMAN, HUMAN.x, HUMAN.y);
}

static void music(){
	//channel 4 reg
	NR41_REG = 0x77; // sound length
	NR42_REG = 0xFF; // volume envelope
	NR43_REG = 0x80; // frequency
	NR44_REG = 0xC0;  // sound management
	
	delay(100);
}


static void ufoSound(){
	//see for reference: https://www.youtube.com/watch?v=psCxZr9iDck&list=PLeEj4c2zF7PaFv5MPYhNAkBGrkx4iPGJo&index=5
	
	//Channel 1 registers
	NR10_REG = 0x25; //channel 1, register 0 : Frequency sweep settings
	NR11_REG = 0x31; //channel 1, register 1 : Wave pattern duty & sound length
	NR12_REG = 0x50; //channel 1, register 2 : Volume envelope
	NR13_REG = 0x00; //channel 1, register 3 : Frequency Least significant bits
	NR14_REG = 0xa2; //channel 1, register 4 : Playback and frequency MSbs
	
	
}

void main(){
	
	//declare font var
	font_t min_font;
	
	if(game_running == 0){
		showSplash();
		game_running = 1;
	}
	
	//Sound setup - must follow this order
	NR52_REG = 0x80; //{1000 0000} in binary, activates sound
	NR50_REG = 0x77; //sets volume for left and right channel to max (0x77)
	NR51_REG = 0xFF; //{1111 1111}, select sound channels to use, sets all 4 channels for Left and Right
	
	
	
	//set font
	font_init();
	min_font = font_load(font_min);
	font_set(min_font);
	
	//load tiles for background into mem
	set_bkg_data(37, 8, TileLabel);
	//load map into mem
	set_bkg_tiles(0, 0, 40, 18, house);
	
	

	//draw score area of screen and move it so background isn't blocked
	set_win_tiles(0, 0, 6, 1, window );
	move_win(6, 130);
	
	set_ufo();
	set_human();
	setScoreOne();
	setScoreTwo();
	setScoreThree();
	
	
	
	//general display functions
	SHOW_SPRITES;
    SHOW_BKG;
	SHOW_WIN;
	DISPLAY_ON;
	
	while(!collis(&UFO, &HUMAN)){
		//load sprite data
		set_sprite_data(0, 20, TileLabel);
	
		
		//char controls
		if(joypad() & J_LEFT){
			UFO.x -= speed;
			actorMove(&UFO, UFO.x, UFO.y);
		}
		if(joypad() & J_RIGHT){
			UFO.x += speed;
			actorMove(&UFO, UFO.x, UFO.y);
		}
		if(joypad() & J_UP){
			UFO.y -= speed;
			actorMove(&UFO, UFO.x, UFO.y);
		}
		if(joypad() & J_DOWN){
			UFO.y += speed;
			actorMove(&UFO, UFO.x, UFO.y);
		}
		
		HUMAN.x += (speed + h_v);
		actorMove(&HUMAN, HUMAN.x, HUMAN.y);
		
		//continously scroll the background w/ delay so it isnt too quick
		scroll_bkg(1, 0);
		music();
		//delay so game isn't too fast
		delay(20);
	}
	
	ufoSound();
	score++;
	h_v++;
	main();
	
}


void drawSplash(){
	DISPLAY_OFF;
	set_bkg_data(0, 195, a_ab_splsh_data);
	set_bkg_tiles(0, 0, 20, 18, a_ab_splsh_map);
	delay(200);
	
	SHOW_BKG;
	DISPLAY_ON;
	waitpad(J_A);
	
}

void showSplash(){
	wait_vbl_done();
	drawSplash();
	
}



