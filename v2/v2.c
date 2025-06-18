#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

#define LARGEUR 40        // Largeur maximale du terminal
#define HAUTEUR 20        // Hauteur maximale du terminal
#define TAILLE_SERPENT 10 // Taille du serpent

// Variable pour la direction initiale
char direction = 'd'; // Droite

// Fonction pour effacer l'écran
void effacerEcran() {
    system("clear");
}

// Fonction pour configurer le terminal en mode non canonique
void configurerModeNonCanonique() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

// Fonction pour restaurer le mode canonique du terminal
void restaurerModeCanonique() {
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// Fonction pour configurer l'entrée clavier en mode non-bloquant
void setNonBlockingMode() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Fonction pour lire une touche sans bloquer le programme
char lireTouche() {
    char touche;
    if (read(STDIN_FILENO, &touche, 1) == 1) {
        return touche;
    }
    return 0;  // Retourne 0 si aucune touche n'a été appuyée
}

// Fonction pour dessiner le serpent
void dessinerSerpent(int lesX[], int lesY[]) {
    for (int i = 0; i < TAILLE_SERPENT - 1; i++) {
        printf("\033[%d;%dH", lesY[i], lesX[i]);
        printf("X");
    }
    printf("\033[%d;%dH", lesY[TAILLE_SERPENT - 1], lesX[TAILLE_SERPENT - 1]);
    printf("O"); // Tête du serpent
    fflush(stdout);
}

// Fonction pour déplacer le serpent
void progresser(int lesX[], int lesY[], char direction) {
    // Déplace chaque partie du corps vers la position de la partie suivante
    for (int i = 0; i < TAILLE_SERPENT - 1; i++) {
        lesX[i] = lesX[i + 1];
        lesY[i] = lesY[i + 1];
    }

    // Mettre à jour la position de la tête selon la direction
    switch (direction) {
        case 'd': lesX[TAILLE_SERPENT - 1]++; break;  // Droite
        case 'q': lesX[TAILLE_SERPENT - 1]--; break;  // Gauche
        case 'z': lesY[TAILLE_SERPENT - 1]--; break;  // Haut
        case 's': lesY[TAILLE_SERPENT - 1]++; break;  // Bas
    }

    // Vérification pour que le serpent disparaisse de l'écran et réapparaisse de l'autre côté
    if (lesX[TAILLE_SERPENT - 1] < 0) lesX[TAILLE_SERPENT - 1] = LARGEUR - 1;
    if (lesX[TAILLE_SERPENT - 1] >= LARGEUR) lesX[TAILLE_SERPENT - 1] = 0;
    if (lesY[TAILLE_SERPENT - 1] < 0) lesY[TAILLE_SERPENT - 1] = HAUTEUR - 1;
    if (lesY[TAILLE_SERPENT - 1] >= HAUTEUR) lesY[TAILLE_SERPENT - 1] = 0;
}

// Fonction principale
int main() {
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];

    // Initialisation de la position du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        lesX[i] = 20 - i;
        lesY[i] = 20;
    }

    // Configuration du terminal en mode non canonique et non bloquant
    configurerModeNonCanonique();
    setNonBlockingMode();

    while (1) {
        effacerEcran();
        dessinerSerpent(lesX, lesY);
        usleep(200000);  // Temporisation pour contrôler la vitesse

        // Lire la touche appuyée pour changer la direction
        char touche = lireTouche();
        if (touche) {
            // Ne pas autoriser le demi-tour immédiat
            if ((touche == 'd' && direction != 'q') ||
                (touche == 'q' && direction != 'd') ||
                (touche == 'z' && direction != 's') ||
                (touche == 's' && direction != 'z')) {
                direction = touche;
            }
        }

        // Avancer le serpent dans la direction actuelle
        progresser(lesX, lesY, direction);
    }

    // Restauration des paramètres du terminal
    restaurerModeCanonique();

    return 0;
}
