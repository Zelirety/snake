/**
 * @file snake.c
 * @brief Jeu du snake dans le terminal
 * @author LE HEBEL Nathan 
 * @version 4.0
 * @date 30/11/2024
 *
 * Dans cette quatrième version, le serpent se déplace et se téléporte en passant par des trous percés aux quatre côtés du plateau.
 * Des pommes apparaissent aléatoirement, et le serpent s'allonge en les mangeant.
 * La partie est gagnée lorsque 10 pommes sont mangées.
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
#define ARRET 'a' // Touche d'arrêt
#define POMMES_A_MANGER 10
#define ANNEAUX 'X' // Corps du serpent
#define TETE 'O' // Tête du serpent
#define HAUT 'z' // Touche pour aller en haut
#define BAS 's' // Touche pour aller en bas
#define DROITE 'd' // Touche pour aller à droite
#define GAUCHE 'q' // Touche pour aller à gauche
#define INIT_X 40 // Position initiale X
#define INIT_Y 20 // Position initiale Y
#define HAUTEUR 40 // Hauteur du plateau
#define LARGEUR 80 // Largeur du plateau
#define BORDURE '#' // Bordure du plateau
#define VIDE ' ' // Case vide du plateau
#define NB_PAVES 4 // Nombre de pavés
#define TAILLE_PAVES 5 // Taille des pavés
#define POMME '6' // Caractère représentant une pomme

/* Déclaration des fonctions */  
/**
 * @brief Initialise le plateau de jeu avec des bordures et des cases vides.
 * @param taille_serpent La taille initiale du serpent.
 * @param plateau Le plateau de jeu à initialiser.
 */
void initPlateau(int taille_serpent,char plateau[HAUTEUR][LARGEUR]);

/**
 * @brief Initialise les pavés du plateau.
 * @param taille_serpent La taille du serpent.
 * @param plateau Le plateau de jeu avec les pavés à ajouter.
 */
void initPaves(int taille_serpent,char plateau[HAUTEUR][LARGEUR]);

/**
 * @brief Dessine le plateau à l'écran.
 * @param plateau Le plateau à dessiner.
 */
void dessinerPlateau(char plateau[HAUTEUR][LARGEUR]);

/**
 * @brief Vérifie si une touche a été pressée.
 * @return 1 si une touche a été pressée, 0 sinon.
 */
int kbhit();

/**
 * @brief Déplace le curseur à une position spécifique dans le terminal.
 * @param x La position X à laquelle déplacer le curseur.
 * @param y La position Y à laquelle déplacer le curseur.
 */
void gotoXY(int x, int y);

/**
 * @brief Affiche un caractère à une position donnée dans le terminal.
 * @param x La position X où afficher le caractère.
 * @param y La position Y où afficher le caractère.
 * @param c Le caractère à afficher.
 */
void afficher(int x, int y, char c);

/**
 * @brief Efface un caractère à une position donnée dans le terminal.
 * @param x La position X à effacer.
 * @param y La position Y à effacer.
 */
void effacer(int x, int y);

/**
 * @brief Dessine le serpent à l'écran.
 * @param taille_serpent La taille du serpent.
 * @param posX Le tableau des positions X du serpent.
 * @param posY Le tableau des positions Y du serpent.
 */
void dessinerSerpent(int taille_serpent,int posX[taille_serpent], int posY[taille_serpent]);

/**
 * @brief Vérifie si le serpent se touche lui-même.
 * @param taille_serpent La taille du serpent.
 * @param posX Le tableau des positions X du serpent.
 * @param posY Le tableau des positions Y du serpent.
 * @return true si le serpent se touche lui-même, false sinon.
 */
bool verifSeTouche(int taille_serpent,int posX[taille_serpent], int posY[taille_serpent]);

/**
 * @brief Fait progresser le serpent selon la direction choisie et vérifie les collisions.
 * @param taille_serpent La taille du serpent.
 * @param posX Le tableau des positions X du serpent.
 * @param posY Le tableau des positions Y du serpent.
 * @param direction La direction actuelle du serpent.
 * @param plateau Le plateau de jeu.
 * @param peutAvancer Un pointeur vers un booléen qui indique si le serpent peut continuer à avancer.
 * @param mangeUnePomme Un pointeur vers un booléen qui indique si le serpent a mangé une pomme.
 */
void progresser(int taille_serpent,int posX[taille_serpent], int posY[taille_serpent],char direction,char plateau[HAUTEUR][LARGEUR],bool *peutAvancer, bool *mangeUnePomme);

/**
 * @brief Ajoute une pomme aléatoirement sur le plateau.
 * @param taille_serpent La taille du serpent.
 * @param plateau Le plateau de jeu.
 * @param posX Le tableau des positions X du serpent.
 * @param posY Le tableau des positions Y du serpent.
 */
void ajouterPomme(int taille_serpent,char plateau[HAUTEUR][LARGEUR], int posX[taille_serpent], int posY[taille_serpent]);

/**
 * @brief Efface l'écran du terminal.
 */
void effacerEcran();

/**
 * @brief Désactive l'écho des touches dans le terminal.
 */

void disableEcho();

/**
 * @brief Réactive l'écho des touches dans le terminal.
 */
void enableEcho();

/**
 * @brief Fonction principale du programme qui exécute la boucle de jeu du snake.
 * @return EXIT_SUCCESS si le programme s'exécute correctement.
 */
int main(){
    int taille_serpent = 10;
    float vitesse = 100000;
    int posX[10000];
    int posY[10000];
    int x = INIT_X;
    int y = INIT_Y;
    bool peutAvancer = true;
    bool mangeUnePomme = false;
    int pommesMangees = 0;
    char direction = DROITE;
    char touche = DROITE;
    char plateau[HAUTEUR][LARGEUR];
    
    effacerEcran(); // Préparer l'écran
    initPlateau(taille_serpent,plateau);
    dessinerPlateau(plateau);
    ajouterPomme(taille_serpent,plateau, posX, posY);
    
    /* Génerer les coordonnées initiales */
    for(int i = 0; i < taille_serpent; i++) { 
        posX[i] = x - i;
        posY[i] = y;
    }

    disableEcho(); // Désactive l'affichage des caractères dans le terminal lorsqu'une touche est pressée

    /* Boucle principale du snake qui continue tant que la touche ESC n'a pas été pressée */
    while(peutAvancer && pommesMangees <= POMMES_A_MANGER){ 
        fflush(stdout);
        usleep(vitesse);
        for (int i = 0; i < taille_serpent; i++) {     
            effacer(posX[i+1], posY[i+1]); // Efface le serpent
        }
        progresser(taille_serpent,posX, posY,direction,plateau,&peutAvancer,&mangeUnePomme);
        dessinerSerpent(taille_serpent,posX, posY);
        
        if (mangeUnePomme){
            pommesMangees++;
            taille_serpent++;
            vitesse /= 1.1;
            if (pommesMangees <= POMMES_A_MANGER){
                ajouterPomme(taille_serpent,plateau, posX, posY);
            }    
        }

        if (kbhit() == 1){
            touche = getchar();
            if (touche == ARRET){  // Condition d'arrêt
                peutAvancer = false;
            }
            if((touche == GAUCHE && direction != DROITE) || // Change direction si touche directionnelle pressée et touche != direction opposée
               (touche == DROITE && direction != GAUCHE) ||
               (touche == BAS && direction != HAUT) ||
               (touche == HAUT && direction != BAS)){ 
                direction = touche;
            }
        }        
    }
    enableEcho();
    gotoXY(LARGEUR/2 - 4, HAUTEUR/2);
    if (pommesMangees >= POMMES_A_MANGER){
        printf("VICTOIRE !!!!!!");
    }
    else{
        printf("DEFAITE !!!!!!");
    }
    gotoXY(1, HAUTEUR + 1);
    printf("\n");    
    return EXIT_SUCCESS;
}

void initPlateau(int taille_serpent,char plateau[HAUTEUR][LARGEUR]) {
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
	plateau[0][LARGEUR/2] = VIDE;
	plateau[HAUTEUR/2][0] = VIDE;
	plateau[HAUTEUR/2][LARGEUR-1] = VIDE;
	plateau[HAUTEUR-1][LARGEUR/2] = VIDE;

	initPaves(taille_serpent,plateau);

	// return plateau[HAUTEUR][LARGEUR];
}

void initPaves(int taille_serpent,char plateau[HAUTEUR][LARGEUR]){
    srand(time(NULL));
	int x,y;
	for (int pave_place = 0; pave_place < NB_PAVES; pave_place++) {
		x = rand() % (LARGEUR - TAILLE_PAVES - 2) + 1;
		y = rand() % (HAUTEUR - TAILLE_PAVES - 2) + 1;
		/* Le pavé ne peut pas apparaitre sur le seprent ou sur les bordures, donc on boucle jusqu'à avoir x,y corrects*/
		while ((x > (INIT_X - taille_serpent - TAILLE_PAVES) && x < INIT_X + TAILLE_PAVES && x > 2 && x < 79) || (y > (INIT_Y + TAILLE_PAVES) && y < INIT_Y && y > 2 && y < 39)){
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

void dessinerSerpent(int taille_serpent,int posX[taille_serpent], int posY[taille_serpent]){
	/*!
	\brief Fonction qui affiche entièrement le serpent dans le terminal
	\param posX Le tableau de l'abscisse de chaque cellule du serpent, dans l'ordre
	\param posY Le tableau de l'ordonnée de chaque cellule du serpent, dans l'ordre
	*/
	
	for (int i = 0; i < taille_serpent; i++){ // Affiche O pour la tete, X pour le corps, dans l'ordre des cellules et selon la taille du serpent
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

bool verifSeTouche(int taille_serpent,int posX[taille_serpent], int posY[taille_serpent]){
	for (int i = 1; i < taille_serpent; i++){
		if (posX[i] == posX[0] && posY[i] == posY[0]){
			return true;
		}
	}
	return false;
}

void progresser(int taille_serpent,int posX[taille_serpent], int posY[taille_serpent],char direction,char plateau[HAUTEUR][LARGEUR],bool *peutAvancer, bool *mangeUnePomme){
	/*!
	\brief Fonction qui incrémente les abscisses de chaque cellule du sepent,
	 dans le tableau posX, afin de le déplacer d'un cran
	\param posX Le tableau de l'abscisse de chaque cellule du serpent, dans l'ordre
	\param posY Le tableau de l'ordonnée de chaque cellule du serpent, dans l'ordre
	*/
	for(int i = taille_serpent-1; i >= 1; i--){ // Deplacement du corps du serpent
			posX[i] = posX[i - 1];
			posY[i] = posY[i - 1];
		}

	/* La tête, qui dirige le corps, se déplace, et en fonction de la direction entrée au clavier, jusqu'à rencontrer un obastacle*/
	if (direction == HAUT) {
		posY[0]--; // La tête avance
		if (plateau[posY[0]-1][posX[0]-1] == BORDURE){ // Check une possible collision
			*peutAvancer =  false;
		}	
	}
	if (direction == BAS) {
		posY[0]++;
		if (plateau[posY[0]-1][posX[0]-1] == BORDURE){
			*peutAvancer =  false;
		}	
	}
	if (direction == GAUCHE) {
		posX[0]--;
		if (plateau[posY[0]-1][posX[0]-1] == BORDURE){
			*peutAvancer =  false;
		}		
	}
	else if (direction == DROITE) {
		posX[0]++;
		if (plateau[posY[0]-1][posX[0]-1] == BORDURE){
			*peutAvancer =  false;
		}		
	}
	if (verifSeTouche(taille_serpent,posX,posY)){ // Check une possible collision avec son propre corps
		*peutAvancer =  false;
	}

	if (plateau[posY[0]-1][posX[0]-1] == POMME){ // Check une possible collision avec une pomme 
		*mangeUnePomme =  true;
	}
	else{
		*mangeUnePomme = false;
	}

	/* Le serpent se téléporte lorsqu'il rentre dans l'un des 4 trous aux milieux de chaque cotés*/
	if (posX[0]-1 == LARGEUR/2 && posY[0]-1 == 0 && direction == HAUT){
		posY[0] = HAUTEUR;  // Téléportation de la tête
		for (int j = 1; j < taille_serpent; j++){ // Réajustement du corps pour une transition plus lisse
			posY[j] -= 1;
		}
	}
	else if (posX[0]-1 == LARGEUR/2 && posY[0]-1 == HAUTEUR-1 && direction == BAS){
		posY[0] = 0;
		for (int j = 1; j < taille_serpent; j++){
			posY[j] += 1;
		}
	}
	else if (posX[0]-1 == LARGEUR-1 && posY[0]-1 == HAUTEUR/2 && direction == DROITE){
		posX[0] = 0;
		for (int j = 1; j < taille_serpent; j++){
			posX[j] += 1;
		}
	}
	else if (posX[0]-1 == 0 && posY[0]-1 == HAUTEUR/2 && direction == GAUCHE){
		posX[0] = LARGEUR;
		for (int j = 1; j < taille_serpent; j++){
			posX[j] -= 1;
		}
	}
}

void ajouterPomme(int taille_serpent,char plateau[HAUTEUR][LARGEUR], int posX[taille_serpent], int posY[taille_serpent]) {
    int x, y;
    bool positionValide = false;
    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires
    
    while (!positionValide) {
        // Générer une position aléatoire dans les limites du plateau
        x = rand() % (LARGEUR - 2) + 1; // Exclure les bordures
        y = rand() % (HAUTEUR - 2) + 1;

        // Vérifier si la position n'est pas occupée par le serpent ou un pavé
        if (plateau[y][x] == VIDE) {
            positionValide = true;

            // Vérifier que la position n'est pas occupée par le serpent
            for (int i = 0; i < taille_serpent; i++) {
                if (posX[i] == x + 1 && posY[i] == y + 1) {
                    positionValide = false;
                    break;
                }
            }
        }
    }

    // Placer la pomme sur le plateau
    plateau[y][x] = POMME;

    // Afficher la pomme à l'écran
    afficher(x + 1, y + 1, POMME); // x+1 et y+1 pour s'aligner avec les coordonnées terminales
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
