#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "compila.h"

int main (void) {
  funcp f;
  FILE *myfp;

	if ((myfp = fopen ("programa", "r")) == NULL) {
    	perror ("nao conseguiu abrir arquivo!");
    	exit(1);
	}

	f = compila(myfp);
 
	//ret = (*f)();



  fclose(myfp);
  return 0;
}


