#include "screen.h"
#include <stdlib.h>
#include "keyboard.h"
#include <string.h>

int opcao = 1;

void draw_title()
{
    int titulo_y = MINY + 2;
    // Exibir título do jogo
    screenSetColor(YELLOW, BLACK);
    screenGotoxy((MAXX + MINX) / 2 - 7, titulo_y);
    printf("================");
    screenGotoxy((MAXX + MINX) / 2 - 4, titulo_y + 1);
    printf("Void Runner");
    screenGotoxy((MAXX + MINX) / 2 - 7, titulo_y + 2);
    printf("================");
}

void draw_menu_options()
{
    int primeira_opcao_y = MINY + 6;

    // Opções do menu
    screenGotoxy((MAXX + MINX) / 2 - 4, primeira_opcao_y);
    printf("%s [ JOGAR ]", opcao == 1 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 10, primeira_opcao_y + 2);
    printf("%s [ INSTRUÇÕES ]", opcao == 2 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 8, primeira_opcao_y + 4);
    printf("%s [ SCORES ]", opcao == 3 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 8, primeira_opcao_y + 6);
    printf("%s [ CRÉDITOS ]", opcao == 4 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 8, primeira_opcao_y + 8);
    printf("%s [ SAIR ]", opcao == 5 ? "->" : "  ");
}

void draw_border()
{
    char hbc = BOX_HLINE;
    char vbc = BOX_VLINE;

    screenClear();
    screenBoxEnable();

    screenGotoxy(MINX, MINY);
    printf("%c", BOX_UPLEFT);

    for (int i = MINX + 1; i < MAXX; i++)
    {
        screenGotoxy(i, MINY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MINY);
    printf("%c", BOX_UPRIGHT);

    for (int i = MINY + 1; i < MAXY; i++)
    {
        screenGotoxy(MINX, i);
        printf("%c", vbc);
        screenGotoxy(MAXX, i);
        printf("%c", vbc);
    }

    screenGotoxy(MINX, MAXY);
    printf("%c", BOX_DWNLEFT);
    for (int i = MINX + 1; i < MAXX; i++)
    {
        screenGotoxy(i, MAXY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MAXY);
    printf("%c", BOX_DWNRIGHT);

    screenBoxDisable();
}

void show_instructions()
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    screenGotoxy((MAXX + MINX) / 2 - 10, MINY + 2);
    printf("===== INSTRUÇÕES =====");

    int startY = MINY + 5;
    int offsetX = (MAXX + MINX) / 2 - 10;

    screenGotoxy(offsetX, startY);
    printf("| A | -> Mover Esq.");
    screenGotoxy(offsetX, startY + 2);
    printf("| D | -> Mover Dir.");
    screenGotoxy(offsetX, startY + 4);
    printf("| Espaço | -> Atirar");
    screenGotoxy(offsetX, startY + 7);
    printf("Dica: Evite colisões!");
    screenGotoxy(offsetX, startY + 11);
    printf("Pressione qualquer");
    screenGotoxy(offsetX, startY + 12);
    printf("tecla para voltar...");
    screenUpdate();
    readch();
    screenClear();
}

void show_credits()
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    int startY = MINY + 5;
    int offsetX = (MAXX + MINX) / 2 - 10;

    // Título
    screenGotoxy((MAXX + MINX) / 2 - 10, MINY + 2);
    printf("====== CRÉDITOS ======");

    // Créditos
    screenGotoxy(offsetX, startY);
    printf("Desenvolvido por:");
    screenGotoxy(offsetX, startY + 2);
    printf("- Felipe Rêgo");
    screenGotoxy(offsetX, startY + 3);
    printf("- Fernando Cavalcanti");
    screenGotoxy(offsetX, startY + 4);
    printf("- Pedro Gusmão");
    screenGotoxy(offsetX, startY + 8);
    printf("Muito Obrigado!");
    screenGotoxy(offsetX, startY + 12);
    printf("Pressione qualquer");
    screenGotoxy(offsetX, startY + 13);
    printf("tecla para voltar...");
    screenUpdate();
    readch();
    screenClear();
}

void show_scores()
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    screenGotoxy((MAXX + MINX) / 2 - 10, MINY + 2);
    printf("======= SCORES =======");
    screenGotoxy((MAXX + MINX) / 2 - 2, MINY + 4);
    printf("Top 05");

    int startY = MINY + 5;
    int offsetX = (MAXX + MINX) / 2 - 10;

    // Créditos
    screenGotoxy(offsetX, startY);
    printf("Top 5");
    screenGotoxy(offsetX, startY + 12);
    printf("Pressione qualquer");
    screenGotoxy(offsetX, startY + 13);
    printf("tecla para voltar...");
    screenUpdate();
    readch();
    screenClear();
}

void start_screen()
{
    int ch = 0;

    draw_border();
    draw_title();
    draw_menu_options();
    screenUpdate();

    while (1)
    {
        if (keyhit())
        {
            ch = readch();

            switch (ch)
            {
            case 'w':
                opcao -= 1;
                if (opcao < 1)
                    opcao = 5;
                break;
            case 's':
                opcao += 1;
                if (opcao > 5)
                    opcao = 1;
                break;
            case '\n':
                switch (opcao)
                {
                case 1:
                    return;
                case 2:
                    show_instructions();
                    break;
                case 3:
                    show_scores();
                    break;
                case 4:
                    show_credits();
                    break;
                case 5:
                    screenDestroy();
                    exit(0);
                }
                break;
            default:
                break;
            }

            draw_border();
            draw_title();
            draw_menu_options();
            screenUpdate();
        }
    }
}
