#include <stdio.h>
#include <string.h>
#include "personagem.h"
#include "utils.h"

void criar_Personagem(personagem *j) {
    int opcao;

    do {
        printf("\n+=========== ESCOLHA SEU HERÓI ===========+\n");
        printf("|   [1] Clara                             |\n");
        printf("|   [2] Felipe                            |\n");
        printf("|   [3] Miguel                            |\n");
        printf("+=========================================+\n");
        printf(">> Opção: ");
        opcao = ler_Inteiro();

        switch (opcao) {
            case 1:
                strncpy(j->nome, "Clara", sizeof(j->nome) - 1);
                break;
            case 2:
                strncpy(j->nome, "Felipe", sizeof(j->nome) - 1);
                break;
            case 3:
                strncpy(j->nome, "Miguel", sizeof(j->nome) - 1);
                break;
            default:
                printf("\n[!] Opção inválida! Escolha entre 1 a 3.\n");
                break;
        }
    } while (opcao < 1 || opcao > 3);

    // garante o terminador \0 na string
    j->nome[sizeof(j->nome) - 1] = '\0';

    // inicializa os atributos iniciais
    j->vida_max = 30;
    j->vida = j->vida_max;
    j->nivel = 1;
    j->atq = 5;
    j->def = 2;
    j->xp = 0;
    j->ouro = 20;
    j->inventario = NULL;
    // slots de equipamentos vazios
    j->arma_eq = NULL;
    j->armadura_eq = NULL;
}

int xp_Necessario(int nivel) {
    return 20 * nivel;
}

// calcula ataque total (base + bônus da arma)
int atq_Total(personagem *j) {
    if (j->arma_eq != NULL) {
        return j->atq + j->arma_eq->efeito;
    }

    return j->atq;
}

int def_Total(personagem *j) {
    if (j->armadura_eq != NULL) {
        return j->def + j->armadura_eq->efeito;
    }

    return j->def;
}

void mostrar_Personagem(personagem *j) {
    printf("\n+======== FICHA DO PERSONAGEM ========+\n");
    printf("  @  Nome   : %s\n", j->nome);
    printf("  +  Nível  : %d\n", j->nivel);
    printf("  <3 Vida   : %d/%d\n", j->vida, j->vida_max);
    printf("  /  Ataque : %d (+%d da arma)\n", atq_Total(j), atq_Total(j) - j->atq);
    printf("  #  Defesa : %d (+%d da armadura)\n", def_Total(j), def_Total(j) - j->def);
    printf("  *  XP     : %d/%d\n", j->xp, xp_Necessario(j->nivel));
    printf("  $  Ouro   : %d\n", j->ouro);
    printf("+=====================================+\n");
}

// soma o xp ganho e sobe o nível se aplicável
void subiu_Level(personagem *j, int xp) {
    if (xp <= 0) {
        return;
    }
    
    j->xp += xp;

    while (j->xp >= xp_Necessario(j->nivel)) {
        // desconta o xp do nível e o resto vai para o próximo
        j->xp -= xp_Necessario(j->nivel);
        j->nivel++;
        // bônus de atributos a cada nível
        j->vida_max += 10;
        j->atq += 3;
        j->def += 2;
        j->vida = j->vida_max;

        printf("\n  *.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*\n");
        printf("  *   LEVEL UP!  %s chegou ao nível %d!\n", j->nome, j->nivel);
        printf("  *   Vida máx: %d | Ataque: %d | Defesa: %d\n  *   (vida restaurada!)\n", j->vida_max, j->atq, j->def);
        printf("  *.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*\n");
    }
}

// cura a vida sem passar do máximo e devolve quanto foi curado
int curar_Personagem(personagem *j, int quantidade) {
    int antes = j->vida;

    j->vida += quantidade;
    if (j->vida > j->vida_max) {
        j->vida = j->vida_max;
    }

    return j->vida - antes;
}
