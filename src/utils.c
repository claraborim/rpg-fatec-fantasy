#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// dado de 1 a 12
int rolar_D12() {
    return rand() % 12 + 1;
}

// pausa nos diálogos, turnos, etc
void continuar() {
    int c;

    printf("\n  ... [ENTER] para continuar ...");
    while ((c = getchar()) != '\n' && c != EOF);
}

// validação para caso o usuário insira um caractere
int ler_Inteiro() {
    int valor, c;

    if (scanf("%d", &valor) != 1) {
        valor = 0;
    }
    // limpa o que sobrou no buffer
    while ((c = getchar()) != '\n' && c != EOF);

    return valor;
}
