#include <stdio.h>
#include "batalha.h"
#include "inventario.h"
#include "utils.h"

// rola o d12 e calcula o dano do ataque
int calcular_Dano(int atq, int def) {
    int d12 = rolar_D12();

    // 1 = errou o ataque
    if (d12 == 1) {
        printf("    [d12 = 1]  Errou o ataque! (miss)\n");
        return 0;
    }

    // dano base
    int dano = atq - def;
    if (dano < 1) {
        dano = 1;
    }

    // 12 = acerto crítico (dobro de dano)
    if (d12 == 12) {
        printf("    [d12 = 12]  *** ACERTO CRÍTICO! ***\n");
        dano *= 2;
    }
    else {
        printf("    [d12 = %d]\n", d12);
    }

    return dano;
}

// retorna 1 se o jogador venceu, 0 se foi derrotado
int batalha(personagem *j, monstro *m) {
    printf("\n<<<<<<<<<<<<<<< BATALHA >>>>>>>>>>>>>>>\n !!! %s apareceu !!!\n", m->nome);

    // turno do jogador, depois monstro
    while (j->vida > 0 && m->vida > 0) {
        int defendendo = 0;
        int acao_feita = 0;

        printf("\n-----------------------------------------------------------------------\n [%s] Vida: %d/%d   x   [%s] Vida: %d\n-----------------------------------------------------------------------\n",
               j->nome, j->vida, j->vida_max, m->nome, m->vida);

        // repete o menu até o jogador fazer uma ação que gaste o turno
        while (!acao_feita) {
            printf("\n  [1] Atacar\n  [2] Defender\n  [3] Inventário\n>> Opção: ");
            int opcao = ler_Inteiro();

            switch (opcao) {
                case 1: {
                    printf("\n  >>> %s ataca!\n", j->nome);
                    int dano = calcular_Dano(atq_Total(j), m->def);
                    m->vida -= dano;
                    if (m->vida < 0) {
                        m->vida = 0;
                    }
                    printf("    -> Dano causado: %d | %s: %d de vida\n", dano, m->nome, m->vida);
                    acao_feita = 1;
                    break;
                }
                case 2:
                    printf("\n  [#] %s se prepara para se defender!\n", j->nome);
                    defendendo = 1;
                    acao_feita = 1;
                    break;
                case 3:
                    acao_feita = usar_Item(j);
                    break;
                default:
                    printf("\n  [!] Opção inválida!\n");
                    break;
            }
        }

        // se o monstro morreu, não ataca de volta
        if (m->vida == 0) {
            break;
        }

        // turno do monstro
        printf("\n  <<< %s ataca!\n", m->nome);
        int dano = calcular_Dano(m->atq, def_Total(j));

        if (defendendo && dano > 1) {
            dano = (dano + 1) / 2;
            printf("    [#] A defesa reduziu o dano!\n");
        }

        j->vida -= dano;
        if (j->vida < 0) {
            j->vida = 0;
        }
        printf("    -> Dano causado: %d | %s: %d/%d de vida\n", dano, j->nome, j->vida, j->vida_max);
        continuar();
    }

    if (j->vida == 0) {
        printf("\n  x_x  %s foi derrotado...\n", j->nome);
        return 0;
    }

    printf("\n*** VITÓRIA! %s derrotou %s! ***\n", j->nome, m->nome);

    // recompensas da vitória
    j->ouro += m->ouro_morte;
    printf("  [+] Você ganhou %d de XP e %d de ouro.\n", m->xp_morte, m->ouro_morte);
    subiu_Level(j, m->xp_morte);
    continuar();
    return 1;
}
