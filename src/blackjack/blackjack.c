#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "io.h"
#include "blackjack.h"

#define MAX_LEN_BARAJA 52
#define LEN_PALO 13

struct Carta {
    int num;         
    char palo[STR_LEN];
};

int jugar_blackjack(void);
static int jugar_mano(void);
static int iniciar_variables(void);
static void mostrar(void);
static bool repartir(int cantidad, bool destino);static int calcular_puntos(struct Carta mano[MAX_LEN_BARAJA], int len);
static bool in_array(struct Carta carta, struct Carta array[], int len);

static struct Carta BARAJA[MAX_LEN_BARAJA];
static struct Carta MANO[MAX_LEN_BARAJA];
static struct Carta DEALER[MAX_LEN_BARAJA];
static int LEN_BARAJA, LEN_MANO, LEN_DEALER;
static char PALOS[4][STR_LEN];

int jugar_blackjack(void) {
    io_restaurar();
    if (iniciar_variables() != 0) {
        fprintf(stderr, "ERROR: No se pudo iniciar el blackjack\n");
        return 1;
    }

    
    while (true) {
        repartir(2, true);
        repartir(2, false);
        
        jugar_mano();
        
        break;
    }

    io_iniciar();
    return 0;
}

static int jugar_mano(void){
    char accion[STR_LEN];
    int puntos_jugador=0;
    int puntos_dealer=0;

    while (true)
    {
        mostrar();

        fgets(accion, STR_LEN, stdin);
        accion[strcspn(accion, "\n")] = 0;
        if (strcmp(accion, "s")==0 || strcmp(accion, "si")==0 || strcmp(accion, "sí")==0){
            break;
        } else if (strcmp(accion, "q")==0){
            return -1;
        } else {
            repartir(1, true);
        }
    }
    
    // Ia dealer
    while (calcular_puntos(DEALER, LEN_DEALER)<17){
        repartir(1, false);
        mostrar();
        getchar();
    }
    
    puntos_jugador=calcular_puntos(MANO, LEN_MANO);
    puntos_dealer=calcular_puntos(DEALER, LEN_DEALER);

    printf("Dealer: %d\nJugador: %d\n", puntos_dealer, puntos_jugador);

    return 0;
}

static int iniciar_variables(void) {
    strcpy(PALOS[0], "picas");
    strcpy(PALOS[1], "corazones");
    strcpy(PALOS[2], "tréboles");
    strcpy(PALOS[3], "diamantes");

    LEN_BARAJA = LEN_MANO = LEN_DEALER = 0;

    struct Carta baraja_temporal[MAX_LEN_BARAJA];
    int idx = 0;

    for (int palo = 0; palo < 4; palo++) {
        for (int num = 1; num <= LEN_PALO; num++) {
            baraja_temporal[idx].num = num;
            strcpy(baraja_temporal[idx].palo, PALOS[palo]);
            idx++;
        }
    }

    while (LEN_BARAJA < MAX_LEN_BARAJA) {
        int carta = rand() % MAX_LEN_BARAJA;
        if (in_array(baraja_temporal[carta], BARAJA, LEN_BARAJA)) continue;
        BARAJA[LEN_BARAJA++] = baraja_temporal[carta];
    }

    return 0;
}

static bool repartir(int cantidad, bool destino) {
    for (int c = 0; c < cantidad; c++) {
        if (LEN_BARAJA == 0) return false;
        if (destino) {
            MANO[LEN_MANO++] = BARAJA[--LEN_BARAJA];
        } else {
            DEALER[LEN_DEALER++] = BARAJA[--LEN_BARAJA];
        }
    }
    return true;
}

static void mostrar(void) {
    limpiar_pantalla();

    printf("Dealer: ");
    for (int i = 0; i < LEN_DEALER; i++) {
        printf("%d de %s  ", DEALER[i].num, DEALER[i].palo);
    }
    printf("\nJugador: ");
    for (int i = 0; i < LEN_MANO; i++) {
        printf("%d de %s  ", MANO[i].num, MANO[i].palo);
    }
    printf("\n");

    printf("Te plantas? ");
}

static int calcular_puntos(struct Carta mano[MAX_LEN_BARAJA], int len){
    int total = 0, ases = 0;

    for (int i = 0; i < len; i++) {
        int n = mano[i].num;
        if (n == 1) ases++;
        else if (n > 10) total += 10;
        else total += n;
    }

    total += ases;

    while (ases > 0 && total + 10 <= 21) {
        total += 10;
        ases--;
    }

    return total;
}

static bool in_array(struct Carta carta, struct Carta array[], int len) {
    for (int i = 0; i < len; i++) {
        if (array[i].num == carta.num &&
            strcmp(array[i].palo, carta.palo) == 0)
            return true;
    }
    return false;
}
