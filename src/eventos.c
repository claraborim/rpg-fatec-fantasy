#include <stdio.h>
#include <stdlib.h>
#include "eventos.h"
#include "inventario.h"
#include "utils.h"

// armadilhas que dão dano, mas nunca matam o jogador
void evento_Armadilha(personagem *j) {
    // dano entre 3 e 6
    int dano = rand() % 4 + 3;
    // sorteio entre 4 armadilhas
    int tipo = rand() % 4;
    // situação: texto da armadilha, sorte: se vai ter dano ou não (caso esteja com 1 de vida)
    const char *situacao;
    const char *sorte;

    switch (tipo) {
        case 0:
            situacao = "Você estava entrando na Fatec quando um monstro saiu do meio das árvores, te desceu a porrada e foi embora como se nada tivesse acontecido!";
            sorte = "Por sorte, você ouviu o barulho nas árvores e entrou correndo antes que o monstro te alcançasse.";
            break;
        case 1:
            situacao = "Você foi pegar sua mochila do chão e um escorpião saiu de dentro... ele te picou!";
            sorte = "Por sorte, você viu o escorpião a tempo e ele foi embora sem te picar.";
            break;
        case 2:
            situacao = "Você se apoiou na grade do lago e acabou caindo na água podre... o cheiro vai grudar em você pelo resto do dia!";
            sorte = "Por sorte, você se desequilibrou, mas se segurou na grade e não caiu.";
            break;
        default:
            situacao = "Você foi atravessar do P10 para o P11, mas um carro surgiu do além e te atropelou!";
            sorte = "Por sorte, o carro freou a tempo e só te deu um susto.";
            break;
    }

    printf("\n+--------------- ARMADILHA! ---------------+\n");
    printf("  [!] %s\n", situacao);

    // garante que sobre pelo menos 1 de vida
    if (dano >= j->vida) {
        dano = j->vida - 1;
    }

    if (dano > 0) {
        j->vida -= dano;
        printf("  -> Você perdeu %d de vida! (%d/%d)\n", dano, j->vida, j->vida_max);
    }
    // se estiver com 1 de vida o dano vira 0
    else {
        printf("  -> %s\n", sorte);
    }

    continuar();
}

// sorteio dos locais da fatec
void evento_Local(personagem *j) {
    int local = rand() % 3;

    printf("\n+------------- LOCAL DA FATEC -------------+\n");

    switch (local) {
        case 0: {
            printf("  [@] Você passa na cantina e come um baffs bem recheado e quentinho.\n");
            int curou = curar_Personagem(j, 15);
            if (curou > 0) {
                printf("  [+] Você recuperou %d de vida! (%d/%d)\n", curou, j->vida, j->vida_max);
            }
            else {
                printf("  [i] Delicioso, mas sua vida já estava cheia.\n");
            }
            break;
        }
        case 1:
            printf("  [@] Na biblioteca, você acha dinheiro esquecido dentro de um livro.\n");
            j->ouro += 10;
            printf("  [+] Você ganhou 10 de ouro! (total: %d)\n", j->ouro);
            break;
        case 2: {
            printf("  [@] O bebedouro estava funcionando hoje e a água não estava podre!\n");
            int curou = curar_Personagem(j, 10);
            if (curou > 0) {
                printf("  [+] Você recuperou %d de vida! (%d/%d)\n", curou, j->vida, j->vida_max);
            }
            else {
                printf("  [i] Gosto razoável, mas sua vida já estava cheia.\n");
            }
            break;
        }
    }
    continuar();
}

// baús
void evento_Bau(personagem *j) {
    int sorte = rolar_D12();

    printf("\n+--------------- BAÚ ENCONTRADO ---------------+\n");
    printf("  [$] Você encontra um baú esquecido numa sala vazia!\n");

    // resultado do d12 decide o item de dentro
    if (sorte <= 4) {
        // ouro sorteado entre 5 e 15
        int ouro = rand() % 11 + 5;
        j->ouro += ouro;
        printf("  [+] Dentro tinha %d de ouro! (total: %d)\n", ouro, j->ouro);
    }
    else if (sorte <= 8) {
        adicionar_Item(j, "Big Baffs", POCAO, 30, 5);
        printf("  [+] Dentro tinha uma Big Baffs!\n");
    }
    else if (sorte <= 10) {
        adicionar_Item(j, "Teclado Mecânico", ARMA, 2, 20);
        printf("  [+] Dentro tinha um Teclado Mecânico! (Ataque +3)\n");
    }
    else {
        adicionar_Item(j, "Moletom Rasgado", ARMADURA, 1, 20);
        printf("  [+] Dentro tinha um Moletom Rasgado! (Defesa +2)\n");
    }

    continuar();
}
