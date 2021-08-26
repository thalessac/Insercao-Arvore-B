//--------------------------------------------------------------
// INSER��O EM ARVORE B
//--------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream>


//-------------------------------------------
//Definicao das estruturas

// CHLINK eh um par <chave,link> - dentro de cada p�gina h� 3 CHLINKs
typedef struct {
	int chave;
	int linkdir;
} CHLINK;

// definicao de pagina da �rvore / registro do arquivo
typedef struct {
	int np; // armazene aqui o nro da pagina no arquivo
	int cont; // armazene aqui a quantidade de chaves existentes no vetor CHLINK[]
	CHLINK item[3]; // a chave[0] eh desprezada, e o linkdir[0] eh na verdade o link esquerdo da chave[1]
} PAGINA;

//-------------------------------------------
//M�todos

// FUNCOES AUXILIARES

/**
 * M�todo para fazer a troca entre duas posi��es no vetor item. Um item troca de lugar com outro
 */
void swap_item(CHLINK item[], int i, int j);

/**
 * M�todo para ordenar os itens de acordo com sua chave
 */
void sort_item(CHLINK item[], int n);

/**
 * M�todo para fazer a troca entre duas chaves no vetor item. Uma chave troca de lugar coma outra
 */
void swap_chave(CHLINK item[], int i, int j);

/**
 * M�todo para ordenar as chaves, sem alterar a posi��o dos links
 */
void sort_chave(CHLINK item[], int n);

/**
 * Realiza a impressao de uma arvore
 */
void imprimir_pagina(PAGINA* pagina);

//FUNCOES DE BUSCA

/**
 * Verifica, com uma busca sequencial, se uma chave est� em uma p�gina
 */
bool is_in_chlink(CHLINK item[], int ch);

/**
 * Fun��o auxiliar para a busca. Dada uma p�gina, encontra o filho para onde a busca deve prosseguir
 */
int encontra_filho(PAGINA* pagina, int ch);

//FUNCOES AUXILIARES DA FUNCAO PRINCIPAL

/**
 * Fun��o que promove um split em uma p�gina com overflow e retorna a chave promovida para o n�vel acima
 */
CHLINK split(PAGINA arvore[], PAGINA* pagina, int ch, int* raiz, int* N);

/**
 * Fun��o que insere uma chave na �rvore com chamadas recursivas
 */
int inserir_chave(PAGINA arvore[], int pai, CHLINK ch, CHLINK* ch_promovida, int* raiz, int* N);

// FUNCAO PRINCIPAL

/**
 * Abre  oarquivo, promove a insercao e encerra
 */
void inserir(char nomearq[], int* raiz, int ch);


//-------------------------------------------
//Codigo dos m�todos


void swap_item(CHLINK item[], int i, int j)
{

    CHLINK k = item[i];
    item[i] = item[j];
    item[j] = k;
}

void sort_item(CHLINK item[], int n){
    if (n < 1)return;

    for (int i=0; i<n-1; i++)
        if (item[i].chave > item[i+1].chave)
            swap_item(item, i, i+1);
    sort_item(item, n-1);
}

void swap_chave(CHLINK item[], int i, int j)
{

    int k = item[i].chave;
    item[i].chave = item[j].chave;
    item[j].chave = k;
}


void sort_chave(CHLINK item[], int n){
    if (n < 1)return;

    for (int i=0; i<n-1; i++)
        if (item[i].chave > item[i+1].chave)
            swap_chave(item, i, i+1);
    sort_chave(item, n-1);
}

void imprimir_pagina(PAGINA* pagina){
    printf("\n\nNo pagina %d \n", pagina->np);
    printf("Chaves: ");
    for (int i=1; i<=pagina->cont; i++){
        printf("%d ", pagina->item[i].chave);
        }
    printf("\nLinks: ");
    for (int i=0; i<=pagina->cont; i++){
        printf("%d ", pagina->item[i].linkdir);
        }
    }


//BUSCA NA ARVORE B

/**
 * Verifica, com uma busca sequencial, se uma chave est� em uma p�gina
 */
bool is_in_chlink(CHLINK item[], int ch){

    if (item[1].chave == ch || item[2].chave == ch)
            return true;
    return false;
}

/**
 * Fun��o auxiliar para a busca. Dada uma p�gina, encontra o filho para onde a busca deve prosseguir
 */
int encontra_filho(PAGINA* pagina, int ch){

    int n = pagina->cont;

    for (int i=1; i<=n; i++){

        if (ch < pagina->item[i].chave)
            return pagina->item[i-1].linkdir;
    }

    return pagina->item[n].linkdir;

}

//FUNCOES AUXILIARES DA FUNCAO PRINCIPAL

CHLINK split(PAGINA arvore[], PAGINA* pagina, CHLINK ch, int* raiz, int* N){
/*ESSA FUN��O FOI ALTERADA EM RELA��O A ENTREGA INICIAL*/

    CHLINK ch_promovida;
    //Quebrar a p�gina em duas
    PAGINA nova_pagina;

    //pagina temporaria para armazenar todas as chaves envolvidas no split, com seu link direito
    CHLINK item_temp[3];
    item_temp[0] = ch;
    item_temp[1] = pagina->item[1];
    item_temp[2] = pagina->item[2];
    int linkesq = pagina->item[0].linkdir; //Armazenamos o link esquerdo da pagina que sera dividida

    //ordenar chaves com seu link direito
    sort_item(item_temp, 3);

    //promove a chave mediana
    ch_promovida = item_temp[1];

    //reorganizar ponteiros

    //P�gina � direita
    nova_pagina.cont=1;
    nova_pagina.np = *N;
    *N+=1;  //atualiza o tamanho da �rvore
    nova_pagina.item[1] = item_temp[2];
    nova_pagina.item[0].linkdir = item_temp[1].linkdir; //link esquerdo

    //P�gina atual fica apenas com a parte esquerda. Vamos elimiar a mediana e a por��o direita
    pagina->cont=1;
    pagina->item[1] = item_temp[0];
    pagina->item[2].chave = NULL;
    pagina->item[0].linkdir = linkesq; //recebe o link esquerdo de volta

    //Chave promovida aponta para a nova p�gina (direita) criada
    ch_promovida.linkdir = nova_pagina.np;

    //Escreve a p�gina nova
    arvore[nova_pagina.np] = nova_pagina;
    //Reescreve p�gina atual
    arvore[pagina->np] = *pagina;

    if (pagina->np == *raiz){
        //Se a p�gina com overflow for a raiz, cria-se uma nova raiz
        PAGINA nova_raiz;
        nova_raiz.np = *N;
        *N+=1; //Atualiza o tamanho da �rvore
        *raiz = nova_raiz.np;
        nova_raiz.cont = 1;
        nova_raiz.item[0].linkdir = pagina->np; //link esquerdo aponta para a raiz atual
        nova_raiz.item[1].linkdir = ch_promovida.linkdir; //link direito aponta para a p�gina criada ap�s o split
        nova_raiz.item[1].chave = ch_promovida.chave; //nova raiz recebe a chave promovida (mediana)
        arvore[nova_raiz.np] = nova_raiz;
    }

    return ch_promovida; //Chave promovida
}


int inserir_chave(PAGINA arvore[], int pai, CHLINK ch, CHLINK* ch_promovida, int* raiz, int* N){

    /*
    Flags:
        0 :: erro (chave j� existe na arvore)
        1 :: Chave inserida com promo��o
        -1 :: Chave inserida sem promo��o

    */

    if (pai == -1){
        *ch_promovida = ch;
        return 1; //flag que indica que houve promo��o na chave
        //Se chegou em uma folha, iremos inserir a chave na volta recursiva
    }

    PAGINA pagina = arvore[pai];

    if (is_in_chlink(pagina.item, ch.chave)){
        return 0;
    }

    //imprimir_pagina(&pagina);
    int filho = encontra_filho(&pagina, ch.chave);

    int flag = inserir_chave(arvore, filho, ch, ch_promovida, raiz, N); //chamada recursiva

    if (flag!=1){
        //Se n�o houve promo��o
        return flag;
    }

    //Se houver espa�o, inserir
    if (pagina.cont == 1){
        pagina.item[2] = *ch_promovida;
        pagina.cont+=1;
        //ordenar
        if (pagina.item[1].chave > pagina.item[2].chave)
            swap_item(pagina.item, 1,2);

        //escreve p�gina na arvore
        arvore[pai] = pagina;

        return -1;//Inseriu SEM promo��o de chave

    }

    //Caso, contr�rio, split
    if (pagina.cont == 2){

        *ch_promovida = split(arvore, &pagina, *ch_promovida, raiz, N); //chave a ser inserida passa a ser a chave promovida na volta da recurs�o

        return 1; //Houve promo��o de chave
    }
}


void inserir(char nomearq[], int* raiz, int ch) {

	// abrir o arquivo
	// fazer a insercao / divisao / promocao etc.
	// atualizar o nro da *raiz se necessario
	// fechar o arquivo


	FILE* arq;
	int MAX = 1000;

	//Tente abrir o arquivo
	if ((arq = fopen(nomearq,"r"))==NULL) {
       arq = fopen(nomearq,"w");//Se n�o existir, iremos criar
	} //FIM caso n�o haja conte�do no arquivo

	if (*raiz == -1){ //�rvore vazia, criar p�gina 0, inserir elemento e encerrar
        PAGINA pagina[1];
        pagina[0].np = 0;
        *raiz = 0;
        pagina[0].cont = 1;
        pagina[0].item[1].chave = ch;
        pagina[0].item[0].linkdir = -1;
        pagina[0].item[1].linkdir = -1;
        fwrite(pagina, sizeof(PAGINA),1 ,arq);
        fclose(arq);
        return; //FIM caso a raiz seja -1
    }

    //Serializar a arvore

    /**
     * Recebemos um arquivo bin�rio contendo uma �rvore e iremos armazena-la em um vetor
     */
    PAGINA vetor_arvore[MAX];
    int N = fread(vetor_arvore, sizeof(PAGINA), MAX, arq); //N � a quantidade de p�ginas
    fclose(arq);

    //Chamar algoritmo de inser��o
    CHLINK chave; //chave a ser inserida
    CHLINK chave_promovida; //vari�vel que armazena a chave promovida caso ocorra overflow

    chave.chave = ch; //Inicializa��o da chave
    chave.linkdir = -1;

    int pai = *raiz; //o primeiro pai para iniciar a busca e inser��o � a raiz da �rvore

	inserir_chave(vetor_arvore, pai, chave, &chave_promovida, raiz, &N); // A raiz e o N s�o atualizados durante o algoritmo de inser��o

	//Ao fim da inser��o, reescrever o arquivo
	arq = fopen(nomearq,"w");
	fwrite(vetor_arvore, sizeof(PAGINA),N ,arq);
    fclose(arq);

}

int main()
{
    int MAX = 1000;

	char nomearq[] = "minhaarvore.bin";
	int raiz = -1;

	// chame a insercao aqui


	//Teste 1: 3 8 12 7 15 2 1 8 9
	inserir(nomearq, &raiz, 3);
	inserir(nomearq, &raiz, 8);
	inserir(nomearq, &raiz, 12);
	inserir(nomearq, &raiz, 7);
	inserir(nomearq, &raiz, 15);
	inserir(nomearq, &raiz, 2);
	inserir(nomearq, &raiz, 1);
	inserir(nomearq, &raiz, 8);
	inserir(nomearq, &raiz, 9);


    /*
	//Teste 2: 13 10 7 6 12 5 4 3 11 14 15

	inserir(nomearq, &raiz, 13);
	inserir(nomearq, &raiz, 10);
	inserir(nomearq, &raiz, 7);
	inserir(nomearq, &raiz, 6);
	inserir(nomearq, &raiz, 12);
	inserir(nomearq, &raiz, 5);
	inserir(nomearq, &raiz, 4);
	inserir(nomearq, &raiz, 3);
	inserir(nomearq, &raiz, 11);
	inserir(nomearq, &raiz, 14);
	inserir(nomearq, &raiz, 15);
    */

    /*
	//Teste 3: 20 5 12 8 6 9 4 3 15 14 6 13 10 16 17 18 21

	inserir(nomearq, &raiz, 20);
	inserir(nomearq, &raiz, 5);
	inserir(nomearq, &raiz, 12);
	inserir(nomearq, &raiz, 8);
	inserir(nomearq, &raiz, 6);
	inserir(nomearq, &raiz, 9);
	inserir(nomearq, &raiz, 4);
	inserir(nomearq, &raiz, 3);
	inserir(nomearq, &raiz, 15);
	inserir(nomearq, &raiz, 14);
	inserir(nomearq, &raiz, 6);
	inserir(nomearq, &raiz, 13);
	inserir(nomearq, &raiz, 10);
	inserir(nomearq, &raiz, 16);
	inserir(nomearq, &raiz, 17);
	inserir(nomearq, &raiz, 18);
	inserir(nomearq, &raiz, 21);
    */

    /*
	//Teste 4: 1 2 3 4 5 6 7 8 9 10 11 12

	inserir(nomearq, &raiz, 1);
	inserir(nomearq, &raiz, 2);
	inserir(nomearq, &raiz, 3);
	inserir(nomearq, &raiz, 4);
	inserir(nomearq, &raiz, 5);
	inserir(nomearq, &raiz, 6);
	inserir(nomearq, &raiz, 7);
	inserir(nomearq, &raiz, 8);
	inserir(nomearq, &raiz, 9);
	inserir(nomearq, &raiz, 10);
	inserir(nomearq, &raiz, 11;
	inserir(nomearq, &raiz, 12);
	*/

	/*
	//Teste 5: 5 8 12 6 4 2 7 2 11

	inserir(nomearq, &raiz, 5);
	inserir(nomearq, &raiz, 8);
	inserir(nomearq, &raiz, 12);
	inserir(nomearq, &raiz, 6);
	inserir(nomearq, &raiz, 4);
	inserir(nomearq, &raiz, 2);
	inserir(nomearq, &raiz, 7);
	inserir(nomearq, &raiz, 11);
	*/

	//Impress�o da arvore final

	PAGINA vetor_arvore[MAX];
	FILE* arq = fopen(nomearq,"r");
    int N = fread(vetor_arvore, sizeof(PAGINA), MAX, arq); //N � a quantidade de p�ginas
    fclose(arq);

    printf("\n=========ARVORE FINAL======\n");
    printf("\nRaiz: %d\n", raiz);
	for(int i = 0; i<N; i++){
        imprimir_pagina(&vetor_arvore[i]);

    }
}






