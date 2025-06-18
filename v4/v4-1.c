#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>

/* Déclaration des constantes */ 

#define TAILLE 10  // Taille du serpent
#define USLEEP 100000  // Temps de pause en ms entre chaque itération de la boucle principale
#define ARRET 'a' // Touche d'arrêt
/* Corps du serpent */ 
#define ANNEAUX 'X'
#define TETE 'O'
/* Touches directionelles */
#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'
/* Position initiale */
#define INIT_X 40
#define INIT_Y 20
/* Initialiser le cadre et les pavés*/
#define HAUTEUR 40
#define LARGEUR 80
#define BORDURE '#'
#define VIDE ' '
#define NB_PAVES 4
#define TAILLE_PAVES 5
#define POMME '6' //caractere pour la pomme


/* Déclaration des fonctions */  

void initPlateau(char plateau[HAUTEUR][LARGEUR]);
void initPaves(char plateau[HAUTEUR][LARGEUR]);
void dessinerPlateau(char plateau[HAUTEUR][LARGEUR]);
int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int positionsX[TAILLE], int positionsY[TAILLE]);
bool verifSeTouche(int positionsX[TAILLE], int positionsY[TAILLE]);
void progresser(int posX[TAILLE], int posY[TAILLE], char direction, char plateau[HAUTEUR][LARGEUR], bool *peutAvancer, int *tailleSerpent, int *usleepTime, int pommeX, int pommeY,bool *pommeMangee);
void effacerEcran();
void disableEcho();
void enableEcho();
void placerPomme(char plateau[HAUTEUR][LARGEUR], int *pommeX, int *pommeY);


int main() {
    int posX[10000];
    int posY[10000];
    int tailleSerpent = TAILLE;
    int usleepTime = USLEEP;

    int pommeX, pommeY;

    int x = INIT_X;
    int y = INIT_Y;
    bool peutAvancer = true;
    bool pommeMangee = false;
    char direction = DROITE;
    char touche = DROITE;
    char plateau[HAUTEUR][LARGEUR];

    effacerEcran(); // Préparer l'écran
    initPlateau(plateau);
    placerPomme(plateau, &pommeX, &pommeY); // Placer la première pomme
    dessinerPlateau(plateau);

    /* Générer les coordonnées initiales */
    for (int i = 0; i < tailleSerpent; i++) {
        posX[i] = x - i;
        posY[i] = y;
    }

    disableEcho(); // Désactiver l'affichage des caractères dans le terminal lorsqu'une touche est pressée

    /* Boucle principale du snake */
    while (peutAvancer) {
        fflush(stdout);
        usleep(usleepTime);
        for (int i = 0; i < tailleSerpent; i++) {
            effacer(posX[i + 1], posY[i + 1]); // Effacer le serpent
        }
		progresser(posX, posY, direction, plateau, &peutAvancer, &tailleSerpent, &usleepTime, pommeX, pommeY,&pommeMangee);
        dessinerSerpent(posX, posY);

        if (pommeMangee){
            usleepTime /= 1.3;
            placerPomme(plateau, &pommeX, &pommeY); 
        }

        if (kbhit() == 1) {
            touche = getchar();
            if (touche == ARRET) { // Condition d'arrêt
                peutAvancer = false;
            }
            if ((touche == GAUCHE && direction != DROITE) || // Changer direction si touche directionnelle pressée et touche != direction opposée
                (touche == DROITE && direction != GAUCHE) ||
                (touche == BAS && direction != HAUT) ||
                (touche == HAUT && direction != BAS)) {
                direction = touche;
            }
        }
    }

    printf("\n");
    gotoXY(1, HAUTEUR + 1);
    enableEcho();
    return EXIT_SUCCESS;
}

void initPlateau(char plateau[HAUTEUR][LARGEUR]) {
    for (int i = 0; i < HAUTEUR; i++) {
        for (int j = 0; j < LARGEUR; j++) {
            if (i == 0 || i == HAUTEUR - 1 || j == 0 || j == LARGEUR - 1) {
                plateau[i][j] = BORDURE;
            } else {
                plateau[i][j] = VIDE;
            }
        }
    }

    // Ajouter des trous exactement au centre des bordures
    plateau[0][LARGEUR / 2 - 1] = VIDE;                  // Bordure supérieure
    plateau[HAUTEUR - 1][LARGEUR / 2 - 1] = VIDE;        // Bordure inférieure
    plateau[HAUTEUR / 2 - 1][0] = VIDE;                  // Bordure gauche
    plateau[HAUTEUR / 2 - 1][LARGEUR - 1] = VIDE;        // Bordure droite

    initPaves(plateau);
}



void initPaves(char plateau[HAUTEUR][LARGEUR]){
    srand(time(NULL));
	int x,y;
	for (int pave_place = 0; pave_place < NB_PAVES; pave_place++) {
		x = rand() % (LARGEUR - TAILLE_PAVES - 2) + 1;
		y = rand() % (HAUTEUR - TAILLE_PAVES - 2) + 1;
		/* Le pavé ne peut pas apparaitre sur le seprent ou sur les bordures, donc on boucle jusqu'à avoir x,y corrects*/
		while ((x > (INIT_X - TAILLE - TAILLE_PAVES) && x < INIT_X + TAILLE_PAVES && x > 1 && x < 79) || (y > (INIT_Y + TAILLE_PAVES) && y < INIT_Y && y > 1 && y < 39)){
			x = rand() % (LARGEUR - TAILLE_PAVES - 2) + 1;
			y = rand() % (HAUTEUR - TAILLE_PAVES - 2) + 1;
		}

		for (int i = 0; i < TAILLE_PAVES;i++){
			for (int j = 0; j < TAILLE_PAVES;j++){
				plateau[y + i][x + j] = BORDURE;
			}
		}
	}
}

void dessinerPlateau(char plateau[HAUTEUR][LARGEUR]){
	for (int i = 0; i < HAUTEUR; i++) {
        for (int j = 0; j < LARGEUR; j++) {
            printf("%c", plateau[i][j]);
        }    
		printf("\n");
    }
}

int kbhit(){
	/*!
	\brief Fonction de détection de la touche ESC
	\return 1 si le caractere correspondant a ESC est présent, 0 sinon
	*/
   
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

void gotoXY(int x, int y){ 
	/*!
	\brief Fonction de déplacement dans le terminal aux coordonées données
	\param x La position X de la tête du serpent
	\param y La position Y de la tête du serpent
	*/
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c){
	/*!
	\brief Fonction qui affiche un caractère composant le serpent aux coordonnées données
	\param x La position X de la tête du serpent
	\param y La position Y de la tête du serpent
	\param c Le caractere à afficher
	*/
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y){
	/*!
	\brief Fonction qui efface un caractère composant le serpent aux coordonnées données
	\param x La position X de la tête du serpent
	\param y La position Y de la tête du serpent
	*/
	char space = ' ';
    gotoXY(x, y);
   	printf("%c",space);
}

void dessinerSerpent(int posX[TAILLE], int posY[TAILLE]){
	/*!
	\brief Fonction qui affiche entièrement le serpent dans le terminal
	\param posX Le tableau de l'abscisse de chaque cellule du serpent, dans l'ordre
	\param posY Le tableau de l'ordonnée de chaque cellule du serpent, dans l'ordre
	*/
	
	for (int i = 0; i < TAILLE; i++){ // Affiche O pour la tete, X pour le corps, dans l'ordre des cellules et selon la taille du serpent
		if (posX[i] >= 1 && posY[i] >= 1){
			if(i == 0) 
			{
				afficher(posX[i], posY[i], TETE); // Tete
			}
			else // N'affiche pas tout le serpent si le serpent est trop a gauche
			{
				afficher(posX[i],posY[i], ANNEAUX); // Corps
			}
		}	
	}
}

bool verifSeTouche(int posX[TAILLE], int posY[TAILLE]){
	for (int i = 1; i < TAILLE; i++){
		if (posX[i] == posX[0] && posY[i] == posY[0]){
			return true;
		}
	}
	return false;
}

void progresser(int posX[TAILLE], int posY[TAILLE], char direction, char plateau[HAUTEUR][LARGEUR], bool *peutAvancer, int *tailleSerpent, int *usleepTime, int pommeX, int pommeY,bool *pommeMangee) {
    // Déplacer le serpent
    for (int i = *tailleSerpent - 1; i >= 1; i--) {
        posX[i] = posX[i - 1];
        posY[i] = posY[i - 1];
    }

    // Mise à jour de la position de la tête
    if (direction == HAUT) {
        posY[0]--;
    } else if (direction == BAS) {
        posY[0]++;
    } else if (direction == GAUCHE) {
        posX[0]--;
    } else if (direction == DROITE) {
        posX[0]++;
    }

    // Vérifier si le serpent entre dans un trou et se téléporte de l'autre côté
    if (posY[0] == 0 && posX[0] == LARGEUR / 2) { // Trou bordure supérieure
        posY[0] = HAUTEUR - 2;
    } else if (posY[0] == HAUTEUR - 1 && posX[0] == LARGEUR / 2) { // Trou bordure inférieure
        posY[0] = 1;
    } else if (posX[0] == 0 && posY[0] == HAUTEUR / 2) { // Trou bordure gauche
        posX[0] = LARGEUR - 2;
    } else if (posX[0] == LARGEUR - 1 && posY[0] == HAUTEUR / 2) { // Trou bordure droite
        posX[0] = 1;
    }

    if (plateau[posY[0]-1][posX[0]-1] == POMME){ // Check une possible collision avec une pomme 
		*pommeMangee =  true;
	}
	else{
		*pommeMangee = false;
	}

    // Affichage pour débogage
    printf("Tête du serpent (X: %d, Y: %d)\n", posX[0], posY[0]);

    // Vérifier si le serpent mange une pomme
    if (posX[0] == pommeX && posY[0] == pommeY) {
        (*tailleSerpent)++;           // Augmenter la taille du serpent
        *usleepTime = (*usleepTime > 20000) ? *usleepTime - 5000 : *usleepTime; // Augmenter la vitesse
        placerPomme(plateau, &pommeX, &pommeY); // Placer une nouvelle pomme
    }

    // Vérifier les collisions avec les bordures ou les pavés
    if (plateau[posY[0]][posX[0]] != VIDE && plateau[posY[0]][posX[0]] != POMME) {
        *peutAvancer = false;
    }

    // Vérifier si le serpent se mord la queue
    if (verifSeTouche(posX, posY)) {
        *peutAvancer = false;
    }
}


void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
	{
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
	{
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
	{
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
	{
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
void placerPomme(char plateau[HAUTEUR][LARGEUR], int *pommeX, int *pommeY) {
    srand(time(NULL));
    do {
        *pommeX = rand() % (LARGEUR - 2) + 1; // Coordonnées aléatoires (hors bordures)
        *pommeY = rand() % (HAUTEUR - 2) + 1;
    } while (plateau[*pommeY][*pommeX] != VIDE); // Vérifier que la position est vide

    plateau[*pommeY][*pommeX] = POMME; // Placer la pomme
}
void effacerEcran() {
    system("clear"); // Nettoie l'écran pour les systèmes UNIX/Linux
}
