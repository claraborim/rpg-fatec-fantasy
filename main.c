#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

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
    char nome[20];
    int vida;
    int atq;
    int def;
    int xp_morte;
    int ouro_morte;
} monstro;

// pausa nos diálogos, turnos, etc
void continuar() {
    printf("\n[ENTER] para continuar...");
    while (getchar() != '\n');
}

// validação para caso o usuário insira um caractere
int ler_Inteiro() {
    int valor;

    if (scanf("%d", &valor) != 1) {
        valor = 0;
    }
    while (getchar() != '\n');

    return valor;
}

void criar_Personagem(personagem *j) {
    int opcao;

    do {
        printf("\nEscolha o nome do personagem:\n");
        printf("1 = Clara\n2 = Felipe\n3 = Miguel\n");
        printf("Opção: ");
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
            printf("\nOpção inválida! Escolha entre 1 a 3.\n");
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

monstro criar_Monstro(const char *nome, int vida, int atq, int def, int xp, int ouro) {
    monstro m;

    strncpy(m.nome, nome, sizeof(m.nome) - 1);
    m.nome[sizeof(m.nome) - 1] = '\0';
    m.vida = vida;
    m.atq = atq;
    m.def = def;
    m.xp_morte = xp;
    m.ouro_morte = ouro;

    return m;
}

int xp_Necessario(int nivel) {
    return 20 * nivel;
}

void mostrar_Personagem(personagem *j) {
    printf("\n-=-=- Ficha do Personagem -=-=-\n");
    printf("Nome:   %s\n", j->nome);
    printf("Nível:  %d\n", j->nivel);
    printf("Vida:   %d/%d\n", j->vida, j->vida_max);
    printf("Ataque: %d\n", j->atq);
    printf("Defesa: %d\n", j->def);
    printf("XP:     %d/%d\n", j->xp, xp_Necessario(j->nivel));
    printf("Ouro:   %d\n", j->ouro);
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
        printf("Erro: sem memória!\n");
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
        printf("Esse item não existe.\n");
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

    printf("\n-=-=- Inventário -=-=-\n");

    // se a cabeça for NULL, o inventário está vazio
    if (j->inventario == NULL) {
        printf("(vazio)\n");
        return;
    }

    int i = 1;

    for (item *aux = j->inventario; aux != NULL; aux = aux->prox) {
        printf("%d. [%s] %s | ", i++, tipos[aux->tipo], aux->nome);

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

        printf(" | Valor: %d ouro(s)\n", aux->valor);
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
        printf(" (d12 = 1) Errou o ataque!\n");
        return 0;
    }

    int dano = atq - def;
    if (dano < 1) {
        dano = 1;
    }

    if (d12 == 12) {
        printf(" (d12 = 12) Acerto crítico!\n");
        dano *= 2;
    }
    else {
        printf(" (d12 = %d)\n", d12);
    }

    return dano;
}

void subiu_Level(personagem *j, int xp) {
    if (j->nivel >= 10 || xp <= 0) {
        return;
    }
    
    j->xp += xp;

    while ((j->nivel < 10) && (j->xp >= xp_Necessario(j->nivel))) {
        j->xp -= xp_Necessario(j->nivel);
        j->nivel++;
        j->vida_max += 8;
        j->atq += 2;
        j->def += 1;
        j->vida = j->vida_max;

        printf("\n*** %s subiu para o nível %d! ***\n", j->nome, j->nivel);
        printf("Vida máx: %d | Ataque: %d | Defesa: %d (vida restaurada!)\n", j->vida_max, j->atq, j->def);
    }

    if (j->nivel >= 10) {
        j->xp = 0;
    }
}

int batalha(personagem *j, monstro *m) {
    printf("\n-=-=- %s apareceu! -=-=-\n", m->nome);

    while (j->vida > 0 && m->vida > 0) {
        printf("\n%s ataca:\n", j->nome);
        int dano = calcular_Dano(j->atq, m->def);
        m->vida -= dano;
        if (m->vida < 0) {
            m->vida = 0;
        }
        printf(" Dano causado: %d | %s: %d de vida\n", dano, m->nome, m->vida);

        if (m->vida == 0) {
            break;
        }

        continuar();

        printf("\n%s ataca:\n", m->nome);
        dano = calcular_Dano(m->atq, j->def);
        j->vida -= dano;
        if (j->vida < 0) {
            j->vida = 0;
        }
        printf(" Dano causado: %d | %s: %d/%d de vida\n", dano, j->nome, j->vida, j->vida_max);
        continuar();
    }

    if (j->vida == 0) {
        printf("\n%s foi derrotado...\n", j->nome);
        return 0;
    }

    printf("\n%s derrotou %s!\n", j->nome, m->nome);

    j->ouro += m->ouro_morte;
    printf("Você ganhou %d de XP e %d de ouro.\n", m->xp_morte, m->ouro_morte);
    subiu_Level(j, m->xp_morte);
    continuar();
    return 1;
}

int main() {
    // inclui caracteres/acentos ptbr
    setlocale(LC_ALL, "portuguese");

    // inicializa números aleatórios para o dado (d12)
    srand(time(NULL));

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("           RPG Fatec           \n");
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");

    personagem jogador;
    criar_Personagem(&jogador);
    mostrar_Personagem(&jogador);
    continuar();

    adicionar_Item(&jogador, "Poção de Cura", POCAO, 10, 5);
    mostrar_Inventario(&jogador);
    continuar();

    monstro goblin = criar_Monstro("Goblin", 15, 6, 1, 50, 8);

    if (batalha(&jogador, &goblin)) {
        mostrar_Personagem(&jogador);
    } else {
        printf("Fim de jogo.\n");
    }

    liberar_Inventario(&jogador);
    return 0;
}