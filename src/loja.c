#include <stdio.h>
#include <stdlib.h>
#include "loja.h"
#include "inventario.h"
#include "utils.h"

// catálogo da loja: o último da lista fica no topo da pilha
const item_loja catalogoLoja[] = {
    { "Big Baffs", POCAO, 30, 10 },
    { "Big Baffs", POCAO, 30, 10 },
    { "Big Baffs", POCAO, 30, 10 },
    { "Camisa da Fatec", ARMADURA, 3, 45 },
    { "Bíblia de Matemática do Paulo", ARMA, 4, 40 }
};

// calcula quantos itens tem no catálogo
const int TOTAL_CATALOGO = (int)(sizeof(catalogoLoja) / sizeof(catalogoLoja[0]));

// deixa a pilha vazia
void criar_Pilha(pilha *p) {
    p->topo = NULL;
}

// retorna 1 se a pilha não tem nenhum item
int pilha_Vazia(pilha *p) {
    return p->topo == NULL;
}
 
// push: o item novo vira o topo da pilha
int empilhar_Loja(pilha *p, item_loja it) {
    no_loja *novo = malloc(sizeof(no_loja));
 
    if (novo == NULL) {
        printf("[ERRO] Sem memória!\n");
        return 0;
    }
 
    // o nó guarda uma cópia dos dados do item
    novo->dados = it;
    // o novo nó aponta para o antigo topo
    novo->prox = p->topo;
    p->topo = novo;
 
    return 1;
}
 
// pop: tira o topo da pilha e devolve os dados em *aux (retorna 0 se estava vazia)
int desempilhar_Loja(pilha *p, item_loja *aux) {
    if (p->topo == NULL) {
        return 0;
    }
 
    no_loja *removido = p->topo;
    // guarda os dados antes de liberar o nó
    *aux = removido->dados;
    // o segundo passa a ser o topo
    p->topo = removido->prox;
 
    free(removido);
 
    return 1;
}
 
// consulta o topo da pilha sem remover
item_loja *ver_Topo(pilha *p) {
    if (p->topo == NULL) {
        return NULL;
    }
 
    return &p->topo->dados;
}
 
// move o nó do topo de uma pilha para o topo de outra, sem alocar nem liberar memória
void mover_Topo(pilha *origem, pilha *destino) {
    if (origem->topo == NULL) {
        return;
    }
 
    no_loja *no = origem->topo;
    // tira o nó da origem
    origem->topo = no->prox;
    // coloca o mesmo nó no topo do destino
    no->prox = destino->topo;
    destino->topo = no;
}
 
// libera todos os nós (desempilha)
void liberar_Pilha(pilha *p) {
    item_loja descarte;
 
    while (desempilhar_Loja(p, &descarte)) {
    }
}
 
// loja depois de cada luta: o vendedor mostra o item do topo da maleta (pilha)
void abrir_Loja(personagem *j, pilha *maleta) {
    const char *tipos[] = {"Arma", "Armadura", "Poção"};
    // pilha auxiliar: guarda os itens que o jogador já viu e não comprou
    pilha vistos;
    int comprando = 1;
 
    criar_Pilha(&vistos);
 
    printf("\n[$] A vendedora Angelina surge no corredor e abre a maleta:\n");
    printf("    \"Psiu! Quer comprar alguma coisa?\"\n");
 
    while (comprando) {
        // consulta o topo sem remover
        item_loja *topo = ver_Topo(maleta);
 
        printf("\n+================== LOJA ==================+\n");
        printf("  $  Seu ouro: %d\n\n", j->ouro);
 
        // maleta vazia
        if (topo == NULL) {
            if (pilha_Vazia(&vistos)) {
                printf("  [!] O vendedor já vendeu tudo!\n");
            }
            else {
                printf("  [i] Esse era o último item da maleta.\n");
            }
            break;
        }
 
        printf("  [%s] %s  >>  ", tipos[topo->tipo], topo->nome);
 
        switch (topo->tipo) {
            case ARMA:
                printf("Ataque +%d", topo->efeito);
                break;
            case ARMADURA:
                printf("Defesa +%d", topo->efeito);
                break;
            case POCAO:
                printf("Cura +%d", topo->efeito);
                break;
        }
 
        printf("  ($ %d ouro(s))\n", topo->preco);
        printf("\n  [1] Comprar\n  [2] Ver o próximo item\n  [0] Sair da loja\n>> Opção: ");
        int opcao = ler_Inteiro();
 
        switch (opcao) {
            case 1:
                if (j->ouro < topo->preco) {
                    printf("\n  [!] Você precisa de mais %d de ouro.\n", topo->preco - j->ouro);
                }
                else {
                    item_loja comprado;
 
                    // pop: o item sai da maleta e não volta mais
                    desempilhar_Loja(maleta, &comprado);
                    j->ouro -= comprado.preco;
                    // o valor de revenda do item é metade do preço
                    adicionar_Item(j, comprado.nome, comprado.tipo, comprado.efeito, comprado.preco / 2);
                    printf("\n  [+] Você comprou %s por %d de ouro! (ouro restante: %d)\n", comprado.nome, comprado.preco, j->ouro);
                }
                break;
            case 2:
                // o item do topo vai para a pilha auxiliar e o próximo aparece
                mover_Topo(maleta, &vistos);
                break;
            case 0:
                comprando = 0;
                break;
            default:
                printf("\n  [!] Opção inválida!\n");
                break;
        }
    }
 
    // devolve os itens vistos para a maleta (mover de volta inverte de novo e restaura a ordem)
    while (!pilha_Vazia(&vistos)) {
        mover_Topo(&vistos, maleta);
    }
 
    printf("\n  [i] Você se despede do vendedor.\n");
}
