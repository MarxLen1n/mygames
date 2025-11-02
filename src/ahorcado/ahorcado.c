#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "io.h"
#include "ahorcado.h"

static int inicializar_variables(void);
static int cargar_palabras(void);
static void mostrar(void);
static bool in_array(char elemento, char array[], size_t len);
static void liberar_palabras(void);

static char LETRAS[64], PALABRA[STR_LEN], PALABRA_LIMPIA[STR_LEN], INTENTADAS[64];
static char MAX_FALLOS, FALLOS;
static int N_INTENTADAS, N_PALABRAS;
static char **PALABRAS = NULL;

int jugar_ahorcado(void) {
    int res = inicializar_variables();
    if (res != 0) {
        fprintf(stderr, "ERROR: Sucedió un error al iniciar el ahorcado\n");
        liberar_palabras();
        return res;
    }

    bool ganar = false;
    while (FALLOS < MAX_FALLOS && !ganar) {
        mostrar();

        int k = getchar();
        while (getchar() != '\n'); // limpiar buffer

        k = tolower(k);

        if (k == 'q') {
            liberar_palabras();
            return 0;
        } else if (in_array(k, INTENTADAS, N_INTENTADAS)) {
            printf("Ya has intentado '%c'\n", k);
            sleep(1);
            continue;
        } else if (!in_array(k, LETRAS, strlen(LETRAS))) {
            printf("'%c' no es un carácter válido\n", k);
            sleep(1);
            continue;
        }

        INTENTADAS[N_INTENTADAS++] = k;

        if (!in_array(k, PALABRA_LIMPIA, strlen(PALABRA_LIMPIA))) {
            FALLOS++;
            printf("'%c' no está en la palabra!\n", k);
            sleep(1);
            continue;
        }

        printf("¡'%c' sí está en la palabra!\n", k);
        sleep(1);

        ganar = true;
        for (int idx = 0; idx < strlen(PALABRA_LIMPIA); idx++) {
            if (!in_array(PALABRA_LIMPIA[idx], INTENTADAS, N_INTENTADAS)) {
                ganar = false;
                break;
            }
        }

        if (ganar) {
            mostrar();
            printf("\n🎉 ¡Has ganado! La palabra era: %s\n", PALABRA);
            liberar_palabras();
            return 0;
        }
    }

    printf("\n💀 Has perdido. La palabra era: %s\n", PALABRA);
    liberar_palabras();
    return 0;
}

// -------------------
// INICIALIZACIÓN
// -------------------
static int inicializar_variables(void) {
    srand(time(NULL)); // Semilla aleatoria
    int res = cargar_palabras();
    if (res != 0) return res;

    FALLOS = 0;
    MAX_FALLOS = 6;

    strcpy(PALABRA, PALABRAS[rand() % N_PALABRAS]);
    strcpy(PALABRA_LIMPIA, PALABRA);

    // Normalizar palabra (eliminar tildes para comparar)
    for (size_t i = 0; i < strlen(PALABRA_LIMPIA); i++) {
        quitar_tildes(&PALABRA_LIMPIA[i]);
        PALABRA_LIMPIA[i] = tolower(PALABRA_LIMPIA[i]);
    }

    INTENTADAS[0] = '\0';
    N_INTENTADAS = 0;

    // Letras válidas (incluye tildes y ñ)
    strcpy(LETRAS, "abcdefghijklmnopqrstuvwxyzáéíóúñ");

    return 0;
}

// -------------------
// CARGAR PALABRAS
// -------------------
static int cargar_palabras(void) {
    FILE *archivo = fopen(RUTA_PALABRAS, "r");
    if (archivo == NULL) {
        fprintf(stderr, "ERROR: No se pudo abrir %s: ", RUTA_PALABRAS);
        perror("fopen");
        return 1;
    }

    char linea[STR_LEN];
    int n_lineas = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        linea[strcspn(linea, "\n")] = '\0';
        if (strlen(linea) == 0) continue; // saltar líneas vacías

        char **tmp = realloc(PALABRAS, (n_lineas + 1) * sizeof(char *));
        if (!tmp) {
            perror("realloc");
            liberar_palabras();
            fclose(archivo);
            return 2;
        }
        PALABRAS = tmp;

        PALABRAS[n_lineas] = malloc(strlen(linea) + 1);
        if (!PALABRAS[n_lineas]) {
            perror("malloc");
            liberar_palabras();
            fclose(archivo);
            return 2;
        }

        strcpy(PALABRAS[n_lineas], linea);
        n_lineas++;
    }

    fclose(archivo);
    N_PALABRAS = n_lineas;

    if (N_PALABRAS == 0) {
        fprintf(stderr, "ERROR: No se encontraron palabras en el archivo\n");
        return 3;
    }

    return 0;
}

// -------------------
// MOSTRAR TABLERO
// -------------------
static void mostrar(void) {
    limpiar_pantalla();

    printf("############\n#          #\n# AHORCADO #\n#          #\n############\n\n");
    printf("  __\n |  |\n |  ");
    if (FALLOS >= 1) printf("0");
    printf("\n | ");
    if (FALLOS >= 3) printf("/");
    else printf(" ");
    if (FALLOS >= 2) printf("|");
    if (FALLOS >= 4) printf("\\");
    printf("\n | ");
    if (FALLOS >= 5) printf("/");
    else printf(" ");
    if (FALLOS >= 6) printf(" \\");
    printf("\n |____\n");

    printf("\nPalabra: ");
    for (int i = 0; i < strlen(PALABRA); i++) {
        if (in_array(PALABRA_LIMPIA[i], INTENTADAS, N_INTENTADAS))
            printf(" %c ", PALABRA[i]);
        else
            printf(" _ ");
    }

    printf("\nFalladas: ");
    for (size_t idx = 0; idx < N_INTENTADAS; idx++) {
        if (!in_array(INTENTADAS[idx], PALABRA_LIMPIA, strlen(PALABRA_LIMPIA)))
            printf("%c ", INTENTADAS[idx]);
    }

    printf("\nFallos restantes: %d\n", MAX_FALLOS - FALLOS);
    printf("Introduce una letra (Q para salir): ");
}

// -------------------
// UTILIDADES
// -------------------
static bool in_array(char elemento, char array[], size_t len) {
    for (size_t idx = 0; idx < len; idx++) {
        if (elemento == array[idx]) return true;
    }
    return false;
}

static void liberar_palabras(void) {
    if (PALABRAS) {
        for (int i = 0; i < N_PALABRAS; i++)
            free(PALABRAS[i]);
        free(PALABRAS);
        PALABRAS = NULL;
    }
}
