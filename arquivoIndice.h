#ifndef ARQUIVOINDICE_H
#define ARQUIVOINDICE_H

    typedef struct indicecabecalho_
    {
        char status;   // Status do arquivo.
        int noRaiz;    // RRN do nó raiz
        int nroNiveis; // Número de níveis da árvore binária
        int proxRRN;   // Valor do RRN do próximo nó a ser inserido.
        int nroChaves; // Armazena o número de chaves já inseridas no arquivo.
    }INDICE_CABECALHO;

    typedef struct indiceregistro_
    { 
        int nivel;  // Nível em que o registro se encontra
        int n;      // Número de chaves inseridas na página atual
        int C[5];   // Vetor que armazena as chaves, referentes ao campo "idNascimento"
        int PR[5];  // Vetor que armazena o RRN referente ao arquivo original, do campo idNascimento respectivo.
        int P[6];   // Vetor que armazena, sequencialmente, a refência para os nós filhos da página atual.
    }INDICE_REGISTRO;

    INDICE_REGISTRO indiceRegistroVazio();
    int insercaoArquivoIndice(FILE* indice, int RRN, int chave, int referencia, int* chavePromovida, int* referenciaPromovida, int* promocaoFilhoDireita, int* promocaoFilhoEsquerda, INDICE_CABECALHO* cabecalho, int nivel);
    void criacaoNoRaiz(FILE* indice, int chave, int referencia, int filhoDireita, int filhoEsquerda, INDICE_CABECALHO* cabecalho);
    int buscaArquivoIndice(FILE* indice, int RRN, int chave, int* rrnAchado, int* numIteracoes);
#endif
