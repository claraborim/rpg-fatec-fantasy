#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// dado de 1 a 12
int rolar_D12() {
    return rand() % 12 + 1;
}

// define tipos de itens e numera a partir de 0
typedef enum {
    ARMA, ARMADURA, POCAO
} TipoItem;

// nó da lista encadeada do inventário
typedef struct Item {
    char nome[20];
    TipoItem tipo;
    int efeito;
    int valor;
    struct Item *prox;
} item;

typedef struct Personagem {
    char nome[20];
    int vida;
    int vida_max;
    int nivel;
    int atq;
    int def;
    int xp;
    int ouro;
    item *inventario;
    item *arma_eq;
    item *armadura_eq;
} personagem;

typedef struct Monstro {
    char nome[60];
    int vida;
    int atq;
    int def;
    int xp_morte;
    int ouro_morte;
    // 1 = chefe final, 0 = monstro comum
    int chefe;
} monstro;

// catálogo dos monstros
const monstro listaMonstros[] = {
    { "Fábio, o Mestre do Sono Profundo", 16, 5, 1, 50, 15, 0 },
    { "Cris, a Rainha do Ar-Condicionado", 24, 7, 3, 50, 20, 0 },
    { "Robson, o Rei da Confusão Mental", 26, 12, 3, 50, 25, 0 },
    { "Serjão, o Dono dos Cabarés", 60, 15, 5, 50, 30, 0 },
    { "Dimas, o Ceifador de Médias", 110, 18, 6, 0, 0, 1 }
};

// calcula quantos monstros tem na lista
#define TOTAL_MONSTROS ((int)(sizeof(listaMonstros) / sizeof(listaMonstros[0])))

// nó da fila encadeada de monstros
typedef struct NoMonstro {
    monstro dados;
    struct NoMonstro *prox;
} no_monstro;

typedef struct Fila {
    no_monstro *inicio;
    no_monstro *fim;
} fila;

// item à venda na loja
typedef struct ItemLoja {
    char nome[20];
    TipoItem tipo;
    int efeito;
    int preco;
} item_loja;

// catálogo da loja: o último da lista fica no topo da pilha
const item_loja catalogoLoja[] = {
    { "Poção de Cura", POCAO, 30, 10 },
    { "Poção de Cura", POCAO, 30, 10 },
    { "Poção de Cura", POCAO, 30, 10 },
    { "Jaqueta da Fatec", ARMADURA, 3, 45 },
    { "Faca de Plástico", ARMA, 4, 40 }
};

// calcula quantos itens tem no catálogo
#define TOTAL_CATALOGO ((int)(sizeof(catalogoLoja) / sizeof(catalogoLoja[0])))

// nó da pilha encadeada da loja
typedef struct NoLoja {
    item_loja dados;
    struct NoLoja *prox;
} no_loja;

typedef struct Pilha {
    no_loja *topo;
} pilha;

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

// deixa a fila vazia
void criar_Fila(fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
}

// insere o monstro no fim da fila
int enfileirar_Monstro(fila *f, monstro m) {
    no_monstro *novo = malloc(sizeof(no_monstro));

    if (novo == NULL) {
        printf("[ERRO] Sem memória!\n");
        return 0;
    }

    // o nó guarda uma cópia dos dados do monstro
    novo->dados = m;
    novo->prox = NULL;

    // fila vazia: novo nó é o início
    if (f->fim == NULL) {
        f->inicio = novo;
    }
    // antigo último nó aponta para o novo
    else {
        f->fim->prox = novo;
    }

    f->fim = novo;

    return 1;
}

// remove o monstro do início da fila
int desenfileirar_Monstro(fila *f) {
    if (f->inicio == NULL) {
        return 0;
    }

    no_monstro *removido = f->inicio;
    // o segundo passa a ser o primeiro
    f->inicio = removido->prox;

    // se a fila esvaziou, fim vira NULL
    if (f->inicio == NULL) {
        f->fim = NULL;
    }

    free(removido);

    return 1;
}

// consulta o primeiro da fila sem remover
monstro *ver_Primeiro(fila *f) {
    if (f->inicio == NULL) {
        return NULL;
    }

    return &f->inicio->dados;
}

// libera todos os nós (desenfileira)
void liberar_Fila(fila *f) {
    while (desenfileirar_Monstro(f)) {
    }
}

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

// cura a vida sem passar do máximo e devolve quanto foi curado
int curar_Personagem(personagem *j, int quantidade) {
    int antes = j->vida;

    j->vida += quantidade;
    if (j->vida > j->vida_max) {
        j->vida = j->vida_max;
    }

    return j->vida - antes;
}

// armadilhas que dão dano, mas nunca matam o jogador
void evento_Armadilha(personagem *j) {
    // dano entre 3 e 6
    int dano = rand() % 4 + 3;
    // sorteio entre 4 armadilhas
    int tipo = rand() % 4;
    // situação: texto da armadilha, sorte: se vai ter dano ou não
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
        adicionar_Item(j, "Poção de Cura", POCAO, 30, 5);
        printf("  [+] Dentro tinha uma Poção de Cura!\n");
    }
    else if (sorte <= 10) {
        adicionar_Item(j, "Teclado Mecânico", ARMA, 3, 20);
        printf("  [+] Dentro tinha um Teclado Mecânico! (Ataque +3)\n");
    }
    else {
        adicionar_Item(j, "Jaleco Reforçado", ARMADURA, 2, 20);
        printf("  [+] Dentro tinha um Jaleco Reforçado! (Defesa +2)\n");
    }

    continuar();
}

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

int main() {
    setlocale(LC_ALL, "portuguese");

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
    adicionar_Item(&jogador, "Poção de Cura", POCAO, 30, 5);

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