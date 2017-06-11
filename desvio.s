// Desvio
// - DIF: é a diferenca do endereco da linha pra onde vai o desvio e endereco da primeira instrucao apos instrucao do desvio (&linha - &instrucaoPósDesvio)
// - obs: utilizar a instrução para DIF = 4bytes (serve também para os casos de dif < 4 bytes)

//I) Parametro
// -obs: podemos também mover o parametro para o %edx e fazer a comparação com %edx -> verificar melhor opcao
cmpl $0, %edi /* 83 ff 00 */
cmpl $0, %esi /* 83 fe 00 */

jne linha /* 0f 85 DIF (DIF = 00 00 00 00) */

//II) Variavel local
movl -4(%rbp), %edx /* 8b 55 fc */
movl -8(%rbp), %edx /* 8b 55 f8 */
movl -12(%rbp), %edx /* 8b 55 f4 */
movl -16(%rbp), %edx /* 8b 55 f0 */ ->	8b 55 (ff + 1 - 4*i)

cmpl $0, %edx /* 83 fa 00 */
jne linha /* 0f 85 DIF (DIF = 00 00 00 00) */

//Proposta de algoritmo para preencher DIF:
1. guardar o endereço de todas as linhas lidas no arquivo
2. guardar o endereco da posicao do byte que precisa ser preenchido com o endereço de desvio em um vetor de endereco de posicoes
3. guardar o endereço da instrução logo depois da instrução de desvio em um vetor de endereços pós desvio
4. guardar o numero da linha para onde o controle vai desviar (lido do arquivo) em um vetor de linhas de desvio
5. finalizar a traducao de todo o codigo no arquivo, pulando os bytes do DIF ou preenchendo com 0x00
7. enquanto houverem valores no vetor de endereço de posicoes:
6. - acessar um elemento do vetor de endereco de linha através do numero da linha de desvio
7. - subtrair o endereco da instrucao pós instrução de desvio do endereço da linha de desvio 


objdump analisado:

0000000000000000 <.text>:
   0:    83 ff 00                 cmp    $0x0,%edi
   3:    0f 85 00 00 00 00        jne    0x9
   9:    83 fe 00                 cmp    $0x0,%esi
   c:    0f 85 00 00 00 00        jne    0x12
  12:    8b 55 fc                 mov    -0x4(%rbp),%edx
  15:    83 fa 00                 cmp    $0x0,%edx
  18:    0f 85 00 00 00 00        jne    0x1e
  1e:    8b 55 f8                 mov    -0x8(%rbp),%edx
  21:    83 fa 00                 cmp    $0x0,%edx
  24:    0f 85 00 00 00 00        jne    0x2a
  2a:    8b 55 f4                 mov    -0xc(%rbp),%edx
  2d:    83 fa 00                 cmp    $0x0,%edx
  30:    0f 85 00 00 00 00        jne    0x36
  36:    8b 55 f0                 mov    -0x10(%rbp),%edx
  39:    83 fa 00                 cmp    $0x0,%edx
  3c:    0f 85 00 00 00 00        jne    0x42

