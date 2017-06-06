#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compila.h"

int main (void) {
  funcp f;
  FILE *myfp;
  int a;

	if ((myfp = fopen ("squad", "r")) == NULL) {
    	perror ("nao conseguiu abrir arquivo!");
    	exit(1);
	}

	f = compila(myfp);

	a = (*f)(10);
	printf("\n %d \n", a);


  fclose(myfp);
  return 0;
}
/*1932053504*/
/*1278945280*/
/*2004310016*/
