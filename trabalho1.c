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
 
    int      tamanho_cidadeMae;
    int      tamanho_cidadeBebe;
    char     cidadeMae[100];
    char     cidadeBebe[100];
    int      idNascimento;
    int      idadeMae;
    char     dataNascimento[11];
    char     sexoBebe;
    char     estadoMae[3];
    char     estadoBebe[3];

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

void transfereDados(char* nomeArquivoCSV, char* nomeArquivoBinario, CABECALHO* cabecalho)
{

    /*
    *      Função que transfere os dados de um arquivo .txt ou .csv "entrada" para um arquivo .bin "registros".
    *     Os arquivos são abertos e fechados dentro da função.
    */

    // Struct Registro que armazenará os dados já separados e processados de um dado registro.
    REGISTRO  registroProcessado;
    
    // String que armazenará uma uma "linha" do arquivo .txt. ou .csv, contendo os daods ainda não processados.
    char entradaDeString[129 + 7];
    
    // Variável que servirá de Contador para determinar o tamanho dos campos de Tamanho Variável.
    int  tamanhoString = 0;
    
    // Variável que armazenará as duas strings.
    char nomeCidades[97];

    char c;
    
    FILE* entrada   = fopen(nomeArquivoCSV, "r");
    FILE* binario = fopen(nomeArquivoBinario, "wb+");

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
            for(int i = 1; i < 10; i++) registroProcessado.dataNascimento[i] = '$';
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
        

        // Insere os dois indicadores de tamanho dos campos com tamanho variável com um único acesso de disco.
        int tamanhos[2] = {registroProcessado.tamanho_cidadeMae, registroProcessado.tamanho_cidadeBebe}; 
        fwrite(tamanhos, sizeof(int), 2, binario);

        int contador = 0;

        // Insere '$' em todas posições da string
        for(int i = registroProcessado.tamanho_cidadeBebe + registroProcessado.tamanho_cidadeMae; i < 97; i++) nomeCidades[i] = '$';
   
        // Insere a cidade da Mãe na string, sobrescrevendo os '$'
        for(contador = 0; contador < tamanhos[0]; contador++) nomeCidades[contador] = registroProcessado.cidadeMae[contador];
        
        // Insere a cidade do Bebe na string, sobrescrevendo os '$'
        for(contador; contador < (tamanhos[0] + tamanhos[1]); contador++) nomeCidades[contador] = registroProcessado.cidadeBebe[contador - tamanhos[0]];
        contador = 0;
      
        
        fwrite(nomeCidades                       , sizeof(char)     , 97, binario);
        
        fwrite(&registroProcessado.idNascimento  , sizeof(int)      , 1, binario);

        fwrite(&registroProcessado.idadeMae      , sizeof(int)      , 1, binario);

        fwrite(&registroProcessado.dataNascimento, sizeof(char) * 10, 1, binario);

        fwrite(&registroProcessado.sexoBebe      , sizeof(char)     , 1, binario);

        fwrite(registroProcessado.estadoMae      , sizeof(char) * 2 , 1, binario);
        
        fwrite(registroProcessado.estadoBebe     , sizeof(char) * 2 , 1, binario);
        // Fim da Inserção em Disco

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

        while(fread(&r.tamanho_cidadeMae, sizeof(int), 1, binario) != 0)
        {

            if( r.tamanho_cidadeMae != -1 ) // Impede a leitura de um registro removido logicamente.
            {
                fread(&r.tamanho_cidadeBebe, sizeof(int), 1, binario);

                
                if(r.tamanho_cidadeMae == 0)
                {
                    r.cidadeMae[0] = '-';
                    r.cidadeMae[1] = '\0';
                }else
                {
                    fread(r.cidadeMae , sizeof(char), r.tamanho_cidadeMae , binario);
                    r.cidadeMae[r.tamanho_cidadeMae] = '\0';
                } 

                if(r.tamanho_cidadeBebe == 0)
                {
                    r.cidadeBebe[0] = '-';
                    r.cidadeBebe[1] = '\0';
                }else
                {
                    r.cidadeBebe[r.tamanho_cidadeBebe] = '\0';
                    fread(r.cidadeBebe, sizeof(char), r.tamanho_cidadeBebe, binario);
                } 


                fseek(binario, (97 - r.tamanho_cidadeBebe - r.tamanho_cidadeMae), SEEK_CUR);

                fread(&r.idNascimento, sizeof(int), 1, binario);
                fread(&r.idadeMae, sizeof(int), 1, binario);
                fread(r.dataNascimento, sizeof(char), 10, binario);
                if(r.dataNascimento[0] == '\0')
                {
                    r.dataNascimento[0] = '-'; r.dataNascimento[1] = '\0';
                }else r.dataNascimento[10] = '\0';
                fread(&r.sexoBebe, sizeof(char), 1, binario);
                fread(r.estadoMae, sizeof(char), 2, binario);
                if(r.estadoMae[0] == '\0')
                {
                    r.estadoMae[1] = '\0'; r.estadoMae[0] = '-';
                }
                else r.estadoMae[2] = '\0';

                fread(r.estadoBebe, sizeof(char), 2, binario);
                if(r.estadoBebe[0] == '\0')
                {
                    r.estadoBebe[0] = '-'; r.estadoBebe[1] = '\0';
                } 
                else r.estadoBebe[2] = '\0';

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
        }
        fclose(binario); return;
    }
    fclose(binario);
}

int main(int argc, char* argv[])
{
    // Variável que Armazena o comando correspondente a uma das funcionalidades suportadas pelo programa.
    int comando;

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
    }

    return 0;
}