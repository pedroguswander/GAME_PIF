#include "screen.h"
#include <stdlib.h>
#include "keyboard.h"

void draw_border() {
    char hbc = BOX_HLINE;
    char vbc = BOX_VLINE;

    screenClear();
    screenBoxEnable();

    screenGotoxy(MINX, MINY);
    printf("%c", BOX_UPLEFT);

    for (int i = MINX + 1; i < MAXX; i++) {
        screenGotoxy(i, MINY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MINY);
    printf("%c", BOX_UPRIGHT);

    for (int i = MINY + 1; i < MAXY; i++) {
        screenGotoxy(MINX, i);
        printf("%c", vbc);
        screenGotoxy(MAXX, i);
        printf("%c", vbc);
    }

    screenGotoxy(MINX, MAXY);
    printf("%c", BOX_DWNLEFT);
    for (int i = MINX + 1; i < MAXX; i++) {
        screenGotoxy(i, MAXY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MAXY);
    printf("%c", BOX_DWNRIGHT);

    screenBoxDisable();
}

void show_instructions() {
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

void show_credits() {
    draw_border();
    screenSetColor(YELLOW, BLACK);

    
    int startY = MINY + 5;
    int offsetX = (MAXX + MINX) / 2 - 10;

    // Título
    screenGotoxy((MAXX + MINX) / 2 - 9, MINY + 2);
    printf("===== CRÉDITOS =====");

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


void start_screen() {
    int opcao = 1;
    int ch = 0;
    int tituloY = MINY + 2;
    int primeiraOpcaoY = MINY + 6;

    draw_border();

    // Exibir título do jogo
    screenSetColor(YELLOW, BLACK);
    screenGotoxy((MAXX + MINX) / 2 - 7, tituloY);
    printf("================");
    screenGotoxy((MAXX + MINX) / 2 - 4, tituloY + 1);
    printf("Void Runner");
    screenGotoxy((MAXX + MINX) / 2 - 7, tituloY + 2);
    printf("================");

    // Opções do menu
    screenGotoxy((MAXX + MINX) / 2 - 4, primeiraOpcaoY);
    printf("%s [ JOGAR ]", opcao == 1 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 10, primeiraOpcaoY + 2);
    printf("%s [ INSTRUÇÕES ]", opcao == 2 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 8, primeiraOpcaoY + 4);
    printf("%s [ CRÉDITOS ]", opcao == 3 ? "->" : "  ");
    screenGotoxy((MAXX + MINX) / 2 - 8, primeiraOpcaoY + 6);
    printf("%s [ SAIR ]", opcao == 4 ? "->" : "  ");

    screenUpdate();

    while (1) {
        if (keyhit()) {
            ch = readch();

            switch (ch) {
                case 'w':
                    opcao -= 1;
                    if (opcao < 1) opcao = 4;
                    break;
                case 's':
                    opcao += 1;
                    if (opcao > 4) opcao = 1;
                    break;
                case '\n':
                    switch (opcao) {
                        case 1:
                            return;
                        case 2:
                            show_instructions();
                            break;
                        case 3:
                            show_credits();
                            break;
                        case 4:
                            screenDestroy();
                            exit(0);
                    }
                    break;
                default:
                    break;
            }

            draw_border();
            screenGotoxy((MAXX + MINX) / 2 - 7, tituloY);
            printf("================");
            screenGotoxy((MAXX + MINX) / 2 - 4, tituloY + 1);
            printf("Void Runner");
            screenGotoxy((MAXX + MINX) / 2 - 7, tituloY + 2);
            printf("================");
            screenGotoxy((MAXX + MINX) / 2 - 4, primeiraOpcaoY);
            printf("%s [ JOGAR ]", opcao == 1 ? "->" : "  ");
            screenGotoxy((MAXX + MINX) / 2 - 8, primeiraOpcaoY + 2);
            printf("%s [ INSTRUÇÕES ]", opcao == 2 ? "->" : "  ");
            screenGotoxy((MAXX + MINX) / 2 - 6, primeiraOpcaoY + 4);
            printf("%s [ CRÉDITOS ]", opcao == 3 ? "->" : "  ");
            screenGotoxy((MAXX + MINX) / 2 - 6, primeiraOpcaoY + 6);
            printf("%s [ SAIR ]", opcao == 4 ? "->" : "  ");
            screenUpdate();
        }
    }
}

