#include <stdio.h>
#include "jogo.h"
#include "batalha.h"
#include "eventos.h"
#include "inventario.h"
#include "utils.h"

// sorteia o que acontece no explorar
int explorar(personagem *j, fila *f, pilha *maleta) {
    int d12 = rolar_D12();
    printf("\n[EXPLORAR] Você explora a Fatec... (d12 = %d)\n", d12);

    if (d12 == 1 || d12 == 2) {
        evento_Armadilha(j);
    }
    else if (d12 == 3 || d12 == 4) {
        evento_Local(j);
    }
    else if (d12 == 5 || d12 == 6) {
        evento_Bau(j);
    }
    else {
        // checa o próximo da fila
        monstro *proximo = ver_Primeiro(f);

        // se sobrou o boss final, vira opção no menu
        if (proximo == NULL || proximo->chefe) {
            printf("\n  [!!!] Você sente uma presença poderosa... o Dimas te espera.\n");
            continuar();
        }
        else {
            // cria cópia para não alterar o monstro
            monstro copia = *proximo;

            if (!batalha(j, &copia)) {
                return 0;
            }

            // se venceu, o monstro sai da fila
            desenfileirar_Monstro(f);
            // após cada luta, a loja abre
            abrir_Loja(j, maleta);
        }
    }

    return 1;
}

void menu(personagem *j, fila *f, pilha *maleta) {
    int jogando = 1;

    while (jogando) {
        monstro *proximo = ver_Primeiro(f);
        // o Dimas só aparece quando é o próximo da fila
        int dimas_liberado = (proximo != NULL && proximo->chefe);

        printf("\n+============== MENU ==============+\n");
        printf("   [1] Explorar\n   [2] Ver Ficha\n   [3] Inventário\n   [4] Sair\n");

        if (dimas_liberado) {
            printf("   [5] Enfrentar o Dimas  <!>\n");
        }

        printf("+==================================+\n");
        printf(">> Opção: ");
        int opcao = ler_Inteiro();

        switch (opcao) {
            case 1:
                if (!explorar(j, f, maleta)) {
                    printf("\n=============== FIM DE JOGO ===============\n");
                    jogando = 0;
                }
                break;
            case 2:
                mostrar_Personagem(j);
                continuar();
                break;
            case 3:
                gerenciar_Inventario(j);
                break;
            case 4:
                jogando = 0;
                break;
            case 5:
                if (dimas_liberado) {
                    monstro chefe = *proximo;

                    if (batalha(j, &chefe)) {
                        desenfileirar_Monstro(f);
                        printf("\n*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
                        printf("*  VOCÊ DERROTOU O DIMAS! FIM DE JOGO!  *\n");
                        printf("*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n\n");
                    }
                    else {
                        printf("\n=============== FIM DE JOGO ===============\n\n");
                    }
                    // encerra o jogo depois da luta com o Dimas
                    jogando = 0;
                }
                else {
                    printf("\n  [!] Opção Inválida!\n");
                    continuar();
                }
                break;
            default:
                printf("\n  [!] Opção Inválida!\n");
                continuar();
                break;
        }
    }
}
