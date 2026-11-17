#ifndef LOJA_H
#define LOJA_H

#include "personagem.h"

// item à venda na loja
typedef struct ItemLoja {
    char nome[40];
    TipoItem tipo;
    int efeito;
    int preco;
} item_loja;

// catálogo da loja e quantos itens tem nele (definidos em loja.c)
extern const item_loja catalogoLoja[];
extern const int TOTAL_CATALOGO;

// nó da pilha encadeada da loja
typedef struct NoLoja {
    item_loja dados;
    struct NoLoja *prox;
} no_loja;

typedef struct Pilha {
    no_loja *topo;
} pilha;

// deixa a pilha vazia
void criar_Pilha(pilha *p);

// retorna 1 se a pilha não tem nenhum item
int pilha_Vazia(pilha *p);

// push: o item novo vira o topo da pilha
int empilhar_Loja(pilha *p, item_loja it);

// pop: tira o topo da pilha e devolve os dados em *aux (retorna 0 se estava vazia)
int desempilhar_Loja(pilha *p, item_loja *aux);

// consulta o topo da pilha sem remover
item_loja *ver_Topo(pilha *p);

// move o nó do topo de uma pilha para o topo de outra, sem alocar nem liberar memória
void mover_Topo(pilha *origem, pilha *destino);

// libera todos os nós (desempilha)
void liberar_Pilha(pilha *p);

// loja depois de cada luta: o vendedor mostra o item do topo da maleta (pilha)
void abrir_Loja(personagem *j, pilha *maleta);

#endif
