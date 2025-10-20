#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "io.h"
#include "piedra_papel_tijera.h"

int jugar_piedra_papel_tijera(void);
static int comprobar_ganador(char *jug, int maq);

static char JUGADAS[4][STR_LEN]={
    "piedra", 
    "papel",  
    "tijera", 
    "salir"
};

int jugar_piedra_papel_tijera(void){
    while (true)
    {
        // RECOGER INPUT USUARIO
        char input[STR_LEN]="";
        printf("Tu jugada: (piedra, papel, tijera)\n");
        fgets(input, STR_LEN, stdin);
        input[strcspn(input, "\n")]='\0';
        if (strcmp(input, "q")==0) break;
        
        //GENERAR JUGADA MAQUINA
        int jugada_maquina = rand() % 3;

        printf("La jugada de la máquina ha sido %s\n", JUGADAS[jugada_maquina]);

        int resultado = comprobar_ganador(input, jugada_maquina);
        switch(resultado)
        {
            case 0:
                printf("¡Fue un empate!\n");
                break;
            case 1:
                printf("¡Ganaste! :)\n");
                break;
            case 2:
                printf("Perdiste... :(\n");
                break;
            case 3:
                printf("¡Adiós!\n");
                return 0;
            case 4:
                fprintf(stderr, "ERROR: probablemente una entrada no válida '%s'. Asegúrate de escribir 'piedra', 'papel' o 'tijera'.\n", input);
                break;
        }
    }

    return 0;
}

static int comprobar_ganador(char *jug, int maq){
    /*Returns:
    0 Si es empate
    1 Si gana el jugador
    2 Si gana la maquina
    3 Para salir
    4 Input erroneo */
    int jugada=4;
    for (int idx=0; idx<4; idx++)
    {
        if (strcmp(jug, JUGADAS[idx])==0) {
            jugada=idx;
            break;
        }
    }

    if (jugada==4){ // Jugada no válida
        return 4;
    } else if (jugada==3){ //Salir
        return 3;
    } else if (jugada==maq){ // Empate
        return 0;
    } else if (maq == (jugada + 1) % 3){ //Gana maquina
        return 2;
    }
    return 1;
}
