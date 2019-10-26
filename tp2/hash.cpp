#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <regex>
#define LINE_SIZE 99999
#define BUCKET_SIZE 2 //Tamanho do bloco de disco (4096 bytes) / Tamanho de cada registro (1508 bytes) = 3016
#define BUCKET_BLOCKS 2 //Blocos por bucket
using namespace std;

struct artigo{                  //Struct para guardar cada registro
    int id;
    char titulo [300];
    int ano;
    char autores [150];
    int citacoes;
    char atualizacao [20];
    char snippet [1024];
};

struct bloco{                       //Struct de bloco de bucket
    artigo registros[BUCKET_SIZE];
};

struct bucket {                     //Struct de bucket
    bloco blocos [BUCKET_BLOCKS];   //3016 bytes * 2 = 6032
    int overflow_pos;               //4 bytes
    char resto [2156];              //2156 de espaco sobrando no 2º bloco
                                    //1 Bucket ocupa 6036 bytes e deixa de usar 2156 bytes 
};

int primo(int num){                             //Funcao de primo para melhorar o hashing
    for (int i = 2; i <= num / 2; i++) {
        if (num % i == 0) {
        return 0;
        }
    }
    return 1;
}

artigo PegarCampos(string line,regex reg, FILE *bd){    // Entra no arquivo CSV, verifica e trata erros dos campos
    regex no_x00  ("(\\x00)");                          // e retorna uma struct de registro
    line = regex_replace(line,no_x00," ");
    regex noValue ("(;;)");
    line = regex_replace(line,noValue,";NULL;");
    regex endl_error ("([^\"]\n | [^NULL]\n )");
    string no_endl_error = regex_replace(line,endl_error," ");
    if(line != no_endl_error){
        char linha_quebrada [999999];
        fgets ( linha_quebrada, 999999,bd );
        line = line.substr(0, line.size()-1);
        line += linha_quebrada ;
    }  
    int os_matches = 0;
    smatch matches;
    artigo article;
    while(regex_search(line, matches, reg)){
        os_matches += 1;
        if(os_matches == 1){
            string numero =  matches.str(1).substr(1,  matches.str(1).size()-2);
            stringstream conversao(numero);
            conversao >> article.id ;
        }
        if(os_matches == 2) {
            for (int i = 0; i < matches.str(1).size()-2; i++) { 
                article.titulo[i] = matches.str(1)[i+1];  
            }
        }
        if(os_matches == 3) {
            string numero =  matches.str(1).substr(1,  matches.str(1).size()-2);
            stringstream conversao(numero);
            conversao >> article.ano;
         }
        if(os_matches == 4) {
            for (int i = 0; i < matches.str(1).size()-2; i++) { 
                article.autores[i] = matches.str(1)[i+1];  
            }
        }
        if(os_matches == 5){
            string numero =  matches.str(1).substr(1,  matches.str(1).size()-2);
            stringstream conversao(numero);
            conversao >> article.citacoes;
        }
        if(os_matches == 6){
            for (int i = 0; i < matches.str(1).size()-2; i++) { 
                article.atualizacao[i] = matches.str(1)[i+1];  
            }
        }
        if(os_matches == 7){
            for (int i = 0; i < matches.str(1).size()-2; i++) { 
                article.snippet[i] = matches.str(1)[i+1];  
            }
        }
        line = matches.suffix().str();
    }
    if(os_matches < 7){
        exit(0);
    }
    return article;
  }


int inserir(bucket &bucket_destino, artigo paper){                              //Adiciona um artigo em um bucket 
    if(bucket_destino.blocos[paper.id%BUCKET_BLOCKS].registros[0].id == 0){      //que tenha espaço disponivel
        bucket_destino.blocos[paper.id%BUCKET_BLOCKS].registros[0] = paper;
        return 1;
    }
    if(bucket_destino.blocos[paper.id%BUCKET_BLOCKS].registros[1].id == 0){
        bucket_destino.blocos[paper.id%BUCKET_BLOCKS].registros[1] = paper;
        return 1;
    }
    return 0;
}
void hash_function(artigo paper, vector<bucket> &buckets, int bucket_num){     
    int bucket_destino = paper.id % bucket_num;                                   //Encaminha um registro para um bucket
    while(inserir(buckets.at(bucket_destino),paper) == 0 ){                       //Caso tenha colisao entao cria-se outro
        cout << "TRATANDO COLISAO";                                                 //Bucket que reberá o registro
        bucket novo;
        buckets.push_back(novo);
        buckets.at(bucket_destino).overflow_pos = buckets.size();
        bucket_destino = buckets.at(bucket_destino).overflow_pos;
    }

}

int main(){


     FILE * pFile;                                            //Tratamento de erros do banco de dados que o regex não pega
    char c = ' ';
    pFile=fopen("arquivo.entrada.tp2.bd1.artigos.csv","r+");
    fseek(pFile,200355981,SEEK_SET);
    putc (c , pFile);
    fseek(pFile,290162250,SEEK_SET);
    putc (c , pFile);
    fclose (pFile);
    

    int registros_no_arquivo = 0;
    int cont = 0;                                                           //contador de registros
    char linha [LINE_SIZE];                                                 //espaço alocado para leitura de cada linha
    FILE *bd;
    bd = fopen("arquivo.entrada.tp2.bd1.artigos.csv","r"); //Leitura do Arquivo contendo o banco de dados
    if (bd == NULL) perror ("O banco de dados não está na pasta do programa"); //Caso ocorra algum erro
    else{           
        while(fgets ( linha, LINE_SIZE,bd ) != NULL){ 
            registros_no_arquivo += 1;                                          //Pega o total de registros no arquivo CSV
        }
       
        fseek(bd,0,SEEK_SET);  
        int bucket_num = registros_no_arquivo / (BUCKET_SIZE * BUCKET_BLOCKS);
        while(primo(bucket_num) == 0){                                      //Faz a quantidade de buckets ser o número primo mais próximo
            bucket_num +=1;                                                 //Para a função de hash ser boa
        }
       
        
        vector<bucket> buckets (bucket_num);                          //Cria buckets de acordo com o tamanho de registros

        for (int i = 0; i < buckets.size(); i++){
            bucket buck;
            buckets.at(i) = buck;                                     //Instacia cada um
        }
        int contador = 1;
        
        regex campos ("(\"(.*?)\"|NULL)");                              //Regra de sintaxe de campo do CSV
        
        
        while(fgets ( linha, LINE_SIZE,bd ) != NULL){                           //Carrega uma linha do banco de dados
            artigo paper = PegarCampos(linha,campos,bd);
            cout << contador << endl;
            if(contador % 1000 == 0) cout << contador << endl;
            hash_function(paper,buckets,bucket_num);
            contador +=1;
        }
        cout << "Registros: " << contador << endl;
        cout << "Buckets: " << bucket_num << endl;
        cout << "Buckets de OverFlow: " << buckets.size() - bucket_num << endl;   //Mostra dados do Banco de dados
        fclose(bd);

        ofstream meta;                   //Cria um arquivo de meta dados contendo informacoes necessarias para a consulta
        meta.open ("MetaDados.txt");
        meta << bucket_num << endl;
        meta << buckets.size() - bucket_num << endl;
        meta.close();

        FILE *arq;
        arq = fopen("BancoDeDados.dat", "wb");  //Cria o banco de dados
        char resto = ' ';
        
        for (int i = 0; i < buckets.size(); i++)
        {
            fwrite(&buckets.at(i),sizeof(bucket),1,arq); //Escreve os buckets no disco
           
        }
        fclose(arq);
        
    }
    
    return 0;
}