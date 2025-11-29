#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =======================================================
// DEFINIÇÕES E CONSTANTES
// =======================================================
#define TAMANHO_TABELA 10 // Tamanho da Tabela Hash (simplificado)
#define MIN_PISTAS 2      // Mínimo de pistas necessárias para acusação

// =======================================================
// ESTRUTURAS DE DADOS
// =======================================================

// 1. Estrutura para os cômodos (Mapa da Mansão - Árvore Binária)
struct Sala {
    char nome[40];
    char pista[80]; // Pista estática associada
    char suspeitoPista[40]; // Suspeito associado a esta pista (para a Hash)
    struct Sala *esquerda;
    struct Sala *direita;
};

// 2. Estrutura para as Pistas Coletadas (Árvore Binária de Busca - BST)
struct PistaNode {
    char conteudo[80];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
};

// 3. Estrutura para a Tabela Hash (Lista Encadeada para colisões)
struct HashItem {
    char pista[80];    // Chave: Conteúdo da Pista
    char suspeito[40]; // Valor: Nome do Suspeito
    struct HashItem *proximo;
};

// Tabela Hash global (Array de ponteiros para HashItem)
struct HashItem *tabelaHash[TAMANHO_TABELA];


// =======================================================
// FUNÇÕES DA TABELA HASH
// =======================================================

/**
 * @brief Função de hash simples (soma dos valores ASCII e módulo).
 *
 * @param chave A string da pista.
 * @return O índice na tabela hash.
 */
int funcaoHash(const char *chave) {
    int valor = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        valor += chave[i];
    }
    return valor % TAMANHO_TABELA;
}

/**
 * @brief Insere a associação Pista -> Suspeito na Tabela Hash.
 *
 * @param pista Conteúdo da pista (Chave).
 * @param suspeito Nome do suspeito (Valor).
 */
void inserirNaHash(const char *pista, const char *suspeito) {
    int indice = funcaoHash(pista);

    // Cria um novo item na Hash
    struct HashItem *novoItem = (struct HashItem *)malloc(sizeof(struct HashItem));
    if (novoItem == NULL) {
        printf("ERRO: Falha na alocacao de memoria para a Hash.\n");
        exit(1);
    }
    strncpy(novoItem->pista, pista, sizeof(novoItem->pista) - 1);
    novoItem->pista[sizeof(novoItem->pista) - 1] = '\0';
    strncpy(novoItem->suspeito, suspeito, sizeof(novoItem->suspeito) - 1);
    novoItem->suspeito[sizeof(novoItem->suspeito) - 1] = '\0';
    novoItem->proximo = NULL;

    // Trata colisão (inserção no início da lista encadeada)
    if (tabelaHash[indice] == NULL) {
        tabelaHash[indice] = novoItem;
    } else {
        novoItem->proximo = tabelaHash[indice];
        tabelaHash[indice] = novoItem;
    }
}

/**
 * @brief Consulta o suspeito associado a uma pista na Tabela Hash.
 *
 * @param pista Conteúdo da pista (Chave).
 * @param buffer Suspeito (Valor) retornado.
 * @return 1 se encontrado, 0 caso contrário.
 */
int encontrarSuspeito(const char *pista, char *buffer) {
    int indice = funcaoHash(pista);
    struct HashItem *atual = tabelaHash[indice];

    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            strncpy(buffer, atual->suspeito, 40);
            return 1; // Suspeito encontrado
        }
        atual = atual->proximo;
    }

    return 0; // Pista não encontrada na Hash
}

/**
 * @brief Libera a memória alocada para a Tabela Hash.
 */
void liberarHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        struct HashItem *atual = tabelaHash[i];
        while (atual != NULL) {
            struct HashItem *proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
        tabelaHash[i] = NULL;
    }
}


// =======================================================
// FUNÇÕES DA ÁRVORE DE PISTAS (BST)
// =======================================================

/**
 * @brief Cria dinamicamente um novo nó para a Árvore de Pistas.
 */
struct PistaNode* criarPistaNode(const char *conteudoPista) {
    struct PistaNode *novoNode = (struct PistaNode *)malloc(sizeof(struct PistaNode));
    if (novoNode == NULL) {
        printf("ERRO: Falha na alocacao de memoria.\n");
        exit(1);
    }
    strncpy(novoNode->conteudo, conteudoPista, sizeof(novoNode->conteudo) - 1);
    novoNode->conteudo[sizeof(novoNode->conteudo) - 1] = '\0';
    novoNode->esquerda = NULL;
    novoNode->direita = NULL;
    return novoNode;
}

/**
 * @brief Insere recursivamente uma nova pista na BST.
 *
 * @param raiz A raiz da subárvore atual.
 * @param conteudoPista O texto da pista a ser inserida.
 * @return O ponteiro para a raiz atualizada da subárvore.
 */
struct PistaNode* inserirPista(struct PistaNode *raiz, const char *conteudoPista) {
    if (raiz == NULL) {
        return criarPistaNode(conteudoPista);
    }
    int comparacao = strcmp(conteudoPista, raiz->conteudo);

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, conteudoPista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, conteudoPista);
    }
    // Não insere duplicatas (comparacao == 0)
    return raiz;
}

/**
 * @brief Exibe todas as pistas coletadas em ordem alfabética (percurso In-Order).
 *
 * @param raiz A raiz da Árvore de Pistas (BST).
 */
void exibirPistas(struct PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("    -> \"%s\"\n", raiz->conteudo);
        exibirPistas(raiz->direita);
    }
}

/**
 * @brief Libera a memória alocada para a Árvore de Pistas.
 */
void liberarPistas(struct PistaNode *raiz) {
    if (raiz == NULL) return;

    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

/**
 * @brief Percorre a BST (In-Order) e conta quantas pistas apontam para o suspeito acusado.
 * Esta função é crucial para o julgamento final.
 *
 * @param raiz A raiz da BST de pistas coletadas.
 * @param suspeitoAcusado O nome do suspeito.
 * @return A contagem de pistas que o incriminam.
 */
int contarPistasPorSuspeito(struct PistaNode *raiz, const char *suspeitoAcusado) {
    if (raiz == NULL) {
        return 0;
    }
    
    int contagem = 0;
    char suspeitoPista[40] = "";

    // 1. Percorre a esquerda
    contagem += contarPistasPorSuspeito(raiz->esquerda, suspeitoAcusado);

    // 2. Verifica a Raiz
    // Usa a Hash para relacionar a pista coletada ao suspeito
    if (encontrarSuspeito(raiz->conteudo, suspeitoPista)) {
        if (strcmp(suspeitoPista, suspeitoAcusado) == 0) {
            contagem++;
            printf("      [+] Pista: \"%s\" aponta para **%s**.\n", raiz->conteudo, suspeitoAcusado);
        }
    }

    // 3. Percorre a direita
    contagem += contarPistasPorSuspeito(raiz->direita, suspeitoAcusado);

    return contagem;
}


// =======================================================
// FUNÇÕES DO MAPA DA MANSÃO (ÁRVORE BINÁRIA)
// =======================================================

/**
 * @brief Cria dinamicamente um novo cômodo (Sala).
 *
 * @param nomeSala Nome do cômodo.
 * @param pistaConteudo Conteúdo da pista.
 * @param suspeito Suspeito incriminado pela pista.
 * @return Um ponteiro para a nova Sala alocada.
 */
struct Sala* criarSala(const char *nomeSala, const char *pistaConteudo, const char *suspeito) {
    struct Sala *novaSala = (struct Sala *)malloc(sizeof(struct Sala));
    if (novaSala == NULL) {
        printf("ERRO: Falha na alocacao de memoria.\n");
        exit(1);
    }

    strncpy(novaSala->nome, nomeSala, sizeof(novaSala->nome) - 1);
    novaSala->nome[sizeof(novaSala->nome) - 1] = '\0';
    
    strncpy(novaSala->pista, pistaConteudo, sizeof(novaSala->pista) - 1);
    novaSala->pista[sizeof(novaSala->pista) - 1] = '\0';

    strncpy(novaSala->suspeitoPista, suspeito, sizeof(novaSala->suspeitoPista) - 1);
    novaSala->suspeitoPista[sizeof(novaSala->suspeitoPista) - 1] = '\0';
    
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;

    return novaSala;
}

/**
 * @brief Monta o mapa inicial da mansão e a Tabela Hash de associações.
 *
 * @param pistasRaiz Ponteiro para a raiz da BST de pistas coletadas.
 * @return Um ponteiro para o nó raiz (Hall de Entrada).
 */
struct Sala* montarMapaEHash() {
    // Inicialização da Tabela Hash
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabelaHash[i] = NULL;
    }

    // Criação dos Cômodos com Pistas e Suspeitos (Mapa Fixo)
    
    // Nível 0 (Raiz)
    struct Sala *hall = criarSala("Hall de Entrada", "Encontrado um ticket de onibus na lareira.", "Empregado");
    inserirNaHash(hall->pista, hall->suspeitoPista);
    
    // Nível 1
    struct Sala *salaEstar = criarSala("Sala de Estar", "A maça mordida tinha batom vermelho.", "Senhora");
    inserirNaHash(salaEstar->pista, salaEstar->suspeitoPista);

    struct Sala *cozinha = criarSala("Cozinha", "A faca sumiu, mas o chef nao se lembra.", "Chef");
    inserirNaHash(cozinha->pista, cozinha->suspeitoPista);

    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    // Nível 2
    struct Sala *biblioteca = criarSala("Biblioteca", "Uma pagina de diario com data rasgada.", "Senhora");
    inserirNaHash(biblioteca->pista, biblioteca->suspeitoPista); // Segunda pista para a Senhora

    struct Sala *jardim = criarSala("Jardim", ""); // Sem Pista
    
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    struct Sala *despensa = criarSala("Despensa", "O cofre estava aberto e vazio.", "Empregado");
    inserirNaHash(despensa->pista, despensa->suspeitoPista); // Segunda pista para o Empregado

    struct Sala *salaJantar = criarSala("Sala de Jantar", "Havia um forte cheiro de cigarro barato.", "Mordomo");
    inserirNaHash(salaJantar->pista, salaJantar->suspeitoPista);

    cozinha->esquerda = despensa;
    cozinha->direita = salaJantar;
    
    // Nível 3 (Folhas)
    struct Sala *escritorio = criarSala("Escritorio (Folha)", "Uma luva de seda preta no chao.", "Mordomo");
    inserirNaHash(escritorio->pista, escritorio->suspeitoPista); // Segunda pista para o Mordomo

    biblioteca->esquerda = escritorio;

    return hall;
}

/**
 * @brief Permite a navegação do jogador, coleta de pistas e inserção na BST.
 *
 * @param mapaRaiz A raiz do mapa.
 * @param pistasRaiz Ponteiro para a raiz da BST de pistas.
 */
void explorarSalas(struct Sala *mapaRaiz, struct PistaNode **pistasRaiz) {
    struct Sala *salaAtual = mapaRaiz;
    char escolha[5];

    printf("=======================================================\n");
    printf("         EXPLORANDO A MANSAO DETECTIVE QUEST           \n");
    printf("=======================================================\n\n");
    printf("Instrucoes: 'e'=Esquerda, 'd'=Direita, 's'=Sair e Acusar.\n\n");

    while (salaAtual != NULL) {
        printf("-------------------------------------------------------\n");
        printf("VOCE ESTA EM: **%s**\n", salaAtual->nome);

        // Verifica e coleta a pista
        if (strlen(salaAtual->pista) > 0) {
            printf("\n🚨 PISTA ENCONTRADA! Coletando...\n");
            printf("   > Conteudo: \"%s\"\n", salaAtual->pista);
            
            // Insere a pista na BST (ela gerencia duplicação)
            *pistasRaiz = inserirPista(*pistasRaiz, salaAtual->pista);
            
            // Exibe o suspeito relacionado (uso imediato da Hash)
            char suspeitoRelacionado[40] = "";
            if (encontrarSuspeito(salaAtual->pista, suspeitoRelacionado)) {
                 printf("   > Esta pista aponta para: **%s**.\n", suspeitoRelacionado);
            } else {
                 printf("   > Erro: Suspeito nao encontrado para esta pista na Hash.\n");
            }
            
            // Limpa a pista da sala para evitar coleta duplicada
            salaAtual->pista[0] = '\0';
        } else {
            printf("\nNao ha novas pistas aqui.\n");
        }

        // Exibe opções de navegação
        printf("\nPara onde deseja ir?\n");
        if (salaAtual->esquerda != NULL) {
             printf(" [e] Esquerda (para %s)\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
             printf(" [d] Direita (para %s)\n", salaAtual->direita->nome);
        }
        printf(" [s] Sair do Jogo e Iniciar o Julgamento\n");

        printf("\nDIGITE SUA ESCOLHA (e/d/s): ");
        if (fgets(escolha, sizeof(escolha), stdin) == NULL) break;
        escolha[strcspn(escolha, "\n")] = '\0';

        // Controle da Decisão
        if (strcmp(escolha, "s") == 0 || strcmp(escolha, "S") == 0) {
            printf("\nENCERRANDO EXPLORACAO.\n");
            break;
        } else if (strcmp(escolha, "e") == 0 || strcmp(escolha, "E") == 0) {
            salaAtual = salaAtual->esquerda;
            if (salaAtual == NULL) printf("\nCaminho esquerdo finalizado. Voltando ao menu.\n");
        } else if (strcmp(escolha, "d") == 0 || strcmp(escolha, "D") == 0) {
            salaAtual = salaAtual->direita;
            if (salaAtual == NULL) printf("\nCaminho direito finalizado. Voltando ao menu.\n");
        } else {
            printf("\nOpcao invalida. Tente novamente.\n");
        }
        printf("\n");
    }
}

/**
 * @brief Conduz à fase de julgamento final, solicitando a acusação do jogador.
 *
 * @param pistasRaiz A raiz da BST de pistas coletadas.
 */
void verificarSuspeitoFinal(struct PistaNode *pistasRaiz) {
    char suspeitoAcusado[40];
    int pistasIncriminadoras;

    printf("=======================================================\n");
    printf("           FASE DE JULGAMENTO: QUEM E O CULPADO?       \n");
    printf("=======================================================\n\n");
    
    printf("Suspeitos possiveis (verifique suas pistas):\n");
    printf("   - Senhora\n   - Empregado\n   - Chef\n   - Mordomo\n\n");

    printf("DIGITE O NOME DO SUSPEITO ACUSADO: ");
    if (fgets(suspeitoAcusado, sizeof(suspeitoAcusado), stdin) == NULL) {
        printf("Erro de leitura.\n");
        return;
    }
    suspeitoAcusado[strcspn(suspeitoAcusado, "\n")] = '\0';

    printf("\n--- VERIFICANDO EVIDENCIAS CONTRA %s ---\n", suspeitoAcusado);
    
    // Usa a recursão (In-Order) e a Hash para contar as pistas
    pistasIncriminadoras = contarPistasPorSuspeito(pistasRaiz, suspeitoAcusado);

    printf("\n-------------------------------------------------------\n");
    printf("RESULTADO DO JULGAMENTO:\n");
    printf("   Pistas Encontradas para %s: **%d**\n", suspeitoAcusado, pistasIncriminadoras);
    printf("   Minimo Requerido: %d pistas.\n\n", MIN_PISTAS);

    if (pistasIncriminadoras >= MIN_PISTAS) {
        printf("🎉 SUCESSO! EVIDENCIAS SUFICIENTES! 🎉\n");
        printf("   **%s** e o culpado! Seu dossie e impecavel.\n", suspeitoAcusado);
    } else {
        printf("❌ FRACASSO! EVIDENCIAS INSUFICIENTES! ❌\n");
        printf("   Voce precisa de, no minimo, %d pistas para acusar %s. Volte e procure mais!\n", MIN_PISTAS, suspeitoAcusado);
    }
}


// =======================================================
// FUNÇÃO PRINCIPAL
// =======================================================

int main() {
    struct Sala *mapaRaiz = NULL;
    struct PistaNode *pistasRaiz = NULL;

    printf("=======================================================\n");
    printf("        BEM VINDO AO DETECTIVE QUEST NIVEL MESTRE!     \n");
    printf("=======================================================\n\n");

    // Montagem do mapa da mansão e inicialização da Hash
    mapaRaiz = montarMapaEHash();

    // Início da exploração interativa
    if (mapaRaiz != NULL) {
        explorarSalas(mapaRaiz, &pistasRaiz);
    } else {
        printf("ERRO FATAL: Nao foi possivel montar o mapa da mansao.\n");
        return 1;
    }

    // --- Exibição das Pistas Coletadas ---
    printf("\n\n*******************************************************\n");
    printf("     DOSSIE ATUAL: PISTAS COLETADAS (Ordem Alf.)       \n");
    printf("*******************************************************\n");
    if (pistasRaiz != NULL) {
        exibirPistas(pistasRaiz);
    } else {
        printf("    Nenhuma pista foi coletada.\n");
    }
    printf("*******************************************************\n\n");

    // --- Fase de Julgamento ---
    if (pistasRaiz != NULL) {
        verificarSuspeitoFinal(pistasRaiz);
    } else {
        printf("Nao e possivel fazer uma acusacao sem nenhuma pista coletada!\n");
    }

    // --- Limpeza de Memória ---
    liberarMapa(mapaRaiz);
    liberarPistas(pistasRaiz);
    liberarHash();

    return 0;
}