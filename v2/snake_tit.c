#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

const int N=10, TEMPORISATION=100000; // taille maximum du serpent | temps d'attente entre chaque tour de boucle //
const char ARRET='a', ANNEAUX='X', TETE='O', HAUT='z', BAS='s', DROITE='d', GAUCHE='q'; //condition d'arret | Anneaux et Tete du serpent | directions du serpent //


int kbhit(){
   
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


// Procédure permetant d'aller a une position X Y dans un terminal   //
void gotoXY(int x, int y) { 
    printf("\033[%d;%df", y, x);
}

// Procédure permetant d'afficher un caractere aux positions X Y //
void afficher(int x, int y, char c){
   gotoXY(x, y);
   printf("%c", c);
}

// Procédure permetant d'effacer un caractere aux positions X Y //
void effacer(int x, int y){
   gotoXY(x, y);
   printf(" ");
}

// Procédure permetant de  dessiner le serpent en entier //
void dessinerSerpent(int lesX[], int lesY[]){
   int c;
   for(c=1;c < N;c++){
      afficher(lesX[c], lesY[c], ANNEAUX);
   }

   afficher(lesX[0], lesY[0], TETE);
}


// Procédure permetant d'iterer la position en X du serpent en fonction de sa direction//

void progresser(int lesX[], int lesY[], char direction){
   int c;
   if(direction == HAUT){
      for(c=N-1;c > 0;c--){
         lesY[c] = lesY[c-1];
         lesX[c] = lesX[c-1];
      }
      lesY[0] = lesY[0] - 1;
   }
   if(direction == BAS){
      for(c=N-1;c > 0;c--){
         lesY[c] = lesY[c-1];
         lesX[c] = lesX[c-1];
      }
      lesY[0] = lesY[0] + 1;
      
   }
   if(direction == DROITE){
      for(c=N-1;c > 0;c--){
         lesY[c] = lesY[c-1];
         lesX[c] = lesX[c-1];
      }
      lesX[0] = lesX[0] + 1;
      
    }
   if(direction == GAUCHE){
      for(c=N-1;c > 0;c--){
         lesY[c] = lesY[c-1];
         lesX[c] = lesX[c-1];
      }
      lesX[0] = lesX[0] - 1;
      
   }
}

// Procédure qui permet de supprimer les inputs //

void disableEcho() {    
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Procédure qui permet de réactiver les inputs //

void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}



int main(){
   // déclaration des variables //

   int xStart=20, yStart=20, c;     // positions de départ du serpent | compteur//

   int lesX[N], lesY[N];      // tableaux repertoriant les positions du serpent //

   char taper = DROITE, Direction= DROITE; // touche tapé | direction du serpent //


   // Valeurs de depart de la tete du serpent //
   
   lesX[0] = xStart;
   lesY[0] = yStart;
   

   // Initialise la position de toutes les parties , tete et queue //

   for(c=1;c < N;c++){
      lesX[c] = lesX[c-1] - 1;
      lesY[c] = lesY[c-1];
   }
   system("clear");

   //for(c=0;c < N;c++){                                    |   Permet d'afficher de maniere claire les valeurs des tableaux
   //   printf("%d | %d\n", lesX[c], lesY[c]);              |   repertoriant les coordonnées du serpent, a laisser
   //}                                                      |   en commentaire car c'est utile seullement pour une verification


   // Affichage et déplacement du serpent //
   dessinerSerpent(lesX, lesY); 
   while(taper!=ARRET){
      effacer(lesX[N-1], lesY[N-1]);          // efface le dernier X de la queue en debut de boucle //
      progresser(lesX, lesY, Direction);      // augmente les coordonées du serpent                 //
      dessinerSerpent(lesX, lesY);            // dessine le serpent avec ses nouvelles coordonées   //
      if(kbhit()==1){                         // test si la touche "a" est appuyé                   //
         taper = getchar();
      }
      if(taper == GAUCHE && Direction != DROITE){    // évite au serpent de faire demi-tour //
         Direction = taper;
      }
      if(taper == DROITE && Direction != GAUCHE){
         Direction = taper;
      }
      if(taper == BAS && Direction!= HAUT){
         Direction = taper;
      }
      if(taper == HAUT && Direction!= BAS){
         Direction = taper;
      }
      if(lesX[0] == 0 || lesY[0] == 0){
         for(c=1;c < N;c++){
            effacer(lesX[N-c], lesY[N-c]); // Peut etre un bug //
            usleep(TEMPORISATION);
         }
         taper=ARRET;
      }
      disableEcho();
      usleep(TEMPORISATION);          // met le programme en pause  //
   }


   return EXIT_SUCCESS;
}




