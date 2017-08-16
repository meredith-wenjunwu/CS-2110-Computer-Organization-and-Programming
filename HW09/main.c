//Author: Wenjun Wu
#include "myLib.h"
#include "river.h" //title screen
#include "zebra.h" //zebra object
#include "gameOver.h" //gameover screen
#include "crocodile_left.h" //crocodile face left
#include "crocodile_right.h" //crocodile face right
#include "gameBG.h" //game background
#include "winZebra.h" //win screen
#include <stdio.h>
#include <stdlib.h>

int main() {
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	int gameMode = 0;
	int result = 2; //store result --> win or lose
	int move = 1; //moving flag
	int dist = rand() % 3 + 1; //random moving distance
	int direction; //random direction
	int dir; //temporary dir storage
	int life = 2;// Number of lives
	char str[17]; //Str that store the number of lives
	int flag = 0;

	//crocodile's starting location 
	//{25, 25, 55, 75, 100, 125, 125, 150, 150};
	//{0, 125, 55, 0, 100, 125, 50, 25, 100};

	 int loc_h[] = {30, 30, 30, 75, 75, 125, 125, 125};
	 int loc_c[] = {0, 65, 135, 45, 100, 0, 65, 135};
	 int num = 8;
	//Title screen
	while(1) {
		if (gameMode == 0) {
			life = 2;
			drawImage3(0,0,240,160,river_data);
			drawString(140, 110, "Press Enter to Start", MAGENTA);
			drawString(150, 90, "Use <-, ^, v, -> to move", MAGENTA);
			if(KEY_DOWN_NOW(BUTTON_START)) {
				gameMode = 1;
				drawImage3(0, 0, 240, 160, BLACK);
				drawRect(0, 0, 160, 240, WHITE);

			}
		}

		Object crocodile[num];
		Object zebra;
		zebra.row = 0;
		zebra.col = 110;
		zebra.rowOrg = 0;
		zebra.colOrg = 110;

		for(int i = 0; i < num; i++) {
			direction = rand() % 2; //random direction
			direction = 2*direction - 1; // direction = -1 or 1
			crocodile[i].row = loc_h[i];
			crocodile[i].col = loc_c[i];
			crocodile[i].rowOrg = loc_h[i];
			crocodile[i].colOrg = loc_c[i];

			crocodile[i].dir = direction;
		}

		while (gameMode == 1) {
			
			// Draw Background
			drawImage3(0,200, 40, 160, gameBG_data);


			// Delete old zebra
			drawRect(zebra.rowOrg, zebra.colOrg, 20, 10, WHITE);

			
			//Keyboard action
			// reset to title screen
			if(KEY_DOWN_NOW(BUTTON_SELECT)) {
					gameMode = 0;
			}
			// Move left
			if (KEY_DOWN_NOW(BUTTON_LEFT)) {
				zebra.col-=3;
				if (zebra.col < 0) {zebra.col = 0;}
			}
			// Move right
			if(KEY_DOWN_NOW(BUTTON_RIGHT)){
					zebra.col+=3;
					if(zebra.col>180) {zebra.col=180;}
			}
			// Move up
			if (KEY_DOWN_NOW(BUTTON_UP)) {
				zebra.row--;
				if (zebra.row <= 0) {
					zebra.row = 0;
				}
			}
			// Move down
			if (KEY_DOWN_NOW(BUTTON_DOWN)) {
				zebra.row++;
			}



			Object* temp;


			// Check collision, if thereis collision, then reduce number of lives left
			
				for(int i = 0; i<num; i++) {
					temp = crocodile+i;
					if (collision(&zebra, temp) == 1) {
							if (flag <= 0) {
								life--;
								//Delete old text
								drawRect(0, 0, 8, 110, WHITE);
								flag = 40;
							} else {
								flag--;
							}
							break;
					}
					if (i == num -1) {
						flag = 0;
					}
				}


			if (life <= 0) {
				gameMode = 2;
				result = over;
			}

			//Move Crocodile
			if (move == 1) {
				for (int i = 0; i < num; i++) {
					dist = rand() % 3 + 1; //left or right crocodile
					temp = crocodile + i;
					dir = (*temp).dir;
					(*temp).col = (*temp).col + dist * dir;

					if((*temp).col + dist < 0)  {
						dist = -dist;
						(*temp).col = 0;
						(*temp).dir = -dir;
					} else if ((*temp).col + dist > 170) {
						dist = -dist;
						(*temp).col = 170;
						(*temp).dir = -dir;
					}
				}
			}
			// Check if Zebra has crossed the river
			if (zebra.row >= 140) {
				gameMode = 2;
				result = win;
			}
			


			//Draw Zebra
			drawImage3(zebra.row, zebra.col, 10, 20, zebra_data);
			zebra.rowOrg = zebra.row;
			zebra.colOrg = zebra.col;

			// Draw Crocodile
			if (move == 1) {
				move = 0;
				for (int i = 0; i < num; i++) {
					drawRect(crocodile[i].rowOrg, crocodile[i].colOrg, 20, 30, WHITE);
				}
				for (int i = 0; i < num; i++) {
					temp = crocodile + i;

					if ((*temp).dir  > 0) {
						drawImage3((*temp).row, (*temp).col, 30, 20, crocodile_right_data);
					} else {
						drawImage3((*temp).row, (*temp).col, 30, 20, crocodile_left_data);
					}
					(*temp).rowOrg = (*temp).row;
					(*temp).colOrg = (*temp).col;
				}
			} else {
				move = 1;
			}

			sprintf(str, "Life Remaining: %d", life);
			drawString(0, 0, str, RED);

			waitForVblank();


		}

		// Display result --> win or lose
		while(gameMode == 2) {
			if (result) {
				drawImage3(0, 0, 240, 160, winZebra_data);
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
						gameMode = 0;
				}
			} else {
				drawImage3(0, 0, 240, 160, gameOver_data);
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
						gameMode = 0;
				}
			}
		}
	}
}

