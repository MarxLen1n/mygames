#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdbool.h>

#define STR_LEN 512
// COLORES


// Inicializa la terminal en modo "raw" (sin esperar Enter, sin eco)
void io_iniciar(void);

// Restaura la configuración original del terminal
void io_restaurar(void);

// Lee una tecla (devuelve código ASCII o código extendido para flechas)
int getch(void);

// Limpia la pantalla
void limpiar_pantalla(void);

// Crea un menu
char menu(char titulo[STR_LEN], char subtitulo[STR_LEN], char opciones[][STR_LEN], char len);

bool quitar_tildes(char *letra);

#endif
