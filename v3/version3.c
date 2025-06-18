/**
* @file snake.c
* @brief Jeu du snake dans le terminal
* @author LE HEBEL Nathan 
* @version 2.0
* @date 25/10/2024
*
* Dans cette deuxième version, le serpent se déplace aux quatres directions cardinales conformément
* aux touches rentrées par l'utilisateur 
* L'objectif final est un snake complétement fonctionnel
*
*/

/* Déclaration des fichiers inclus */

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
void progresser(int positionsX[TAILLE], int positionsY[TAILLE], char direction,char plateau[HAUTEUR][LARGEUR],bool *peutAvancer);
void effacerEcran();
void disableEcho();
void enableEcho();



int main(){
    int posX[TAILLE];
 	int posY[TAILLE];
	int x = INIT_X;
	int y = INIT_Y;
	bool peutAvancer = true;
	char direction = DROITE;
	char touche = DROITE;
	char plateau[HAUTEUR][LARGEUR];
	
	effacerEcran(); // Préparer l'écran
	initPlateau(plateau);
	dessinerPlateau(plateau);
	
	/* Génerer les coordonnées initiales */
	for(int i = 0; i < TAILLE; i++) 
	{ 
		posX[i] = x - i;
		posY[i] = y;
	}

	disableEcho(); // Désactive l'affichage des caractères dans les terminal lorsqu'une touche est pressée

	/* Boucle principale du snake qui continue tant que la touche ESC n'a pas été pressée */
	while(peutAvancer){ 
		fflush(stdout);
		usleep(USLEEP);
		for (int i = 0; i < TAILLE; i++)
		{ 	
			effacer(posX[i+1], posY[i+1]); // Efface le serpent
		}
		progresser(posX, posY,direction,plateau,&peutAvancer);
		dessinerSerpent(posX, posY);
		
		if (kbhit() == 1){
			touche = getchar();
			if (touche == ARRET){  // Condition d'arret
				peutAvancer = false;
			}
			if((touche == GAUCHE && direction != DROITE) || // Change direction si touche directionelle pressée et touche != direction opposée
			   (touche == DROITE && direction != GAUCHE) ||
			   (touche == BAS && direction != HAUT) ||
			   (touche == HAUT && direction != BAS)){ 
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
			if (i == 0 || i == HAUTEUR - 1 || j == 0 || j == LARGEUR - 1){
				plateau[i][j] = BORDURE;
			}
			else{
				plateau[i][j] = VIDE;
			}
            // plateau[i][j] = (i == 0 || i == HAUTEUR - 1 || j == 0 || j == LARGEUR - 1) ? BORDURE : VIDE;
        }
    }
	initPaves(plateau);

	// return plateau[HAUTEUR][LARGEUR];
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

void progresser(int posX[TAILLE], int posY[TAILLE],char direction,char plateau[HAUTEUR][LARGEUR],bool *peutAvancer){
	/*!
	\brief Fonction qui incrémente les abscisses de chaque cellule du sepent,
	 dans le tableau posX, afin de le déplacer d'un cran
	\param posX Le tableau de l'abscisse de chaque cellule du serpent, dans l'ordre
	\param posY Le tableau de l'ordonnée de chaque cellule du serpent, dans l'ordre
	*/
	for(int i = TAILLE-1; i >= 1; i--){
			posX[i] = posX[i - 1];
			posY[i] = posY[i - 1];
		}
	if (direction == HAUT) {
		posY[0]--;
		if (plateau[posY[0]-1][posX[0]-1] != VIDE){
			*peutAvancer =  false;
		}	
	}
	if (direction == BAS) {
		posY[0]++;
		if (plateau[posY[0]-1][posX[0]-1] != VIDE){
			*peutAvancer =  false;
		}	
	}
	if (direction == GAUCHE) {
		posX[0]--;
		if (plateau[posY[0]-1][posX[0]-1] != VIDE){
			*peutAvancer =  false;
		}		
	}
	else if (direction == DROITE) {
		posX[0]++;
		if (plateau[posY[0]-1][posX[0]-1] != VIDE){
			*peutAvancer =  false;
		}		
	}
	if (verifSeTouche(posX,posY)){
		*peutAvancer =  false;
	}
	}

void effacerEcran() {
	/*!
	\brief Fonction pour effacer l'écran
	*/
    system("clear"); // Nettoie le terminal
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