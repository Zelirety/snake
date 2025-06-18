#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// déclaration des constantes

#define HAUTEUR 40
#define LARGEUR 79

#define NB_PAVES 4
#define TAILLE_PAVE 5

#define TEMPORISATION 100000

#define TAILLE 10

#define TETE 'O'
#define CORPS 'X'

#define X 40
#define Y 20

#define STOP 'a'

#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'

#define BORDURE '#'
#define ESPACE ' '

#define POMME '6'
#define MAX_POMME 10

typedef int t_serpent[TAILLE];
typedef int t_plateau[LARGEUR][HAUTEUR];

int TailleSerpent = TAILLE;
int Tempo = TEMPORISATION; 

// Prototypes des fonctions
int kbhit();
void disableEcho();
void enableEcho();

void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);

void dessinerSerpent(t_serpent lesX, t_serpent lesY);
void progresser(t_serpent lesX, t_serpent lesY, char direction, bool *collision, t_plateau plateau, bool *PommeMangee);

void initPlateau(t_plateau plateau);
void dessinerPlateau(t_plateau plateau);

void placerPaves(t_plateau plateau, t_serpent lesX, t_serpent lesY);
void ajouterPomme(t_plateau plateau);

void tpSerpent(t_serpent lesX, t_serpent lesY);

int main()
{
    // déclaration des variables
    int x, y, indice;
    char touche = ' ', Direction = DROITE; // Direction de départ vers la droite
    t_plateau plateau;
    t_serpent lesX;
    t_serpent lesY;
    bool collision = false;
    bool PommeMangee = false;
    int CompteurPomme = 0;

    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires

    // saisie des données

    x = X;
    y = Y;

    // Initisation du serpent
    for (indice = 0; indice < TAILLE; indice++)
    { 
        lesX[indice] = x;
        x--;
        lesY[indice] = y;
    }

    system("clear");
    initPlateau(plateau);
    placerPaves(plateau, lesX, lesY);
    dessinerPlateau(plateau);
    ajouterPomme(plateau);
    disableEcho();

    while (touche != STOP)
    { // boucle principale qui continue tant que l'utilisateur n'appuie pas sur 'a'
        effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);
        progresser(lesX, lesY, Direction, &collision, plateau, &PommeMangee);
        dessinerSerpent(lesX, lesY);
        if (kbhit() == 1)
        {
            touche = getchar();
        }

        if(PommeMangee == true){     
            PommeMangee = false;                            
            Tempo = Tempo - 5000;
            CompteurPomme++;
            ajouterPomme(plateau);
        }


        if(CompteurPomme == MAX_POMME){                               
            touche=STOP;
        }


        if (touche == BAS && Direction != HAUT) // Empêche le serpent de faire demi tour sur lui-même peut import le direction
        {
            Direction = touche;
        }
        if (touche == HAUT && Direction != BAS)
        {
            Direction = touche;
        }
        if (touche == GAUCHE && Direction != DROITE)
        {
            Direction = touche;
        }
        if (touche == DROITE && Direction != GAUCHE)
        {
            Direction = touche;
        }
        if (collision == true)
        {
            touche = STOP;
        }
        usleep(Tempo);
    }
    enableEcho();
    return EXIT_SUCCESS;
}

int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere present

    int unCaractere = 0;
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

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

void disableEcho()
{
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

void enableEcho()
{
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

void gotoXY(int x, int y)
{ // fonction qui déplace le curseur
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c)
{ // fonction qui affiche un caractère à une position
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y)
{ // fonction qui efface un caractère
    gotoXY(x, y);
    printf(" ");
}

void dessinerSerpent(t_serpent lesX, t_serpent lesY)
{ // fonction qui affiche le serpent
    int indice;

    if (lesX[0] > 0 && lesY[0] > 0)
    { // Vérifie si la tête n'est pas en valeur négative sinon ne l'affiche pas
        afficher(lesX[0], lesY[0], TETE);
    }

    for (indice = 1; indice < TAILLE; indice++)
    { // Pareil pour le corps
        if (lesX[0] > 0 && lesY[0] > 0)
        {
            afficher(lesX[indice], lesY[indice], CORPS);
        }
    }
}

void progresser(t_serpent lesX, t_serpent lesY, char direction, bool *collision, t_plateau plateau, bool *PommeMangee)
{
    int indice;

    // Décalage des segments du corps
    for (indice = TAILLE - 1; indice > 0; indice--)
    {
        lesX[indice] = lesX[indice - 1];
        lesY[indice] = lesY[indice - 1];
    }

    // Mise à jour de la tête en fonction de la direction
    if (direction == HAUT)
    {
        lesY[0] = lesY[0] - 1;
    }

    else if (direction == BAS)
    {
        lesY[0] = lesY[0] + 1;
    }

    else if (direction == GAUCHE)
    {
        lesX[0] = lesX[0] - 1;
    }

    else if (direction == DROITE)
    {
        lesX[0] = lesX[0] + 1;
    }

    // Collision avec le bord
    if (plateau[lesX[0] - 1][lesY[0] - 1] == BORDURE)
    {
        *collision = true;
    }

    // Collision avec le corps
    for (indice = 1; indice < TAILLE; indice++)
    {
        if (lesX[0] == lesX[indice] && lesY[0] == lesY[indice])
        {
            *collision = true;
        }
    }

    // Gestion de la téléportation du serpent
    tpSerpent(lesX, lesY);

    // Vérifie si une pomme est mangée
    if(plateau[lesX[0]][lesY[0]] == POMME){
        *PommeMangee = true;
        TailleSerpent++;
   }
}

// Fonction qui initialise le plateau de jeu
void initPlateau(t_plateau plateau) {
    int colonne, ligne;
    for (ligne = 0; ligne < HAUTEUR; ligne++) {
        for (colonne = 0; colonne < LARGEUR; colonne++) {
            if ((ligne == 0 || ligne == HAUTEUR - 1) && (colonne != LARGEUR / 2)) {
                plateau[colonne][ligne] = BORDURE;
            } else if ((colonne == 0 || colonne == LARGEUR - 1) && (ligne != HAUTEUR / 2)) {
                plateau[colonne][ligne] = BORDURE;
            } else {
                plateau[colonne][ligne] = ESPACE;
            }
        }
    }
}

// Fonction qui affiche le plateau de jeu
void dessinerPlateau(t_plateau plateau)
{
    int colonne, ligne;

    for (ligne = 0; ligne < HAUTEUR; ligne++)
    {
        for (colonne = 0; colonne < LARGEUR; colonne++)
        {
            afficher(colonne + 1, ligne + 1, plateau[colonne][ligne]);
        }
    }
}

// Fonction qui place les paves sur le plateau de jeu
void placerPaves(t_plateau plateau, t_serpent lesX, t_serpent lesY) {
    int x, y, i, j;
    for (int p = 0; p < NB_PAVES; p++) {
        x = rand() % (LARGEUR - TAILLE_PAVE - 4) + 3;       // Calcul de la position x du coin supérieur gauche du pave
        y = rand() % (HAUTEUR - TAILLE_PAVE - 4) + 3;       // Calcul de la position x du coin supérieur gauche du pave

        if(x <= lesX[0] && x >= lesX[0] - TAILLE){          
            x = x + TAILLE + 1;
        }                                                   // On vérifie que les paves ne se chevauchent pas avec le serpent
        if(y <= lesY[0] && y >= lesY[0] - TAILLE){
            y = y + TAILLE + 1;
        }
        for (i = 0; i < TAILLE_PAVE; i++) {
            for (j = 0; j < TAILLE_PAVE; j++) {
                plateau[x + i][y + j] = BORDURE;
            }
        }
    }
}

void tpSerpent(t_serpent lesX, t_serpent lesY) {
   if(lesX[0] - 1 == LARGEUR && lesY[0] - 1 == HAUTEUR/2){
      lesX[0] = 1;         
   }

   else if(lesX[0] + 1 == 1 && lesY[0] - 1 == HAUTEUR/2){
      lesX[0] = LARGEUR;
   }

   else if(lesY[0] + 1 == HAUTEUR && lesX[0] + 1 == LARGEUR/2){
      lesY[0] = 1;
   }
      
   else if(lesY[0] - 1 == 1 && lesX[0] - 1 == LARGEUR/2){
      lesY[0] = HAUTEUR;
   }
}

void ajouterPomme(t_plateau plateau){

   int XP=0, YP=0;

   

   while(plateau[XP][YP] == BORDURE){

      plateau[XP][YP] = BORDURE;
      afficher(XP, YP, plateau[XP][YP]);

      XP = rand() % ((LARGEUR - 1) - 4) + 3 ;      

      if(XP <= X && XP >= X - TailleSerpent){
         XP = XP + TailleSerpent + 1;
      }
      if(XP >= (LARGEUR - 1)){
         XP = XP - 1;
      }

      YP = rand() % ((HAUTEUR - 1) - 4) + 3 ; 

      if(YP <= Y && YP >= Y - TailleSerpent){
         YP = YP + TailleSerpent + 1;
      }
      if(YP >= (HAUTEUR - 1)){
         YP = YP - 1;
      }
   }

   plateau[XP][YP] = POMME;
   afficher(XP, YP, plateau[XP][YP]);
}