#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ini.h"
#include "io.h"
#include "buscaminas/buscaminas.h"
#include "robots/robots.h"
#include "ahorcado/ahorcado.h"
#include "juego_delaVida/juego_delaVida.h"
#include "piedra_papel_tijera/piedra_papel_tijera.h"
#include "tres_enRaya/tres_enRaya.h"
#include "tragaperras/tragaperras.h"

#define LINEAS_LICENCIA 500
#define STR_LEN 128

char LICENCIA_RESUMIDA[LINEAS_LICENCIA][STR_LEN];
int cargar_licencia(void);
void mostrar_licencia(void);

int main(int argc, char *argv[]) {
    srand(time(NULL));
    io_iniciar();
    
    //Opciones
    int r = 0;
    if (argc > 1) {
        if (strcmp(argv[1], "--buscaminas") == 0) r = jugar_buscaminas();
        else if (strcmp(argv[1], "--robots") == 0) r = jugar_robots();
        else if (strcmp(argv[1], "--ahorcado") == 0) r = jugar_ahorcado();
        else if (strcmp(argv[1], "--vida") == 0) r = jugar_juego_delaVida();
        else if (strcmp(argv[1], "--ppt") == 0) r = jugar_piedra_papel_tijera();
        else if (strcmp(argv[1], "--tres") == 0) r = jugar_tres_enRaya();
        else if (strcmp(argv[1], "--tragaperras") == 0) r = jugar_tragaperras();
        else if (strcmp(argv[1], "--license") == 0) {
            mostrar_licencia();
            r = 0;
        } else if (strcmp(argv[1], "--help") == 0) {
            printf("Uso: mygames [opción]\n");
            printf("Opciones disponibles:\n");
            printf("  --menu            Abre el menú interactivo (por defecto)\n");
            printf("  --buscaminas      Inicia el Buscaminas\n");
            printf("  --ahorcado        Inicia el Ahorcado\n");
            printf("  --robots          Inicia Robots\n");
            printf("  --tres_eRaya      Inicia Tres en Raya\n");
            printf("  --tragaperras     Inicia la tragaperras\n");
            printf("  --license         Muestra la licencia del proyecto\n");
            printf("  --help            Muestra esta ayuda\n");
            r = 0;
        } else {
            fprintf(stderr, "ERROR: Opción inválida: %s\nUsa --help para ver opciones.\n", argv[1]);
            r = 1;
        }
        io_restaurar();
        getchar();
        return r;
    }

    // Menu interactivo
    char titulo[STR_LEN] = "MYGAMES", subtitulo[STR_LEN] = "ELIGE UN JUEGO PARA JUGAR";
    char opciones[][STR_LEN] = {"BUSCAMINAS", "ROBOTS", "AHORCADO", "JUEGO DE LA VIDA", "PIEDRA PAPEL O TIJERA", "TRES EN RAYA"};
    char len = 6;
    bool on = true;
    while (on) {
        char eleccion = menu(titulo, subtitulo, opciones, len);
        switch (eleccion) {
            case 0: jugar_buscaminas(); break;
            case 1: jugar_robots(); break;
            case 2: io_restaurar(); jugar_ahorcado(); io_iniciar(); break;
            case 3: jugar_juego_delaVida(); break;
            case 4: io_restaurar(); jugar_piedra_papel_tijera(); io_iniciar(); break;
            case 5: io_restaurar(); jugar_tres_enRaya(); io_iniciar(); break;
            default: on = false; break;
        }    char titulo[STR_LEN] = "MYGAMES", subtitulo[STR_LEN] = "ELIGE UN JUEGO PARA JUGAR";

        if (eleccion != len) {
            printf("\n\nPulsa cualquier tecla para continuar\n");
            getchar();
        }
    }
    io_restaurar();
    return 0;
}

int cargar_licencia(void) {
    FILE *archivo = fopen("LICENCIA_RESUMIDA", "r");
    if (!archivo) {
        fprintf(stderr, "ERROR: ");
        perror("No se pudo abrir el archivo de licencia");
        return 1;
    }

    int idx = 0;
    while (idx < LINEAS_LICENCIA && fgets(LICENCIA_RESUMIDA[idx], STR_LEN, archivo)) {
        LICENCIA_RESUMIDA[idx][strcspn(LICENCIA_RESUMIDA[idx], "\n")] = '\0';
        idx++;
    }

    fclose(archivo);
    return idx; // Devuelve cantidad de líneas leídas
}

void mostrar_licencia(void) {
    int lineas = cargar_licencia();
    if (lineas <= 0) return;

    io_restaurar(); // Para que no haya problemas con el modo raw
    printf("\n=== LICENCIA DEL PROYECTO ===\n\n");

    for (int i = 0; i < lineas; i++) {
        printf("%s\n", LICENCIA_RESUMIDA[i]);
    }
    printf("\n");

    io_iniciar();
}
