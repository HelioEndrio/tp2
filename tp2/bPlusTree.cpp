#include <iostream>
#define m 1 //para calcular limite maximo e minimo do no

using namespace std;

//estrutura que pode ser alterada pra se adequar ao endereco do bloco
typedef int tipoBloco;

//estrutura que pode ser alterada pra se adequar ao valor do registro
typedef int tipoValor;

//estrutura que vai guardar ID e endereco do bloco do artigo
typedef struct{
	tipoValor elemento;
	tipoBloco endereco;
} tipoInfo;

//estrutura que genrecia as folhas e os nos internos da arvore
typedef struct BNo{
	tipoInfo* dados; //dados chave do no
	struct BNo** filhos; //armazena filhos do no (ultimo filho aponta para a folha irma, caso o no seja folha)
	int nChaves; //quantas chaves tem o no atualmente
	bool folha; //saber se e folha
} TipoBNo;

void criarArvore(TipoBNo** raiz){
	(*raiz) = NULL;
	return;
}

TipoBNo* criarFolha(tipoInfo dado){
	TipoBNo* folha = new TipoBNo;
	folha->dados = new tipoInfo[2*m];
	folha->filhos = new TipoBNo*[2*m+1];
	folha->dados[0] = dado;
	//zerar todos os filhos
	for(int i=0; i<2*m+1;i++){
		folha->filhos[i] = NULL;
	}
	folha->nChaves=1;
	folha->folha = true;
	return folha;
}

TipoBNo* criarBloco(tipoValor dado){
	TipoBNo* bloco = new TipoBNo;
	bloco->dados = new tipoInfo[2*m];
	bloco->filhos = new TipoBNo*[2*m+1];
	bloco->dados[0].elemento = dado;
	bloco->dados[0].endereco = NULL;
	//zerar todos os filhos
	for(int i=0; i<2*m+1;i++){
		bloco->filhos[i] = NULL;
	}
	bloco->nChaves=1;
	bloco->folha = false;
	return bloco;
}

void mostrarFolha(TipoBNo* folha){
	if(folha){
		//printar folha
		cout<<"\nmostrarFolha: folha("<<folha<<")\ndados:";
		for(int i=0; i<folha->nChaves; i++){
			cout<<" "<<folha->dados[i].elemento;
		}
		cout<<"\nenderecos:";
		for(int i=0; i<folha->nChaves; i++){
			cout<<" "<<folha->dados[i].endereco;
		}
		if(folha->filhos[2*m]){
			cout<<"\nmostrarFolha: tem irmao-> folha("<<(folha->filhos[2*m])<<")\n";
			//mostrarFolha(folha->filhos[2*m]);
		}
		else{
			cout<<"\nmostrarFolha: nao tem irmao\n";
		}
		cout<<endl;
	}
	else{
		cout<<"\nmostrarFolha: Nao tem nada\n";
	}
}

void mostrarArvore(TipoBNo* raiz){
	if(raiz){
		if(raiz->folha){
			mostrarFolha(raiz);
		}
		else{
			for(int i=0; i<=raiz->nChaves;i++){
				mostrarArvore(raiz->filhos[i]);
			}
		}
	}
}

//inserir dentro de folha que ainda cabe elemento
void inserirFolha(TipoBNo** folha, tipoInfo dado){
	//cout<<"\ninserirFolha: entrou na funcao\n";
	int i = (*folha)->nChaves; //pega ultima posicao disponivel
	//caso elemento menos doq o elemento q ja existe na folha
	while(i>0 and (*folha)->dados[i-1].elemento>dado.elemento){
		(*folha)->dados[i] = (*folha)->dados[i-1];
		i--;
	}
	(*folha)->dados[i] = dado;
	(*folha)->nChaves++;
}

TipoBNo* dividirFolha(TipoBNo** folha, tipoInfo dado){
	int posicao = (*folha)->nChaves; //pega ultima posicao disponivel
	//caso elemento menos doq o elemento q ja existe na folha
	while(posicao>0 and (*folha)->dados[posicao-1].elemento>dado.elemento){
		posicao--;
	}
	//cout<<"\ndividirFolha: posicao "<<posicao<<endl;

	TipoBNo *a, *b, *c;
	
	if(posicao<m){
		//cout<<"\ndividirFolha: posicao<"<<m<<endl;
		a = criarBloco((*folha)->dados[m-1].elemento);
		b = criarFolha(dado);
		for(int i=0; i<m-1; i++){
			inserirFolha(&b, (*folha)->dados[i]);
		}
		a->filhos[0] = b;

		c = criarFolha((*folha)->dados[m-1]);
		for(int i=m; i<2*m; i++){
			inserirFolha(&c, (*folha)->dados[i]);
		}
	}
	else{
		if(posicao>m){
			//cout<<"\ndividirFolha: posicao>"<<m<<endl;
			a = criarBloco((*folha)->dados[m].elemento);
			b = criarFolha((*folha)->dados[0]);
			for(int i=1; i<m; i++){
				inserirFolha(&b, (*folha)->dados[i]);
			}
			a->filhos[0] = b;

			c = criarFolha(dado);
			for(int i=m; i<2*m; i++){
				inserirFolha(&c, (*folha)->dados[i]);
			}
			
		}
		else{
			//cout<<"\ndividirFolha: posicao=="<<m<<endl;
			a = criarBloco(dado.elemento);
			b = criarFolha((*folha)->dados[0]);
			for(int i=1; i<m; i++){
				inserirFolha(&b, (*folha)->dados[i]);
			}
			a->filhos[0] = b;

			c = criarFolha((*folha)->dados[m]);
			for(int i=m+1; i<2*m; i++){
				inserirFolha(&c, (*folha)->dados[i]);
			}
			inserirFolha(&c, dado);
		}
	}
	a->filhos[1] = c;

	free(*folha);
	//cout<<"\ndividirFolha: retornou normalmente\n";
	return a;
}

void inserirBNo(TipoBNo** pai, TipoBNo* filho){
	int posicao = (*pai)->nChaves;
	while(posicao>0 and filho->dados[0].elemento<(*pai)->dados[posicao-1].elemento){
		(*pai)->dados[posicao] = (*pai)->dados[posicao-1];
		(*pai)->filhos[posicao+1] = (*pai)->filhos[posicao];
		(*pai)->filhos[posicao] = (*pai)->filhos[posicao-1];
		posicao--;
	}
	(*pai)->dados[posicao] = filho->dados[0];
	(*pai)->filhos[posicao] = filho->filhos[0];
	(*pai)->filhos[posicao+1] = filho->filhos[1];
	(*pai)->nChaves++;
	cout<<"\ninserirBNo: PAI";
	mostrarArvore((*pai));
	free(filho);
}

void dividirBNo(TipoBNo** Bloco, TipoBNo* filho){
	cout<<"\ndividirBNo: entrou na funcao\n";
	int posicao = (*Bloco)->nChaves; //pega ultima posicao disponivel
	//caso elemento menos doq o elemento q ja existe na Bloco
	while(posicao>0 and (*Bloco)->dados[posicao-1].elemento>filho->dados[0].elemento){
		posicao--;
	}
	cout<<"\ndividirBNo: posicao "<<posicao<<endl;

	TipoBNo *a, *b, *c, *aux;
	
	if(posicao<m){
		cout<<"\ndividirBNo: posicao<"<<m<<endl;
		a = criarBloco((*Bloco)->dados[m-1].elemento);
		b = criarBloco(filho->dados[0].elemento);

		for(int i=0; i<m-1;i++){ //salvar chaves no novo bloco
			aux = criarBloco((*Bloco)->dados[i].elemento);
			inserirBNo(&b, aux);
		}
		//cout<<"\ndividirBNo: BLOCO B";
		//mostrarArvore(b);
		for(int i=0; i<posicao; i++){ //manter referencias dos filhos das chaves
			b->filhos[i] = (*Bloco)->filhos[i];
		}
		//chaves do filho sobrepoem as do pai (pois estao atualizadas)
		b->filhos[posicao] = filho->filhos[0];
		b->filhos[posicao+1] = filho->filhos[1];

		for(int i=posicao+2; i<=m;i++){ //manter referencias dos filhos das chaves
			b->filhos[i] = (*Bloco)->filhos[i];
		}

		//salvar em pai a o filho b (b->dados<a->dados[0])
		a->filhos[0] = b;

		c = criarBloco((*Bloco)->dados[m].elemento);
		for(int i=m+1; i<2*m; i++){
			aux = criarBloco((*Bloco)->dados[i].elemento);
			inserirBNo(&c, aux);
		}
		for(int i=m; i<=2*m; i++){
			c->filhos[i-m] = (*Bloco)->filhos[i];
		}
	}
	else{
		if(posicao>m){
			cout<<"\ndividirBNo: posicao>"<<m<<endl;
			a = criarBloco((*Bloco)->dados[m].elemento);
			b = criarBloco((*Bloco)->dados[0].elemento);

			for(int i=1; i<m;i++){ //salvar chaves no novo bloco
				aux = criarBloco((*Bloco)->dados[i].elemento);
				inserirBNo(&b, aux);
			}
			//cout<<"\ndividirBNo: BLOCO B";
			//mostrarArvore(b);
			for(int i=0; i<m; i++){ //manter referencias dos filhos das chaves
				b->filhos[i] = (*Bloco)->filhos[i];
			}

			//salvar em pai a o filho b (b->dados<a->dados[0])
			a->filhos[0] = b;
			//chaves do filho sobrepoem as do pai (pois estao atualizadas)
			//b->filhos[posicao] = filho->filhos[0];
			//b->filhos[posicao+1] = filho->filhos[1];

			//for(int i=posicao; i<m-1;i++){ //manter referencias dos filhos das chaves
			//	b->filhos[i+2] = (*Bloco)->filhos[i+1];
			//}


			c = criarBloco(filho->dados[0].elemento);
			for(int i=m; i<2*m; i++){
				aux = criarBloco((*Bloco)->dados[i].elemento);
				inserirBNo(&c, aux);
			}
			for(int i=m; i<posicao-1; i++){
				c->filhos[i-m+1] = (*Bloco)->filhos[i+1];
			}
			c->filhos[posicao-m] = filho->filhos[0];
			c->filhos[posicao-(m-1)] = filho->filhos[1];
			//for(int i=posicao; i<m-1;i++){	
			//}
		/*
		*/
		}
		else{ //posicao==m
			cout<<"\ndividirBNo: posicao=="<<m<<endl;
			a = criarBloco(filho->dados[0].elemento);
			b = criarBloco((*Bloco)->dados[0].elemento);

			for(int i=1; i<m;i++){
				aux = criarBloco((*Bloco)->dados[i].elemento);
				inserirBNo(&b, aux);
			}
			//cout<<"\ndividirBNo: BLOCO B";
			//mostrarArvore(b);
			for(int i=0; i<posicao; i++){
				b->filhos[i] = (*Bloco)->filhos[i];
			}
			for(int i=posicao; i<=m;i++){
				b->filhos[i] = filho->filhos[i-posicao];
			}

			a->filhos[0] = b;

			c = criarBloco((*Bloco)->dados[m].elemento);
			for(int i=m+1; i<2*m; i++){
				aux = criarBloco((*Bloco)->dados[i].elemento);
				inserirBNo(&c, aux);
			}
			for(int i=m; i<=2*m; i++){
				c->filhos[i-m] = (*Bloco)->filhos[i];
			}
			for(int i=posicao; i<=m;i++){
				c->filhos[i-1] = filho->filhos[i];
			}
		}
	}
	a->filhos[1] = c;
	free(*Bloco);
	//cout<<"\ndividirBloco: retornou normalmente\n";
	(*Bloco) = a;
}

void inserirDado(TipoBNo** raiz, tipoInfo dado, TipoBNo** pai){
	if(*raiz){
		//cout<<"\ninserirDado: Tem raiz\n";
		if((*raiz)->folha){ //e folha
			//cout<<"\ninserirDado: eh folha\n";
			if((*raiz)->nChaves<2*m){ //existe espaco para insercao na folha
				//cout<<"\ninserirDado: ainda cabe\n";
				inserirFolha(raiz, dado);
			}
			else{ //folha cheia
				TipoBNo *aux;
				//cout<<"\ninserirDado: Folha cheia, divide e sobe um para o pai\n";
				if(pai){ //existe pai
					cout<<"\ninserirDado: existe pai\n";
					aux = dividirFolha(raiz, dado);
					cout<<"\ninserirDado: BLOCO AUX";
					mostrarArvore(aux);
					cout<<endl;
					if((*pai)->nChaves<2*m){
						cout<<"\ninserirDado: cabe no pai\n";
						inserirBNo(pai, aux);
					}
					else{
						cout<<"\ninserirDado: pai esta cheio\n";
						dividirBNo(pai, aux);
					}
				}
			/*
			*/
				else{
					//cout<<"\ninserirDado: nao existe pai\n";
					*raiz = dividirFolha(raiz, dado);
				}

			}
		}
		else{ //nao e foolha (no interno)
			cout<<"\ninserirDado: no interno\n";
			int i=0;
			while(i<(*raiz)->nChaves and dado.elemento>(*raiz)->dados[i].elemento){
				i++;
			}
				inserirDado(&(*raiz)->filhos[i], dado, raiz);
		}
	}
	else{
		//cout<<"\ninserirDado: Nao tem raiz\n";
		*raiz = criarFolha(dado);
	}
}


TipoBNo* buscarDado(TipoBNo* raiz, tipoValor dado){
	if(raiz){
		for (int i = 0; i < (raiz->nChaves); i++){
			if(raiz->dados[i].elemento>dado){
				return buscarDado(raiz->filhos[i], dado);
			}
			else{
				if(raiz->dados[i].elemento==dado){
					if(raiz->folha){
						return raiz;
					}
					else{
						return buscarDado(raiz->filhos[i+1], dado);
					}
				}
			}
		}
		cout<<endl;
		if(dado>raiz->dados[raiz->nChaves].elemento){
			return buscarDado(raiz->filhos[raiz->nChaves], dado);
		}
	}
	else{
		return NULL;
	}
}
/*

*/

int main(){
	TipoBNo* arvore;
	tipoValor numero;
	tipoBloco endereco;
	tipoInfo elemento;

	//cria a raiz da arvore
	criarArvore(&arvore);

	cout<<"\nInsira os "<<2*m<<" primeiros valores\n";
	for(int i=0;i<2*m;i++){
		cout<<"Insira alguem: ";
		cin>>numero;
		//cout<<"Insira o endereco: ";
		//cin>>endereco;
		elemento.elemento = numero;
		//elemento.endereco = endereco;
		inserirDado(&arvore, elemento, NULL);

	}
	cout<<"\nraiz:";
	for(int i=0; i<arvore->nChaves;i++){
		cout<<" "<<arvore->dados[i].elemento;
	}
	cout<<endl;
	cout<<"\nArvore:\n";
	mostrarArvore(arvore);
	cout<<endl;

	
	cout<<"opcao inserir: ";
	cin>>numero;

	while(numero){
		//cout<<"Insira o endereco: ";
		//cin>>endereco;
		elemento.elemento = numero;
		//elemento.endereco = endereco;
		inserirDado(&arvore, elemento, NULL);

		cout<<"\nraiz:";
		for(int i=0; i<arvore->nChaves;i++){
			cout<<" "<<arvore->dados[i].elemento;
		}
		cout<<endl;
		cout<<"\nArvore:\n";
		mostrarArvore(arvore);
		cout<<endl;

		cout<<"\nopcao inserir: ";
		cin>>numero;
	}

	cout<<"\nopcao buscar: ";
	cin>>numero;

	TipoBNo* enderecoNo;

	while(numero){

		enderecoNo = buscarDado(arvore, numero);
		cout<<"\nEnderecoNo do bloco: "<<enderecoNo;
		cout<<endl;
		mostrarArvore(enderecoNo);

		cout<<"\nArvore:\n";
		mostrarArvore(arvore);
		cout<<endl;

		cout<<"opcao buscar: ";
		cin>>numero;
	}

	/*
	*/
	return 0;
}