#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "compila.h"

void dump(void *p, int n) {
  unsigned char *p1 = (unsigned char *)p;
  while (n--) {
    printf("%p - %02x\n",p1,*p1);
    p1++;
  }
}

//TODO: [GABRIELLE] trocar o retorno das funcoes que retornam uma alocação de memoria local para void (passando endereço de codigo como parametro) ou retornando e depois dando free no espaço alocado

typedef int (*funcp) ();
//[GABRIELLE] ta dando warning pq essas variaveis nao estao sendo usadas
static unsigned char cod_ret[5] = { 0x8b, 0x45, 0xfc, 0xc9, 0xc3 };							/* ret */
static unsigned char cod_pilha[4] = { 0x55, 0x48, 0x89, 0xe5 };		/* pushq %rbp   movq %rsp, %rbp */
static unsigned char cod_sub_rsp[4] = { 0x48, 0x83, 0xec, 0x10 };		/* subq $16, %rsp */

void gera_cod_ret(FILE *f, unsigned char *codigo, int *pos){
	char c0;
    fscanf(f, "et%c", &c0);
	memcpy((unsigned char*)(codigo + *pos - 1), cod_ret, 5);
	*pos += 5;
}

void faz_operacao(unsigned char * codigo_in, int *pos, char op, char var, int inx) { // Recebe do operador e o segundo termo da atribuicao
	unsigned char codigo[6];
	int size = 0; //tamanho da instrucao, que varia se for parametro, variavel local ou constante
	switch(op) {
		case '+': { //Adicao

			if(var == 'p') { // Parametro
				codigo[0] = 0x01;
				if(inx == 1) //p1
					codigo[1] = 0xfa;
				else		//p2
					codigo[1] = 0xf2;
				size = 2;
			}
			else if(var == 'v') { // Variavel local
				codigo[0] = 0x03;
				codigo[1] = 0x55;
				codigo[2] = 0xff + 1 - 4*(unsigned char)inx;
				size = 3;
			}
			else { // Constante
				if(inx < 256) { //[GABRIELLE] nao precisa fazer manipulacao dos bits pra colocar os valores em little endian?
					codigo[0] = 0x83;
					codigo[1] = 0xc2;
					codigo[2] = (unsigned char) inx;
					size = 3;
				}
				else {
					codigo[0] = 0x81;
					codigo[1] = 0xc2;
					codigo[2] = (unsigned char) (inx & 0xff);
					codigo[3] = (unsigned char) ((inx >> 8) & 0xff);
					codigo[4] = (unsigned char) ((inx >> 16) & 0xff);
					codigo[5] = (unsigned char) ((inx >> 24) & 0xff);
					size = 6;
				}
			}
			break;
		}
		case '-': {
			
			if(var == 'p') { // Parametro
				codigo[0] = 0x29;
				if(inx == 1) //p1
					codigo[1] = 0xfa;
				else		//p2
					codigo[1] = 0xf2;
				size = 2;
			}
			else if(var == 'v') { // Variavel local
				codigo[0] = 0x2b;
				codigo[1] = 0x55;
				codigo[2] = 0xff + 1 - 4*(unsigned char)inx;
				size = 3;
			}
			else { // Constante
				if(inx < 255) { //[GABRIELLE] nao precisa fazer manipulacao dos bits pra colocar os valores em little endian?
					codigo[0] = 0x83;
					codigo[1] = 0xea;
					codigo[2] = (char) inx;
					size = 3;
				}
				else {
					codigo[0] = 0x81;
					codigo[1] = 0xea;
					codigo[2] = (unsigned char) (inx & 0xff);
					codigo[3] = (unsigned char) ((inx >> 8) & 0xff);
					codigo[4] = (unsigned char) ((inx >> 16) & 0xff);
					codigo[5] = (unsigned char) ((inx >> 24) & 0xff);
					size = 6;
				}
			}
			break;
		}
		case '*': {
			
			if(var == 'p') { // Parametro
				codigo[0] = 0x0f;
				codigo[1] = 0xaf;
				if(inx == 1) //p1
					codigo[2] = 0xd7;
				else		//p2
					codigo[2] = 0xd6;
				size = 3;
			}
			else if(var == 'v') { // Variavel local
				codigo[0] = 0x0f;
				codigo[1] = 0xaf;
				codigo[2] = 0x55;
				codigo[3] = 0xff + 1 - 4*(unsigned char)inx;
				size = 4;
			}
			else { // Constante
				if(inx < 128) { //[GABRIELLE] nao precisa fazer manipulacao dos bits pra colocar os valores em little endian?
					codigo[0] = 0x6b;
					codigo[1] = 0xd2;
					codigo[2] = (char) inx;
					size = 3;
				}
				else {
					codigo[0] = 0x69;
					codigo[1] = 0xd2;
					codigo[2] = (inx & 0xff);
					codigo[3] = (unsigned char) ((inx >> 8) & 0xff);
					codigo[4] = (unsigned char) ((inx >> 16) & 0xff);
					codigo[5] = (unsigned char) ((inx >> 24) & 0xff);
					size = 6;
				}
			}
			break;
		}
	}

	memcpy((unsigned char*)(codigo_in + *pos - 1), (unsigned char*)codigo, size); //copia a nova instrucao para o vetor de codigo
	*pos += size; //atualiza posicao final do vetor de codigo

	//return codigo;
}

void move_lugar_certo(unsigned char* codigo_in, int *pos, char var, int inx){	// Recebe o termo que recebera a atribuicao
	unsigned char codigo[3];
	int size = 0;
	codigo[0] = 0x89;
	if(var == 'p') { //Atribuicao em parametro
		if(inx == 1) //p1
			codigo[1] = 0xd7; 
		else //p2
			codigo[1] = 0xd6;
		size = 2;
	}
	else { //Atribuicao em variavel local
		codigo[1] = 0x55;
		codigo[2] = 0xff + 1 - 4*(unsigned char)inx;
		size = 3;
	}

	memcpy((unsigned char*)(codigo_in + *pos - 1), (unsigned char*)codigo, size);
	*pos += size; //atualiza a posicao final do vetor de codigo
}

void gera_cod_atribuicao(FILE *f, unsigned char *codigo, int *pos, char c){
	int idx0, idx1, idx2, size = 0;
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
			size = 2;
			break;
		}
		case 'v': { //Variavel local
			// Mover variavel pro %edx
			cod_atribuicao[0] = 0x8b;
			cod_atribuicao[1] = 0x55;
			cod_atribuicao[2] = 0xff + 1 - 4*(unsigned char)idx1;
			size = 3;
			break;
		}
		case '$': { //Constante
			// Mover constante pro %edx
			cod_atribuicao[0] = 0xba;


			cod_atribuicao[1] = idx1 & 0xff;
			cod_atribuicao[2] = (unsigned char) ((idx1 >> 8) & 0xff);
			cod_atribuicao[3] = (unsigned char) ((idx1 >> 16) & 0xff);
			cod_atribuicao[4] = (unsigned char) ((idx1 >> 24) & 0xff);
			size = 5;
			break;
		}
	}
	memcpy((unsigned char*)(codigo+ *pos - 1), cod_atribuicao, size);
	*pos += size; //atualiza a posicao final do vetor de codigo

	//Fazendo a operacao no %edx
	faz_operacao(codigo, pos, op, var2, idx2);


	//Movendo pro lugar certo
	move_lugar_certo(codigo, pos, var0, idx0);
			

}

void gera_cod_desvio (FILE *f, unsigned char * codigo, int * pos, int *idxJmp, long *endPosJmp, int *endDestino, int *posJmp){
	char varp;
    int idx, linha, size = 0;
    unsigned char codigo_desvio[12]; //tamanho maximo do codigo de desvio

	fscanf(f, "f %c%d %d", &varp, &idx, &linha);

	switch (varp){ // Checa primeiro termo da atribuicao
		case 'p': {
			codigo_desvio[0] = 0x83;
			codigo_desvio[2] = 0x00;
			if (idx == 1) 
				codigo_desvio[1] = 0xff; 
			else //idx == 2
				codigo_desvio[1] = 0xfe;
			size = 3;
			break;
		}
		case 'v': {
			codigo_desvio[0] = 0x8b;
			codigo_desvio[1] = 0x55;
			codigo_desvio[2] = 0xff + 1 - 4*(unsigned char)idx;
			
			codigo_desvio[3] = 0x83;
			codigo_desvio[4] = 0xfa;
			codigo_desvio[5] = 0x00;

			size = 6;
			break;
		}
	}

	codigo_desvio[size] = 0x0f;
	codigo_desvio[size + 1] = 0x85;
	size += 2;

	memcpy((unsigned char*)(codigo+ *pos - 1), codigo_desvio, size);
	*pos += size; //atualiza a posicao final do vetor de codigo
	
	idxJmp[*posJmp] = *pos; //grava a posicao do primeiro byte do codigo que será preenchido com a diferenca dos enderecos
	*pos += 4; //pula os quatro bytes seguintes
	endPosJmp[*posJmp] = (long)&codigo[*pos]; //guarda o endereço da instrução logo depois da instrução de desvio
	endDestino[*posJmp] = linha; //guarda numero da linha para onde o jmp vai
	(*posJmp)++;


}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

funcp compila (FILE *f){
	unsigned char *codigo = (unsigned char*)malloc(sizeof(char)*500);
	int linha = 1, pos = 0, i, aux, endDestino[500], idxJmp[500], countJmp = 0;
	char c;
	long endLinhas[500], endPosJmp[500], endDif;
/* 
	endLinhas = array que armazena o endereco de cada uma das linhas do arquivo
	endJmp = array que armazena o endereco da posicao do codigo que precisa preencher com o endereco do jump
	indexJmp = array que armazena o endereco da posicao do codigo onde esta a primeira instrucao pos jump
*/

//Inicializando o vetor
    for(i = 0; i < 500; i++){
    	codigo[i] = 0;
    	/*
    	idxJmp[i] = 0; //nao sei se precisa inicializar com zeros
    	endPosJmp = 0; //nao sei se precisa inicializar com zeros
    	endDestino[i] = 0; //nao sei se precisa inicializar com zeros
    	*/
    }
	
    //TODO: inserir aqui o codigo inicial (pushq %rbp etc)
    memcpy(codigo, cod_pilha, 4); //copia instrucao de ativacao para o vetor de codigo
	memcpy((unsigned char*)(codigo + 4), cod_sub_rsp, 4); //copia instrucao de alocacao de espaço para o vetor de codigo
	pos += 9; //atualiza posicao final do vetor de codigo


	while ((c = fgetc(f)) != EOF) {
		endLinhas[linha - 1] = (long)(&codigo[pos]); //verificar se é a posicao atual ou a posicao seguinte que precisa ser guardada
    	
    	printf("linha atual %d  %d  %x \n", linha, pos, &codigo[pos]);

	    switch (c) {
	      case 'r': { /* retorno */
	      	printf("ENTROU NO RET\n");
	        gera_cod_ret(f, codigo, &pos);
	        break;
	      }
	      case 'v': 
	      case 'p': {  /* atribuicao */
	      	printf("ENTROU NA ATRIBUICAO \n");
			gera_cod_atribuicao(f, codigo, &pos, c);
	        break;
	      }
	      case 'i': { /* desvio */
	      printf("ENTROU NO DESVIO \n");
	        gera_cod_desvio (f, codigo, &pos, idxJmp, endPosJmp, endDestino, &countJmp);
	        break;
	      }
	      default: error("Comando Desconhecido", linha);
	    }
	    linha ++;
	   	fscanf(f, " ");
	}

  //DUMP CODIGO
  dump(codigo, pos-1);

//preenche o vetor com os endereços q estao faltando
	for (i=0; i<countJmp; i++){
		linha = endDestino[i]; //pega a linha pra qual sera feito o desvio do jmp
		aux = endLinhas[linha - 1]; //descobre qual o endereco onde comeca a linha da instrucao de destino
		endDif = aux - endPosJmp[i]; //calcula a diferenca do endereco de destino e endereco da instrucao apos o jmp
		aux = idxJmp[i]; //pega a posicao do codigo q sera preenchida com endLinha
		codigo[aux] = (unsigned char)endDif; //armazena o 1o byte do dif
		codigo[aux+1] = (unsigned char)(endDif>>8); //armazena o 2o byte
		codigo[aux+2] = (unsigned char)(endDif>>16); //armazena o 3o byte
		codigo[aux+3] = (unsigned char)(endDif>>24); //armazena o 4o byte
	}

  return (funcp) codigo;
}
