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
#define INIT_X 20
#define INIT_Y 20

/* Déclaration des fonctions */  

int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int positionsX[TAILLE], int positionsY[TAILLE]);
void progresser(int positionsX[TAILLE], int positionsY[TAILLE], char direction);
void effacerEcran();
void disableEcho();
void enableEcho();

int main(){
    int posX[TAILLE];
 	int posY[TAILLE];
	int x = INIT_X;
	int y = INIT_Y;
	bool doitQuitter = false;
	char direction = DROITE;
	char touche = DROITE;
	
	effacerEcran(); // Préparer l'écran
	
	/* Génerer les coordonnées initiales */
	for(int i = 0; i < TAILLE; i++) 
	{ 
		posX[i] = x - i;
		posY[i] = y;
	}

	disableEcho(); // Désactive l'affichage des caractères dans les terminal lorsqu'une touche est pressée

	/* Boucle principale du snake qui continue tant que la touche ESC n'a pas été pressée */
	while(!doitQuitter){ 
		fflush(stdout);
		usleep(USLEEP);
		for (int i = 0; i < TAILLE; i++)
		{ 	
			effacer(posX[i+1], posY[i+1]); // Efface le serpent
		}
		progresser(posX, posY,direction);
		dessinerSerpent(posX, posY);
		
		if (kbhit() == 1){
			touche = getchar();
			if (touche == ARRET){  // Condition d'arret
				doitQuitter = true;
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
    
    return EXIT_SUCCESS;
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
		if (posX[i] > 1 && posY[i] > 1){
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

void progresser(int posX[TAILLE], int posY[TAILLE],char direction){
	/*!
	\brief Fonction qui incrémente les abscisses de chaque cellule du sepent,
	 dans le tableau posX, afin de le déplacer d'un cran
	\param posX Le tableau de l'abscisse de chaque cellule du serpent, dans l'ordre
	\param posY Le tableau de l'ordonnée de chaque cellule du serpent, dans l'ordre
	*/
	if (direction == HAUT)
	{
		for(int i = TAILLE-1; i >= 1; i--)
		{
			posX[i] = posX[i - 1];
			posY[i] = posY[i - 1];
		}	
		posY[0]--;	
	}
	else if (direction == BAS)
	{
		for(int i = TAILLE-1; i >= 1; i--)
		{
			posX[i] = posX[i - 1];
			posY[i] = posY[i - 1];
		}	
		posY[0]++;		
	}
	else if (direction == GAUCHE)
	{
		for(int i = TAILLE-1; i >= 1; i--)
		{
			posX[i] = posX[i - 1];
			posY[i] = posY[i - 1];
		}	
		posX[0]--;		
	}
	else if (direction == DROITE)
	{
		for(int i = TAILLE-1; i >= 1; i--)
		{
			posX[i] = posX[i - 1];
			posY[i] = posY[i - 1];
		}	
		posX[0]++;	
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
