/*!
 \file
 \page Général 
 
 \author GOURDON Gabriel
 \version 1.0
 \date 22 octobre 2024
 \brief Un snake pour la SAE 1.01
**/

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Déclaration des constantes
/**
 * \def TAILLE_TABLEAU
 * \brief La taille du tableau dans lequel le serpent va évoluer
 */
#define TAILLE_TABLEAU 40 

/**
 * \def TAILLE_SERPENT
 * \brief La taille du serpent
 * Elle ne changera pas pour le moment
 */
#define TAILLE_SERPENT 10

#define VITESSE_JEU 200000

// Déclaration des fonctions fournies
void gotoXY(int x, int y);
int checkAKeyPress();

// Déclaration des fonctions demandées
void genererSerpent(int positionsX[TAILLE_SERPENT], int positionsY[TAILLE_SERPENT], int x, int y); // Check
void afficher(int x, int y, char c); // Check
void effacer(int x, int y); // Check
void dessinerSerpent(int positionsX[TAILLE_SERPENT], int positionsY[TAILLE_SERPENT]); // Check
void progresser(int positionsX[TAILLE_SERPENT], int positionsY[TAILLE_SERPENT]); // Check
void effacerEcran(); // Check


/**
* \fn int main()
* \brief Programme principal.
* \return Code de sortie du programme (0 : sortie normale).
* 
* Le programme principal exécute le code donné
*/
int main()
{
 int positionsX[TAILLE_SERPENT];
 int positionsY[TAILLE_SERPENT];
 bool devraitQuitter = false;
 int x, y;
 bool valValides = false;
	
 printf("Entrez la position initiale de la tête du serpent (x, puis y): ");
 // Demander à l'utilisateur de rentrer la position initiale
 while(!valValides)
 {
  scanf("%d", &x);
  scanf("%d", &y);
  if(x < 0)
  {
   printf("Le X ne peut pas être en dessous de 0 ! Rentrer de nouvelles valeurs : ");
  }
  else if(x > 40)
  {
   printf("X ne peut pas dépasser 40 ! Rentrer de nouvelles valeurs : ");
  }
  else if(y < 0)
  {
   printf("Le Y ne peut pas être en dessous de 0 ! Rentrer de nouvelles valeurs : ");
  }
  else if(y > 40)
  {
   printf("Le Y ne peut pas dépasser 40 ! Rentrer de nouvelles valeurs : ");
  }
  else
  {
   valValides = true;
  }
 }

 effacerEcran(); // Préparer l'écran
 genererSerpent(positionsX, positionsY, x, y);

 dessinerSerpent(positionsX, positionsY);
	
 while(!devraitQuitter) // Boucle du jeu, tester la touche d'arrêt, sinon, continuer
 {
  usleep(VITESSE_JEU);
  effacerEcran();
  if(checkAKeyPress())
  {
   devraitQuitter = true;
  }
  progresser(positionsX, positionsY);
  dessinerSerpent(positionsX, positionsY);
 }
 printf("\n");
    return 0; 
}

// Définition des fonctions fournies
void gotoXY(int x, int y) { 
    printf("\033[%d;%df", y, x);
}
int checkAKeyPress(){
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
 
 if(ch == 'A' || ch == 'a'){
  ungetc(ch, stdin);
  unCaractere=1;
 } 
 return unCaractere;
}

// Definition des fonctions demandées
void afficher(int x, int y, char c)
{
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y)
{
    gotoXY(x, y);
    printf(" ");
}

void effacerEcran()
{
    system("clear");
}

/*!
 \fn void genererSerpent(int positions[TAILLE_SERPENT][2], int x, int y)
 \brief La fonction qui génère le serpent
 \param positions La liste des positions du serpent
 \param x La position X de la tête du serpent
 \param y La position Y de la tête du serpent
 Cette fonction créé la liste des positions du serpent dans la liste en argument positions
*/

void genererSerpent(int positionsX[TAILLE_SERPENT], int positionsY[TAILLE_SERPENT], int x, int y)
{
 for(int nbCellule = 0; nbCellule < TAILLE_SERPENT; nbCellule++) // Génerer des coordonées de x à (x + TAILLE_SERPENT) pour le serpent
 {
  positionsX[nbCellule] = x - nbCellule;
  positionsY[nbCellule] = y;
 }
}

void dessinerSerpent(int positionsX[TAILLE_SERPENT], int positionsY[TAILLE_SERPENT])
{
 for(int iDessine = 0; iDessine < TAILLE_SERPENT; iDessine++) // Dessine le serpent dans l'ordre des cellules, un X pour le corps, un O pour la tête
 {
  int aDessinerX = positionsX[iDessine];
  int aDessinerY = positionsY[iDessine];

  if(iDessine == 0)
  {
   afficher(aDessinerX, aDessinerY, 'O');
  }
  else
  {
   afficher(aDessinerX, aDessinerY, 'X');
  }
 }
}

void progresser(int positionsX[TAILLE_SERPENT], int positionsY[TAILLE_SERPENT])
{
 for(int i = 0; i < TAILLE_SERPENT; i++) // Avancer chacune des cellules du serpent
 {
  positionsX[i]++;
 }
}