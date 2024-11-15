/*
// Created by Pedro Gusmão on 26/10/2024.
//
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include "ui_utils.h"

#define PLAYER_VEL 1
#define BULLET_VEL 1
#define OBJECT_VEL 1
#define COIN_VEL 1
#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 3

typedef struct
{
    char nome[50];
    int score;
} Jogador;

int score = 0;

char player_sprite[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {' ', '^', ' '},
    {'/', '|', '\\'},
    {'<', '-', '>'}};
// char object_sprite1[] = "===^^^===";
char object_sprite2[] = "===//===";
char object_sprite3[] = "===-----===";
// char object_sprite4[] = "===**===";
char object_sprite5[] = "=======";

typedef struct position
{
    int x;
    int y;
} position;

typedef struct player
{
    int x;
    int y;
    int direction;
    char img;
    struct player *next;
} player;

typedef struct object
{
    position pos;
    int life;
    char *sprite;
    int is_destructible;
    int hit_area_init;
    int hit_area_end;
    struct object *next;
} object;

typedef struct particle
{
    position pos;
    char img;
    struct particle *next;
} particle;

typedef struct collectable {
    position pos;
    char sprite;
    struct collectable *next;
} collectable;

int delay_to_action(double delay_time, clock_t *last_t) {
    clock_t current_t = clock();
    if ((double)(current_t - *last_t) / CLOCKS_PER_SEC >= delay_time)
    {
        *last_t = current_t;
        return 1;
    }
    return 0;
}

int create_random_Xposition(int minx, int maxx, int sprite_length)
{
    return rand() % (maxx - sprite_length - minx + 1) + minx;
}

int check_collision(player ship, object *objects)
{
    object *iterate_object = objects;

    while (iterate_object != NULL)
    {

        for (int i = 0; iterate_object->sprite[i] != '\0'; i++)
        {
            if (((ship.x <= (iterate_object->pos.x + i) && (ship.x + PLAYER_WIDTH - 1) >= iterate_object->pos.x + i)) && (ship.y <= (iterate_object->pos.y) && (ship.y + PLAYER_HEIGHT - 1) >= (iterate_object->pos.y)))
            {
                return 0;
            }
        }
        iterate_object = iterate_object->next;
    }
    return 1;
}

int is_obj_destroyed(particle *bullet, object *obj)
{
    if (obj->is_destructible)
    {
        if ((bullet->pos.x >= obj->pos.x + obj->hit_area_init) && (bullet->pos.x <= obj->pos.x + obj->hit_area_end))
        {
            return 1;
        }
    }
    return 0;
}

void handle_collision_object_bullet(object **objects, particle **bullets)
{
    object *curr_object = *objects, *prev_object = NULL;
    particle *curr_bullet = *bullets, *prev_bullet = NULL;

    while (curr_bullet != NULL)
    {
        int bullet_destroyed = 0;
        curr_object = *objects;
        prev_object = NULL;

        while (curr_object != NULL)
        {
            if (curr_bullet->pos.y == curr_object->pos.y)
            {
                int relative_x = curr_bullet->pos.x - curr_object->pos.x;
                if (relative_x >= 0 && relative_x < strlen(curr_object->sprite))
                {
                    char hit_char = curr_object->sprite[relative_x];

                    if ((strcmp(curr_object->sprite, object_sprite2) == 0 && hit_char == '/') ||
                        (strcmp(curr_object->sprite, object_sprite3) == 0 && hit_char == '-'))
                    {

                        if (prev_object == NULL)
                        {
                            *objects = curr_object->next;
                        }
                        else
                        {
                            prev_object->next = curr_object->next;
                        }

                        score += 100;
                        free(curr_object->sprite);
                        free(curr_object);

                        bullet_destroyed = 1;
                        break;
                    }

                    if (hit_char == '=')
                    {
                        bullet_destroyed = 1;
                        break;
                    }
                }
            }

            prev_object = curr_object;
            curr_object = curr_object->next;
        }

        if (bullet_destroyed)
        {
            if (prev_bullet == NULL)
            {
                *bullets = curr_bullet->next;
            }
            else
            {
                prev_bullet->next = curr_bullet->next;
            }
            particle *temp_bullet = curr_bullet;
            curr_bullet = curr_bullet->next;
            free(temp_bullet);
        }
        else
        {
            prev_bullet = curr_bullet;
            curr_bullet = curr_bullet->next;
        }
    }
}

void drawPlayer(char (*ps)[PLAYER_WIDTH], player ship)
{
    for (int i = 0; i < PLAYER_HEIGHT; i++)
    {
        for (int j = 0; j < PLAYER_WIDTH; j++)
        {
            screenGotoxy(ship.x + j, ship.y + i);
            printf("%c", ps[i][j]);
        }
    }
    screenUpdate();
}

void draw_bullets(particle *head)
{
    particle *iterate_bullets = head;

    while (iterate_bullets != NULL)
    {
        screenGotoxy((iterate_bullets->pos).x, (iterate_bullets->pos).y);
        printf("%c", iterate_bullets->img);
        iterate_bullets = iterate_bullets->next;
    }
}

void add_bullet(particle **head, int x, int y, char img)
{
    particle *iterate_bullets = *head, *new_bullet = (particle *)malloc(sizeof(particle));
    (new_bullet->pos).x = x;
    (new_bullet->pos).y = y;
    new_bullet->img = img;
    new_bullet->next = NULL;

    if (*head == NULL)
    {
        *head = new_bullet;
    }
    else
    {
        while (iterate_bullets->next != NULL)
        {
            iterate_bullets = iterate_bullets->next;
        }
        iterate_bullets->next = new_bullet;
    }
}

void move_bullets(particle *head)
{
    particle *iterate_bullets = head;

    while (iterate_bullets != NULL)
    {
        (iterate_bullets->pos).y -= BULLET_VEL;
        iterate_bullets = iterate_bullets->next;
    }
}

void remove_bullets(particle **head)
{
    particle *iterate_bullets = *head, *temp = *head;
    if (*head != NULL)
    {
        if ((*head)->pos.y <= MINY)
        {
            *head = (*head)->next;
            free(temp);
        }

        while (iterate_bullets != NULL && iterate_bullets->next != NULL)
        {
            if (iterate_bullets->next->pos.y <= MINY)
            {
                temp = iterate_bullets->next;
                iterate_bullets->next = iterate_bullets->next->next;
                free(temp);
            }
            else
            {
                iterate_bullets = iterate_bullets->next;
            }
        }
    }
}

int len_bullets(particle *head)
{
    particle *iterate_bullets = head;
    int count = 0;
    while (iterate_bullets != NULL)
    {
        count += 1;
        iterate_bullets = iterate_bullets->next;
    }
    return count;
}

char *choose_enemy_sprite()
{
    unsigned int seed = time(0);
    int sprite_choice = rand_r(&seed) % 3 + 1;

    switch (sprite_choice)
    {
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

int define_enemy_type(object *enemy)
{
    if (strcmp(enemy->sprite, object_sprite2) == 0)
    {
        enemy->hit_area_init = 3;
        enemy->hit_area_end = 4;
        return 1;
    }
    if (strcmp(enemy->sprite, object_sprite3) == 0)
    {
        enemy->hit_area_init = 3;
        enemy->hit_area_end = 7;
        return 1;
    }

    enemy->hit_area_init = 0;
    enemy->hit_area_end = 0;
    return 0;
}

void add_object(object **head, int x, int y, int life, char *sprite)
{
    object *iterate_object = *head, *new_object = (object *)malloc(sizeof(object));
    new_object->sprite = (char *)malloc(sizeof(char) * strlen(sprite));
    strcpy(new_object->sprite, sprite);
    new_object->is_destructible = define_enemy_type(new_object);
    (new_object->pos).x = x;
    (new_object->pos).y = y;
    new_object->next = NULL;

    if (*head == NULL)
    {
        *head = new_object;
    }
    else
    {
        while (iterate_object->next != NULL)
        {
            iterate_object = iterate_object->next;
        }
        iterate_object->next = new_object;
    }
}

void draw_object(object *head)
{
    object *iterate_object = head;
    while (iterate_object != NULL)
    {
        for (int i = 0; iterate_object->sprite[i] != '\0'; i++)
        {
            screenGotoxy(iterate_object->pos.x + i, iterate_object->pos.y);
            printf("%c", iterate_object->sprite[i]);
        }
        iterate_object = iterate_object->next;
    }
}

void spawn_enemy(object **head)
{
    char *enemy_sprite = choose_enemy_sprite();
    int enemy_x = create_random_Xposition(MINX, MAXX, strlen(enemy_sprite));
    add_object(head, enemy_x, -2, 2, enemy_sprite);
}

void move_object(object **head, int player_y)
{
    object *iterate_object = *head, *prev_object = NULL;
    while (iterate_object != NULL)
    {

        if (iterate_object->pos.y < player_y + 5)
        {
            iterate_object->pos.y += OBJECT_VEL;
        }
        if (iterate_object->pos.y >= player_y + 5)
        {
            if (prev_object == NULL)
            {
                *head = iterate_object->next;
            }
            else
            {
                prev_object->next = iterate_object->next;
            }

            free(iterate_object);
            iterate_object = (prev_object == NULL) ? *head : prev_object->next;
        }
        else
        {
            prev_object = iterate_object;
            iterate_object = iterate_object->next;
        }
    }
}

void add_collectables(collectable **coin_head, int x, int y, char sprite) {
    collectable *iterate_coin = *coin_head, *new_coin = (collectable *) malloc(sizeof(collectable));
    new_coin->pos.x = x;
    new_coin->pos.y = y;
    new_coin->sprite = sprite;
    new_coin->next = NULL;
    if (*coin_head == NULL) {
        *coin_head = new_coin;
    }
    else {
        while (iterate_coin->next != NULL) {
            iterate_coin = iterate_coin->next;
        }
        iterate_coin->next = new_coin;
    }
}

void spawn_collectables(collectable **coin) {
    int coin_x = create_random_Xposition(MINX, MAXX, 1);
    add_collectables(coin, coin_x, 0, '$');
}

void draw_collectables(collectable *coin_head) {
    collectable *iterate_coin = coin_head;

    while (iterate_coin != NULL) {
        screenGotoxy(iterate_coin->pos.x, iterate_coin->pos.y);
        screenSetColor(GREEN, BLACK);
        printf("%c", iterate_coin->sprite);
        iterate_coin = iterate_coin->next;
    }
    screenSetColor(YELLOW, BLACK);
}

void move_collectables(collectable *coin_head, int vel) {
    collectable *iterate_coin = coin_head;

    while (iterate_coin != NULL) {
        iterate_coin->pos.y += vel;
        iterate_coin = iterate_coin->next;
    }
}

void salvar(const char *nome, int score) {
    FILE *arquivo;
    Jogador jogadores[100];
    int n = 0;

    arquivo = fopen("hall.txt", "r");
    while (fscanf(arquivo, "%20s %d", jogadores[n].nome, &jogadores[n].score) == 2)
    {
        n++;
    }
    fclose(arquivo);

    Jogador novo;
    strcpy(novo.nome, nome);
    novo.score = score;

    jogadores[n++] = novo;

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (jogadores[i].score < jogadores[j].score)
            {
                Jogador temp = jogadores[i];
                jogadores[i] = jogadores[j];
                jogadores[j] = temp;
            }
        }
    }
    arquivo = fopen("hall.txt", "w");
    for (int i = 0; i < n; i++)
    {
        fprintf(arquivo, "%s %d\n", jogadores[i].nome, jogadores[i].score);
    }
    fclose(arquivo);
}

void draw_game_information(int score, particle *bullets)
{
    screenGotoxy(MAXX + 5, MINY + 1);
    printf("SCORE %d", score);

    int bullets_to_shoot = len_bullets(bullets);
    screenGotoxy(MAXX + 5, MINY + 4);

    if (bullets_to_shoot == 2)
        printf(" ");
    else if (bullets_to_shoot == 1)
        printf("| ");
    else if (bullets_to_shoot == 0)
        printf("| |");
}

void move(player *ship)
{
    ship->x += PLAYER_VEL * ship->direction;
}

int main()
{
    screenInit(1);
    keyboardInit();
    start_screen();
    srand(time(0));

    // char nome[21];
    // printf("nome: ");
    // scanf("%20s", nome);

    player ship = {85, 18, 0, '>', NULL};
    particle *ship_bullets = NULL;
    collectable *coins = NULL;
    object *enemy = NULL;
    char *enemy_sprite = choose_enemy_sprite();
    int enemy_x = create_random_Xposition(MINX, MAXX, strlen(enemy_sprite));
    add_object(&enemy, enemy_x, -2, 2, enemy_sprite);

    clock_t spawn_clock = clock(), move_clock = clock(), score_clock = clock(), spawn_coin_clock = clock(),
    coin_clock = clock(), bullet_clock = clock();

    int run = 1;

    while (run)
    {
        if (keyhit())
        {
            switch (readch())
            {
            case 'a':
                if (ship.x > MINX + 1)
                {
                    ship.direction = -1;
                    move(&ship);
                }
                break;
            case 'd':
                if (ship.x < (MAXX - PLAYER_WIDTH - 1))
                {
                    ship.direction = 1;
                    move(&ship);
                }
                break;
            case ' ':
                if (len_bullets(ship_bullets) < 2 && delay_to_action(0.001, &bullet_clock))
                {
                    add_bullet(&ship_bullets, ship.x + 1, ship.y - 1, '|');
                }
                break;
            case 'x':
                run = 0;
                break;
            default:
                break;
            }
        }
        system("clear");
        draw_border();

        if (delay_to_action(0.01, &score_clock))
        {
            score += 10;
        }

        if (delay_to_action(0.002, &move_clock))
        {
            move_object(&enemy, ship.y);
            if (delay_to_action(0.02, &spawn_clock))
            {
                spawn_enemy(&enemy);
            }
        }

        if (delay_to_action(0.003, &coin_clock)) {
            move_collectables(coins, COIN_VEL);
            if (delay_to_action(0.03, &spawn_coin_clock)) {
                spawn_collectables(&coins);
            }
        }

        run = check_collision(ship, enemy);
        handle_collision_object_bullet(&enemy, &ship_bullets);
        move_bullets(ship_bullets);
        remove_bullets(&ship_bullets);
        draw_object(enemy);
        drawPlayer(player_sprite, ship);
        draw_bullets(ship_bullets);
        draw_collectables(coins);
        draw_game_information(score, ship_bullets);

        screenUpdate();
        usleep(33333);
    }

    // salvar(nome, score);
    screenHomeCursor();
    printf("Você Perdeu");
    usleep(100000);
    keyboardDestroy();
    screenDestroy();

    return 0;
}
