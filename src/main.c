#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "personagem.h"
#include "inventario.h"
#include "monstro.h"
#include "loja.h"
#include "jogo.h"

int main() {
    // exibir UTF-8 para acentos
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    setlocale(LC_ALL, ".UTF-8");

    // inicializa números aleatórios para o dado (d12)
    srand(time(NULL));

    printf("\n+===============================+\n");
    printf("|                               |\n");
    printf("|          RPG  FATEC           |\n");
    printf("|                               |\n");
    printf("+===============================+\n");

    personagem jogador;

    fila filaMonstros;

    pilha maleta;

    criar_Personagem(&jogador);
    // item inicial do inventário
    adicionar_Item(&jogador, "Big Baffs", POCAO, 30, 5);

    // monta a fila de monstros na ordem
    criar_Fila(&filaMonstros);
    for (int i = 0; i < TOTAL_MONSTROS; i++) {
        // se faltar memória, libera o que já foi alocado e encerra
        if (!enfileirar_Monstro(&filaMonstros, listaMonstros[i])) {
            liberar_Fila(&filaMonstros);
            liberar_Inventario(&jogador);
            return 1;
        }
    }

    // monta a maleta do vendedor
    criar_Pilha(&maleta);
    for (int i = 0; i < TOTAL_CATALOGO; i++) {
        if (!empilhar_Loja(&maleta, catalogoLoja[i])) {
            liberar_Pilha(&maleta);
            liberar_Fila(&filaMonstros);
            liberar_Inventario(&jogador);
            return 1;
        }
    }

    menu(&jogador, &filaMonstros, &maleta);

    // libera toda a memória alocada
    liberar_Pilha(&maleta);
    liberar_Fila(&filaMonstros);
    liberar_Inventario(&jogador);
    return 0;
}
