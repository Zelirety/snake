/**
* @file snake.c
* @brief Jeu du snake dans le terminal
* @author LE HEBEL Nathan 
* @version 1.0
* @date 25/10/2024
*
* Dans cette première version, un serpent est généré aux coordonées entrées par l'utilisateur et se déplace
* vers la droite jusqu'à l'intervention de l'utilisateur
* L'objectif final est un snake complétement fonctionnel
*
*/

// Déclaration des fichiers inclus
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>

// Déclaration des constantes
#define TAILLE 10  // Taille du serpent
#define USLEEP 200000  // Temps de pause en ms entre chaque itération de la boucle principale

// Déclaration des fonctions 
int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int positionsX[TAILLE], int positionsY[TAILLE]);
void progresser(int positionsX[TAILLE], int positionsY[TAILLE]);
void effacerEcran();

int main(){
    int posX[TAILLE];
 	int posY[TAILLE];
	int x;
	int y;
	bool xyCorrect = false;
	bool doitQuitter = false;
	
	effacerEcran(); // Préparer l'écran
	printf("Entrez la position initiale de la tête du serpent \n");

	while(!xyCorrect){
		printf("X : ");
		scanf("%d", &x);
		printf("Y : ");
		scanf("%d", &y);	

		/* Verfication des exceptions */
		if(x < 0)
		{
			printf("X doit être un entier positif ! \n");
		}
		else if(x == 0)
		{
			printf("X ne doit pas être égal à 0 ! \n");
		}
		else if(x > 40)
		{
			printf("X doit être inférieur 40 ! \n");
		}
		else if(y < 0)
		{
			printf("Y doit être un entier positif ! \n");
		}
		else if(y == 0)
		{
			printf("Y ne doit pas être égal à 0 ! \n");
		}
		else if(y > 40)
		{
			printf("Y doit être inférieur 40 ! \n");
		}
		else
		{
			xyCorrect = true;
		}	
	}

	effacerEcran();
	
	/* Génerer les coordonnées initiales */
	for(int i = 0; i < TAILLE; i++) 
	{ 
		posX[i] = x - i;
		posY[i] = y;
	}

	/* Boucle principale du snake qui continue tant que la touche ESC n'a pas été pressée */
	while(!doitQuitter){ 
		usleep(USLEEP);
		for (int i = 0; i < TAILLE; i++)
		{ 	
			effacer(posX[i+1], posY[i+1]); // Efface le serpent
		}
		progresser(posX, posY);
		dessinerSerpent(posX, posY);
		
		

		if (kbhit()) //Verifie que ESc n'a pas été pressé
		{
			doitQuitter = true;
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
 
	if((ch == 27))
	{
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
		if(i == 0) 
		{
			afficher(posX[i], posY[i], 'O'); // Tete
		}
		else if (posX[i] > 0) // N'affiche pas tout le serpent si le serpent est trop a gauche
		{
			afficher(posX[i],posY[i], 'X'); // Corps
		}
		
	}
}

void progresser(int posX[TAILLE], int posY[TAILLE]){
	/*!
	\brief Fonction qui incrémente les abscisses de chaque cellule du sepent,
	 dans le tableau posX, afin de le déplacer d'un cran
	\param posX Le tableau de l'abscisse de chaque cellule du serpent, dans l'ordre
	\param posY Le tableau de l'ordonnée de chaque cellule du serpent, dans l'ordre
	*/
	for(int i = 0; i <= TAILLE; i++)
	{
		posX[i]++;
	}
}

void effacerEcran() {
	/*!
	\brief Fonction pour effacer l'écran
	*/
    system("clear"); // Nettoie le terminal
}

