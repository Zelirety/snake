#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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

int main() {
    int position = 0;

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
    }

    return 0;
}
