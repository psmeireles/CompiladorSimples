void gera_cod_desvio (FILE *f, unsigned char * codigo, int * pos, long *endJmp, long *endPosJmp, int *endDestino, int *posJmp){
	char varp;
    int idx, linha, size = 0;
    unsigned char codigo_desvio[12]; //tamanho maximo do codigo de desvio

	f = fscanf(f, "f %c%d %d", &varp, &idx, &linha);

	switch (varp){ // Checa primeiro termo da atribuicao
		case 'p': {
			codigo_desvio[0] = 0x83;
			codigo_desvio[2] = 0x00;
			if (&idx == 1) 
				codigo_desvio[1] = 0xff; 
			else //idx == 2
				codigo_desvio[1] = 0xfe;
			size = 3;
			break;
		}
		case 'v'{
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
	
	endJmp[*posJmp] = *pos; //grava a posicao do primeiro byte do codigo que será preenchido com a diferenca dos enderecos
	*pos+=4; //pula os quatro bytes seguintes
	endPosJmp[*posJmp] = (long)&codigo[*pos]; //guarda o endereço da instrução logo depois da instrução de desvio
	endDestino[*posJmp] = linha; //guarda numero da linha para onde o jmp vai
	*posJmp++;


}