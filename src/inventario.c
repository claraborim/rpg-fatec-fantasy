#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventario.h"
#include "utils.h"

// cria o nó e devolve o endereço
item *criar_Item(const char *nome, TipoItem tipo, int efeito, int valor) {
    item *novo = malloc(sizeof(item));
    if (novo == NULL) {
        return NULL;
    }

    // guarda os dados dentro do item
    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';
    novo->tipo = tipo;
    novo->efeito = efeito;
    novo->valor = valor;
    novo->prox = NULL;

    return novo;
}

// cria um nó e coloca no final da lista
void adicionar_Item(personagem *j, const char *nome, TipoItem tipo, int efeito, int valor) {
    // chama a função para criar o nó
    item *novo = criar_Item(nome, tipo, efeito, valor);
    if (novo == NULL) {
        printf("[ERRO] Sem memória!\n");
        return;
    }

    // se a lista está vazia, inicializa o primeiro nó
    if (j->inventario == NULL) {
        j->inventario = novo;
    }
    // se a lista já existe, insere no último nó
    else {
        // aponta para o primeiro nó
        item *aux = j->inventario;

        // percorre a lista até o último nó
        while (aux->prox != NULL) {
            aux = aux->prox;
        }

        // liga o último nó ao novo depois dele
        aux->prox = novo;
    }
}

// procura o item pelo nome e remove da lista
int remover_Item(personagem *j, const char *nome) {
    // nó atual
    item *atual = j->inventario;
    // nó anterior ao atual
    item *anterior = NULL;

    // percorre a lista enquanto não chegar ao fim e o nome do nó atual for diferente do procurado
    while (atual != NULL && strcmp(atual->nome, nome) != 0) {
        anterior = atual;
        atual = atual->prox;
    }

    // se não foi encontrado na lista, retorna 0
    if (atual == NULL) {
        printf("[!] Esse item não existe.\n");
        return 0;
    }

    // se o item for o primeiro nó da lista, a cabeça se torna o segundo nó
    if (anterior == NULL) {
        j->inventario = atual->prox;
    }
    // se o item está no meio ou no fim, o nó anterior pula o removido e aponta para o próximo
    else {
        anterior->prox = atual->prox;
    }

    // libera a memória do nó removido
    free(atual);
    return 1;
}

// equipa o item da posição escolhida
void equipar_Item(personagem *j, int posicao) {
    // nó atual
    item *atual = j->inventario;
    // nó anterior ao atual
    item *anterior = NULL;
 
    // anda até a posição escolhida guardando o anterior
    for (int i = 1; atual != NULL && i < posicao; i++) {
        anterior = atual;
        atual = atual->prox;
    }
 
    // se não foi encontrado, retorna
    if (atual == NULL) {
        printf("\n  [!] Item inválido!\n");
        return;
    }
 
    if (atual->tipo == POCAO) {
        printf("\n  [!] Poções não se equipam, elas são usadas na batalha.\n");
        return;
    }
 
    // tira o nó da lista sem liberar a memória, porque ele vai para o slot
    if (anterior == NULL) {
        j->inventario = atual->prox;
    }
    else {
        anterior->prox = atual->prox;
    }
    atual->prox = NULL;
 
    // se já tinha algo equipado, devolve para o inventário
    if (atual->tipo == ARMA) {
        if (j->arma_eq != NULL) {
            adicionar_Item(j, j->arma_eq->nome, j->arma_eq->tipo, j->arma_eq->efeito, j->arma_eq->valor);
            free(j->arma_eq);
        }
        j->arma_eq = atual;
        printf("\n  [+] Você equipou %s! (Ataque +%d)\n", atual->nome, atual->efeito);
    }
    else {
        if (j->armadura_eq != NULL) {
            adicionar_Item(j, j->armadura_eq->nome, j->armadura_eq->tipo, j->armadura_eq->efeito, j->armadura_eq->valor);
            free(j->armadura_eq);
        }
        j->armadura_eq = atual;
        printf("\n  [+] Você equipou %s! (Defesa +%d)\n", atual->nome, atual->efeito);
    }
}

// imprime a lista de itens no inventário
void mostrar_Inventario(personagem *j) {
    const char *tipos[] = {"Arma", "Armadura", "Poção"};

    printf("\n+============ INVENTÁRIO ============+\n");

    // equipamentos vestidos no momento
    if (j->arma_eq != NULL) {
        printf("  Arma equipada     : %s (Ataque +%d)\n", j->arma_eq->nome, j->arma_eq->efeito);
    }
    else {
        printf("  Arma equipada     : [nenhuma]\n");
    }
 
    if (j->armadura_eq != NULL) {
        printf("  Armadura equipada : %s (Defesa +%d)\n", j->armadura_eq->nome, j->armadura_eq->efeito);
    }
    else {
        printf("  Armadura equipada : [nenhuma]\n");
    }
 
    printf("  ------------------------------------\n");

    // se a cabeça for NULL, o inventário está vazio
    if (j->inventario == NULL) {
        printf("\n  [!] Inventário vazio!\n");
        return;
    }

    int i = 1;

    // percorre a lista até o último nó
    for (item *aux = j->inventario; aux != NULL; aux = aux->prox) {
        printf("  %d. [%s] %s  >>  ", i++, tipos[aux->tipo], aux->nome);

        switch (aux->tipo) {
            case ARMA:
                printf("Ataque +%d", aux->efeito);
                break;
            case ARMADURA:
                printf("Defesa +%d", aux->efeito);
                break;
            case POCAO:
                printf("Cura +%d", aux->efeito);
                break;
        }

        printf("  ($ %d ouro(s))\n", aux->valor);
    }
}

// inventário fora da batalha
void gerenciar_Inventario(personagem *j) {
    int escolha;
 
    do {
        mostrar_Inventario(j);
 
        // sem itens na lista, não tem nada para equipar
        if (j->inventario == NULL) {
            continuar();
            return;
        }
 
        printf("  0. Voltar\n>> Número do item para equipar: ");
        escolha = ler_Inteiro();
 
        if (escolha >= 1) {
            equipar_Item(j, escolha);
        }
    } while (escolha != 0);
}

// apaga todos os nós
void liberar_Inventario(personagem *j) {
    item *aux = j->inventario;

    while (aux != NULL) {
        // antes de apagar guarda o endereço do próximo
        item *prox = aux->prox;

        // apaga o nó atual
        free(aux);
        // avança para o próximo que guardamos
        aux = prox;
    }

    // define o inventário como vazio
    j->inventario = NULL;

    // apaga nós dos slots
    free(j->arma_eq);
    free(j->armadura_eq);
    j->arma_eq = NULL;
    j->armadura_eq = NULL;
}

// usa um item na batalha
int usar_Item(personagem *j) {
    if (j->inventario == NULL) {
        printf("\n  [!] Inventário vazio!\n");
        return 0;
    }

    mostrar_Inventario(j);
    printf("  0. Voltar\n>> Escolha o item: ");
    int escolha = ler_Inteiro();

    // volta ao menu sem gastar turno
    if (escolha < 1) {
        return 0;
    }

    // anda até o nó do número escolhido
    item *aux = j->inventario;
    for (int i = 1; aux != NULL && i < escolha; i++) {
        aux = aux->prox;
    }

    if (aux == NULL) {
        printf("\n  [!] Item inválido!\n");
        return 0;
    }

    if (aux->tipo != POCAO) {
        printf("\n  [!] Só poções podem ser usadas em batalha.\n");
        return 0;
    }

    if (j->vida == j->vida_max) {
        printf("\n  [!] Sua vida já está cheia!\n");
        return 0;
    }

    // copia o nome antes de liberar o nó
    char nome[20];
    snprintf(nome, sizeof(nome), "%s", aux->nome);

    // guarda a vida de antes para mostrar o quanto foi curado
    int antes = j->vida;
    j->vida += aux->efeito;
    // limita a cura até a vida máxima
    if (j->vida > j->vida_max) {
        j->vida = j->vida_max;
    }

    printf("\n  [+] %s usou %s e recuperou %d de vida! (%d/%d)\n", j->nome, nome, j->vida - antes, j->vida, j->vida_max);

    // o item foi consumido, sai da lista
    remover_Item(j, nome);
    return 1;
}
