#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>


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
	else if((ch == 'z'))
	{
		ungetc(ch, stdin);
		unCaractere= 2;
	}
	else if((ch == 'q'))
	{
		ungetc(ch, stdin);
		unCaractere= 3;
	}
	else if((ch == 's'))
	{
		ungetc(ch, stdin);
		unCaractere= 4;
	}
	else if((ch == 'd'))
	{
		ungetc(ch, stdin);
		unCaractere= 6;
	}
	return unCaractere;
}

int main(){
    int i = 1;
    while (i == 1){
        printf(kbhit())
    }

    return EXIT_SUCCESS;
}