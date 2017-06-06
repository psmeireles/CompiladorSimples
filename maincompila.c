#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compila.h"

int main (void) {
  funcp f, f2;
  FILE *myfp, *myfp2;
  int a;

	if ((myfp = fopen ("squad", "r")) == NULL) {
    	perror ("nao conseguiu abrir arquivo!");
    	exit(1);
	}
	if ((myfp2 = fopen ("testeOperacoes", "r")) == NULL) {
    	perror ("nao conseguiu abrir arquivo!");
    	exit(1);
	}

	f = compila(myfp);
	f2 = compila(myfp2);
	a = (*f)(5);
	printf("\n %d \n", a);
	a = (*f2)(6,4);
	printf("\n %d \n", a);

  fclose(myfp);
  return 0;
}
/*1932053504*/
/*1278945280*/
/*2004310016*/
