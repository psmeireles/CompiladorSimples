#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compila.h"

int main (void) {
  funcp f;
  FILE *myfp;
  int a;

	if ((myfp = fopen ("desvio_v3v4", "r")) == NULL) {
    	perror ("nao conseguiu abrir arquivo!");
    	exit(1);
	}

	f = compila(myfp);

	a = (*f)();
	printf("%d", a);


  fclose(myfp);
  return 0;
}
/*1932053504*/
/*1278945280*/
/*2004310016*/
