#ifndef INVENTARIO_H
#define INVENTARIO_H

#include "personagem.h"

// cria o nó e devolve o endereço
item *criar_Item(const char *nome, TipoItem tipo, int efeito, int valor);

// cria um nó e coloca no final da lista
void adicionar_Item(personagem *j, const char *nome, TipoItem tipo, int efeito, int valor);

// procura o item pelo nome e remove da lista
int remover_Item(personagem *j, const char *nome);

// equipa o item da posição escolhida
void equipar_Item(personagem *j, int posicao);

// imprime a lista de itens no inventário
void mostrar_Inventario(personagem *j);

// inventário fora da batalha
void gerenciar_Inventario(personagem *j);

// apaga todos os nós
void liberar_Inventario(personagem *j);

// usa um item na batalha
int usar_Item(personagem *j);

#endif
