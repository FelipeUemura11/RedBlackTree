#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define RED 0
#define BLACK 1

struct Produto{
    int codigo;
    char nome[45];
    int quantidade;
    float preco;
};

struct No{
    struct Produto produto;
    struct No *esq;
    struct No *dir;
    struct No *pai;
    int cor;
};

struct No* criarNo(struct Produto produto){
    struct No* novo = (struct No*)malloc(sizeof(struct No));

    if(novo != NULL){
        novo->produto = produto;
        novo->esq = NULL;
        novo->dir = NULL;
        novo->pai = NULL;
        novo->cor = RED;
    }
    return novo;
}

int ehVermelho(struct No* no){
    return (no != NULL && no->cor == RED);
}

int ehPreto(struct No* no){
    return (no == NULL || no->cor == BLACK);
}

void rotacaoEsq(struct No **raiz){
    
    struct No *novaRaiz = (*raiz)->dir;
    (*raiz)->dir = novaRaiz->esq;

    if(novaRaiz->esq != NULL){
        novaRaiz->esq->pai = *raiz;
    }
    novaRaiz->pai = (*raiz)->pai;
    
    // atualizar os ponteiros do pai
    if((*raiz)->pai == NULL){
        *raiz = novaRaiz; // primeiro noh da arvore
    }else if(*raiz == (*raiz)->pai->esq){
        (*raiz)->pai->esq = novaRaiz;
    }else{
        (*raiz)->pai->dir = novaRaiz;
    }

    novaRaiz->esq = *raiz;
    (*raiz)->pai = novaRaiz;
}

void rotacaoDir(struct No **raiz){

    struct No *novaRaiz = (*raiz)->esq;
    (*raiz)->esq = novaRaiz->dir;

    if(novaRaiz->dir != NULL){
        novaRaiz->dir->pai = *raiz;
    }

    novaRaiz->pai = (*raiz)->pai;

    if((*raiz)->pai == NULL){
        *raiz = novaRaiz;
    }else if(*raiz == (*raiz)->pai->esq){
        (*raiz)->pai->esq = novaRaiz;
    }else{
        (*raiz)->pai->dir = novaRaiz;
    }

    novaRaiz->dir = *raiz;
    (*raiz)->pai = novaRaiz;
}

// Funcao para balancear a arvore AVL
void balancearInsercao(struct No **raiz, struct No *novo) {
    
    struct No *pai = NULL;
    struct No *avo = NULL;
    // o loop vai se manter enquanto houver violacoes da propriedade da arvore rubro-negra
    while(novo != *raiz && ehVermelho(novo) && ehVermelho(novo->pai)){
        pai = novo->pai;
        avo = pai->pai;
        // CASO 1: Pai e filho esquerda do avo
        if(pai == avo->esq){
            struct No *tio = avo->dir;

            // SUBCASO 1.1: Tio >>> RED
            if(ehVermelho(tio)){
                avo->cor = RED;
                pai->cor = BLACK;
                tio->cor = BLACK;
                novo = avo;
            }
            // SUBCASO 1.2: Tio >>> BLACK e Novo >>> Filho dir
            else{
                if(novo == pai->dir){
                    rotacaoEsq(&pai);
                    novo = pai;
                    pai = novo->pai;
                }
                // SUBCASO 1.3: Tio >>> BLACK + Novo >>> Filho esq
                rotacaoDir(&avo);
                int temp = pai->cor;
                pai->cor = avo->cor;
                avo->cor = temp;
                novo = pai;
            }


        }
        // CASO 2: Pai eh filho dir do avo
        else{
            struct No *tio = avo->esq;

            // SubVaso 2.1: Tio >>> RED
            if(ehVermelho(tio)){
                avo->cor = RED;
                pai->cor = BLACK;
                tio->cor = BLACK;
                novo = avo;
            }else{
                // SubCaso 2.2: Tio >>> BLACK + novo filho esta na esq
                if(novo == pai->esq){
                    rotacaoDir(&pai);
                    novo = pai;
                    pai = novo->pai;
                }

                // SubCaso 2.3: Tio >>> BLACK + novo filho esta na dir
                rotacaoEsq(&avo);
                int temp = pai->cor;
                pai->cor = avo->cor;
                avo->cor = temp;
                novo = pai;
            }
        }
    }

    (*raiz)->cor = BLACK;

}

// Funcao para inserir um novo produto na arvore
void inserirProduto(struct No **raiz, struct Produto novoProduto){
    struct No *novo = criarNo(novoProduto);
    if(novo == NULL){
        printf("ERROR : Memoria insuficiente para inserir o produto.\n");
        return;
    }

    struct No *atual = *raiz;
    struct No *pai = NULL;

    // encontrar a posicao de insercao
    while(atual != NULL){
        pai = atual;
        if(novoProduto.codigo < atual->produto.codigo){
            atual = atual->esq;
        }else if(novoProduto.codigo > atual->produto.codigo){
            atual = atual->dir;
        }else{
            printf("ERROR : Produto com codigo %i ja existe.\n", novoProduto.codigo);
            free(novo);
            return;
        }
    }

    novo->pai = pai;

    if(pai == NULL){
        *raiz = novo; // primeiro noh da arvore
    }else if(novoProduto.codigo < pai->produto.codigo){
        pai->esq = novo; // insere a esquerda
    }else{
        pai->dir = novo; // insere a direita
    }
    
    balancearInsercao(raiz, novo);
    printf(" >> Produto [%s] cadastrado com sucesso! << \n", novoProduto.nome);
    
}
// Funcao para encontrar o noh com o valor minimo
struct No* encontrarMinimo(struct No* no){
    while(no->esq != NULL){
        no = no->esq;
    }
    return no;
}

void balancearRemocao(struct No **raiz, struct No *noSucessor){
    if(noSucessor == NULL || noSucessor->pai == NULL){
        return;
    }

    struct No *irmao;

    // soh balanceia se o noh sucessor nao for raiz, nem preto
    while(noSucessor != *raiz && ehPreto(noSucessor)){
        if(noSucessor == noSucessor->pai->esq){
            // define o irmao
            irmao = noSucessor->pai->dir;
            if(irmao == NULL) break; 
            
            // CASO 1: irmao >>> RED
            if(ehVermelho(irmao)){
                irmao->cor = BLACK;
                noSucessor->pai->cor = RED;
                rotacaoEsq(&noSucessor->pai);
                irmao = noSucessor->pai->dir;
            }
            
            // CASO 2: irmao >>> BLACK + filhos esquerdo e direito >>> BLACK
            if((irmao->esq == NULL || ehPreto(irmao->esq)) && 
               (irmao->dir == NULL || ehPreto(irmao->dir))){
                irmao->cor = RED;
                noSucessor = noSucessor->pai;
            }else{
                // CASO 3: filho direito do irmao >>> BLACK
                if(irmao->dir == NULL || ehPreto(irmao->dir)){
                    if(irmao->esq != NULL){
                        irmao->esq->cor = BLACK;
                    }
                    irmao->cor = RED;
                    rotacaoDir(&irmao);
                    irmao = noSucessor->pai->dir;
                }
                
                // CASO 4: irmao >>> cor do pai + cor do pai >>> BLACK + filho direito do irmao >>> BLACK
                irmao->cor = noSucessor->pai->cor;
                noSucessor->pai->cor = BLACK;
                if(irmao->dir != NULL){
                    irmao->dir->cor = BLACK;
                }
                rotacaoEsq(&noSucessor->pai);
                noSucessor = *raiz;
            }
        }else{
            // Caso simetrico, quando o noh sucessor eh filho direito do pai
            irmao = noSucessor->pai->esq;
            if(irmao == NULL) break;  // verifica se o irmao existe
            
            // SUBCASO 1: irmao >>> RED
            if(ehVermelho(irmao)){
                irmao->cor = BLACK;
                noSucessor->pai->cor = RED;
                rotacaoDir(&noSucessor->pai);
                irmao = noSucessor->pai->esq;
            }
            
            // SUBCASO 2: irmao >>> BLACK + filhos esquerdo e direito >>> BLACK
            if((irmao->esq == NULL || ehPreto(irmao->esq)) && 
               (irmao->dir == NULL || ehPreto(irmao->dir))){
                irmao->cor = RED;
                noSucessor = noSucessor->pai;
            }else{
                // SUBCASO 3: filho esquerdo do irmao >>> BLACK
                if(irmao->esq == NULL || ehPreto(irmao->esq)){
                    if(irmao->dir != NULL){
                        irmao->dir->cor = BLACK;
                    }
                    irmao->cor = RED;
                    rotacaoEsq(&irmao);
                    irmao = noSucessor->pai->esq;
                }
                
                // SUBCASO 4: irmao >>> cor do pai + cor do pai >>> BLACK + filho esquerdo do irmao >>> BLACK
                irmao->cor = noSucessor->pai->cor;
                noSucessor->pai->cor = BLACK;
                if(irmao->esq != NULL){
                    irmao->esq->cor = BLACK;
                }
                rotacaoDir(&noSucessor->pai);
                noSucessor = *raiz;
            }
        }
    }
    noSucessor->cor = BLACK;
}

int removerProduto(struct No **raiz, int removerProdutoCodigo){
    if(*raiz == NULL){
        printf(" >> Produto com codigo %i nao encontrado!\n", removerProdutoCodigo);
        return 0;
    }

    struct No *no = *raiz;
    struct No *pai = NULL;

    // buscar o noh a ser removido
    while(no != NULL && no->produto.codigo != removerProdutoCodigo){
        pai = no;
        if(removerProdutoCodigo < no->produto.codigo){
            no = no->esq;
        }else{
            no = no->dir;
        }
    }

    if(no == NULL){
        printf(" >> Produto com codigo %i nao encontrado!\n", removerProdutoCodigo);
        return 0;
    }

    struct No *noSucessor, *noRemovido = no;
    int corOriginal = noRemovido->cor;

    // CASO 1: No folha(sem filhos)
    if(no->esq == NULL && no->dir == NULL){
        if(pai == NULL){
            *raiz = NULL;
        }else{
            if(no == pai->esq){
                pai->esq = NULL;
            }else{
                pai->dir = NULL;
            }
        }
        noSucessor = NULL;
    }
    
    // CASO 2: No com 1 filho
    else if(no->esq == NULL){
        noSucessor = no->dir;
        if(pai == NULL){
            *raiz = noSucessor;
        }else{
            if(no == pai->esq){
                pai->esq = noSucessor;
            }else{
                pai->dir = noSucessor;
            }
        }
        noSucessor->pai = pai;
    }else if(no->dir == NULL){
        noSucessor = no->esq;
        if(pai == NULL){
            *raiz = noSucessor;
        }else{
            if(no == pai->esq){
                pai->esq = noSucessor;
            }else{
                pai->dir = noSucessor;
            }
        }
        noSucessor->pai = pai;
    }
    // CASO 3: No com dois filhos
    else{
        noRemovido = encontrarMinimo(no->dir); // retorna o noh com o menor valor a subarv dir
        corOriginal = noRemovido->cor;
        noSucessor = noRemovido->dir;
        
        // Reorganiza os nos para substituir o no pelo sucessor
        if(noRemovido->pai == no){
            if(noSucessor != NULL){
                noSucessor->pai = noRemovido;
            }
        }else{
            if(noSucessor != NULL){
                noSucessor->pai = noRemovido->pai;
            }
            noRemovido->pai->esq = noSucessor;
            noRemovido->dir = no->dir;
            no->dir->pai = noRemovido;
        }

        if(pai == NULL){
            *raiz = noRemovido;
        }else{
            if(no == pai->esq){
                pai->esq = noRemovido;
            }else{
                pai->dir = noRemovido;
            }
        }
        
        // atualiza os ponteiros e a cor do noh sucessor
        noRemovido->pai = pai;
        noRemovido->esq = no->esq;
        no->esq->pai = noRemovido;
        noRemovido->cor = no->cor;
    }

    free(no);
    // Se o noh removido for preto, chama o balanceamento
    if(corOriginal == BLACK){
        balancearRemocao(raiz, noSucessor);
    }
    
    return 1;    

}

// Funcao de busca que retorna o noh encontrado
struct No *buscarProduto(struct No *raiz, int codigo){
    if (raiz == NULL || raiz->produto.codigo == codigo)
        return raiz;
    
    if (codigo < raiz->produto.codigo)
        return buscarProduto(raiz->esq, codigo);
    
    return buscarProduto(raiz->dir, codigo);
}
// Funcao para listar todos os produtos da arvore
void listarProdutos(struct No *raiz){
    if (raiz != NULL) {
        listarProdutos(raiz->esq);
        printf(" >> Codigo: %d\n", raiz->produto.codigo);
        printf(" >> Nome: %s\n", raiz->produto.nome);
        printf(" >> Quantidade: %d\n", raiz->produto.quantidade);
        printf(" >> Preco: %.2f\n", raiz->produto.preco);
        printf(" >> Cor: %s\n", raiz->cor == RED ? "Vermelho" : "Preto");
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
    int codigo;

    while(opc != 0){
        printf("\n========= <Sistema de Inventario> =========\n");
        printf("        <1> - Inserir produto  - <1>\n");
        printf("        <2> - Remover produto  - <2>\n");
        printf("        <3> - Buscar produto   - <3>\n");
        printf("        <4> - Listar produtos  - <4>\n");
        printf("        <0> -      Sair        - <0>\n");
        printf("========= <Sistema de Inventario> =========\n");

        printf("\n >> Escolha uma opcao: ");
        scanf("%i", &opc);
        getchar();

        switch(opc){
            case 1:
                printf(" == INSERIR PRODUTO ==\n");
                printf(" >> Codigo do produto: ");
                scanf("%i", &produto.codigo);
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
                printf(" == REMOVER PRODUTO ==\n");
                printf(" >> Codigo do produto: ");
                scanf("%i", &codigo);
                getchar();

                if(removerProduto(&raiz, codigo)){
                    printf(" >> produto removido com sucesso!\n");
                }
                break;
            case 3:
                printf(" >> Codigo do produto: ");
                scanf("%i", &codigo);
                getchar();

                struct No *encontrado = buscarProduto(raiz, codigo);
                if(encontrado != NULL) {
                    printf("\nProduto encontrado:\n");
                    printf("Codigo: %d\n", encontrado->produto.codigo);
                    printf("Nome: %s\n", encontrado->produto.nome);
                    printf("Quantidade: %d\n", encontrado->produto.quantidade);
                    printf("Preco: %.2f\n", encontrado->produto.preco);
                    printf("Cor do no: %s\n", encontrado->cor == RED ? "Vermelho" : "Preto");
                } else {
                    printf("Produto nao encontrado!\n");
                }
                break;
            case 4:
                printf("\n=== Lista de Produtos ===\n");
                if(raiz == NULL) {
                    printf("Nenhum produto cadastrado!\n");
                } else {
                    listarProdutos(raiz);
                }
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
