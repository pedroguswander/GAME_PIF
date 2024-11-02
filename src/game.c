/*
// Created by Pedro Gusmão on 26/10/2024.
//
*/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define PLAYER_VEL  1
#define BULLET_VEL 1
#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 3

char player_sprite[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {' ', '^', ' '},
    {'/', '|', '\\'},
    {'<', '-', '>'}
};

typedef struct position {
    int x;
    int y;
} position;

typedef struct player {
    int x;
    int y;
    int direction;
    char img;
    struct player *next;
} player;

typedef struct particle {
    position pos;
    char img;
    struct particle *next;
} particle;

void drawPlayer(char (*ps) [PLAYER_WIDTH] ,player ship) {
    for (int i = 0; i < PLAYER_HEIGHT; i++) {
        for (int j = 0; j < PLAYER_WIDTH; j++) {
                screenGotoxy(ship.x+j, ship.y+i);
                printf("%c", ps[i][j]);
        }
    }
    screenUpdate();
}

void draw_bullets(particle *head) {
    particle *iterate_bullets = head;
    while (iterate_bullets != NULL) {
        screenGotoxy((iterate_bullets->pos).x, (iterate_bullets->pos).y);
        printf("%c", iterate_bullets->img);
        iterate_bullets = iterate_bullets->next;
    }
}

void add_bullet(particle **head, int x, int y, char img) {
    particle *iterate_bullets = *head, *new_bullet = (particle *) malloc(sizeof(particle));
    (new_bullet->pos).x = x;
    (new_bullet->pos).y = y;
    new_bullet->img = img;
    new_bullet->next = NULL;

    if (*head == NULL) {
        *head = new_bullet;
    }
    else {
        while (iterate_bullets->next != NULL) {
            iterate_bullets = iterate_bullets->next;
        }
        iterate_bullets->next = new_bullet;
    }
}

void shoot_bullets(particle *head) {
    particle *iterate_bullets = head;

    while (iterate_bullets != NULL) {
        (iterate_bullets->pos).y -= BULLET_VEL;
        iterate_bullets = iterate_bullets->next;
    }
}
void remove_bullets(particle **head) {

}

int len_bullets(particle *head) {
    particle *iterate_bullets = head;
    int count = 0;
    while (iterate_bullets != NULL) {
        count+=1;
        iterate_bullets = iterate_bullets->next;
    }
    return count;
}

void move(player *ship) {
    ship->x += PLAYER_VEL * ship->direction;
}

int main() {
    screenInit(1);
    keyboardInit();
    //timerInit(1000);
    player ship = {85,18,0,'>',NULL} ;
    particle *ship_bullets = NULL;
    int run = 1;

    while(run) {
        if (keyhit()) {
            switch (readch()) {
                case 97: //move left A
                    if (ship.x > MINX) {
                        ship.direction = -1;
                        move(&ship);
                    }
                break;
                case 100://move right D
                    if (ship.x < (MAXX - PLAYER_WIDTH)) {
                        ship.direction = 1;
                        move(&ship);
                    }
                break;
                case 120: //shoot particle X
                    if (len_bullets(ship_bullets) < 5) {
                        add_bullet(&ship_bullets, ship.x, ship.y-1, '|');
                    }
                    break;
                case 32: //quit game SPACE
                    run = 0;
                break;
                default:
            }
        }
        system("clear");
        shoot_bullets(ship_bullets);
        drawPlayer(player_sprite, ship);
        draw_bullets(ship_bullets);
        screenUpdate();
        usleep(10000);
    }

    keyboardDestroy();
    screenDestroy();
    //timerDestroy();

    return 0;
}

