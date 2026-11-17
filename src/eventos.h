#ifndef EVENTOS_H
#define EVENTOS_H

#include "personagem.h"

// armadilhas que dão dano, mas nunca matam o jogador
void evento_Armadilha(personagem *j);

// sorteio dos locais da fatec
void evento_Local(personagem *j);

// baús
void evento_Bau(personagem *j);

#endif
