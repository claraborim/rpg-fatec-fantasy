#ifndef BATALHA_H
#define BATALHA_H

#include "personagem.h"
#include "monstro.h"

// rola o d12 e calcula o dano do ataque
int calcular_Dano(int atq, int def);

// retorna 1 se o jogador venceu, 0 se foi derrotado
int batalha(personagem *j, monstro *m);

#endif
