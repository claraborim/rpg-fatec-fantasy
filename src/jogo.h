#ifndef JOGO_H
#define JOGO_H

#include "personagem.h"
#include "monstro.h"
#include "loja.h"

// sorteia o que acontece no explorar
int explorar(personagem *j, fila *f, pilha *maleta);

void menu(personagem *j, fila *f, pilha *maleta);

#endif