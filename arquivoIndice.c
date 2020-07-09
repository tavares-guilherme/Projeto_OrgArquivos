#include <stdio.h>
#include <stdlib.h>
#include "arquivoIndice.h"

INDICE_REGISTRO recebeRegistro(int* bytesLidos, INDICE_REGISTRO novo)
{
    /*
     *      Funçao que, dado um vetor de valores inteiros lidos, que representam um Registro lido (diretamente pelo fread),
     *  o converte para ser armazenado corretamente na struct "INDICE_REGISTRO".
     */

    int contador = 0; // Contador utilizado para percorrer o vetor "bytesLidos"
    
        novo.nivel = bytesLidos[contador];
        contador++;

        novo.n = bytesLidos[contador];
        contador++;
        

        for(int i = 0; i < 5; i++)  // Salva as chaves de busca e o RRN do arquivo originário.
        {
            novo.C[i] = bytesLidos[contador];
            contador++;

            novo.PR[i] = bytesLidos[contador];
            contador++;
        }

        for(int i = 0; i < 6; i++) // Slava os RRN dos nós filhos.
            novo.P[i] = bytesLidos[i + 12];
 
    

    return novo;
}

int* preparaRegistro(INDICE_REGISTRO registro)
{
    /*
     *  Função que converte uma struct "INDICE_REGISTRO", em uma sequência de bytes que já pode ser inserida no arquivo.
     */

    int* bytes;

    bytes = (int*) malloc(sizeof(int) * 18);
    for(int i = 0; i < 18; i++) bytes[i] = -1;

    bytes[0] = registro.nivel;
    bytes[1] = registro.n;

    for(int i = 0; i < 5; i++)
    {
        bytes[2 + i*2] = registro.C[i];
        bytes[3 + i*2] = registro.PR[i];
    }   

    for(int i = 0; i < 6; i++)
        bytes[12 + i] = registro.P[i];
    
    printf("Bytes:\n");
    for(int i = 0; i < 18; i++) printf("%d ", bytes[i]);
    printf("\n");
    return bytes;
}

INDICE_REGISTRO indiceRegistroVazio()
{
    /*
     *  Função para gerar os bytes, em forma de array de inteiros, de um registro vazio.
     */
    INDICE_REGISTRO reg;

    for(int i = 0; i < 5; i++)
    {
        reg.C[i] = -1;
        reg.PR[i] = -1;
        reg.P[i] = -1;
    }

    reg.P[5] = -1;
    reg.n = 0;
    reg.nivel = 0;

    return(reg);
}

void paginaImprimir(INDICE_REGISTRO palavra)
{
    printf(" == REG == \n");
    printf("CHAVE:\n");
    for(int i = 0; i < 5; i++) printf("%d\t", palavra.C[i]);
    printf("\nRRN:\n");
    for(int i = 0; i < 5; i++) printf("%d\t", palavra.PR[i]);
    printf("\nFILHOS:\n");
    for(int i = 0; i < 6; i++) printf("%d\t", palavra.P[i]);
    printf("\n");
}

int buscaArquivoIndice(FILE* indice, int RRN, int chave, int* rrnAchado, int* numIteracoes)
{
    /*
     *      Função que, dado um arquivo de índice já aberto e um dado valor, realiza uma busca e retorna em qual RRN (Do arquivo de Indice) o valor foi encontrado.
     *  Caso o valor não seja encontrado, a função retorna -1.
     * 
     *  FILE* indice        - Arquivo onde será feita a busca.
     *  int   RNN           - Armazena o RRN do nó em que será realizada a busca.
     *  int chave           - Armazena a chave de busca.
     *  int* rrnachado      - Chave PR a ser retornada.
     *  int* numIterações   - Contador que deve ser passado através de uma variável inicializada com 0 e armazena o número de chamadas realizadas nesta função.
     */ 
    
    INDICE_REGISTRO aux;
    (*numIteracoes)++;

    /* Caso o RRN seja inválido, retorna que o dado não foi encontrado. */
    if(RRN <= 0) return -1;
    else 
    {
        /* Lê o nó no dado RRN */
        int bytesLidos[18];
        fseek(indice, 72 * (RRN + 1), SEEK_SET);
        fread(bytesLidos, sizeof(int) * 18, 1, indice);
        aux = recebeRegistro(bytesLidos, aux);
        //paginaImprimir(aux);
    }
    
    /* Busca o Índice no nó atual*/
    for(int i = 0; i < aux.n; i++)
    {
        if(chave == aux.C[i])
        {
            *rrnAchado = aux.PR[i];
            return 1;
        }
        
        if(chave < aux.C[i])
            return(buscaArquivoIndice(indice, aux.P[i], chave, rrnAchado, numIteracoes));
    }

    return(buscaArquivoIndice(indice, aux.P[aux.n], chave, rrnAchado, numIteracoes));
}

INDICE_REGISTRO insercaoOrdenada(INDICE_REGISTRO pagina, int chave, int referencia, int RRN)
{
    /*
     *      Função para realizar uma inserção, em RAM, num registro do arquivo de índice, de forma a manter o padrão requerido pela Árvore-B,
     *  ou seja, basicamente é feita uma inserção ordenada em um Array.
     * 
     *  INDICE_REGISTRO pagina - Página de disco ( ou nó ), em que será feita a inserção.
     *  int chave              - Chave C a ser inserida.
     *  int referencia         - Chave PR a ser inserida
     *  int RRN                - Filho a Direita da chave a ser inserida.
     * 
     *  OBS: Já é pressuposto que há espaço na página para a inserção da chave, esta condição deve ser tratada antes da chamada desta função.
     */

    // Contador utilizado para percorrer o vetor das Chaves, armazenando, por fim, a posição em que as chaves devem ser inseridas.
    int contador = 0;
    // Variavel auxiliar que armazenará o conteúdo da proxima posição do vetor.
    int aux = -1;
    // Variavel auxiliar utilizada para armazenar o proximo valor a ser inserido na chave devido a realocação dos dados.
    int proximo = -1;

    while(chave > pagina.C[contador] && contador < pagina.n)
        contador++;
    
    pagina.n++;
    // Atualiza o vetor que contém as chaves.
    proximo = pagina.C[contador];
    pagina.C[contador] = chave;

    for(int i = contador + 1; i < pagina.n; i++)
    {
        aux = pagina.C[i];
        pagina.C[i] = proximo;
        proximo = aux;
    }
    
    // Atualiza o vetor que contem a Referencia ao arquivo binario original:
    proximo = pagina.PR[contador];
    pagina.PR[contador] = referencia;
    for(int i = contador + 1; i < pagina.n; i++)
    {
        aux = pagina.PR[i];
        pagina.PR[i] = proximo;
        proximo = aux;
    }

    // Insere a referência do nó filho à direita das chaves inseridas.
    proximo  = pagina.P[contador + 1];
    pagina.P[contador+1] = RRN;

    if(contador + 1 < 6) // Condicional para impedir overflow no array.
    {
        for(int i = contador + 2; i < 6; i++)
        {
            aux = pagina.P[i];
            pagina.P[i] = proximo;
            proximo = aux;
        }
    }
    
    printf("\n=== INSERÇÃO NÁ PÁGINA ===\n");
    printf("C: ");
    for(int i = 0; i < 5; i++) printf("%d\t", pagina.C[i]);
    printf("\n");
    printf("PR: ");
    for(int i = 0; i < 5; i++) printf("%d\t", pagina.PR[i]);
    printf("\n");
    printf("P: ");
    for(int i = 0; i < 6; i++) printf("%d\t", pagina.P[i]);
    printf("\n");
    return pagina;
}

INDICE_REGISTRO split(int chaveNova, int referenciaNova, int filhoDireita, INDICE_REGISTRO* pagina, int* chavePromovida, int* referenciaPromovida)
{

    /*
     *      Esta função serve para realizar split em uma página de disco, para tratar o overflow da inserção em uma página já cheia.
     *     
     *      Retorno: a nova página de disco, que foi criada para armazenar metade das chaves da página original.
     * 
     *      int chaveNova               - Contém a chave C nova, que está sendo inserida.
     *      int referenciaNova          - Contém a chave PR nova, que está sendo inserida.
     *      int filhoDiretia            - Armazena o RRN do filho a direita da nova chave a ser inserida.
     *      INDICE_REGISTRO* pagina     - Página inicial, onde deveria ser inserida as chaves nova. Utilizamos o ponteiro pois haverão 
     * modificações nesta página.
     *      int* chavePromovida         - Chave C que será promovida, e deverá ser inserida em um nível acima.
     *      int* referenciaPromovida    - Chave PR que será promovida, e deverá ser inserida em um nível acima.
     *      int* novaPaginaFilhoDireita - Armazena o RRN do filho a direita das chaves promovidas.
     */
    INDICE_REGISTRO novaPagina;
    
    int chaves[6]; // Array para armazenar, ordenadamente, todas as chaves C, incluindo a nova.
    int PR[6];     // Array para armazenar, ordenadamente, todas as chaves PR, includindo a nova.
    int RRN[7];    // Array para armazenar, ordenadamente, a referência para todos os nós filhos, incluindo o filho a direit das chaves inseridas.
    int chaveAuxiliar = 5; // Esta variável ajuda na inserção ordenada nos arrays, armazenando a posição da primeira chave maior do que a chave nova.

    novaPagina.nivel = pagina->nivel;

    novaPagina.n = 2;
    pagina->n = 3;

    // O loop abaixo serve para encontrar a 'chave auxiliar'.
    for(int i = 0; i < 5; i++)
    {
        if(pagina->C[i] > chaveNova) // Encontra o primeiro valor que a chave nova.
        {
            chaveAuxiliar = i;
            break;
        }
    }

    // Insere a chave nova nos vetores auxiliares.
    chaves[chaveAuxiliar] = chaveNova;
    PR[chaveAuxiliar] = referenciaNova;
    RRN[chaveAuxiliar + 1] = filhoDireita;

    // O laço abaixo serve para preencher completamente os vetores auxiliar, nas posições após a chave nova.
    for(int i = 0; i < 6; i++)
    {
        if(i != chaveAuxiliar) // Pula esta iteração, já que a chave i já foi inserida.
        {
            chaves[i] = pagina->C[i];
            PR[i]     = pagina->PR[i];
        }

        if(i != chaveAuxiliar+1)
            RRN[i] = pagina->P[i];
    }
    // Insere o ultimo elemento de RRN
    if(chaveAuxiliar != 5)
        RRN[6] = pagina->P[5];
    
    // Seleciona as chaves e o RRN de promoção.
    *chavePromovida       = chaves[3];
    *referenciaPromovida  = PR[3];

    // Agora, iremos inserir, na pagina de disco, o contéudo dos vetores iniciais ANTES da chave promovida.
    for(int i = 0; i < 3; i++)
    {
        pagina->C[i]  = chaves[i];
        pagina->PR[i] = PR[i];
        pagina->P[i]  = RRN[i];
    }
    pagina->P[3] = RRN[3];

    for(int i = 4; i < 6; i++)
    {
        novaPagina.C[i-4]  = chaves[i];
        novaPagina.PR[i-4] = PR[i];
        novaPagina.P[i-4]      = RRN[i];
    }
    novaPagina.P[2] = RRN[6];

    // Define como nulo, as posições vazias em ambas as páginas:
    novaPagina.C[2] = -1;
    novaPagina.PR[2] = -1;
    for(int i = 3; i < 5; i++)
    {
        pagina->C[i]  = -1;
        pagina->PR[i] = -1;
        pagina->P[i]  = -1;
        
        novaPagina.C[i]  = -1;
        novaPagina.PR[i] = -1;
        novaPagina.P[i]  = -1;
    }
    novaPagina.P[5] = -1;
    pagina->P[5] = -1;

    return novaPagina;
}

int insercaoArquivoIndice(FILE* indice, int RRN, int chave, int referencia, int* chavePromovida, int* referenciaPromovida, int* promocaoFilhoDireita, int* promocaoFilhoEsquerda, INDICE_CABECALHO* cabecalho, int nivel)
{
    /*
     *      Função para realizar uma inserção num arquivo de índice já aberto.
     * 
     *      Retorno: 1 - Houve promoção
     *               2 - Não houve promoção
     *               3 - Erro na inserção
     *      
     *      ==  Parâmetros == 
     *      FILE* indice                - Ponteiro para o arquivo de índice onde será realizada a inserção. 
     *      int   RRN                   - RRN da página em que devemos começar a buscar a chave, nota-se que, na primeira chamada da função, deve conter
     * o RRN da raíz da árvore.
     *      int   chave                 - Chave a ser inserida.
     *      int   referencia            - Segunda chave a ser inserida, referente ao vetor PR, que armazena o RRN dos registros do arquivo principal.
     *      int*  chavePromovida        - Chave C promovida.
     *      int*  referenciaPromovia    - Chave PR promovida
     *      int   promocaoFilhoDireita  - Retorna o RRN de referência para o filho a direita da chave promovida.       
     *      INDICE_CABECALHO* cabecalho - Serve para retorno, onde serão atualizadas as informações do cabeçalho
     *      int nivel;
     */

    if(RRN == -1) // Construção a partir das folhas
    {      
        *chavePromovida        = chave;
        *referenciaPromovida   = referencia;
        *promocaoFilhoDireita  = -1;
        *promocaoFilhoEsquerda = -1;

        return 1;
    }else
    {
        INDICE_REGISTRO pagina;
        INDICE_REGISTRO novaPagina;
        int*            buffer = malloc(sizeof(int) * 18); 
        int             valorDeRetorno = 0;

        // Armazena, em RAM (pagina), o conteúdo da página atual.
        fseek(indice, (RRN + 1) *72, SEEK_SET);
        fread(buffer, sizeof(int) * 18, 1, indice);

        pagina = recebeRegistro(buffer, pagina);
        // Pesquisa a chave na página atual
        for(int i = 0; i < pagina.n; i++)
        {            
            // Caso a chave seja encontrada, retornamos -1, para indicar erro na inserção.
            if(chave == pagina.C[i]) 
                return 3;

            if(pagina.C[i] > chave)
                valorDeRetorno = insercaoArquivoIndice(indice, pagina.P[i], chave, referencia, chavePromovida, referenciaPromovida, promocaoFilhoDireita, promocaoFilhoEsquerda, cabecalho, pagina.nivel);
        }
        
        // Verifica se a chave de busca é maior que a última chave inserida na página.
        if(chave > pagina.C[pagina.n])
            valorDeRetorno = insercaoArquivoIndice(indice, pagina.P[pagina.n], chave, referencia, chavePromovida, referenciaPromovida, promocaoFilhoDireita, promocaoFilhoEsquerda, cabecalho, pagina.nivel);

        // Se não houve promoção, ou houve erro, retorna o valor de retorno da última chamada recursiva.
        if(valorDeRetorno == 3 || valorDeRetorno == 2)
            return valorDeRetorno;
        else
        {
            // Verifica se há espaço para inserção nesta página, ou seja, se há necessidade de split.
            if(pagina.n < 5)
            {
                pagina = insercaoOrdenada(pagina, *chavePromovida, *referenciaPromovida, *promocaoFilhoDireita);
                buffer = preparaRegistro(pagina);

                // Insere a página atualizada no arquivo.
                fseek(indice, 72 * (RRN + 1), SEEK_SET);
                fwrite(buffer, sizeof(int), 18, indice);

                cabecalho->nroChaves++;
                return 2; // Retorno indicando que não houve promoção
            }else // Caso em que há necessidade de split
            {
                novaPagina = split(*chavePromovida, *referenciaPromovida, *promocaoFilhoDireita, &pagina, chavePromovida, referenciaPromovida);
                
                *promocaoFilhoDireita = cabecalho->proxRRN; // Cria a referência para o novo nó criado no split.
                cabecalho->proxRRN++;
                
                //printf("Direita: %d, Esquerda: %d\n", *promocaoFilhoDireita, RRN);
                // Insera "pagina", com os valores atualizados no vetor atual:
                buffer = preparaRegistro(pagina);
                fseek(indice, 72 * (RRN + 1), SEEK_SET);
                fwrite(buffer, sizeof(int), 18, indice);
                *promocaoFilhoEsquerda = RRN;

                // Insere a nova página originária do split: 
                buffer = preparaRegistro(novaPagina);
                fseek(indice, 72 * (*promocaoFilhoDireita + 1), SEEK_SET);
                fwrite(buffer, sizeof(int), 18, indice);
                
                printf("== SPLIT REALIZADO===\nRRN: %d, Promoção filho direita: %d.\n", RRN, *promocaoFilhoDireita);   
                return 1; // Retorna que houve promoção;
            }
        }
    } 
}

void criacaoNoRaiz(FILE* indice, int chave, int referencia, int filhoDireita, int filhoEsquerda, INDICE_CABECALHO* cabecalho)
{
    /*
     *      Esta função serve para criar um novo nó raiz na Árvore B.
     *      Como a criação do nó raiz implica na insersão de um elemento, já sabemos as posições em que iremos inserir, logo,
     *  podemos fazer esta inserção diretamente, evitando a chamada da função "insercaoOrdenada".
     * 
     *      FILE* indice                - Arquivo binário, já aberto, que contém a árvore.
     *      int chave                   - Chave C a ser inserida.
     *      int referencia              - Chave PR a ser inserida.
     *      int filhoDireita            - Filho a direita do nó Raiz.
     *      int filhoEsquerda           - Filho a esquerda do nó Raiz.
     *      INDICE_CABECALHO* cabecalho - Endereco do cabeçalho do arquivo salvo em RAM.          
     */

    INDICE_REGISTRO raiz = indiceRegistroVazio(); // Struct raiz, inicializada com os valores nulos.
    int* buffer; // Array que contém a struct convertida para um conjunto de inteiros.
    
    // Insere as chaves e os filhos:
    raiz.C[0] = chave;
    raiz.PR[0] = referencia;
    raiz.P[0] = filhoEsquerda;
    raiz.P[1] = filhoDireita;
    // Atualiza o nível e o número de elementos na página:
    raiz.n = 1;
    
    cabecalho->nroNiveis++;
    raiz.nivel = cabecalho->nroNiveis;
    
    // Insere a página no arquivo:
    buffer = preparaRegistro(raiz);    
    fseek(indice, 72 * (cabecalho->proxRRN + 1), SEEK_SET);
    fwrite(buffer, sizeof(int), 18, indice);
    // Atualiza noRaiz como o RRN inserido, e realiza uma iteração no proximo RRN a ser inserido:
    cabecalho->noRaiz = cabecalho->proxRRN; 
    cabecalho->proxRRN++;

    printf("\n ==== RAIZ INSERIDA ===   \n ");
    printf("C: ");
    for(int i = 0; i < 5; i++) printf("%d\t", raiz.C[i]);
    printf("\nPR: ");
    for(int i = 0; i < 5; i++) printf("%d\t", raiz.PR[i]);
    printf("\nP: ");
    for(int i = 0; i < 6; i++) printf("%d\t", raiz.P[i]);
    printf("\n");

    return;
}
