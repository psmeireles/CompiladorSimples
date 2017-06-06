#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compila.h"

int main (void) {
  funcp f;
  FILE *myfp;
  int a;

	if ((myfp = fopen ("desvio_p4", "r")) == NULL) {
    	perror ("nao conseguiu abrir arquivo!");
    	exit(1);
	}

	f = compila(myfp);

	a = (*f)(3,4,5,6);
	printf("%d", a);


  fclose(myfp);
  return 0;
}
