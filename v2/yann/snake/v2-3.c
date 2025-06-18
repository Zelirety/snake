#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

#define TAILLE_SERPENT 9       // Taille du serpent
#define TOUCHE_ARRET 'a'         // Touche pour arrêter le programme
#define DROITE 'd'               // Constante pour la direction droite
#define GAUCHE 'q'               // Constante pour la direction gauche
#define HAUT 'z'                 // Constante pour la direction haut
#define BAS 's'                  // Constante pour la direction bas
#define DECALAGE_ORIGINE_X 20    // Décalage initial en X pour éviter les coordonnées négatives
#define DECALAGE_ORIGINE_Y 20    // Décalage initial en Y pour éviter les coordonnées négatives
#define DELAI 200000             // Délai de rafraîchissement en microsecondes

// Variable pour la direction
char direction = DROITE;

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

// Fonction pour dessiner le serpent avec un décalage d'origine
void dessinerSerpent(int lesX[], int lesY[]) {
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        // Affiche uniquement si les coordonnées avec décalage sont positives
        int posX = lesX[i] + DECALAGE_ORIGINE_X;
        int posY = lesY[i] + DECALAGE_ORIGINE_Y;

        if (posX >= 0 && posY >= 0) {
            printf("\033[%d;%dH", posY, posX);
            printf(i == TAILLE_SERPENT - 1 ? "O" : "X"); // "O" pour la tête
        }
    }
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
        case DROITE: lesX[TAILLE_SERPENT - 1]++; break;
        case GAUCHE: lesX[TAILLE_SERPENT - 1]--; break;
        case HAUT: lesY[TAILLE_SERPENT - 1]--; break;
        case BAS: lesY[TAILLE_SERPENT - 1]++; break;
    }
}

// Fonction principale
int main() {
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];

    // Initialisation de la position du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        lesX[i] = i - TAILLE_SERPENT;  // Initialise pour partir du centre
        lesY[i] = 0;
    }

    // Configuration du terminal en mode non canonique et non bloquant
    configurerModeNonCanonique();
    setNonBlockingMode();

    while (1) {
        effacerEcran();
        dessinerSerpent(lesX, lesY);
        usleep(DELAI);  // Temporisation pour contrôler la vitesse

        // Lire la touche appuyée pour changer la direction ou arrêter
        char touche = lireTouche();
        if (touche) {
            if (touche == TOUCHE_ARRET) {
                break;  // Quitte la boucle si la touche d'arrêt est pressée
            }
            // Vérifie que la nouvelle direction n'est pas l'inverse de la direction actuelle
            if ((touche == DROITE && direction != GAUCHE) ||
                (touche == GAUCHE && direction != DROITE) ||
                (touche == HAUT && direction != BAS) ||
                (touche == BAS && direction != HAUT)) {
                direction = touche;  // Met à jour la direction si elle est valide
            }
        }

        // Avancer le serpent dans la direction actuelle
        progresser(lesX, lesY, direction);
    }

    // Restauration des paramètres du terminal
    restaurerModeCanonique();

    return 0;
}
