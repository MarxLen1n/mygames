#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "io.h"
#include "tragaperras.h"

int jugar_tragaperras(void);
static int iniciar_variables(void);
static int rodar(void);
static void mostrar(int apuesta, bool apostar);
static void animar_rodillos(void);
static void esperar(int milisegundos);

#define JUGADAS_POSIBLES 16
#define TABLA_TAM 118  // suma total de PESOS

static int RODILLO_1, RODILLO_2, RODILLO_3, CARTERA;
static int PREMIOS[JUGADAS_POSIBLES+3] = {
    5, 6, 7, 8, 9, 10, 12, 14,
    17, 20, 25, 30, 35, 45, 55, 70,
    2, 3, 5
};

static int PESOS[JUGADAS_POSIBLES] = {
    16, 14, 13, 12, 11, 10, 9, 8,
    7, 6, 5, 4, 3, 3, 2, 2
};

static const char *SIMBOLOS[JUGADAS_POSIBLES] = {
    "🍒","🍋","🍊","🍉","🍇","🔔","⭐","🍀",
    "💎","🎲","🧿","🪙","💰","👑","🦄","7️⃣"
};

static int TABLA_PESOS[TABLA_TAM];
static char MSG[STR_LEN];

int jugar_tragaperras(void) {
    int res = iniciar_variables();
    if (res != 0) {
        fprintf(stderr, "ERROR: No se pudo iniciar la tragaperras\n");
        return res;
    }

    io_restaurar();

    int premio = 0;
    char entrada[STR_LEN];
    int apuesta = 0;

    while (true) {
        mostrar(apuesta, true);

        if (scanf("%s", entrada) != 1) {
            strcpy(MSG, "Entrada inválida\n");
            while (getchar() != '\n');
            continue;
        }

        if (strcmp(entrada, "q") == 0) {
            io_iniciar();
            return 0;
        }

        apuesta = atoi(entrada);
        if (apuesta <= 0) {
            strcpy(MSG, "La apuesta debe ser positiva.\n");
            continue;
        }

        if (apuesta > CARTERA) {
            snprintf(MSG, STR_LEN, "No puedes apostar más de lo que tienes (%d)\n", CARTERA);
            continue;
        }

        CARTERA -= apuesta;
        premio = rodar();
        CARTERA += premio * apuesta;

        if (premio == 0)
            snprintf(MSG, STR_LEN, "Has perdido %d créditos...\n", apuesta);
        else
            snprintf(MSG, STR_LEN, "Has ganado %d créditos!\n", premio * apuesta - apuesta);

        if (CARTERA <= 0) {
            strcpy(MSG, "Te has quedado sin dinero. Fin del juego.\n");
            mostrar(apuesta, false);
            break;
        }
    }
    printf("\n");
    io_iniciar();
    return 0;
}

static int iniciar_variables(void) {
    RODILLO_1 = 0;
    RODILLO_2 = 3;
    RODILLO_3 = 7;
    CARTERA = 300;
    strcpy(MSG, "\n");

    int t = 0;
    for (int idx = 0; idx < JUGADAS_POSIBLES; idx++) {
        for (int j = 0; j < PESOS[idx] && t < TABLA_TAM; j++) {
            TABLA_PESOS[t++] = idx;
        }
    }

    return 0;
}

static int rodar(void) {
    animar_rodillos();

    RODILLO_1 = TABLA_PESOS[rand() % TABLA_TAM];
    RODILLO_2 = TABLA_PESOS[rand() % TABLA_TAM];
    RODILLO_3 = TABLA_PESOS[rand() % TABLA_TAM];

    if (RODILLO_1 == RODILLO_2 && RODILLO_1 == RODILLO_3)
        return PREMIOS[RODILLO_1];

    if (RODILLO_1 == RODILLO_2 || RODILLO_1 == RODILLO_3 || RODILLO_2 == RODILLO_3) {
        if (RODILLO_1 < JUGADAS_POSIBLES/2) return PREMIOS[JUGADAS_POSIBLES];
        else if (RODILLO_1 < JUGADAS_POSIBLES-1) return PREMIOS[JUGADAS_POSIBLES+1];
        else return PREMIOS[JUGADAS_POSIBLES+2];
    }

    return 0;
}

static void mostrar(int apuesta, bool apostar) {
    limpiar_pantalla();

    printf(" ╔═══════════════════════════════════╗\n");
    printf(" ║         🎰  TRAGAPERRAS  🎰       ║\n");
    printf(" ╠═══════════════════════════════════╣\n");
    printf(" ║           ┌────┬────┬────┐        ║\n");
    printf(" ║           │ %s │ %s │ %s │        ║\n", SIMBOLOS[RODILLO_1], SIMBOLOS[RODILLO_2], SIMBOLOS[RODILLO_3]);
    printf(" ║           └────┴────┴────┘        ║\n");
    printf(" ╚═══════════════════════════════════╝\n");
    printf("  💰 Cartera: %dc     Apuesta: %dc\n", CARTERA, apuesta);
    printf(" %s", MSG);
    if (apostar) printf(" Apuesta: ");
}

static void animar_rodillos(void) {
    // 🎞️ Muestra símbolos aleatorios como si giraran
    for (int frame = 0; frame < 20; frame++) {
        RODILLO_1 = rand() % JUGADAS_POSIBLES;
        RODILLO_2 = rand() % JUGADAS_POSIBLES;
        RODILLO_3 = rand() % JUGADAS_POSIBLES;
        mostrar(0, false);
        esperar(50000 + frame * 1000); // ralentiza poco a poco
    }
}

static void esperar(int microsegundos) {
    struct timespec ts;
    ts.tv_sec = microsegundos / 1000000;
    ts.tv_nsec = (microsegundos % 1000000) * 1000;
    nanosleep(&ts, NULL);
}