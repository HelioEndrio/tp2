#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define BLOCO_DE_DISCO 4096
#define BUCKET_SIZE 2
#define BUCKET_BLOCKS 2
#define REG_SIZE 1508
using namespace std;

struct artigo{                          //Struct para guardar cada registro
    int id;
    char titulo [300];
    int ano;
    char autores [150];
    int citacoes;
    char atualizacao [20];
    char snippet [1024];
};

struct bloco{                           //Struct de bloco de bucket
    artigo registros[BUCKET_SIZE];
};

struct bucket {                     //Struct de bucket
    bloco blocos [BUCKET_BLOCKS];   //3016 bytes * 2 = 6032
    int overflow_pos;               //4 bytes
                                      //2156 de espaco sobrando no 2º bloco, mas como é despensavel para a consulta então
                                    //foi retirado.   1 Bucket ocupa 6036 bytes e deixa de usar 2156 bytes 
};
void mostrar_bloco(int consulta, int bucket_num){ //Mostra em quais blocos está o registro da consulta
    int bucket_alvo = consulta % bucket_num;
    if(((bucket_alvo*(BLOCO_DE_DISCO*2)/BLOCO_DE_DISCO) % 2 == 0) && ((bucket_alvo*(BLOCO_DE_DISCO*2)/BLOCO_DE_DISCO) > 1)){
        cout  << "BLOCO DE DISCO: " << (bucket_alvo*(BLOCO_DE_DISCO*2)/BLOCO_DE_DISCO)-1 << " & " << (bucket_alvo*(BLOCO_DE_DISCO*2)/BLOCO_DE_DISCO) << endl;
    }
    else{
        cout  << "BLOCO DE DISCO: " << (bucket_alvo*(BLOCO_DE_DISCO*2)/BLOCO_DE_DISCO)+1 << " & " << (bucket_alvo*(BLOCO_DE_DISCO*2)/BLOCO_DE_DISCO) << endl;        
    }
    cout << endl;
}
void mostrar_artigo(artigo registro){       //Mostra o registro da consulta caso ela exista no banco de dados
    printf("id:\t\t%d \n\n", registro.id);
    printf("Título:\t\t%s \n\n", registro.titulo);
    printf("Ano:\t\t%d \n\n", registro.ano);
    printf("Autores:\t%s \n\n", registro.autores);
    printf("Citações:\t%d \n\n", registro.citacoes);
    printf("Atualização:\t%s \n\n", registro.atualizacao);
    printf("snippet:\t%s \n\n", registro.snippet);
}


int procurar(int id, bucket bucket_consultado,FILE *arq, int bucket_num,int blocos_lidos){//Procura o registro no banco de dados
    blocos_lidos+=2;
    if(id % 2 == 0) {
        if(bucket_consultado.blocos[0].registros[0].id == id){              //Caso exista então mostra e retorna os blocos lidos
            mostrar_bloco(id,bucket_num);
            mostrar_artigo(bucket_consultado.blocos[0].registros[0]);
            return blocos_lidos;
        }
        if(bucket_consultado.blocos[0].registros[1].id == id){
            mostrar_bloco(id,bucket_num);
            mostrar_artigo(bucket_consultado.blocos[0].registros[1]);
            return blocos_lidos;
        }
        else{
            if(bucket_consultado.overflow_pos != 0) {
                bucket consulta_overflow;
                fseek(arq,bucket_consultado.overflow_pos%BLOCO_DE_DISCO,SEEK_SET);
                fread (&consulta_overflow, sizeof(BLOCO_DE_DISCO), 1, arq);
                blocos_lidos +=2;
                procurar (id,consulta_overflow ,arq,bucket_num,blocos_lidos);
            }
            else{
                cout << "Não existe registro com esse valor de ID!" << endl;        //Se nao existir entao para o processo
                return blocos_lidos;
            }
        }
    }
   else{
        if(bucket_consultado.blocos[1].registros[0].id == id){
            mostrar_bloco(id,bucket_num);
            mostrar_artigo(bucket_consultado.blocos[1].registros[0]);
            return blocos_lidos;
        }
        if(bucket_consultado.blocos[1].registros[1].id == id){
            mostrar_bloco(id,bucket_num);
            mostrar_artigo(bucket_consultado.blocos[1].registros[1]);
            return blocos_lidos;
           
        }
        else{
            if(bucket_consultado.overflow_pos != 0) {
                bucket consulta_overflow;
                fseek(arq,bucket_consultado.overflow_pos%BLOCO_DE_DISCO,SEEK_SET);
                fread (&consulta_overflow, sizeof(BLOCO_DE_DISCO), 1, arq);
                blocos_lidos +=2;
                procurar (id,consulta_overflow,arq,bucket_num,blocos_lidos);
                return blocos_lidos;
            }
            else{
                cout << "Não existe registro com esse valor de ID!" << endl;
                return blocos_lidos;
            }
        }
    }
}

int main(){
    FILE *meta;
    meta = fopen("MetaDados.txt","r");      //Retorna metadados do banco de dados
    char linha [10]; 
    string str;
    stringstream ss;
    int  bucket_num;
    fgets ( linha, 10,meta ); 
    sscanf(linha, "%d", &bucket_num);
    fclose(meta);



    FILE *arq;
    arq = fopen("BancoDeDados.dat","rb");   //Cria o banco de dados
    int consulta;
    cin >> consulta;
    bucket bresposta;
    int blocos_lidos;
    int bucket_alvo = consulta % bucket_num;    //Calcula o local esperado do registro
    fseek(arq,bucket_alvo*(BLOCO_DE_DISCO*2),SEEK_SET);
    fread (&bresposta, sizeof(bucket), 1, arq);

    blocos_lidos = procurar(consulta,bresposta,arq,bucket_num,0); //Procura o registro
    cout << "Blocos Lidos: " <<blocos_lidos<< endl;
    fseek(arq,0,SEEK_END);
    cout << "Total de Blocos: " << ftell(arq) / 4096 << endl;   //Mostra informacoes sobre a consulta
    fclose(arq);
    return 1;
}