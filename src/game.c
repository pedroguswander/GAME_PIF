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
#define OBJECT_VEL 1
#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 3

typedef struct {
    char nome[50];
    int score;
} Jogador;

int score = 0;

char player_sprite[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {' ', '^', ' '},
    {'/', '|', '\\'},
    {'<', '-', '>'}
};
//char object_sprite1[] = "===^^^===";
char object_sprite2[] = "===//===";
char object_sprite3[] = "===-----===";
//char object_sprite4[] = "===**===";
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
    int hit_area_init;
    int hit_area_end;
    struct object *next;
} object;

typedef struct particle {
    position pos;
    char img;
    struct particle *next;
} particle;

void draw_border() {

{
    char hbc = BOX_HLINE;
    char vbc = BOX_VLINE;
    
    screenClear();
    screenBoxEnable();
    
    screenGotoxy(MINX, MINY);
    printf("%c", BOX_UPLEFT);

    for (int i=MINX+1; i<MAXX; i++)
    {
        screenGotoxy(i, MINY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MINY);
    printf("%c", BOX_UPRIGHT);

    for (int i=MINY+1; i<MAXY; i++)
    {
        screenGotoxy(MINX, i);
        printf("%c", vbc);
        screenGotoxy(MAXX, i);
        printf("%c", vbc);
    }

    screenGotoxy(MINX, MAXY);
    printf("%c", BOX_DWNLEFT);
    for (int i=MINX+1; i<MAXX; i++)
    {
        screenGotoxy(i, MAXY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MAXY);
    printf("%c", BOX_DWNRIGHT);

    screenBoxDisable();
    
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

        for (int i = 0; iterate_object->sprite[i] != '\0'; i++) {
            if (((ship.x <= (iterate_object->pos.x + i) && (ship.x + PLAYER_WIDTH - 1) >= iterate_object->pos.x + i)) && (ship.y <= (iterate_object->pos.y) && (ship.y + PLAYER_HEIGHT - 1) >= (iterate_object->pos.y))) {
                return 0;
            }
        }
        iterate_object = iterate_object->next;
    }
    return 1;
}

int is_obj_destroyed(particle *bullet, object *obj) {
    if (obj->is_destructible) {
        if ((bullet->pos.x >= obj->pos.x + obj->hit_area_init) && (bullet->pos.x <= obj->pos.x + obj->hit_area_end)) {
            return 1;
        }
    }
    return 0;
}

void handle_collision_object_bullet(object **obj_head, particle **bullet_head) {
    object *objects = *obj_head, *obj_temp = *obj_head;
    particle *bullets = *bullet_head, *bullet_temp = *bullet_head;

    if (objects != NULL && bullets != NULL) {
        for (int i = 0; (*obj_head)->sprite[i] != '\0'; i++) {
            if (((*bullet_head)->pos.y == (*obj_head)->pos.y) && ((*bullet_head)->pos.x == (*obj_head)->pos.x + i)) {
                if (is_obj_destroyed(*bullet_head ,*obj_head)) {
                    *obj_head = (*obj_head)->next;
                    free(obj_temp);
                    score += 100;
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
                    if (is_obj_destroyed(*bullet_head ,objects->next)) {
                        obj_temp = objects->next;
                        objects->next = objects->next->next;
                        free(obj_temp);
                        score += 100;
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

char *choose_enemy_sprite() {
    unsigned int seed = time(0);
    int sprite_choice = rand_r(&seed) % 3 + 1;

    switch (sprite_choice) {
        case 1:
            return object_sprite2;
        case 2:
            return object_sprite3;
        case 3:
            return object_sprite5;
        default:
            return object_sprite2;
    }
}

int define_enemy_type(object *enemy) {
    if (strcmp(enemy->sprite, object_sprite2) == 0) {
        enemy->hit_area_init = 3;
        enemy->hit_area_end = 4;
        return 1;
    }
    if (strcmp(enemy->sprite, object_sprite3) == 0) {
        enemy->hit_area_init = 3;
        enemy->hit_area_end = 7;
        return 1;
    }

    enemy->hit_area_init = 0;
    enemy->hit_area_end = 0;
    return 0;
}

void add_object(object **head, int x, int y, int life, char *sprite) {
    object *iterate_object = *head, *new_object = (object *) malloc(sizeof(object));
    new_object->sprite = (char *) malloc(sizeof(char) * strlen(sprite));
    strcpy(new_object->sprite, sprite);
    new_object->is_destructible = define_enemy_type(new_object);
    (new_object->pos).x = x;
    (new_object->pos).y = y;
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

void spawn_enemy(object **head) {
    char *enemy_sprite = choose_enemy_sprite();
    int enemy_x = create_random_Xposition(MINX, MAXX, strlen(enemy_sprite));
    add_object(head, enemy_x, -2, 2, enemy_sprite);
}

void move_object(object **head, int player_y) {
    object *iterate_object = *head, *prev_object = NULL;
    while (iterate_object != NULL) {

        if (iterate_object->pos.y < player_y + 5) {
            iterate_object->pos.y += OBJECT_VEL;
        }
        if (iterate_object->pos.y >= player_y + 5) {
            if (prev_object == NULL) {
                *head = iterate_object->next;
            } else {
                prev_object->next = iterate_object->next;
            }

            free(iterate_object);
            iterate_object = (prev_object == NULL) ? *head : prev_object->next;
        } else {
            prev_object = iterate_object;
            iterate_object = iterate_object->next;
        }
    }
}
void salvar(const char *nome, int score) {
    FILE *arquivo;
    Jogador jogadores[100];
    int n = 0;

    arquivo = fopen("hall.txt", "r");
        while (fscanf(arquivo, "%20s %d", jogadores[n].nome, &jogadores[n].score) == 2) {
            n++;
        }
        fclose(arquivo);


    Jogador novo;
    strcpy(novo.nome, nome);
    novo.score = score;

    jogadores[n++] = novo;

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (jogadores[i].score < jogadores[j].score) {
                Jogador temp = jogadores[i];
                jogadores[i] = jogadores[j];
                jogadores[j] = temp;
            }
        }
    }
    arquivo = fopen("hall.txt", "w");
    for (int i = 0; i < n; i++) {
        fprintf(arquivo, "%s %d\n", jogadores[i].nome, jogadores[i].score);
    }
    fclose(arquivo);
}

void move(player *ship) {
    ship->x += PLAYER_VEL * ship->direction;
}

int main() {
    screenInit(1);
    keyboardInit();
    srand(time(0));

    char nome[21];
    draw_border();
    int center_x = (MINX + MAXX) / 2 - 3;
    int center_y = MINY + 6;
    screenGotoxy(center_x, center_y);
    printf("Nome: ");
    scanf("%20s", nome);

    player ship = {85,18,0,'>',NULL} ;
    particle *ship_bullets = NULL;
    object *enemy = NULL;
    char *enemy_sprite = choose_enemy_sprite();
    int enemy_x = create_random_Xposition(MINX, MAXX, strlen(enemy_sprite));
    add_object(&enemy, enemy_x, -2, 2, enemy_sprite);
    clock_t spawn_clock = clock(), move_clock = clock(), score_clock = clock();

    int run = 1;
    
    while(run) {
        if (keyhit()) {
            switch (readch()) {
                case 97:
                    if (ship.x > MINX + 1) {
                        ship.direction = -1;
                        move(&ship);
                    }
                break;
                case 100:
                    if (ship.x < (MAXX - PLAYER_WIDTH - 1)) {
                        ship.direction = 1;
                        move(&ship);
                    }
                break;
                case 120:
                    if (len_bullets(ship_bullets) < 5) {
                        add_bullet(&ship_bullets, ship.x, ship.y-1, '|');
                    }
                    break;
                case 32:
                    run = 0;
                    break;
                default:
            }
        }
        system("clear");
        draw_border();

        if (delay_object(0.01, &score_clock)) {
            score += 10;
        }

        if (delay_object(0.002, &move_clock)) {
            move_object(&enemy, ship.y);
            if (delay_object(0.02, &spawn_clock)) {
                spawn_enemy(&enemy);
            }
        }

        run = check_collision(ship, enemy);
        handle_collision_object_bullet(&enemy, &ship_bullets);
        move_bullets(ship_bullets);
        remove_bullets(&ship_bullets);
        draw_object(enemy);
        drawPlayer(player_sprite, ship);
        draw_bullets(ship_bullets);
        screenGotoxy(MAXX + 5, MINY + 1);
        printf("SCORE %d", score);

        screenUpdate();
        usleep(33333);
    }

    salvar(nome, score);
    screenHomeCursor();
    printf("Você Perdeu");
    usleep(100000);
    keyboardDestroy();
    screenDestroy();


    return 0;
}

