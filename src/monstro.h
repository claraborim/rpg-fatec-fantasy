#ifndef MONSTRO_H
#define MONSTRO_H

typedef struct Monstro {
    char nome[60];
    int vida;
    int atq;
    int def;
    int xp_morte;
    int ouro_morte;
    // 1 = chefe final, 0 = monstro comum
    int chefe;
} monstro;

// catálogo dos monstros e quantos monstros tem nele (definidos em monstro.c)
extern const monstro listaMonstros[];
extern const int TOTAL_MONSTROS;

// nó da fila encadeada de monstros
typedef struct NoMonstro {
    monstro dados;
    struct NoMonstro *prox;
} no_monstro;

typedef struct Fila {
    no_monstro *inicio;
    no_monstro *fim;
} fila;

// deixa a fila vazia
void criar_Fila(fila *f);

// insere o monstro no fim da fila
int enfileirar_Monstro(fila *f, monstro m);

// remove o monstro do início da fila
int desenfileirar_Monstro(fila *f);

// consulta o primeiro da fila sem remover
monstro *ver_Primeiro(fila *f);

// libera todos os nós (desenfileira)
void liberar_Fila(fila *f);

#endif
