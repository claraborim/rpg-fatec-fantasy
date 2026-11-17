#include <stdio.h>
#include <stdlib.h>
#include "monstro.h"

// catálogo dos monstros
const monstro listaMonstros[] = {
    { "Fábio, o Mestre do Sono Profundo", 16, 5, 1, 50, 15, 0 },
    { "Cris, a Rainha do Ar-Condicionado", 24, 7, 3, 50, 20, 0 },
    { "Robson, o Rei da Confusão Mental", 26, 12, 3, 50, 25, 0 },
    { "Serjão, o Dono dos Cabarés", 60, 15, 5, 50, 30, 0 },
    { "Dimas, o Ceifador de Médias", 140, 21, 7, 100, 500, 1 }
};

// calcula quantos monstros tem na lista
const int TOTAL_MONSTROS = (int)(sizeof(listaMonstros) / sizeof(listaMonstros[0]));

// deixa a fila vazia
void criar_Fila(fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
}

// insere o monstro no fim da fila
int enfileirar_Monstro(fila *f, monstro m) {
    no_monstro *novo = malloc(sizeof(no_monstro));

    if (novo == NULL) {
        printf("[ERRO] Sem memória!\n");
        return 0;
    }

    // o nó guarda uma cópia dos dados do monstro
    novo->dados = m;
    novo->prox = NULL;

    // fila vazia: novo nó é o início
    if (f->fim == NULL) {
        f->inicio = novo;
    }
    // antigo último nó aponta para o novo
    else {
        f->fim->prox = novo;
    }

    f->fim = novo;

    return 1;
}

// remove o monstro do início da fila
int desenfileirar_Monstro(fila *f) {
    if (f->inicio == NULL) {
        return 0;
    }

    no_monstro *removido = f->inicio;
    // o segundo passa a ser o primeiro
    f->inicio = removido->prox;

    // se a fila esvaziou, fim vira NULL
    if (f->inicio == NULL) {
        f->fim = NULL;
    }

    free(removido);

    return 1;
}

// consulta o primeiro da fila sem remover
monstro *ver_Primeiro(fila *f) {
    if (f->inicio == NULL) {
        return NULL;
    }

    return &f->inicio->dados;
}

// libera todos os nós (desenfileira)
void liberar_Fila(fila *f) {
    while (desenfileirar_Monstro(f)) {
    }
}
