/*
// Created by Pedro Gusmão on 26/10/2024.
//
*/


#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define PLAYER_VEL  5
#define PLAYER_HEIGHT 5
#define PLAYER_WIDTH 5

char player_sprite[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {' ', ' ', '^', ' ', ' '},
    {' ', '/', '|', '\\', ' '},
    {'<', '-', '-', '-', '>'},
    {' ', ' ', '|', ' ', ' '},
    {' ', '/', ' ', '\\', ' '}
};

typedef struct player {
    int x;
    int y;
    char img;
    struct player *next;
} player;

void drawSprite(char (*ps) [PLAYER_WIDTH] ,player ship) {
    for (int i = 0; i < PLAYER_HEIGHT; i++) {
        for (int j = 0; j < PLAYER_WIDTH; j++) {
                screenGotoxy(ship.x+j, ship.y+i);
                printf("%c", ps[i][j]);
        }
    }
}

void move(player *ship) {
    ship->x += PLAYER_VEL;
}

int main() {
    screenInit(0);
    keyboardInit();
    timerInit(100);
    player ship = {34,8,'>',NULL} ;
    int run = 1;

    while(run) {

        switch (readch()) {
            case 119:
                move(&ship);
                break;
            case 32:
                run = 0;
                break;
            default:
        }
        screenClear();
        screenGotoxy(ship.x, ship.y);
        printf("%c", ship.img);
        screenUpdate();
        //drawSprite(player_sprite, ship)

    }

 return 0;
}

