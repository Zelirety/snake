#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>

#define TAILLE 10
#define USLEEP 100000
#define LARGEUR 80
#define HAUTEUR 40
#define ARRET 'a'
#define ANNEAUX 'X'
#define TETE 'O'
#define BORDURE '#'
#define VIDE ' '
#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'
#define INIT_X 40
#define INIT_Y 20
#define NB_PAVES 4
#define TAILLE_PAVE 5

// Déclarations des fonctions
void initPlateau(char plateau[HAUTEUR][LARGEUR]);
void dessinerPlateau(char plateau[HAUTEUR][LARGEUR]);
void ajouterPaves(char plateau[HAUTEUR][LARGEUR]);
int kbhit();
void gotoXY(int x, int y);
void effacerEcran();
void disableEcho();
void enableEcho();
void dessinerSerpent(int posX[TAILLE], int posY[TAILLE], char plateau[HAUTEUR][LARGEUR]);
bool progresser(int posX[TAILLE], int posY[TAILLE], char direction, char plateau[HAUTEUR][LARGEUR]);

// Initialisation du plateau
void initPlateau(char plateau[HAUTEUR][LARGEUR]) {
    for (int i = 0; i < HAUTEUR; i++) {
        for (int j = 0; j < LARGEUR; j++) {
            plateau[i][j] = (i == 0 || i == HAUTEUR - 1 || j == 0 || j == LARGEUR - 1) ? BORDURE : VIDE;
        }
    }
    ajouterPaves(plateau);
}

// Fonction pour ajouter des pavés au plateau
void ajouterPaves(char plateau[HAUTEUR][LARGEUR]) {
    srand(time(NULL));
    for (int p = 0; p < NB_PAVES; p++) {
        int x, y;
        do {
            x = rand() % (LARGEUR - TAILLE_PAVE - 2) + 1;
            y = rand() % (HAUTEUR - TAILLE_PAVE - 2) + 1;
        } while (plateau[y][x] == BORDURE);

        for (int i = 0; i < TAILLE_PAVE; i++) {
            for (int j = 0; j < TAILLE_PAVE; j++) {
                plateau[y + i][x + j] = BORDURE;
            }
        }
    }
}

// Affiche le plateau
void dessinerPlateau(char plateau[HAUTEUR][LARGEUR]) {
    effacerEcran();
    for (int i = 0; i < HAUTEUR; i++) {
        for (int j = 0; j < LARGEUR; j++) {
            printf("%c", plateau[i][j]);
        }
        printf("\n");
    }
}

// Fonction pour effacer l'écran
void effacerEcran() {
    printf("\033[H\033[J");
}

// Désactiver l'écho des caractères dans le terminal
void disableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Réactiver l'écho des caractères dans le terminal
void enableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Détecte si une touche a été pressée
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Dessiner le serpent
void dessinerSerpent(int posX[TAILLE], int posY[TAILLE], char plateau[HAUTEUR][LARGEUR]) {
    for (int i = 0; i < TAILLE; i++) {
        plateau[posY[i]][posX[i]] = (i == 0) ? TETE : ANNEAUX;
    }
}

// Fonction de progression avec détection de collision
bool progresser(int posX[TAILLE], int posY[TAILLE], char direction, char plateau[HAUTEUR][LARGEUR]) {
    for (int i = TAILLE - 1; i > 0; i--) {
        posX[i] = posX[i - 1];
        posY[i] = posY[i - 1];
    }
    if (direction == HAUT) posY[0]--;
    else if (direction == BAS) posY[0]++;
    else if (direction == GAUCHE) posX[0]--;
    else if (direction == DROITE) posX[0]++;

    if (plateau[posY[0]][posX[0]] == BORDURE) return true;
    for (int i = 1; i < TAILLE; i++) {
        if (posX[0] == posX[i] && posY[0] == posY[i]) return true;
    }
    return false;
}

int main() {
    char plateau[HAUTEUR][LARGEUR];
    int posX[TAILLE], posY[TAILLE];
    bool collision = false;
    char direction = DROITE, touche = DROITE;

    initPlateau(plateau);
    for (int i = 0; i < TAILLE; i++) {
        posX[i] = INIT_X - i;
        posY[i] = INIT_Y;
    }

    disableEcho();
    while (!collision) {
        dessinerPlateau(plateau);
        dessinerSerpent(posX, posY, plateau);
        fflush(stdout);
        usleep(USLEEP);

        if (kbhit()) {
            touche = getchar();
            if (touche == ARRET) break;
            if ((touche == GAUCHE && direction != DROITE) ||
                (touche == DROITE && direction != GAUCHE) ||
                (touche == BAS && direction != HAUT) ||
                (touche == HAUT && direction != BAS)) {
                direction = touche;
            }
        }
        collision = progresser(posX, posY, direction, plateau);
    }
    printf("Game Over!\n");
    enableEcho();
    return EXIT_SUCCESS;
}