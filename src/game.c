/*
// Created by Pedro Gusmão on 26/10/2024.
//
*/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define PLAYER_VEL  1
#define BULLET_VEL 1
#define OBJECT_VEL 4
#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 3

char player_sprite[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {' ', '^', ' '},
    {'/', '|', '\\'},
    {'<', '-', '>'}
};
char object_sprite1[] = "===^^^===";
char object_sprite2[] = "===//===";
char object_sprite3[] = "===-----===";
char object_sprite4[] = "===**===";
char object_sprite5[] = "=======";

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

typedef struct object {
    position pos;
    int life;
    char *sprite;
    int is_destructible;
    struct object *next;
} object;

typedef struct particle {
    position pos;
    char img;
    struct particle *next;
} particle;

void draw_border() {

    for (int x = MINX; x <= MAXX; x++) {
        screenGotoxy(x, MINY);
        printf("-");
        screenGotoxy(x, MAXY);
        printf("-");
    }

    for (int y = MINY; y <= MAXY; y++) {
        screenGotoxy(MINX, y);
        printf("|");
        screenGotoxy(MAXX, y);
        printf("|");
    }
}

int delay_object(double delay_time, clock_t *last_t) {
    clock_t current_t = clock();
    if ((double) (current_t - *last_t) / CLOCKS_PER_SEC >= delay_time) {
        *last_t = current_t;
        return 1;
    }
    return 0;
}

int create_random_Xposition(int minx, int maxx, int sprite_length) {
    return rand() % (maxx - sprite_length - minx + 1) + minx;
}

int check_collision(player ship, object *objects) {
    object *iterate_object = objects;

    while (iterate_object != NULL) {
        //int object_lenght = strlen(object_sprite1);
        for (int i = 0; iterate_object->sprite[i] != '\0'; i++) {
            if (((ship.x <= (iterate_object->pos.x + i) && (ship.x + PLAYER_WIDTH - 1) >= iterate_object->pos.x + i)) && (ship.y <= (iterate_object->pos.y) && (ship.y + PLAYER_HEIGHT - 1) >= (iterate_object->pos.y))) {
                return 0;
            }
        }
        iterate_object = iterate_object->next;
    }
    return 1;
}

int is_obj_destroyed(particle *bullet, object *obj, int hit_area_init, int hit_area_end) {
    if (obj->is_destructible) {
        if ((bullet->pos.x >= obj->pos.x + hit_area_init) && (bullet->pos.x <= obj->pos.x + hit_area_end)) {
            return 1;
        }
    }
    return 0;
}

void handle_collision_object_bullet(object **obj_head, particle **bullet_head, int hit_area_init, int hit_area_end) {
    object *objects = *obj_head, *obj_temp = *obj_head;
    particle *bullets = *bullet_head, *bullet_temp = *bullet_head;

    if (objects != NULL && bullets != NULL) {
        for (int i = 0; (*obj_head)->sprite[i] != '\0'; i++) {
            if (((*bullet_head)->pos.y == (*obj_head)->pos.y) && ((*bullet_head)->pos.x == (*obj_head)->pos.x + i)) {
                if (is_obj_destroyed(*bullet_head ,*obj_head, hit_area_init, hit_area_end)) {
                    *obj_head = (*obj_head)->next;
                    free(obj_temp);
                }

                *bullet_head = (*bullet_head)->next;
                free(bullet_temp);
                break;
            }
        }

        while (objects != NULL && objects->next != NULL && *bullet_head != NULL) {
            int flag = 1;

            for (int i = 0; objects->next->sprite[i] != '\0'; i++) {
                if ((*bullet_head)->pos.y == objects->next->pos.y && (*bullet_head)->pos.x == objects->next->pos.x + i) {
                    if (is_obj_destroyed(*bullet_head ,objects->next, hit_area_init,  hit_area_end)) {
                        obj_temp = objects->next;
                        objects->next = objects->next->next;
                        free(obj_temp);
                        flag = 0;
                    }

                    bullet_temp = *bullet_head;
                    *bullet_head = (*bullet_head)->next;
                    free(bullet_temp);
                    break;
                }
            }
            if (flag) {
                objects = objects->next;
            }
        }
    }
}

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

void move_bullets(particle *head) {
    particle *iterate_bullets = head;

    while (iterate_bullets != NULL) {
        (iterate_bullets->pos).y -= BULLET_VEL;
        iterate_bullets = iterate_bullets->next;
    }
}

void remove_bullets(particle **head) {
    particle *iterate_bullets = *head, *temp = *head;
    if (*head != NULL) {
        if ((*head)->pos.y <= MINY) {
            *head = (*head)->next;
            free(temp);
        }

        while (iterate_bullets != NULL && iterate_bullets->next != NULL) {
            if (iterate_bullets->next->pos.y <= MINY) {
                temp = iterate_bullets->next;
                iterate_bullets->next = iterate_bullets->next->next;
                free(temp);
            } else {
                iterate_bullets = iterate_bullets->next;
            }
        }
    }
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

void add_object(object **head, int x, int y, int life, int is_destructible, char *sprite) {
    object *iterate_object = *head, *new_object = (object *) malloc(sizeof(object));
    (new_object->pos).x = x;
    (new_object->pos).y = y;
    new_object->life = life;
    new_object->is_destructible = is_destructible;
    new_object->sprite = (char *) malloc(sizeof(char) * strlen(sprite));
    strcpy(new_object->sprite, sprite);
    new_object->next = NULL;
    if (*head == NULL) {
        *head = new_object;
    }
    else {
        while (iterate_object->next != NULL) {
            iterate_object = iterate_object->next;
        }
        iterate_object->next = new_object;

    }
}

void draw_object(object *head) {
    object *iterate_object = head;
    while (iterate_object != NULL) {
        for (int i = 0; iterate_object->sprite[i] != '\0'; i++) {
            screenGotoxy(iterate_object->pos.x + i, iterate_object->pos.y);
            printf("%c", iterate_object->sprite[i]);
        }
        iterate_object = iterate_object->next;
    }
}

void move_object(object *head) {
    object *iterate_object = head;
    while(iterate_object != NULL) {
        iterate_object->pos.y += OBJECT_VEL;
        iterate_object = iterate_object->next;
    }
}

void move(player *ship) {
    ship->x += PLAYER_VEL * ship->direction;
}

int main() {
    screenInit(1);
    keyboardInit();
    srand(time(0));
    player ship = {85,18,0,'>',NULL} ;
    particle *ship_bullets = NULL;
    object *enemy = NULL;
    int enemy_x = create_random_Xposition(MINX, MAXX, 9);
    add_object(&enemy, enemy_x, 4, 2, 1,  object_sprite2);
    clock_t spawn_clock = clock(), move_clock = clock();

    int run = 1;
    
    while(run) {
        if (keyhit()) {
            switch (readch()) {
                case 97: //move left A
                    if (ship.x > MINX + 1) {
                        ship.direction = -1;
                        move(&ship);
                    }
                break;
                case 100://move right D
                    if (ship.x < (MAXX - PLAYER_WIDTH - 1)) {
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
        draw_border();

        /*if (delay_object(3.0, &spawn_clock)) {
            int enemy_x = create_random_Xposition(MINX, MAXX, 9);
            add_object(&enemy, enemy_x, -2, 2, object_sprite2);
        }*/

        if (delay_object(1.0, &move_clock)) {
            move_object(enemy);
        }

        run = check_collision(ship, enemy);
        handle_collision_object_bullet(&enemy, &ship_bullets, 3, 4);
        move_bullets(ship_bullets);
        remove_bullets(&ship_bullets);
        draw_object(enemy);
        drawPlayer(player_sprite, ship);
        draw_bullets(ship_bullets);

        screenUpdate();
        usleep(33333); //30 FPS
    }


    screenHomeCursor();
    printf("Você Perdeu");
    usleep(100000);
    keyboardDestroy();
    screenDestroy();
    //timerDestroy();

    return 0;
}

