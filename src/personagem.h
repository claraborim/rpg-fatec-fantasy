#ifndef PERSONAGEM_H
#define PERSONAGEM_H

// define tipos de itens e numera a partir de 0
typedef enum {
    ARMA, ARMADURA, POCAO
} TipoItem;

// nó da lista encadeada do inventário
typedef struct Item {
    char nome[40];
    TipoItem tipo;
    int efeito;
    int valor;
    struct Item *prox;
} item;

typedef struct Personagem {
    char nome[20];
    int vida;
    int vida_max;
    int nivel;
    int atq;
    int def;
    int xp;
    int ouro;
    item *inventario;
    item *arma_eq;
    item *armadura_eq;
} personagem;

void criar_Personagem(personagem *j);
int xp_Necessario(int nivel);

// calcula ataque total (base + bônus da arma)
int atq_Total(personagem *j);
int def_Total(personagem *j);

void mostrar_Personagem(personagem *j);

// soma o xp ganho e sobe o nível se aplicável
void subiu_Level(personagem *j, int xp);

// cura a vida sem passar do máximo e devolve quanto foi curado
int curar_Personagem(personagem *j, int quantidade);

#endif
