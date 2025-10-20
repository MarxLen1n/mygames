#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "io.h"

// Guarda la configuración original del terminal
static struct termios oldt;

void io_iniciar(void) {
    struct termios newt;

    // Obtener configuración actual
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Desactivar modo canónico y eco
    newt.c_lflag &= ~(ICANON | ECHO);

    // Aplicar inmediatamente
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void io_restaurar(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int getch(void) {
    char c = getchar();

    // Si es una secuencia de escape (teclas especiales)
    if (c == '\x1b') {
        char siguiente = getchar();
        if (siguiente == '[') {
            char final = getchar();
            switch (final) {
                case 'A': return 1000; // Flecha arriba
                case 'B': return 1001; // Flecha abajo
                case 'C': return 1002; // Flecha derecha
                case 'D': return 1003; // Flecha izquierda
                default:  return final; // Otra secuencia ANSI
            }
        }
    }

    return (int)c; // Tecla normal
}

void limpiar_pantalla(void) {
    printf("\033[2J\033[H"); // Secuencia ANSI: limpia pantalla y mueve cursor al inicio
    fflush(stdout);
}

char menu(char titulo[STR_LEN], char subtitulo[STR_LEN], char opciones[][STR_LEN], char len){
    char seleccion=0;
    int key=0;

    while(true)
    {
        limpiar_pantalla();
        printf("===%s===\n%s\n", titulo, subtitulo);
        for (char idx=0; idx<len; idx++){
            if (idx==seleccion) printf("\033[34m");
            printf("    %s\n\033[0m", opciones[idx]);
        }
        if (seleccion==len) printf("\033[34m");
        printf("    SALIR\033[0m\n");
        key=getch();
        switch(key)
        {
            case 1000:
                seleccion--;
                break;
            case 1001:
                seleccion++;
                break;
            case 'q':
            case 'Q':
                return len;
            case 10: 
                return seleccion;
        }
        if (seleccion<0) seleccion=len;
        else if (seleccion>len) seleccion=0;
    }
}

bool quitar_tildes(char *letra){
    if (letra==NULL) return false;

    if (strcmp(letra, "á")==0){ 
        strcpy(letra, "a"); 
        return true; 
    } else if (strcmp(letra, "é")==0){ 
        strcpy(letra, "é"); 
        return true; 
    } else if (strcmp(letra, "ì")==0){ 
        strcpy(letra, "ì"); 
        return true; 
    } else if (strcmp(letra, "ò")==0){ 
        strcpy(letra, "ò"); 
        return true; 
    } else if (strcmp(letra, "ù")==0){ 
        strcpy(letra, "ù"); 
        return true; 
    }
    
    return false;
}

bool convertir_color(char color[STR_LEN], char *store){
	if (strcmp(color, "azul_fg")==0) strcpy(store, "azul");
}
