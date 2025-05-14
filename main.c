#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Produto{
    char nome[45];
    int id;
    int quantidade;
    float preco;
};

struct No{
    struct Produto produto;
    struct No *esq;
    struct No *dir;
    struct No *pai;
    int altura;
};

// Funcao para obter a altura de um noh
int altura(struct No *no) {
    if(no == NULL){
        return 0;
    }
    return no->altura;
}

// Funcao para obter o fator de balanceamento
int fatorBalanceamento(struct No *no) {
    if(no == NULL){
        return 0;
    }
    return altura(no->esq) - altura(no->dir);
}

// Funcao para atualizar a altura de um noh
void atualizarAltura(struct No *no) {
    if (no == NULL) return;
    int alturaEsq = altura(no->esq);
    int alturaDir = altura(no->dir);
    no->altura = (alturaEsq > alturaDir ? alturaEsq : alturaDir) + 1;
}

void rotacaoEsq(struct No **raizAntiga){
    if(*raizAntiga == NULL || (*raizAntiga)->dir == NULL){
        return;
    }
    struct No *novaRaiz = (*raizAntiga)->dir;
    struct No *temp = novaRaiz->esq;

    novaRaiz->esq = *raizAntiga;
    (*raizAntiga)->dir = temp;
    
    novaRaiz->pai = (*raizAntiga)->pai;
    (*raizAntiga)->pai = novaRaiz;

    if(temp != NULL){
        temp->pai = *raizAntiga;
    }
    if(novaRaiz->pai != NULL){
        if(novaRaiz->pai->esq == *raizAntiga){
            novaRaiz->pai->esq = novaRaiz;
        }else{
            novaRaiz->pai->dir = novaRaiz;
        }
    }
    *raizAntiga = novaRaiz;

    // atualizar alturas apos rotacao
    atualizarAltura((*raizAntiga)->esq);
    atualizarAltura(*raizAntiga);
}

void rotacaoDir(struct No **raizAntiga){
    if(*raizAntiga == NULL || (*raizAntiga)->esq == NULL){
        return;
    }

    struct No *novaRaiz = (*raizAntiga)->esq;
    struct No *temp = novaRaiz->dir;

    novaRaiz->dir = *raizAntiga;
    (*raizAntiga)->esq = temp;

    novaRaiz->pai = (*raizAntiga)->pai;
    (*raizAntiga)->pai = novaRaiz;

    if(temp != NULL){
        temp->pai = *raizAntiga;
    }

    if(novaRaiz->pai != NULL){
        if(novaRaiz->pai->esq == *raizAntiga){
            novaRaiz->pai->esq = novaRaiz;
        } else {
            novaRaiz->pai->dir = novaRaiz;
        }
    }

    *raizAntiga = novaRaiz;

    // atualizar alturas apos rotacao
    atualizarAltura((*raizAntiga)->dir);
    atualizarAltura(*raizAntiga);
}

// Funcao para balancear a arvore AVL
void balancear(struct No **raiz) {
    if (*raiz == NULL) return;

    // atualiza a altura do noh atual
    atualizarAltura(*raiz);

    // calcula o fator de balanceamento
    int fb = fatorBalanceamento(*raiz);

    // Caso 1: rotacao a direita
    if (fb > 1 && (*raiz)->esq != NULL && fatorBalanceamento((*raiz)->esq) >= 0) {
        rotacaoDir(raiz);
    }
    // Caso 2: rotacao a esquerda
    else if (fb < -1 && (*raiz)->dir != NULL && fatorBalanceamento((*raiz)->dir) <= 0) {
        rotacaoEsq(raiz);
    }
    // Caso 3: rotacao dupla a direita
    else if (fb > 1 && (*raiz)->esq != NULL && fatorBalanceamento((*raiz)->esq) < 0) {
        rotacaoEsq(&(*raiz)->esq);
        rotacaoDir(raiz);
    }
    // Caso 4: rotacao dupla a esquerda
    else if (fb < -1 && (*raiz)->dir != NULL && fatorBalanceamento((*raiz)->dir) > 0) {
        rotacaoDir(&(*raiz)->dir);
        rotacaoEsq(raiz);
    }
}

// Funcao para inserir um novo produto na arvore
void inserirProduto(struct No **raiz, struct Produto novoProduto){
    if(*raiz == NULL){
        *raiz = (struct No *)malloc(sizeof(struct No));
        if(*raiz != NULL){
            (*raiz)->produto = novoProduto;
            (*raiz)->esq = NULL;
            (*raiz)->dir = NULL;
            (*raiz)->pai = NULL;
            (*raiz)->altura = 1;
            printf("produto [%s] cadastrado com sucesso!\n", novoProduto.nome);
        }else{
            printf("ERROR : insercao do [%s] invalida, memoria insuficiente.\n", novoProduto.nome);
            return;
        }
    }else{
        if(strcmp(novoProduto.nome, (*raiz)->produto.nome) < 0){
            inserirProduto(&((*raiz)->esq), novoProduto);
            if((*raiz)->esq != NULL){  // verifica se a insercao foi bem sucedida
                (*raiz)->esq->pai = *raiz; // pai do filho a esq recebe o noh atual
            }
        }else if(strcmp(novoProduto.nome, (*raiz)->produto.nome) > 0){
            inserirProduto(&((*raiz)->dir), novoProduto);
            if((*raiz)->dir != NULL){  // verifica se a insercao foi bem sucedida
                (*raiz)->dir->pai = *raiz; // pai do filho a dir recebe o noh atual
            }
        }else{
            printf("produto com este nome ja existe na lista...\n");
            return;
        }
        balancear(raiz);
    }
}

int removerProduto(struct No **raiz, char removerProdutoNome[45]){
    if(*raiz == NULL){
        printf("produto nao encontrado!\n");
        return 0;
    }

    int comparacao = strcmp(removerProdutoNome, (*raiz)->produto.nome);
    // procura o produto a ser removido
    if(comparacao < 0){
        return removerProduto(&((*raiz)->esq), removerProdutoNome);
    }
    else if(comparacao > 0){
        return removerProduto(&((*raiz)->dir), removerProdutoNome);
    }
    else{
        // >> noh encontrado <<
        struct No *temp = *raiz;
        
        // Caso 1: noh folha
        if((*raiz)->esq == NULL && (*raiz)->dir == NULL){
            *raiz = NULL;
        }
        // Caso 2: noh com um filho
        else if((*raiz)->esq == NULL){
            *raiz = (*raiz)->dir;
            if(*raiz != NULL){  // verifica se o novo noh nao eh NULL
                (*raiz)->pai = temp->pai;
            }
        }
        else if((*raiz)->dir == NULL){
            *raiz = (*raiz)->esq;
            if(*raiz != NULL){  // verifica se o novo noh nao eh NULL
                (*raiz)->pai = temp->pai;
            }
        }
        // Caso 3: noh com dois filhos
        else{
            struct No *sucessor = (*raiz)->dir;
            while(sucessor->esq != NULL){
                sucessor = sucessor->esq;
            }
            
            // copia os dados do sucessor para o noh atual
            (*raiz)->produto = sucessor->produto;
            
            // remove o sucessor
            if(sucessor->pai == *raiz){
                sucessor->pai->dir = sucessor->dir;
                if(sucessor->dir != NULL){
                    sucessor->dir->pai = sucessor->pai;
                }
            }else{
                sucessor->pai->esq = sucessor->dir;
                if(sucessor->dir != NULL){
                    sucessor->dir->pai = sucessor->pai;
                }
            }
            temp = sucessor;
        }
        
        free(temp);
        if(*raiz != NULL){  // soh balanceia se a raiz nao for NULL
            balancear(raiz);
        }
        return 1;
    }
}

// Funcao de busca que retorna o noh encontrado
struct No *buscarProduto(struct No *raiz, char buscarProdutoNome[45]){
    if(raiz == NULL){
        return NULL;
    }else{
        if(strcmp(buscarProdutoNome, raiz->produto.nome) == 0){
            return raiz;
        }else if(strcmp(buscarProdutoNome, raiz->produto.nome) < 0){
            return buscarProduto(raiz->esq, buscarProdutoNome);
        }else{
            return buscarProduto(raiz->dir, buscarProdutoNome);
        }
    }
}
// Funcao para listar todos os produtos da arvore
void listarProdutos(struct No *raiz){
    if(raiz != NULL){
        listarProdutos(raiz->esq);
        printf(" >> ID: %i\n", raiz->produto.id);
        printf(" >> Nome: %s\n", raiz->produto.nome);
        printf(" >> Quantidade: %i\n", raiz->produto.quantidade);
        printf(" >> Preco: %.2f\n", raiz->produto.preco);
        printf("\n");
        listarProdutos(raiz->dir);
    }
}
// Funcao de busca que retorna o noh encontrado
void liberarArvore(struct No *raiz) {
    if (raiz != NULL) {
        liberarArvore(raiz->esq);
        liberarArvore(raiz->dir);
        free(raiz);
    }
}

int main(){

    struct No *raiz = NULL;
    struct Produto produto;
    int opc = -1;
    char buscarProdutoNome[45], removerProdutoNome[45];

    while(opc != 0){ // menu de opcoes
        printf("\n========= ProdutoS =========\n");
        printf("<1> -  Inserir produto -  <1>\n");
        printf("<2> -  Remover produto -  <2>\n");
        printf("<3> -  Buscar produto  -  <3>\n");
        printf("<4> -  Listar produtos -  <4>\n");
        printf("<0> -       Sair       -  <0>\n");
        printf("\n========= ProdutoS =========\n");

        printf("\n >> Escolha uma opcao: ");
        scanf("%i", &opc);
        getchar();

        switch(opc){
            case 1:
                printf(" >> ID do produto: ");
                scanf("%i", &produto.id);
                getchar();

                printf(" >> Nome do produto: ");
                fgets(produto.nome, 45, stdin);
                produto.nome[strcspn(produto.nome, "\n")] = 0;

                printf(" >> Quantidade do produto: ");
                scanf("%i", &produto.quantidade);

                printf(" >> Preco do produto: ");
                scanf("%f", &produto.preco);

                inserirProduto(&raiz, produto);
                break;
            case 2:
                printf(" >> Nome do produto: ");
                fgets(removerProdutoNome, 45, stdin);
                removerProdutoNome[strcspn(removerProdutoNome, "\n")] = 0;

                removerProduto(&raiz, removerProdutoNome);
                printf(" >> produto removido com sucesso!\n");
                break;
            case 3:
                printf(" >> Nome do produto: ");
                fgets(buscarProdutoNome, 45, stdin);
                buscarProdutoNome[strcspn(buscarProdutoNome, "\n")] = 0;

                struct No *encontrado = buscarProduto(raiz, buscarProdutoNome);

                if(encontrado != NULL){
                    printf(" >> produto encontrado: \n");
                    printf(" >> ID: %i\n", encontrado->produto.id);
                    printf(" >> Nome: %s\n", encontrado->produto.nome);
                    printf(" >> Quantidade: %i\n", encontrado->produto.quantidade);
                    printf(" >> Preco: %.2f\n", encontrado->produto.preco);
                }else{
                    printf("produto nao encontrado...\n");
                }
                break;
            case 4:
                printf("\n>>> Listagem de produtos <<<\n");
                listarProdutos(raiz);
                break;
            case 0:
                printf(" >> Saindo do sistema <<\n");
                break;
            default:
                printf(" >> Opcao invalida << \n");
                break;
        }
        
    }
    liberarArvore(raiz);
    return 0;
}
