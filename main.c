#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <compila.h>

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

int main (void) {
  int line = 1;
  int  c;
  FILE *myfp;

  if ((myfp = fopen ("programa", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }

  while ((c = fgetc(myfp)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        char c0;
        if (fscanf(myfp, "et%c", &c0) != 1)
          error("comando invalido", line);
        printf("ret\n");
        break;
      }
      case 'v': 
      case 'p': {  /* atribuicao */
        int idx0, idx1, idx2;
        char var0 = c, var1, var2, op;
        if (fscanf(myfp, "%d = %c%d %c %c%d", &idx0, &var1, &idx1,
                   &op, &var2, &idx2) != 6)
            error("comando invalido", line);
          printf("%c%d = %c%d %c %c%d\n",
                var0, idx0, var1, idx1, op, var2, idx2);
        break;
      }
      case 'i': { /* desvio */
        char var0;
        int idx0, num;
        if (fscanf(myfp, "f %c%d %d", &var0, &idx0, &num) != 3)
            error("comando invalido", line);
          printf("if %c%d %d\n", var0, idx0, num);
        break;
      }
      default: error("comando desconhecido", line);
    }
    line ++;
    fscanf(myfp, " ");
  }
  fclose(myfp);
  return 0;
}


