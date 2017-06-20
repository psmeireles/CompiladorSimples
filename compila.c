/* Gabrielle Brandenburg dos Anjos	1510542		3WB
   Pedro Sousa Meireles				1510962		3WB	*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compila.h"

typedef int (*funcp) ();

typedef struct linker {
		int linhaDestino;	
		int idxJmp;
		long endPosJmp;
		long endLinhas;
} Linker;

static unsigned char cod_ret[5] = { 0x8b, 0x45, 0xfc, 0xc9, 0xc3 };	// movl -4(%rbp), %eax	leave	ret 
static unsigned char cod_pilha[4] = { 0x55, 0x48, 0x89, 0xe5 };		// pushq %rbp	movq %rsp, %rbp 
static unsigned char cod_sub_rsp[4] = { 0x48, 0x83, 0xec, 0x10 };	// subq $16, %rsp
static unsigned char *codigo;
static Linker *link;

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

void gera_cod_ret(FILE *f, int *pos, int line){
	char c0;

    if (fscanf(f, "et%c", &c0) != 1) {
         error("comando invalido", line);
        return;
    }

	memcpy((unsigned char*)(codigo + *pos), cod_ret, 5);
	*pos += 5;
}

void faz_operacao(int *pos, char op, char var, int inx) { // Recebe o operador e o segundo termo da atribuicao
	unsigned char instrucao[6]; // 6 eh o tamanho maximo que a instrucao assume
	int size = 0; // Tamanho da instrucao, que varia se for parametro, variavel local ou constante
	switch(op) {
		case '+': { // Adicao

			if(var == 'p') { // Parametro
				// addl %edi ou %esi, %edx
				instrucao[0] = 0x01;
				if(inx == 1) // p1 (%edi)
					instrucao[1] = 0xfa;
				else		 // p2 (%esi)
					instrucao[1] = 0xf2;

				size = 2;
			}
			else if(var == 'v') { // Variavel local
				// addl -x(%rbp), %edx
				instrucao[0] = 0x03;
				instrucao[1] = 0x55;
				instrucao[2] = 0xff + 1 - 4*(unsigned char)inx;

				size = 3;
			}
			else { // Constante
				if(inx < 128) {	
					// addl $0xXX, %edx
					instrucao[0] = 0x83;
					instrucao[1] = 0xc2;
					instrucao[2] = (unsigned char) inx;

					size = 3;
				}
				else {
					// addl $0xXXXXXXXX, %edx
					instrucao[0] = 0x81;
					instrucao[1] = 0xc2;
					instrucao[2] = (unsigned char) (inx & 0xff);
					instrucao[3] = (unsigned char) ((inx >> 8) & 0xff);
					instrucao[4] = (unsigned char) ((inx >> 16) & 0xff);
					instrucao[5] = (unsigned char) ((inx >> 24) & 0xff);
					size = 6;
				}
			}
			break;
		}
		case '-': { // Subtracao
			
			if(var == 'p') { // Parametro
				// subl %edi ou %esi, %edx
				instrucao[0] = 0x29;
				if(inx == 1) // p1 (%edi)
					instrucao[1] = 0xfa;
				else		 // p2 (%esi)
					instrucao[1] = 0xf2;

				size = 2;
			}
			else if(var == 'v') { // Variavel local
				// subl -x(%rbp), %edx
				instrucao[0] = 0x2b;
				instrucao[1] = 0x55;
				instrucao[2] = 0xff + 1 - 4*(unsigned char)inx;

				size = 3;
			}
			else { // Constante
				if(inx < 128) { 
					// subl $0xXX, %edx
					instrucao[0] = 0x83;
					instrucao[1] = 0xea;
					instrucao[2] = (char) inx;

					size = 3;
				}
				else {
					// subl $0xXXXXXXXX, %edx
					instrucao[0] = 0x81;
					instrucao[1] = 0xea;
					instrucao[2] = (unsigned char) (inx & 0xff);
					instrucao[3] = (unsigned char) ((inx >> 8) & 0xff);
					instrucao[4] = (unsigned char) ((inx >> 16) & 0xff);
					instrucao[5] = (unsigned char) ((inx >> 24) & 0xff);

					size = 6;
				}
			}
			break;
		}
		case '*': { // Multiplicacao
			
			if(var == 'p') { // Parametro
				// imull %edi ou %esi, %edx
				instrucao[0] = 0x0f;
				instrucao[1] = 0xaf;
				if(inx == 1) // p1 (%edi)
					instrucao[2] = 0xd7;
				else		 // p2 (%esi)
					instrucao[2] = 0xd6;

				size = 3;
			}
			else if(var == 'v') { // Variavel local
				// imull -x(%rbp), %edx
				instrucao[0] = 0x0f;
				instrucao[1] = 0xaf;
				instrucao[2] = 0x55;
				instrucao[3] = 0xff + 1 - 4*(unsigned char)inx;

				size = 4;
			}
			else { // Constante
				if(inx < 128) { 
					// imull $0xXX, %edx
					instrucao[0] = 0x6b;
					instrucao[1] = 0xd2;
					instrucao[2] = (char) inx;

					size = 3;
				}
				else {
					// imull $0xXXXXXXXX, %edx
					instrucao[0] = 0x69;
					instrucao[1] = 0xd2;
					instrucao[2] = (inx & 0xff);
					instrucao[3] = (unsigned char) ((inx >> 8) & 0xff);
					instrucao[4] = (unsigned char) ((inx >> 16) & 0xff);
					instrucao[5] = (unsigned char) ((inx >> 24) & 0xff);

					size = 6;
				}
			}
			break;
		}
	}

	memcpy((unsigned char*)(codigo + *pos), (unsigned char*)instrucao, size); // Copia a nova instrucao para o vetor de codigo
	*pos += size; // Atualiza posicao final do vetor de codigo
}

void move_lugar_certo( int *pos, char var, int inx){	// Recebe o termo que recebera a atribuicao
	unsigned char instrucao[3]; // 3 eh o tamanho maximo da instrucao
	int size = 0;

	instrucao[0] = 0x89;
	if(var == 'p') { // Atribuicao em parametro
		if(inx == 1) // p1 (%edi)
			// movl %edx, %edi
			instrucao[1] = 0xd7; 
		else		 // p2 (%esi)
			// movl %edx, %esi
			instrucao[1] = 0xd6;

		size = 2;
	}
	else { // Atribuicao em variavel local
		// movl %edx, -x(%rbp)
		instrucao[1] = 0x55;
		instrucao[2] = 0xff + 1 - 4*(unsigned char)inx;

		size = 3;
	}

	memcpy((unsigned char*)(codigo + *pos), (unsigned char*)instrucao, size); // Copia a nova instrucao para o vetor de codigo
	*pos += size; // Atualiza a posicao final do vetor de codigo
}

void gera_cod_atribuicao(FILE *f, int *pos, char c, int line){
	int idx0, idx1, idx2, size = 0;
    char var0 = c, var1, var2, op;
	unsigned char cod_atribuicao[14];	// 14 eh o tamanho maximo que o codigo de atribuicao assume

	if (fscanf(f, "%d = %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6){
		error("comando invalido", line);
		return;
	}
            
	switch (var1){ // Checa primeiro termo da atribuicao
		case 'p': { //Parametro
			// movl %edi ou %esi, %edx
			cod_atribuicao[0] = 0x89;
			if(idx1 == 1)	// p1 (%edi)
				cod_atribuicao[1] = 0xfa;
			else			// p2 (%esi)
				cod_atribuicao[1] = 0xf2;

			size = 2;
			break;
		}
		case 'v': { //Variavel local
			// movl -x(%rbp), %edx
			cod_atribuicao[0] = 0x8b;
			cod_atribuicao[1] = 0x55;
			cod_atribuicao[2] = 0xff + 1 - 4*(unsigned char)idx1;

			size = 3;
			break;
		}
		case '$': { //Constante
			// movl $0xXXXXXXXX, %edx
			cod_atribuicao[0] = 0xba;
			cod_atribuicao[1] = idx1 & 0xff;
			cod_atribuicao[2] = (unsigned char) ((idx1 >> 8) & 0xff);
			cod_atribuicao[3] = (unsigned char) ((idx1 >> 16) & 0xff);
			cod_atribuicao[4] = (unsigned char) ((idx1 >> 24) & 0xff);

			size = 5;
			break;
		}
	}
	memcpy((unsigned char*)(codigo + *pos), cod_atribuicao, size); // Copia a nova instrucao para o vetor de codigo
	*pos += size; // Atualiza a posicao final do vetor de codigo

	// Fazendo a operacao no %edx
	faz_operacao(pos, op, var2, idx2);

	// Movendo pro lugar certo
	move_lugar_certo(pos, var0, idx0);
}

void gera_cod_desvio (FILE *f, int * pos, int *countJmp, int line){
	char varp;
    int idx, linha, size = 0;
    unsigned char codigo_desvio[12]; //12 eh o tamanho maximo do codigo de desvio

	if (fscanf(f, "f %c%d %d", &varp, &idx, &linha) != 3){
            error("comando invalido", line);
            return;
        }

	switch (varp){ // Checa primeiro termo da atribuicao
		case 'p': { // Parametro
			// cmpl $0, %edi ou %esi
			codigo_desvio[0] = 0x83;
			codigo_desvio[2] = 0x00;
			if (idx == 1) // p1 (%edi)
				codigo_desvio[1] = 0xff; 
			else		  // p2 (%esi)
				codigo_desvio[1] = 0xfe;

			size = 3;
			break;
		}
		case 'v': { // Variavel local
			// movl -x(%rbp), %edx
			codigo_desvio[0] = 0x8b;
			codigo_desvio[1] = 0x55;
			codigo_desvio[2] = 0xff + 1 - 4*(unsigned char)idx;

			// cmpl $0, %edx
			codigo_desvio[3] = 0x83;
			codigo_desvio[4] = 0xfa;
			codigo_desvio[5] = 0x00;

			size = 6;
			break;
		}
	}

	// jne linha
	codigo_desvio[size] = 0x0f;
	codigo_desvio[size + 1] = 0x85;
	size += 2;

	memcpy((unsigned char*)(codigo + *pos), codigo_desvio, size); // Copia a nova instrucao para o vetor de codigo
	*pos += size; // Atualiza a posicao final do vetor de codigo
	
	link[*countJmp].idxJmp = *pos; // Guarda a posicao do primeiro byte do codigo que será preenchido com a diferenca dos enderecos
	*pos += 4; // Pula os quatro bytes seguintes
	link[*countJmp].endPosJmp = (long)&codigo[*pos]; // Guarda o endereço da instrução logo depois da instrução de desvio
	link[*countJmp].linhaDestino = linha; // Guarda numero da linha para onde o jump vai
	(*countJmp)++;
}

funcp compila (FILE *f){
	int linha = 1, pos = 0, i, posPreenche, countJmp = 0;
	long endDif, endereco;
	char c;

	//aloca vetor de codigo -> tam 708 = 50 vezes o tamanho do maior comando (14) mais o tamanho do comando da pilha (8) 
	codigo = (unsigned char*)malloc(sizeof(char)*(50*14 + 8)); 
	//aloca a estrutura com os vetores usados para fazer o link dos endereços ao local certo
	link = (Linker*)malloc(sizeof(Linker)*50);

/* 	
	VARIAVEIS DE LIGAÇÃO:
	link[i].linhaDestino = linha para qual sera feito o desvio do jmp
	link[i].idxJmp = armazena o indice do codigo que precisa preencher com o endDif
	link[i].endLinhas = armazena o endereco de cada uma das linhas do arquivo
	link[i].endPosJmp = armazena o endereco da posicao do codigo onde esta a primeira instrucao pós jump
	countJmp = quantidade de jumps que são feitos no codigo
	endDif = diferença do endereco da instrucao após o jump e a instrução de destino do jump
*/

// Inicializando o vetor
    for(i = 0; i < 708; i++){
    	codigo[i] = 0;
    }
	
    memcpy(codigo, cod_pilha, 4); // Copia instrucao de ativacao para o vetor de codigo
	memcpy((unsigned char*)(codigo + 4), cod_sub_rsp, 4); // Copia instrucao de alocacao de espaço para o vetor de codigo
	pos += 8; // Atualiza posicao final do vetor de codigo


	while ((c = fgetc(f)) != EOF) {
		link[linha - 1].endLinhas = (long)&codigo[pos]; // Guarda o endereco do inicio da instrucao da linha

	    switch (c) {
	      case 'r': { // Retorno 
	        gera_cod_ret(f, &pos, linha);
	        break;
	      }
	      case 'v': 
	      case 'p': {  // Atribuicao 
			gera_cod_atribuicao(f, &pos, c, linha);
	        break;
	      }
	      case 'i': { // Desvio 
	        gera_cod_desvio (f, &pos, &countJmp, linha);
	        break;
	      }
	      default: error("Comando Desconhecido", linha);
	    }
	    linha ++;
	   	fscanf(f, " ");
	}

// Preenche o vetor com os endereços que estao faltando
	for (i=0; i<countJmp; i++){
		linha = link[i].linhaDestino;						// Pega a linha para qual sera feito o desvio do jmp
		endereco = link[linha-1].endLinhas;						// Descobre qual o endereco onde comeca a linha da instrucao de destino
		endDif = endereco - link[i].endPosJmp;				// Calcula a diferenca do endereco de destino e endereco da instrucao apos o jmp
		posPreenche = link[i].idxJmp;						// Pega a posicao do codigo q sera preenchida com endLinha
		codigo[posPreenche] = (unsigned char)endDif;			// Armazena o 1o byte do dif
		codigo[posPreenche+1] = (unsigned char)(endDif>>8);		// Armazena o 2o byte
		codigo[posPreenche+2] = (unsigned char)(endDif>>16);	// Armazena o 3o byte
		codigo[posPreenche+3] = (unsigned char)(endDif>>24);	// Armazena o 4o byte
	}

	free(link);
	return (funcp) codigo;
}
