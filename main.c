#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

void limpar_Tela() {
}

// dado de 1 a 12
int rolar_D12() {
    return rand() % 12 + 1;
}

// define tipos de itens e numera a partir de 0
typedef enum {
    ARMA, ARMADURA, POCAO
} TipoItem;

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
} personagem;

typedef struct Monstro {
    char nome[60];
    int vida;
    int atq;
    int def;
    int xp_morte;
    int ouro_morte;
    int chefe;
} monstro;

const monstro listaMonstros[] = {
    { "Fábio, o Mestre do Sono Profundo", 16, 5, 1, 50, 15, 0 },
    { "Cris, a Rainha do Ar-Condicionado", 24, 7, 3, 50, 20, 0 },
    { "Robson, o Rei da Confusão Mental", 26, 12, 3, 50, 25, 0 },
    { "Serjão, o Dono dos Cabarés", 60, 15, 5, 50, 30, 0 },
    { "Dimas, o Ceifador de Médias", 80, 16, 6, 0, 0, 1 }
};

#define TOTAL_MONSTROS ((int)(sizeof(listaMonstros) / sizeof(listaMonstros[0])))

typedef struct NoMonstro {
    monstro dados;
    struct NoMonstro *prox;
} no_monstro;

typedef struct Fila {
    no_monstro *inicio;
    no_monstro *fim;
    int tamanho;
} fila;

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
}

int xp_Necessario(int nivel) {
    return 20 * nivel;
}

void mostrar_Personagem(personagem *j) {
    printf("\n+======== FICHA DO PERSONAGEM ========+\n");
    printf("  @  Nome   : %s\n", j->nome);
    printf("  +  Nível  : %d\n", j->nivel);
    printf("  <3 Vida   : %d/%d\n", j->vida, j->vida_max);
    printf("  /  Ataque : %d\n", j->atq);
    printf("  #  Defesa : %d\n", j->def);
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

    free(atual);
    return 1;
}

// imprime a lista de itens no inventário
void mostrar_Inventario(personagem *j) {
    const char *tipos[] = {"Arma", "Armadura", "Poção"};

    printf("\n+============ INVENTÁRIO ============+\n");

    // se a cabeça for NULL, o inventário está vazio
    if (j->inventario == NULL) {
        printf("\n  [!] Inventário vazio!\n");
        return;
    }

    int i = 1;

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
}

int calcular_Dano(int atq, int def) {
    int d12 = rolar_D12();

    if (d12 == 1) {
        printf("    [d12 = 1]  Errou o ataque! (miss)\n");
        return 0;
    }

    int dano = atq - def;
    if (dano < 1) {
        dano = 1;
    }

    if (d12 == 12) {
        printf("    [d12 = 12]  *** ACERTO CRÍTICO! ***\n");
        dano *= 2;
    }
    else {
        printf("    [d12 = %d]\n", d12);
    }

    return dano;
}

void subiu_Level(personagem *j, int xp) {
    if (xp <= 0) {
        return;
    }
    
    j->xp += xp;

    while (j->xp >= xp_Necessario(j->nivel)) {
        j->xp -= xp_Necessario(j->nivel);
        j->nivel++;
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

int usar_Item(personagem *j) {
    if (j->inventario == NULL) {
        printf("\n  [!] Inventário vazio!\n");
        return 0;
    }

    mostrar_Inventario(j);
    printf("  0. Voltar\n>> Escolha o item: ");
    int escolha = ler_Inteiro();

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

    int antes = j->vida;
    j->vida += aux->efeito;
    if (j->vida > j->vida_max) {
        j->vida = j->vida_max;
    }

    printf("\n  [+] %s usou %s e recuperou %d de vida! (%d/%d)\n",
           j->nome, nome, j->vida - antes, j->vida, j->vida_max);

    remover_Item(j, nome);
    return 1;
}

int batalha(personagem *j, monstro *m) {
    printf("\n<<<<<<<<<<<<<<< BATALHA >>>>>>>>>>>>>>>\n !!! %s apareceu !!!\n", m->nome);

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
                int dano = calcular_Dano(j->atq, m->def);
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

        if (m->vida == 0) {
            break;
        }

        printf("\n  <<< %s ataca!\n", m->nome);
        int dano = calcular_Dano(m->atq, j->def);

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
        if (j->vida > 0) {
            limpar_Tela();
        }
    }

    if (j->vida == 0) {
        printf("\n  x_x  %s foi derrotado...\n", j->nome);
        return 0;
    }

    printf("\n*** VITÓRIA! %s derrotou %s! ***\n", j->nome, m->nome);

    j->ouro += m->ouro_morte;
    printf("  [+] Você ganhou %d de XP e %d de ouro.\n", m->xp_morte, m->ouro_morte);
    subiu_Level(j, m->xp_morte);
    continuar();
    return 1;
}

void criar_Fila(fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

int fila_Vazia(fila *f) {
    return f->inicio == NULL;
}

int enfileirar_Monstro(fila *f, monstro m) {
    no_monstro *novo = malloc(sizeof(no_monstro));

    if (novo == NULL) {
        printf("[ERRO] Sem memória!\n");
        return 0;
    }

    novo->dados = m;
    novo->prox = NULL;

     if (f->fim == NULL) {
        f->inicio = novo;
    }
    else {
        f->fim->prox = novo;
    }

    f->fim = novo;
    f->tamanho++;

    return 1;
}

int desenfileirar_Monstro(fila *f) {
    if (f->inicio == NULL) {
        return 0;
    }

    no_monstro *removido = f->inicio;
    f->inicio = removido->prox;

    if (f->inicio == NULL) {
        f->fim = NULL;
    }

    free(removido);
    f->tamanho--;

    return 1;
}

monstro *ver_Primeiro(fila *f) {
    if (f->inicio == NULL) {
        return NULL;
    }

    return &f->inicio->dados;
}

void liberar_Fila(fila *f) {
    while (desenfileirar_Monstro(f)) {
    }
}

int explorar(personagem *j, fila *f) {
    int d12 = rolar_D12();
    printf("\n[EXPLORAR] Você explora a Fatec... (d12 = %d)\n", d12);

    if (d12 == 1) {
        // TODO: armadilha
    }
    else if (d12 == 2 || d12 == 3) {
        // TODO: local aleatório da Fatec
    }
    else if (d12 == 4 || d12 == 5) {
        // TODO: sala com baú
    }
    else {
        monstro *proximo = ver_Primeiro(f);

        if (proximo == NULL || proximo->chefe) {
            printf("\n  [!!!] Você sente uma presença poderosa... o Dimas te espera.\n");
            continuar();
        }
        else {
            monstro copia = *proximo;

            if (!batalha(j, &copia)) {
                return 0;
            }

            desenfileirar_Monstro(f);
        }
    }

    return 1;
}

void menu(personagem *j, fila *f) {
    int jogando = 1;

    while (jogando) {
        limpar_Tela();

        monstro *proximo = ver_Primeiro(f);
        int dimas_liberado = (proximo != NULL && proximo->chefe);

        printf("\n+============== MENU ==============+\n");
        printf("   [1] Explorar\n   [2] Ver Ficha\n   [3] Inventário\n   [4] Sair\n");

        if (dimas_liberado) {
            printf("   [5] Enfrentar o Dimas  <!>\n");
        }

        printf("+==================================+\n");
        printf(">> Opção: ");
        int opcao = ler_Inteiro();

        switch (opcao)
        {
        case 1:
            if (!explorar(j, f)) {
                printf("\n=============== FIM DE JOGO ===============\n");
                jogando = 0;
            }
            break;
        case 2:
            mostrar_Personagem(j);
            continuar();
            break;
        case 3:
            mostrar_Inventario(j);
            continuar();
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

    criar_Personagem(&jogador);
    adicionar_Item(&jogador, "Poção de Cura", POCAO, 30, 5);

    criar_Fila(&filaMonstros);
    for (int i = 0; i < TOTAL_MONSTROS; i++) {
        if (!enfileirar_Monstro(&filaMonstros, listaMonstros[i])) {
            liberar_Fila(&filaMonstros);
            liberar_Inventario(&jogador);
            return 1;
        }
    }

    menu(&jogador, &filaMonstros);

    liberar_Fila(&filaMonstros);
    liberar_Inventario(&jogador);
    return 0;
}