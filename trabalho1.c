// 11218730 - Guilherme Alves Neves Tavares
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binarioNaTela.h"

#define DEBUG 1

typedef struct registros_
{ 
    /*
     *      Struct que que Representa Um registro de Dados.
     *      Cada valor da Struct representa um Campo. 
     *      Note que, caso algum campo de Tamanho variável, seja nulo, o campo respectivo ao seu indicador de tamanho,
     *  
     */
 
    int      tamanho_cidadeMae;  // ID Campo: 1
    int      tamanho_cidadeBebe; // ID Campo: 2
    char     cidadeMae[100];     // ID Campo: 3
    char     cidadeBebe[100];    // ID Campo: 4
    int      idNascimento;       // ID Campo: 5
    int      idadeMae;           // ID Campo: 6
    char     dataNascimento[11]; // ID Campo: 7
    char     sexoBebe;           // ID Campo: 8
    char     estadoMae[3];       // ID Campo: 9
    char     estadoBebe[3];      // ID Campo: 10

} REGISTRO;
    
typedef struct cabecalho_
{
    /*
    *      Struct que armazena os bytes informativos do cabeçalho.
    *      A struct será inicializada com os valores "prontos" para serem inseridos em um arquivo ainda vazio.
    *     OBS: Note que o Registro de Cabeçalho não conta para o RRN, e para o valor numérico dos valores inseridos, removidos
    *     e atualizados.
    */
    int  rrnProxRegistro;
    int  numRegistrosInseridos;
    int  numRegistrosRemovidos;
    int  numRegistrosAtualizados;
    char status;
} CABECALHO;

// ================================================== FUNÇÕES USADAS EM VÁRIAS FUNCIONALIDADES ================================================================
void imprimeRegistro(REGISTRO r)
{
    /*
     *      Função que, dada um Registro 'r' imprime seu conteúdo na formatação pedida.
     *      A função não faz nenhuma checagem quanto o conteúdo do registro, então deve-se garantir que o registro fornecido
     *  na chamada da função é válido.
     */

    // Primeiro, a função verifica se algum dos campos não está "pronto" para ser impresso:
    if(r.dataNascimento[0] == '\0')
    {
        r.dataNascimento[0] = '-';
        r.dataNascimento[1] = '\0';
    }

    printf("Nasceu em %s/%s, em %s, um bebê de sexo ", r.cidadeBebe, r.estadoBebe, r.dataNascimento);
    switch((int)r.sexoBebe)
    {
        case '0':
            printf("IGNORADO.\n");
            break;
        case '1':
            printf("MASCULINO.\n");
            break;
        case '2':
            printf("FEMININO.\n");
            break;
    }
}

void definirRegistroVazio(REGISTRO* r)
{
    /*
     *      Função que, dado um Registro 'r', inicializa todos campos do Registro como vazios.
     *      Esta função é útil para ser chamada antes da leitura de registros, para que impedir que o programa trabalhe com "lixo" na memória.
     */

    r->cidadeMae[0] = '-';r->cidadeMae[1] = '\0'; 
    r->cidadeBebe[0] = '-';r->cidadeBebe[1] = '\0'; 
    r->idNascimento = -1; 
    r->idadeMae = -1;
    r->dataNascimento[0] = '-'; r->dataNascimento[1] = '\0';
    r->sexoBebe = 0; 
    r->estadoMae[0] = '-'; r->estadoMae[1] = '\0';   
    r->estadoBebe[0] = '-'; r->estadoBebe[1] = '\0'; 
}

void inserirRegistro(REGISTRO* r, FILE* binario)
{

    char nomeCidades[97];          // Variável que armazenará as duas strings.

    // Insere os dois indicadores de tamanho dos campos com tamanho variável com um único acesso de disco.
    int tamanhos[2] = {r->tamanho_cidadeMae, r->tamanho_cidadeBebe}; 
    fwrite(tamanhos, sizeof(int), 2, binario);

    int contador = 0;

    // Insere '$' em todas posições da string
    for(int i = r->tamanho_cidadeBebe + r->tamanho_cidadeMae; i < 97; i++) nomeCidades[i] = '$';

    // Insere a cidade da Mãe na string, sobrescrevendo os '$'
    for(contador = 0; contador < tamanhos[0]; contador++) nomeCidades[contador] = r->cidadeMae[contador];
    
    // Insere a cidade do Bebe na string, sobrescrevendo os '$'
    for(contador; contador < (tamanhos[0] + tamanhos[1]); contador++) nomeCidades[contador] = r->cidadeBebe[contador - tamanhos[0]];

    fwrite(nomeCidades      , sizeof(char)     , 97, binario);
    fwrite(&r->idNascimento  , sizeof(int)      , 1, binario);
    fwrite(&r->idadeMae      , sizeof(int)      , 1, binario);
    fwrite(&r->dataNascimento, sizeof(char) , 10, binario);
    fwrite(&r->sexoBebe      , sizeof(char)     , 1, binario);
    fwrite(r->estadoMae      , sizeof(char) , 2, binario);  
    fwrite(r->estadoBebe     , sizeof(char) , 2, binario);
}

int lerRegistro(REGISTRO* r, FILE* binario)
{
    /*
     *      Função que, dado um arquivo já aberto, lê um Registro e armazena num parâmetro fornecido já inserindo o caractere '\0' no final das strings.
     *      A Leitura direta no arquivo será feita através de uma String auxiliar para que, caso o campo esteja vazio, o conteúdo no registro
     *  esteja corretamente formatado para a impressão ("-").
     *      Retorna 1  caso o registro lido esteja lógicamente removido.
     *      Retorna 0  caso a leitura tenha sido realizada com sucesso.
     *      Retorna -1 caso tenha ocorrido algum problema na leitura do arquivo ou o arquivo esteja no final.
     */

    if(fread(&r->tamanho_cidadeMae, sizeof(int), 1, binario) == 0)
        return -1;

    char auxiliar[101];
    if(r->tamanho_cidadeMae != -1)
    {
                fread(&r->tamanho_cidadeBebe, sizeof(int), 1, binario);

                auxiliar[0] = '\0';
                fread(auxiliar, sizeof(char), r->tamanho_cidadeMae, binario);
                if(auxiliar[0] != '\0')  // Não permite copiar strings vazias
                {
                    auxiliar[r->tamanho_cidadeMae] = '\0';
                    strcpy(r->cidadeMae, auxiliar);
                }

                auxiliar[0] = '\0';
                fread(auxiliar, sizeof(char), r->tamanho_cidadeBebe, binario);
                if(auxiliar[0] != '\0')  // Não permite copiar strings vazias
                {
                    auxiliar[r->tamanho_cidadeBebe] = '\0';
                    strcpy(r->cidadeBebe, auxiliar);
                }

                fseek(binario, (97 - r->tamanho_cidadeBebe - r->tamanho_cidadeMae), SEEK_CUR);
                fread(&r->idNascimento, sizeof(int), 1, binario);
                fread(&r->idadeMae, sizeof(int), 1, binario);

                auxiliar[0] = '\0';
                fread(auxiliar, sizeof(char), 10, binario);
                if(auxiliar[0] != '\0')  // Não permite copiar strings vazias
                {
                    auxiliar[10] = '\0';
                    strcpy(r->dataNascimento, auxiliar);
                }
                fread(&r->sexoBebe, sizeof(char), 1, binario);
                
                auxiliar[0] = '\0';
                fread(auxiliar, sizeof(char), 2, binario);
                if(auxiliar[0] != '\0')  // Não permite copiar strings vazias
                {
                    auxiliar[2] = '\0';
                    strcpy(r->estadoMae, auxiliar);
                }

                auxiliar[0] = '\0';
                fread(auxiliar, sizeof(char), 2, binario);
                if(auxiliar[0] != '\0')  // Não permite copiar strings vazias
                {
                    auxiliar[2] = '\0';
                    strcpy(r->estadoBebe, auxiliar);
                }            
                return 0;
    }else
    {
        return 1;
        fseek(binario, 124, SEEK_CUR);
    }
}

int processarParametros(int* idBuscaCampos, REGISTRO* reg)
{
    /*
     *      Função que serve para processar a entrada do Usuário na funcionalidade 3.
     *      Os dois parâmetros são dois ponteiros que serão alterados nesta função para serem usados no processamento do arquivo.
     *      
     *      A Função Retorna 0 caso tenha sido realizada corretamente e, caso contrário, retorna 1.
     */

    // Vetor constante de Strings. Cada string é o nome de um Campo do Arquivo. o ID do campo do Arquivo é a posição neste Array.
    const char *nomeCampos[8]; 
    nomeCampos[0] = "cidadeMae\0";
    nomeCampos[1] = "cidadeBebe\0";
    nomeCampos[2] = "idNascimento\0";
    nomeCampos[3] = "idadeMae\0";
    nomeCampos[4] = "dataNascimento\0";
    nomeCampos[5] = "sexoBebe\0";
    nomeCampos[6] = "estadoMae\0";
    nomeCampos[7] = "estadoBebe\0";

    char aux[20];    // Variável auxiliar para leitura de Strings.
    int  id; 
    int  numValores; // Variável que armazena o Número de Valores combinados na Busca.

    scanf("%d", &numValores);
        
    // Inicializa o vetor
    for(int i = 0; i < 8; i++) idBuscaCampos[i] = 0;

    // Laço que processará quais campos que serão considerados na busca e seus conteúdos.
    for(int i = 0; i < numValores; i++)
    {
        scanf("%s", aux);
        id = -1;
        // O Próximo Laço verificará qual é o nome do campo fornecido:
        for(int j = 0; j < 8 ; j++)
        {
            /*
             *      Caso a String Auxiliar seja igual a string na posição (j) no Array "nomeCampos":
             *      Atribuimos o valor '1' na posição do vetor corresponde ao campo indicado. 
             */            
            if(strcmp(aux, nomeCampos[j]) == 0)
            {
                if(idBuscaCampos[j] == 1) // Trata o caso de uma entrada inválida com o mesmo campo sendo buscado duas vezes.
                {
                    printf("Entrada Inválida, os campos não podem se repetir.\n");
                    return 1;
                }

                idBuscaCampos[j] = 1;
                id = j;
                j = 8;
            } 
            if(j == 7) 
            {
                printf("Campo inválido.");
                return  1;
            }
        }
        // Verifica qual campo do registro deverá ser lido em sequência.
        switch(id)
        {
            case 0:
                scan_quote_string(reg->cidadeMae);
                reg->tamanho_cidadeMae = strlen(reg->cidadeMae); // Define o tamanho da string
                break;
            case 1:
                scan_quote_string(reg->cidadeBebe);
                reg->tamanho_cidadeBebe = strlen(reg->cidadeBebe); // Define o tamanho da string
                break;
            case 2:
                scan_quote_string(aux);
                if(strcmp(aux, "") == 0) reg->idNascimento = -1;
                else reg->idNascimento = atoi(aux);
                break;
            case 3:
                scan_quote_string(aux);
                if(strcmp(aux, "") == 0) reg->idadeMae = -1;
                else reg->idadeMae = atoi(aux);
                break;
            case 4:
                scan_quote_string(reg->dataNascimento);
                if(strcmp(reg->dataNascimento, "") == 0)
                {
                    reg->dataNascimento[0] = '\0';
                    for(int j = 1; j < 10; j++) reg->dataNascimento[j] = '$';
                }
                break;
            case 5:
                scan_quote_string(&reg->sexoBebe);
                if(reg->sexoBebe == '\0') reg->sexoBebe = 0;
                break;
            case 6:
                scan_quote_string(reg->estadoMae);
                if(strcmp(reg->estadoMae, "") == 0)
                {
                    reg->estadoMae[0] = '\0';
                    reg->estadoMae[1] = '$';
                }
                break;
            case 7:
                scan_quote_string(reg->estadoBebe);
                if(strcmp(reg->estadoMae, "") == 0)
                {
                    reg->estadoBebe[0] = '\0';
                    reg->estadoBebe[1] = '$';
                }
                break;
            default:
                return 1;
        }
    }

    return 0;
}

// ================================================= FUNCIONALIDADE 1 ==============================================================================
void transfereDados(char* nomeArquivoCSV, char* nomeArquivoBinario, CABECALHO* cabecalho)
{

    /*
    *      Função que transfere os dados de um arquivo .txt ou .csv "entrada" para um arquivo .bin "registros".
    *     Os arquivos são abertos e fechados dentro da função.
    */

    REGISTRO  registroProcessado;  // Struct Registro que armazenará os dados já separados e processados de um dado registro. 
    char entradaDeString[129 + 7]; // String que armazenará uma uma "linha" do arquivo .txt. ou .csv, contendo os daods ainda não processados.
    int  tamanhoString = 0;        // Variável que servirá de Contador para determinar o tamanho dos campos de Tamanho Variável.
    char c;                        // Variável utilizada para percorrer o arquivo.csv byte a byte.

    FILE* entrada;
    FILE* binario = fopen(nomeArquivoBinario, "wb+");

    // Trata entradas de arquivos .csv inválidos.
    if(nomeArquivoCSV == NULL || !(entrada = fopen(nomeArquivoCSV, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
		return;
	}

    // Insere o Registro de Cabeçalho.
    char lixo[111];
    for(int i = 0; i < 111; i++) lixo[i] = '$';

    fseek(binario, 0, SEEK_SET);
    cabecalho->status = '0';

    fwrite(&cabecalho->status, sizeof(char), 1, binario);
    fwrite(&cabecalho->rrnProxRegistro, sizeof(int), 1, binario);
    fwrite(&cabecalho->numRegistrosInseridos, sizeof(int), 1, binario);
    fwrite(&cabecalho->numRegistrosRemovidos, sizeof(int), 1, binario);
    fwrite(&cabecalho->numRegistrosAtualizados, sizeof(int), 1, binario);
    fwrite(lixo, sizeof(char), 111, binario);
    // Termina a inserção do Registro de Cabeçalho.

    fgets(entradaDeString, sizeof(entradaDeString), entrada);

    while(fgets(entradaDeString, sizeof(entradaDeString), entrada) != NULL)
    {   
        // Processa a Cidade da Mãe: 
        c = entradaDeString[0];
        registroProcessado.cidadeMae[0] = c;
        while(c != ',')
        {
            c = entradaDeString[tamanhoString];
            if(c != ',')
            {
                registroProcessado.cidadeMae[tamanhoString] = c;
                tamanhoString++;
            }
        }
        
        registroProcessado.tamanho_cidadeMae = tamanhoString;
        registroProcessado.cidadeMae[tamanhoString] = '\0';
        tamanhoString++;

        // Processa a Cidade do Bebê:   
        c = entradaDeString[tamanhoString + 1];
        tamanhoString = 0;
    
        while(c != ',')
        {
            c = entradaDeString[tamanhoString + 1 + registroProcessado.tamanho_cidadeMae];
            if(c != ',')
            {
                registroProcessado.cidadeBebe[tamanhoString] = c;
                tamanhoString++;
            }
        }

        registroProcessado.cidadeBebe[tamanhoString] = '\0';
        registroProcessado.tamanho_cidadeBebe = tamanhoString;
        
        // Processa o ID de Nascimento:
        int offset = tamanhoString + registroProcessado.tamanho_cidadeMae + 2;
        char alphanumeric[5];
        int cont = 0;

        c = entradaDeString[offset];
        alphanumeric[0] = c;

        while(c != ',')
        {
            c = entradaDeString[offset + cont];
            if(c != ',')
            {
                alphanumeric[cont] = c;
                cont++;
            }
        }
        alphanumeric[cont] = '\0';

        registroProcessado.idNascimento = atoi(alphanumeric);

        // Processa a Idade da Mãe:
        offset += cont + 1;
        cont = 0;

        c = entradaDeString[offset];
        if(c == ',')
        {
            registroProcessado.idadeMae = -1;
            offset += cont + 1;
        }
        else
        {
            while(c != ',')
            {
                c = entradaDeString[offset + cont];
                if(c != ',')
                {
                    alphanumeric[cont] = c;
                    cont++;
                }
            }
            alphanumeric[cont] = '\0';
            offset+=cont + 1;
            registroProcessado.idadeMae = atoi(alphanumeric);
        }
        
        // Processa a Data de Nascimento do Bebê:
        if(entradaDeString[offset] == ',')
        {
            registroProcessado.dataNascimento[0] = '\0';
            for(int i = 2; i < 10; i++) registroProcessado.dataNascimento[i] = '$';
            offset++;
        }
        else
        {
            for(int i = 0; i < 10 ; i++) registroProcessado.dataNascimento[i] = entradaDeString[offset + i];        
            offset += 11;
        }

        // Processa o Sexo do Bebê:
        if(entradaDeString[offset] == ',')
        {
            registroProcessado.sexoBebe = '0';
            offset++;
        }
        else 
        {
            registroProcessado.sexoBebe = entradaDeString[offset];
            offset += 2;
        }

        // Processa o Estado da Mãe:
        if(entradaDeString[offset] == ',')
        {
            registroProcessado.estadoMae[0] = '\0';
            registroProcessado.estadoMae[1] = '$';
            registroProcessado.estadoMae[2] = '$';
            offset++;
        }
        else
        {
            registroProcessado.estadoMae[0] = entradaDeString[offset];
            registroProcessado.estadoMae[1] = entradaDeString[offset + 1];
            registroProcessado.estadoMae[2] = '\0';
            offset += 3;
        }
        
        // Processa o Estado do Bebê: 
        if(entradaDeString[offset] == ',' || entradaDeString[offset] == '\n')
        {
            registroProcessado.estadoBebe[0] = '\0';
            registroProcessado.estadoBebe[1] = '$';
            registroProcessado.estadoBebe[2] = '$';
        }
        else
        {
            registroProcessado.estadoBebe[0] = entradaDeString[offset];
            registroProcessado.estadoBebe[1] = entradaDeString[offset + 1];
            registroProcessado.estadoBebe[2] = '\0';    
        }
        // Fim da leitura e processamentos dos dados do arquivo .csv


        // Agora os dados serão escritos no registro:
        
        fseek(binario, 0, SEEK_END);
       
        inserirRegistro(&registroProcessado, binario);

        cabecalho->numRegistrosInseridos++;
        cabecalho->rrnProxRegistro++;
        tamanhoString = 0; 
    }  
    
    fseek(binario, 0, SEEK_SET);
    cabecalho->status = '1';

    fwrite(&cabecalho->status, sizeof(char)              , 1, binario);
    fwrite(&cabecalho->rrnProxRegistro, sizeof(int)      , 1, binario);
    fwrite(&cabecalho->numRegistrosInseridos, sizeof(int), 1, binario);

    fclose(binario);
    fclose(entrada);

    binarioNaTela(nomeArquivoBinario);
}

// ================================================= FUNCIONALIDADE 2 ==============================================================================
void imprimeBinario(char* nomeArquivo)
{
    /*
     *      Função que, dada um nome de um arquivo, imprime todos registros desse arquivos na formatação de saída desejada.
     */

    FILE* binario;
    REGISTRO r;
    
    // Trata os casos em que o arquivo fornecido é inválido
    if(nomeArquivo == NULL || !(binario = fopen(nomeArquivo, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
		return;
	}

    // Variável para ler o status no cabeçalho do arquivo.
    char c;
    // Array que armazena o nome do Tamanho do nome da cidade da mãe e do tamanho donome da cidade do bebê.
    int tamanhoString[2];
    // Armazena os outros valores do Cabeçalho.
    int num[4];
    // Variável auxiliar que armazena o status da leitura do registro
    int regStatus;
    int cont = 0;

    if(binario != NULL)
    {
        fseek(binario, 0, SEEK_SET);

        fread(&c, sizeof(char), 1, binario);
        if(c == '0')
        {
            fclose(binario);
            printf("Falha no processamento do arquivo.\n");
            return;
        }        
        fread(num, sizeof(int), 4, binario);

        fseek(binario, 128, SEEK_SET);

        // Percorre o aqruivo, imprimindo os registros válidos;
        do
        {
            definirRegistroVazio(&r);
            regStatus = lerRegistro(&r, binario);
            if(regStatus == 0)
            {
                printf("%d\t", cont);
                imprimeRegistro(r);
            }
            cont++;
        } while (regStatus != -1);
        
        fclose(binario); return;
    }
    fclose(binario);
}

// ================================================= FUNCIONALIDADE 3 ==============================================================================
int buscaCombinada(char* nomeArquivo, int remocao)
{
    /*
     *      Função que, Receba a entrada do Usuário, e, após armazenas o conteúdo dos campos usados como parâmetros de busca,
     *   percorre o arquivo inteiro e, imprime os Registros que são compatíveis com o parâmetro. 
     *      Para o caso de Remoção, o arquivo valor do status é marcado.     *    
     * 
     *      nomeArquivo: Armazena o nome do arquivo a ser processado.
     *      remocao   : Deve receber 1 caso para ser feita a remoção lógica do registro (FUNCIONALIDADE 5).
     *                  Deve receber 0 caso para que o registro seja impresso.
     *      
     *       RETORNO: Retorna 1 caso haja alguma falha no procesamento do arquivo.
     *                Retorna 0 caso um registro seja encontrado e não haja nenhum problema 
     *      OBS: Esta função também será utilizada na Funcionalidade 5.
     */

    // Abre o arquivo e verifica se este é válido.
    FILE* binario;
    if(nomeArquivo == NULL || !(binario = fopen(nomeArquivo, "rb+"))) {
        printf("Falha no processamento do arquivo.\n");
		return 1;
	}
    
    REGISTRO parametrosDeBusca; // Variável que armazenará um Registro com o valor de cada Campo a ser buscado.
    REGISTRO registroLido;      // Variável que armazenará o conteúdo completo de um registro a ser impresso na tela.
    int idBuscaCampos[8];       // Vetor que armazenará, quais campos deverão ser comparados. 1- Caso o campo deva ser comparado, 0 - Caso Contrário
    int verif;                  // Variável inteira que armazenará 1 caso o registro deva ser impresso na tela e, caso contrário, 0.
    int registroInexistente;    // Variável inteira auxiliar que armazenará 1 caso nenhum registro tenha sido imprimido e, caso contrário, 0. 
    int auxiliar;               // Variável auxiliar que servirá para verificar se o registro foi removido lógicamente.
    char status;
    int  numRegistrosRemovidos;

    // Recebe os Parametros do usuário.
    if(processarParametros(idBuscaCampos, &parametrosDeBusca)) 
    {
        // Não há nenhum parâmetro.
        fclose(binario);
        return 0;
    }
    fseek(binario, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, binario);
    
    // Verifica o status do arquivo antes de realizar as operações.
    if(status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(binario);
        return 1;
    }

    if(remocao) // Caso a operação realizada seja a de Remoção, marca o arquivo como inconsistente e recebe o número de Registros Removidos
    {
        fseek(binario, 0, SEEK_SET);
        status = '0';
        fwrite(&status, sizeof(char), 1, binario);

        fseek(binario, 9, SEEK_SET);
        fread(&numRegistrosRemovidos, sizeof(int), 1, binario);
    }

    fseek(binario, 128, SEEK_SET);
    registroInexistente = 1;

    /*
     *      O Laço abaixo servirá para percorrer o arquivo identificando os campos desejados na busca.
     *      A cada campo lido, verificaremos se este campo é um parâmetro de busca, se sim, compararemos o conteúdo e iremos imprimir, ou não,
     *  o registro.
     */
    while(fread(&auxiliar, sizeof(int), 1, binario) != 0)
    {
        verif = 1;
        definirRegistroVazio(&registroLido);

        // Verifica se o registro foi removido lógicamente
        if(auxiliar != -1)
        {
            registroLido.tamanho_cidadeMae = auxiliar;
            fread(&registroLido.tamanho_cidadeBebe, sizeof(int), 1, binario);
            // Campo cidadeMae: 
            fread(registroLido.cidadeMae, sizeof(char), registroLido.tamanho_cidadeMae, binario);
            registroLido.cidadeMae[registroLido.tamanho_cidadeMae] = '\0';
            if(idBuscaCampos[0]) // Verifica se o campo conta para a busca 
            {
                if(strcmp(registroLido.cidadeMae, parametrosDeBusca.cidadeMae) != 0) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            // Campo cidadeBebe:
            fread(registroLido.cidadeBebe, sizeof(char), registroLido.tamanho_cidadeBebe, binario);
            registroLido.cidadeBebe[registroLido.tamanho_cidadeBebe] = '\0';
            if(idBuscaCampos[1]) // Verifica se o campo conta para a busca 
            {   
                if(strcmp(registroLido.cidadeBebe, parametrosDeBusca.cidadeBebe) != 0) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            fseek(binario, (97 - registroLido.tamanho_cidadeBebe - registroLido.tamanho_cidadeMae), SEEK_CUR);
            // Campo idNascimento:
            fread(&registroLido.idNascimento, sizeof(int), 1, binario);
            if(idBuscaCampos[2]) // Verifica se o campo conta para a busca 
            {
                if(registroLido.idNascimento != parametrosDeBusca.idNascimento) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            // Campo idadeMae
            fread(&registroLido.idadeMae, sizeof(int), 1, binario);
            if(idBuscaCampos[3]) // Verifica se o campo conta para a busca
            {
                if(registroLido.idadeMae != parametrosDeBusca.idadeMae) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            // Campo dataNascimento
            fread(registroLido.dataNascimento, sizeof(char), 10, binario);
            registroLido.dataNascimento[10] = '\0';
            if(idBuscaCampos[4]) // Verifica se o campo conta para a busca
            {
                if(strcmp(registroLido.dataNascimento, parametrosDeBusca.dataNascimento) != 0) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            // Campo sexoBebe
            fread(&registroLido.sexoBebe, sizeof(char), 1, binario);
            if(idBuscaCampos[5]) // Verifica se o campo conta para a busca
            {
                if(registroLido.sexoBebe != parametrosDeBusca.sexoBebe) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            // Campo estadoMae
            fread(registroLido.estadoMae, sizeof(char), 2, binario);
            registroLido.estadoMae[2] = '\0';
            if(idBuscaCampos[6]) // Verifica se o campo conta para a busca
            {
                if(strcmp(registroLido.estadoMae, parametrosDeBusca.estadoMae) != 0) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
            // Campo estadoBebe
            fread(registroLido.estadoBebe, sizeof(char), 2, binario);
            registroLido.estadoBebe[2] = '\0';
            if(idBuscaCampos[7]) // Verifica se o campo conta para a busca
            {
                if(strcmp(registroLido.estadoBebe, parametrosDeBusca.estadoBebe) != 0) // Verifica se o conteúdo dos campos é igual.
                    verif = 0;
            }
        }else // Trata os casos de registros removidos logicamente.
        {
            fseek(binario, 124, SEEK_CUR);
            verif = 0;
        }
        if(verif)
        {
            if(remocao) // Verifica qual Operação deve ser realizada.
            {
                // Remoção:
                int removido = -1;
                fseek(binario, -128, SEEK_CUR);
                fwrite(&removido, sizeof(int), 1, binario);
                fseek(binario, 124, SEEK_CUR);

                numRegistrosRemovidos++;
                registroInexistente = 0;
            }
            else
            {       
                // Impressão: 
                imprimeRegistro(registroLido);
                registroInexistente = 0;
            }
        }
    }

    if(remocao) // Marca o arquivo como consistente após a remoção lógica e atualiza o número de Registros removidos.
    {
        // Status:
        status = '1';
        fseek(binario, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, binario);

        // Numero de registros removidos: 
        fseek(binario, 9, SEEK_SET);
        //fwrite(&numRegistrosRemovidos, sizeof(int), 1, binario);
        fclose(binario);
        if(registroInexistente) return 1;
        else return 0;
    }else
    {
        fclose(binario);
        if(registroInexistente)
        {
            printf("Registro Inexistente.\n");
            return 1;
        }else
        {
            return 0;
        }
        
    }
}

// ================================================= FUNCIONALIDADE 4 ==============================================================================

void buscaRRN(char* nomeArquivo, int RRN)
{
    /*
     *      Função que abre um arquivo binário, dado seu nome e, imprime o registro correspondente ao RRN fornecido.
     */

    // Abre o arquivo e verifica se este é válido.
    FILE* binario;
    REGISTRO reg;
    int regStatus;
    if(nomeArquivo == NULL || !(binario = fopen(nomeArquivo, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
		return;
	}

    // Vai para o Registro com RRN fornecido, pulando o cabeçalho
    fseek(binario, (RRN+1) * 128, SEEK_SET);
    definirRegistroVazio(&reg);

    regStatus = lerRegistro(&reg, binario);
    if(regStatus == 1)
        printf("Registro Inexistente.\n");
    else
    {
        if(regStatus == 0)
            imprimeRegistro(reg);
        else 
            printf("Registro Inexistente.\n");
    }    
    fclose(binario);
}

// ================================================= FUNCIONALIDADE 5 ==============================================================================

void remocaoPorParametros(char* nomeArquivo)
{
    /*
     *      Esta função possui uma funcionalidade simples, ela apenas receberá, do usuário, o valor "n" e chamará a função de busca
     *  n vezes.
     */

    int numeroDeRemocoes; // Variável que armazena o número de remoções a serem realizadas.
    int verif = 1;        // Recebe 1 se nenhum registro foi imprimido, caso contrário, recebe 0.
    FILE* binario;

    // Verifica se o arquivo fornecido é inválido.    
    if(nomeArquivo == NULL || !(binario = fopen(nomeArquivo, "rb+"))) {
        printf("Falha no processamento do arquivo.\n");
		return;
	}

    scanf("%d", &numeroDeRemocoes);
    for(int i = 0; i < numeroDeRemocoes; i++)
    {
        if(buscaCombinada(nomeArquivo, 1)) break;
    }

    binarioNaTela(nomeArquivo);
    return;
}

// ================================================= FUNCIONALIDADE 6 ==============================================================================

void insercaoAdicional(char* nomeArquivo)
{
    /*
     *      Função que recebe do usuário um valor n de registros, e os insere no final do arquivo.
     *      Além disso, a função altera os Campos de registrosInseridos e de proximoRRN do cabeçalho.
     */

    FILE*    binario;
    REGISTRO novo;

    if(nomeArquivo == NULL || !(binario = fopen(nomeArquivo, "rb+"))) {
        printf("Falha no processamento do arquivo.\n");
		return;
	}

    int  numInsercoes;       // Variavel que armazena o número de inserções a serem realizadas.
    int  cabecalhoInsercoes; // Armazena o numero de inserções, armazenadas no cabeçalho. Será utilizado para atualizar após a inserção.
    int  cabecalhoRRN;       // Armazena o numero de inserções, armazenadas no cabeçalho. Será utilizado para atualizar após a inserção.
    char auxiliar[11];       // Variável auxiliar que servirá para tratar entradas nulas
    char status;

    fseek(binario, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, binario);

    if(status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(binario);
        return;
    }else
    {
        status = '0';
        fseek(binario, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, binario);
        fseek(binario, 1, SEEK_SET);
        fread(&cabecalhoRRN, sizeof(int), 1, binario);
        fseek(binario, 5, SEEK_SET);
        fread(&cabecalhoInsercoes, sizeof(int), 1, binario);
        scanf("%d", &numInsercoes);

        for(int i = 0; i < numInsercoes; i++)
        {
            // O Trecho a seguir servirá para receber os dados do Registro do usuário.
            
            // Campo cidadeMae
            scan_quote_string(novo.cidadeMae);
            if(strcmp("", novo.cidadeMae) == 0) // Trata Casos nulos
            {
                novo.cidadeMae[0] = '\0';
                novo.tamanho_cidadeMae = 0;
            }else
            {
                novo.tamanho_cidadeMae = strlen(novo.cidadeMae);
                novo.cidadeMae[novo.tamanho_cidadeMae] = '\0';
            }
            // Campo cidadeBebe
            scan_quote_string(novo.cidadeBebe);
            if(strcmp("", novo.cidadeBebe) == 0) // Trata Casos nulos
            {
                novo.cidadeBebe[0] = '\0';
                novo.tamanho_cidadeBebe = 0;
            }else
            {
                novo.tamanho_cidadeBebe = strlen(novo.cidadeBebe);
                novo.cidadeBebe[novo.tamanho_cidadeBebe] = '\0';
            }
            // Campo idNascimento  
            scanf("%s", auxiliar);
            if(auxiliar[0] == 'N' || auxiliar[0] == 'n')
                novo.idNascimento = -1; // NULO
            else novo.idNascimento = atoi(auxiliar);  // Valor válido
            // Campo idadeMae
            scanf("%s", auxiliar);
            if(auxiliar[0] == 'N' || auxiliar[0] == 'n')
                novo.idadeMae = -1; // NULO
            else novo.idadeMae = atoi(auxiliar); // Valor Válido
            // Campo dataNascimento
            scan_quote_string(novo.dataNascimento);
            if(strcmp("", novo.dataNascimento) == 0) // Trata Casos nulos
            {
                novo.dataNascimento[0] = '\0';
                for(int i = 1; i < 10; i++) novo.dataNascimento[i] = '$';
            }else novo.dataNascimento[10] = '\0';
            // Campo sexoBebe
            scan_quote_string(&novo.sexoBebe);
            if(novo.sexoBebe == '\0') // Trata Casos nulos
            {
                novo.sexoBebe = '0';
            }
            // Campo estadoMae
            scan_quote_string(novo.estadoMae);
            if(strcmp("", novo.estadoMae) == 0) // Trata Casos nulos
            {
                novo.estadoMae[0] = '\0';
                novo.estadoMae[1] = '$'; novo.estadoMae[2] = '$';
            }else novo.estadoMae[2] = '\0';
            // Campo estadoBebe
            scan_quote_string(novo.estadoBebe);
            if(strcmp("", novo.estadoBebe) == 0) // Trata Casos nulos
            {
                novo.estadoBebe[0] = '\0';
                novo.estadoBebe[1] = '$'; novo.estadoBebe[2] = '$';
            }else novo.estadoBebe[2] = '\0';

            // Insereno final do arquivo e faz as iteraçoes dos campos do cabeçalho
            fseek(binario, 0, SEEK_END);
            inserirRegistro(&novo, binario);
            cabecalhoRRN++;
            cabecalhoInsercoes++;
        }

        // O Trecho abaixo atualizará o registro de cabeçalho
        status = '1';
        fseek(binario, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, binario);
        fseek(binario, 1, SEEK_SET);
        fwrite(&cabecalhoRRN, sizeof(int), 1, binario);
        fseek(binario, 5, SEEK_SET);
        fwrite(&cabecalhoInsercoes, sizeof(int), 1, binario);

        fclose(binario);
        binarioNaTela(nomeArquivo);
        return;
    }    
}

// ================================================= FUNCIONALIDADE 7 ==============================================================================

void registroAtualizar(char* nomeArquivo)
{
    /*
     *      Função que serve para atualizar registros no arquivo.
     *      O usuário deve fornecer o RRN do registro e os campos em específico que devem ser atualizados.
     *      Assim, serão processados os parâmetros fornecidos pelo usuário, e alterados os registros fornecidos, lembrando de atualizar o cabeçalho e
     *  impedir "overflow" no arquivo, ou seja, que os campos de tamanho variável fornecidos não sobrecrevam os campos de tamanho fixo.
     */

    FILE* binario;
    REGISTRO mudancas; // Armazenará os novos Campos, que deverão ser inseridos no RRN fornecido.

    int idBuscaCampos[8]; // Vetor que armazenará, quais campos deverão ser atualizados. 1- Caso o campo deva ser atualizado, 0 - Caso Contrário
    int RRN;              // Variável que armazena o RRN do registro em que as atualizações serão atualizadas
    int cabecalhoAtualizacoes; // Variável que armazena valor, do cabeçalho, de registros atualizados, servirá para atualizar o cabeçalho no final da funçao.
    int numAtualizacoes; // Variável que armazena o número de atualizaçoes que serão feitas.
    int cabecalhoRRN;    // Armazenará o valor "rrnProximoRegistro" do cabeçalho.
    char* cidadeBebe;     // String que armazenará o valor do campo "ciadadeBebe" para impedir conflitos neste campo ao atualizar o campo "cidadeMae".
    int tamanho_cidadeBebe;
    int tamanho_cidadeMae;
    char status;
    char lixo = '$';

    if(nomeArquivo == NULL || !(binario = fopen(nomeArquivo, "rb+"))) {
        printf("Falha no processamento do arquivo.\n");
		return;
	}
    // Recebe as informações necessárias do cabeçalho
    fseek(binario, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, binario);
    // Checa o status do aqruivo antes de realizar alguma operação.
    if(status == '0') 
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(binario);
        return;
    }
    fseek(binario, 0, SEEK_SET); status = '0';
    fwrite(&status, sizeof(char), 1, binario); // Marca o arquivo como inconsistente
    // Recebe as informaçoes relevantes do cabeçalho: 
    fseek(binario, 13, SEEK_SET);
    fread(&cabecalhoAtualizacoes, sizeof(int), 1, binario);

    scanf("%d",&numAtualizacoes);

    for(int i = 0; i < numAtualizacoes; i++)
    {
        scanf("%d", &RRN);
        if(processarParametros(idBuscaCampos, &mudancas))
        {
            printf("Falha no processamento do arquivo.\n");
            fclose(binario);
            return;
        }
                
        if(RRN < cabecalhoRRN)
        {
            fseek(binario, (RRN + 1) * 128, SEEK_SET); // Vai para o Registro indicado (Pulando o cabeçalho).
            fread(&tamanho_cidadeMae, sizeof(int), 1, binario);
            if(tamanho_cidadeMae != -1) // Verifica se o registro foi removido.
            {
                fread(&tamanho_cidadeBebe, sizeof(int), 1, binario);
                // Campo cidadeMae
                if(idBuscaCampos[0]) // Verifica se o registro deve ser atualizado e que o tamanho da string é valido.
                {
                   // Antes da atualização do registro, devemos armazenar o campo "cidadeBebe"
                    cidadeBebe = (char*) malloc(sizeof(char) * tamanho_cidadeBebe);
                    fseek(binario, (sizeof(char) * tamanho_cidadeMae), SEEK_CUR);
                    fread(cidadeBebe, sizeof(char), tamanho_cidadeBebe, binario); // Armazena o Campo "ciadadeBebe"
                    // Atualiza o campo CidadeMae: 
                    fseek(binario, (RRN + 1)*128, SEEK_SET); // Retorna para o início do registro
                    fwrite(&mudancas.tamanho_cidadeMae, sizeof(int), 1, binario);
                    fseek(binario, sizeof(int), SEEK_CUR); // Pula para o campo "cidadeMae"
                    fwrite(mudancas.cidadeMae, sizeof(char), mudancas.tamanho_cidadeMae, binario);

                    fwrite(cidadeBebe, sizeof(char), tamanho_cidadeBebe, binario);
                    tamanho_cidadeMae = mudancas.tamanho_cidadeMae;
                    fseek(binario, -tamanho_cidadeBebe, SEEK_CUR);
                }else // Caso não deva ser feita nenhuma atualização, apenas "pulamos" para o proximo Campo.
                    fseek(binario, sizeof(char) * tamanho_cidadeMae, SEEK_CUR);
                
                // Campo cidadaeBebe
                if(idBuscaCampos[1]) // Verifica se o registro deve ser atualizado e que o tamanho da string é valido.
                {
                    // Atualiza o Tamanho do campo "cidadeBebe"
                    fseek(binario, (RRN + 1)*128 + 4, SEEK_SET);
                    fwrite(&mudancas.tamanho_cidadeBebe, sizeof(int), 1, binario);
                    // Atualiza o campo "cidadeBebe" em si:
                    fseek(binario, sizeof(char) * tamanho_cidadeMae, SEEK_CUR);
                    fwrite(mudancas.cidadeBebe, sizeof(char), mudancas.tamanho_cidadeBebe, binario);
                    tamanho_cidadeBebe = mudancas.tamanho_cidadeBebe;
                }else
                { 
                    fseek(binario, sizeof(char) * tamanho_cidadeBebe, SEEK_CUR);
                }
                fseek(binario, (RRN + 1) * 128 + 105, SEEK_SET); // Pula o Lixo contido no registro.

                // Campo idNascimento
                if(idBuscaCampos[2])
                    fwrite(&mudancas.idNascimento, sizeof(int), 1, binario);
                else
                    fseek(binario, sizeof(int), SEEK_CUR);
                // Campo idadeMae
                if(idBuscaCampos[3])
                {    
                    fwrite(&mudancas.idadeMae, sizeof(int), 1, binario);
                }else
                    fseek(binario, sizeof(int), SEEK_CUR);
                // Campo dataNascimento
                if(idBuscaCampos[4])
                    fwrite(mudancas.dataNascimento, sizeof(char), 10, binario);
                else
                    fseek(binario, 10, SEEK_CUR);
                // Campo sexoBebe
                if(idBuscaCampos[5])
                    fwrite(&mudancas.sexoBebe, sizeof(char), 1, binario);
                else
                    fseek(binario, 1, SEEK_CUR);
                // Campo estadoMae
                if(idBuscaCampos[6])
                    fwrite(mudancas.estadoMae, sizeof(char), 2, binario);
                else
                    fseek(binario, 2, SEEK_CUR);
                // Campo estadoBebe
                if(idBuscaCampos[7])
                    fwrite(mudancas.estadoBebe, sizeof(char), 2, binario);
                else
                    fseek(binario, 2, SEEK_CUR);

                cabecalhoAtualizacoes++;
            }
        }
    }  
    fseek(binario, 0, SEEK_SET);
    status = '1';
    fwrite(&status, sizeof(char), 1, binario);
    fseek(binario, 13, SEEK_SET);
    fwrite(&cabecalhoAtualizacoes, sizeof(int), 1, binario);

    fclose(binario);
    binarioNaTela(nomeArquivo);
}

int main(int argc, char* argv[])
{
    // Variável que Armazena o comando correspondente a uma das funcionalidades suportadas pelo programa.
    int comando;
    int RRN;
    CABECALHO cabecalho;

    /*
     *      Inicializa os valores do cabeçalho
     */
    cabecalho.status                  = '1';
    cabecalho.rrnProxRegistro         = 0;
    cabecalho.numRegistrosInseridos   = 0;
    cabecalho.numRegistrosRemovidos   = 0;
    cabecalho.numRegistrosAtualizados = 0;

    // As duas proximas strings armazenam os nomes dos arquivos que podem ser abertos.
    char nomeArquivoCSV[67];
    char nomeArquivoBinario[67];
    
    scanf("%d", &comando);

    switch(comando)
    {   
        case 1:

            scanf("%s %s", nomeArquivoCSV, nomeArquivoBinario);
            
            transfereDados (nomeArquivoCSV, nomeArquivoBinario, &cabecalho);
            break;

        case 2:

            scanf("%s", nomeArquivoBinario);
            imprimeBinario(nomeArquivoBinario);
            break;

        case 3:

            scanf("%s", nomeArquivoBinario);
            buscaCombinada(nomeArquivoBinario, 0);
            break;
        case 4:

            scanf("%s", nomeArquivoBinario);
            scanf("%d", &RRN);
            buscaRRN(nomeArquivoBinario, RRN);
            break;
        case 5:

            scanf("%s", nomeArquivoBinario);
            remocaoPorParametros(nomeArquivoBinario);
            break;

        case 6:
            scanf("%s", nomeArquivoBinario);
            insercaoAdicional(nomeArquivoBinario);
            break;
        case 7:
            scanf("%s", nomeArquivoBinario);
            registroAtualizar(nomeArquivoBinario);
            break;
            
    }
    return 0;
}