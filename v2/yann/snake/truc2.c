#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

#define LARGEUR 100 // Largeur du terminal
#define TAILLE_SERPENT 9 // Taille du serpent

// Fonction pour effacer l'écran
void effacerEcran() {
    system("clear");
}
// Fonction pour afficher le serpent
void afficherSerpent(int position) {
    // Affiche des espaces pour correspondre à la position actuelle du serpent
    for (int i = 0; i < position; i++) {
        printf(" ");
    }

    // Affiche le serpent (4 'X' et une 'O' pour la tête)
    for (int i = 0; i < TAILLE_SERPENT - 1; i++) {
        printf("X");
    }
    printf("O\n");
}

// Fonction pour vérifier si une touche a été appuyée
int kbhit(void) {
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

int main() {
    int position = 40;

    // Boucle principale pour le déplacement du serpent
    while (1) {
        effacerEcran(); // Efface l'écran
        afficherSerpent(position); // Affiche le serpent à sa nouvelle position
        usleep(200000); // Temporisation (200 millisecondes)

        // Déplace le serpent vers la droite
        position++;

        // Réinitialise la position si le serpent atteint la fin de l'écran
        if (position > LARGEUR - TAILLE_SERPENT) {
            position = 0;
        }

        // Arrêter le programme si une touche est appuyée
        if (kbhit()) {
            break; // Sort de la boucle principale et arrête le programme
        }
    }

    return 0;
}
