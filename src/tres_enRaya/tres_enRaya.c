#include <stdio.h>
#include "io.h"
#include "tres_enRaya.h"

int jugar_tres_enRaya(void);
static int iniciar_variables(void);
static void mostrar(void);
static char comprobar_ganador(void);
static char get_char(char num);

#define LADO 3

static char TABLERO[LADO][LADO], TURNO, GANADOR;
static int C_FONDO, C_X, C_O, C_NUMEROS, C_BORDES;
// TURNO = 'X', 'O'
// GANADOR = 'X', 'O', 'E': empate, ' ': ninguno

int jugar_tres_enRaya(void){  
    int res=iniciar_variables();
    if (res!=0){
        fprintf(stderr, "ERROR: No se pudo iniciar el tres en raya");
        return res;
    }

    while (GANADOR==' ')
    {
        mostrar();

        char entrada[8];
        printf("\nIntroduce tu jugada (1–9 o q para salir): ");
        scanf("%7s", entrada);
        while (getchar() != '\n'); // limpia el buffer de entrada

        if (entrada[0] == 'q' || entrada[0] == 'Q') return 0;
        int jugada = entrada[0] - '0';

        if (jugada<1 || jugada>9){
            printf("Jugada inválida. Introdice un número del 1 al 9.\n");
            continue;
        }

        int y = (jugada - 1) / 3;
        int x = (jugada - 1) % 3;

        if (TABLERO[y][x]!=' ') {
            printf("La casilla número %d ya está ocupada\n", jugada);
            continue;
        }

        TABLERO[y][x] = TURNO;

        if (TURNO=='O') TURNO='X';
        else TURNO='O';

        GANADOR=comprobar_ganador();
    }
    mostrar();
    
    return 0;
}

static int iniciar_variables(void){
    for (int y=0; y<LADO; y++){
        for (int x=0; x<LADO; x++) TABLERO[y][x]=' ';
    }
    TURNO='X';
    GANADOR=' ';

    C_FONDO   = 0;   // sin fondos
    C_X       = 31;  // rojo
    C_O       = 34;  // azul
    C_NUMEROS = 90;  // gris claro
    C_BORDES  = 37;  // blanco
    return 0;
}

static void color(int fg, int bg) {
    printf("\033[38;5;%d;48;5;%dm", fg, bg);
}

static void reset_color(void) {
    printf("\033[0m");
}

static void mostrar(void) {
    limpiar_pantalla();

    // Cabecera
    color(C_BORDES, C_FONDO);
    printf("╔══════════════════════════╗\n");
    printf("║      ");
    color(226, C_FONDO); // Amarillo brillante
    printf("TRES EN RAYA");
    color(C_BORDES, C_FONDO);
    printf("        ║\n");
    printf("╚══════════════════════════╝\n\n");
    reset_color();

    // Marco superior
    color(C_BORDES, C_FONDO);
    printf("     ╔═══╦═══╦═══╗\n");
    reset_color();

    for (int y = 0; y < 3; y++) {
        color(C_BORDES, C_FONDO);
        printf("     ║");
        reset_color();

        for (int x = 0; x < 3; x++) {
            char c = TABLERO[y][x];
            if (c == ' ') {
                int pos = y * 3 + x + 1;
                color(C_NUMEROS, C_FONDO);
                printf(" %d ", pos);
            } else if (c == 'X') {
                color(C_X, C_FONDO);
                printf(" %c ", c);
            } else {
                color(C_O, C_FONDO);
                printf(" %c ", c);
            }
            reset_color();
            color(C_BORDES, C_FONDO);
            printf("║");
            reset_color();
        }

        printf("\n");
        if (y < 2) {
            color(C_BORDES, C_FONDO);
            printf("     ╠═══╬═══╬═══╣\n");
            reset_color();
        }
    }

    color(C_BORDES, C_FONDO);
    printf("     ╚═══╩═══╩═══╝\n\n");
    reset_color();

    if (GANADOR == ' ') {
        printf("Turno de ");
        if (TURNO == 'X') color(C_X, C_FONDO);
        else color(C_O, C_FONDO);
        printf("%c", TURNO);
        reset_color();
        printf("\n\n");
    } else if (GANADOR == 'E') {
        color(220, C_FONDO);
        printf("¡Empate!");
        reset_color();
        printf("\n\n");
    } else {
        printf("¡Ganó ");
        if (GANADOR == 'X') color(C_X, C_FONDO);
        else color(C_O, C_FONDO);
        printf("%c", GANADOR);
        reset_color();
        printf("!\n\n");
    }
}


static char comprobar_ganador(void){
    // ' ': no hay GANADOR, 'O' o 'X': GANADOR, 'E': empate
    for (int y=0; y<LADO; y++){
        if (TABLERO[y][0] != ' ' &&
            TABLERO[y][0] == TABLERO[y][1] &&
            TABLERO[y][1] == TABLERO[y][2])
            return TABLERO[y][0];
    }
    for (int x=0; x<LADO; x++){
        if (TABLERO[0][x] != ' ' &&
            TABLERO[0][x] == TABLERO[1][x] &&
            TABLERO[1][x] == TABLERO[2][x])
            return TABLERO[0][x];
    }

    int idx=0;
    char diagonal_1[3];
    char diagonal_2[3];

    for (char d=0; d<LADO; d++){
        diagonal_1[idx]=TABLERO[d][d];
        diagonal_2[idx] = TABLERO[d][LADO - 1 - d];
        idx++;
    }

    if (diagonal_1[0]==diagonal_1[1] && diagonal_1[0]==diagonal_1[2]) return diagonal_1[0];
    if (diagonal_2[0]==diagonal_2[1] && diagonal_2[0]==diagonal_2[2]) return diagonal_2[0];

    for (int y=0; y<LADO; y++) {
        for (int x=0; x<LADO; x++){
            if (TABLERO[y][x]==' ') return ' ';
        }
    }

    return 'E';
}
