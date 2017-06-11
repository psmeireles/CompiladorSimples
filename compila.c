#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <compila.h>

typedef int (*funcp) ();

static unsigned char cod_ret[] = { 0x8b, 0x45, 0xfc, 0xc9, 0xc3 };							/* ret */
static unsigned char cod_pilha[] = { 0x55, 0x48, 0x89, 0xe5 };		/* pushq %rbp   movq %rsp, %rbp */
static unsigned char cod_sub_rsp[] = { 0x48, 0x83, 0xec, 0x10 };		/* subq $16, %rsp */

void gera_cod_ret(FILE *f, unsigned char *codigo){
	char c0;
    fscanf(f, "et%c", &c0);
	strcat((char *)codigo, (char *)cod_ret);
}

unsigned char * faz_operacao(char op, char var, int inx){ // Recebe do operador e o segundo termo da atribuicao
	unsigned char codigo[6];
	switch(op) {
		case '+': { //Adicao

			if(var = 'p'){ // Parametro
				codigo[0] = 0x01;
				if(inx = 1) //p1
					codigo[1] = 0xfa;
				else		//p2
					codigo[1] = 0xf2;
			}
			else if(var = 'v'){ // Variavel local
				codigo[0] = 0x03;
				codigo[1] = 0x55;
				codigo[2] = 0xff + 1 - 4*(unsigned char)inx;
			}
			else{ // Constante
				if(inx < 256){
					codigo[0] = 0x83;
					codigo[1] = 0xc2;
					codigo[2] = (unsigned char) inx;
				}
				else{
					codigo[0] = 0x81;
					codigo[1] = 0xc2;
					codigo[2] = (unsigned char) (inx & 0xff);
					codigo[3] = (unsigned char) ((inx >> 8) & 0xff);
					codigo[4] = (unsigned char) ((inx >> 16) & 0xff);
					codigo[5] = (unsigned char) ((inx >> 24) & 0xff);
				}
			}
			return codigo;
		}
		case '-': {
			
			if(var = 'p'){ // Parametro
				codigo[0] = 0x29;
				if(inx = 1) //p1
					codigo[1] = 0xfa;
				else		//p2
					codigo[1] = 0xf2;
			}
			else if(var = 'v'){ // Variavel local
				codigo[0] = 0x2b;
				codigo[1] = 0x55;
				codigo[2] = 0xff + 1 - 4*(unsigned char)inx;
			}
			else{ // Constante
				if(inx < 255){
					codigo[0] = 0x83;
					codigo[1] = 0xea;
					codigo[2] = (char) inx;
				}
				else{
					codigo[0] = 0x81;
					codigo[1] = 0xea;
					codigo[2] = (unsigned char) (inx & 0xff);
					codigo[3] = (unsigned char) ((inx >> 8) & 0xff);
					codigo[4] = (unsigned char) ((inx >> 16) & 0xff);
					codigo[5] = (unsigned char) ((inx >> 24) & 0xff);
				}
			}
			return codigo;
		}
		case '*': {
			
			if(var = 'p'){ // Parametro
				codigo[0] = 0x0f;
				codigo[1] = 0xaf;
				if(inx = 1) //p1
					codigo[2] = 0xd7;
				else		//p2
					codigo[2] = 0xd6;
			}
			else if(var = 'v'){ // Variavel local
				codigo[0] = 0x0f;
				codigo[1] = 0xaf;
				codigo[2] = 0x55;
				codigo[3] = 0xff + 1 - 4*(unsigned char)inx;
			}
			else{ // Constante
				if(inx < 128){
					codigo[0] = 0x6b;
					codigo[1] = 0xd2;
					codigo[2] = (char) inx;
				}
				else{
					codigo[0] = 0x69;
					codigo[1] = 0xd2;
					codigo[2] = (inx & 0xff);
					codigo[3] = (unsigned char) ((inx >> 8) & 0xff);
					codigo[4] = (unsigned char) ((inx >> 16) & 0xff);
					codigo[5] = (unsigned char) ((inx >> 24) & 0xff);
				}
			}
			return codigo;
		}
	}
}

unsigned char * move_lugar_certo(char var, int inx){	// Recebe o termo que recebera a atribuicao
	unsigned char codigo[3];
	codigo[0] = 0x89;
	if(var == 'p'){ //Atribuicao em parametro
		if(inx == 1) //p1
			codigo[1] = 0xd7; 
		else //p2
			codigo[1] = 0xd6;
	}
	else{ //Atribuicao em variavel local
		codigo[1] = 0x55;
		codigo[2] = 0xff + 1 - 4*(unsigned char)inx;
	}
	return codigo;
}

unsigned char * gera_cod_atribuicao(FILE *f, char c){
	int idx0, idx1, idx2;
    char var0 = c, var1, var2, op;
	unsigned char cod_atribuicao[14];	// 14 eh o tamanho maximo que o codigo de atribuicao assume

    fscanf(f, "%d = %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2);

	switch (var1){ // Checa primeiro termo da atribuicao
		case 'p': { //Parametro
			// Mover parametro pro %edx
			cod_atribuicao[0] = 0x89;
			if(idx1 == 1)	
				cod_atribuicao[1] = 0xfa;
			else
				cod_atribuicao[1] = 0xf2;

			break;
		}
		case 'v': { //Variavel local
			// Mover variavel pro %edx
			cod_atribuicao[0] = 0x8b;
			cod_atribuicao[1] = 0x55;
			cod_atribuicao[2] = 0xff + 1 - 4*(unsigned char)idx1;

			break;
		}
		case '$': { //Constante
			// Mover constante pro %edx
			cod_atribuicao[0] = 0xba;
			cod_atribuicao[1] = idx1 & 0xff;
			cod_atribuicao[2] = (unsigned char) ((idx1 >> 8) & 0xff);
			cod_atribuicao[3] = (unsigned char) ((idx1 >> 16) & 0xff);
			cod_atribuicao[4] = (unsigned char) ((idx1 >> 24) & 0xff);

			break;
		}
	}

	//Fazendo a operacao no %edx
	strcat( (char *) cod_atribuicao, (char *) faz_operacao(op, var2, idx2) );

	//Movendo pro lugar certo
	strcat( (char *) cod_atribuicao, (char *) move_lugar_certo(var0, idx0) ); 
			
	return cod_atribuicao;
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

funcp compila (FILE *f){
	unsigned char *codigo;
	int line = 1;
    int  c;

	while ((c = fgetc(f)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        gera_cod_ret(f, codigo);
        break;
      }
      case 'v': 
      case 'p': {  /* atribuicao */
		strcat( (char *) codigo, (char *) gera_cod_atribuicao(f, c) );
        break;
      }
      case 'i': { /* desvio */
        char var0;
        int idx0, num;
        if (fscanf(f, "f %c%d %d", &var0, &idx0, &num) != 3)
            error("Comando Invalido", line);
          printf("if %c%d %d\n", var0, idx0, num);
        break;
      }
      default: error("Comando Desconhecido", line);
    }
    line ++;
    fscanf(f, " ");
  }
  return 0;
}
