#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "io.h"
#include "ahorcado.h"

static int inicializar_variables(void);
static int cargar_palabras(void);
static void mostrar(void);
static bool in_array(char elemento, char array[], size_t len);

static char LETRAS[27], PALABRA[STR_LEN], PALABRA_LIMPIA[STR_LEN], INTENTADAS[27], MAX_FALLOS, FALLOS;
static int N_INTENTADAS, N_PALABRAS;
static char **PALABRAS=NULL;

int jugar_ahorcado(void){
    int res=inicializar_variables();
    if (res!=0){
        fprintf(stderr, "ERROR: Sucedió un error al iniciar el ahorcado\n");
        return res;
    }
	
	bool ganar=false;
    while (FALLOS<MAX_FALLOS && !ganar){
        mostrar();

        int k=getchar();
        while (getchar()!='\n'); // limpiar buffer

        if (k=='Q'){
            return 0;
        } else if (in_array(k, INTENTADAS, N_INTENTADAS)) {
            printf("Ya has intentado %c\n", k);
            sleep(1);
            continue;
        } else if (!in_array(k, LETRAS, strlen(LETRAS))) {
            printf("%c no es un carácter válido\n", k);
            sleep(1);
            continue;
        }

        INTENTADAS[N_INTENTADAS++]=k;

        if (!in_array(k, PALABRA, strlen(PALABRA))) {
            FALLOS++;
            printf("%c no está en la palabra!\n", k);
            sleep(1);
            continue;
        }
        printf("%c sí está en la palabra!\n", k);
        sleep(1);

        ganar=true;
        for (int idx=0; idx<strlen(PALABRA); idx++) {
            if (!in_array(PALABRA[idx], INTENTADAS, N_INTENTADAS)) {
                ganar=false;
                break;
            }
        }

        if (ganar){
            mostrar();
            printf("\n¡Has ganado! La palabra era: %s\n", PALABRA);
            return 0;
        }
    }

    printf("\nHas perdido. La palabra era: %s\n", PALABRA);
    
    //Liberar memoria
    for (int i = 0; i < N_PALABRAS; i++)
		free(PALABRAS[i]);
	free(PALABRAS);
	PALABRAS = NULL;

    return 0;
}

static int inicializar_variables(void){
    int res=cargar_palabras();
	if (res!=0) return res;
	
	FALLOS=0;

    strcpy(PALABRA, PALABRAS[rand()%N_PALABRAS]);
    strcpy(PALABRA_LIMPIA, PALABRA);
    char idx=0;
    while (true){
        if (!quitar_tildes(&PALABRA_LIMPIA[idx])) break;
        idx++;
    }

    INTENTADAS[0]='\0';
    N_INTENTADAS=0;
    strcpy(LETRAS, "abcdefghijklmnopqrstuvwxyz");
    
    MAX_FALLOS=6;
    FALLOS=0;
    
    return 0;
}

static int cargar_palabras(void){
    FILE *archivo = fopen(RUTA_PALABRAS, "r");
    if (archivo==NULL){
        fprintf(stderr, "ERROR: ");
		perror("fopen");
		return 1;
	}
	
	char linea[STR_LEN];
	int n_lineas=0;
	while (fgets(linea, sizeof(linea), archivo)){
		linea[strcspn(linea, "\n")]='\0';
		
		char **tmp = realloc(PALABRAS, (n_lineas+1) * sizeof(char *));
        if (!tmp) {
            fprintf(stderr, "ERROR: ");
            perror("realloc");
            return 2;
        }
        PALABRAS=tmp;

        // Copiar la palabra en memoria nueva
        PALABRAS[n_lineas] = malloc(strlen(linea)+1);
        if (!PALABRAS[n_lineas]) {
            fprintf(stderr, "ERROR: ");
            perror("malloc");
            for (int idx=0; idx<n_lineas; idx++) free(PALABRAS[idx]);
            free(PALABRAS);
            fclose(archivo);
            return 2;
        }
        strcpy(PALABRAS[n_lineas], linea);
        n_lineas++;
	}	
	fclose(archivo);
    N_PALABRAS=n_lineas;
    return 0;
}

static void mostrar(void){
    limpiar_pantalla();

    printf("############\n#          #\n# AHORCADO #\n#          #\n############\n\n");
    printf("  __\n |  |\n |  ");
    if (FALLOS>=1) printf("0");
    printf("\n | ");
    if (FALLOS>=3) printf("/");
    else printf(" ");
    if (FALLOS>=2) printf("|");
    if (FALLOS>=4) printf("\\");
    printf("\n | ");
    if (FALLOS>=5) printf("/");
    else printf(" ");
    if (FALLOS>=6) printf(" \\");
    printf("\n |____\n");

    printf(  "Palabra: ");
    for (int i = 0; i < strlen(PALABRA); i++) {
        if (in_array(PALABRA[i], INTENTADAS, N_INTENTADAS))
            printf(" %c ", PALABRA_LIMPIA[i]);
        else
            printf(" _ ");
    }

    printf("\nFalladas: ");
    for (size_t idx=0; idx<strlen(INTENTADAS); idx++){
        if (!in_array(INTENTADAS[idx], PALABRA, strlen(PALABRA))) printf("%c", INTENTADAS[idx]);
    }
    printf("\nFallos restantes: %d\n", MAX_FALLOS - FALLOS);
    printf("Introduce una letra: ");
}

static bool in_array(char elemento, char array[], size_t len){
    for (size_t idx = 0; idx < len; idx++) {
        if (elemento == array[idx]) return true;
    }
    return false;
}
