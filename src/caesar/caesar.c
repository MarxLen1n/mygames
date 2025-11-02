#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "io.h"
#include "caesar.h"

int exec_caesar(void);
static int codificar_caesar(int rotacion, char mensaje[STR_LEN]);
static int index_char(char elemento, char array[]);

static wchar_t ALFABETO[] = 
L"ABCDEFGHIJKLMNÑOPQRSTUVWXYZÁÉÍÓÚÜ"
L"abcdefghijklmnñopqrstuvwxyzáéíóúü"
L"0123456789 .:,;¡!¿?'\\-_()[]/\\<>{}\"";

#define LEN_ALFABETO (sizeof(ALFABETO)/sizeof(ALFABETO[0]) - 1)

int exec_caesar(void) {
    io_restaurar();

    char input[STR_LEN];
    char comando = 'c';
    char mensaje[STR_LEN] = "";
    int rotacion = 0;

    printf("Introduce un comando: [comando] [mensaje] -[rotación]\n");
    printf("    comandos: 'c' para codificar, 'd' para decodificar.\n");
    printf("    mensaje: el mensaje a codificar o decodificar.\n");
    printf("    rotación: cuánto se ruedan las letras.\n");
    printf(": ");

    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "ERROR: no se pudo leer la entrada.\n");
        return -1;
    }

    comando = input[0];

    int idx = 2, msg_i = 0;
    while (input[idx] != '-' && input[idx] != '\0' && input[idx] != '\n') {
        mensaje[msg_i++] = input[idx];
        idx++;
    }
    mensaje[msg_i] = '\0';

    if (input[idx] == '-') {
        idx++;
        rotacion = atoi(&input[idx]);
    }

    io_iniciar();
    
    if (comando == 'c')
        return codificar_caesar(rotacion, mensaje);
    else if (comando == 'd')
        return codificar_caesar(-rotacion, mensaje);
    else {
        fprintf(stderr, "ERROR: Comando '%c' desconocido\n", comando);
        return -1;
    }
}

static int index_char(wchar_t elemento, wchar_t array[]) {
    for (int i = 0; i < LEN_ALFABETO; i++) {
        if (array[i] == elemento)
            return i;
    }
    return -1;
}

static int codificar_caesar(int rotacion, const char *mensaje_utf8) {
    setlocale(LC_ALL, "");

    wchar_t mensaje[STR_LEN];
    mbstowcs(mensaje, mensaje_utf8, STR_LEN);

    while (rotacion < 0)
        rotacion += LEN_ALFABETO;
    rotacion %= LEN_ALFABETO;

    for (int i = 0; mensaje[i] != L'\0'; i++) {
        int pos = index_char(mensaje[i], ALFABETO);
        if (pos == -1) wprintf(L"?");
        else wprintf(L"%lc", ALFABETO[(pos + rotacion) % LEN_ALFABETO]);
    }
    wprintf(L"\n");
    return 0;
}

