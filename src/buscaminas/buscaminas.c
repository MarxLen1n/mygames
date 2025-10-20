#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <termios.h>
#include <unistd.h>
#include "io.h"
#include "buscaminas.h"

static int iniciar_variables(void);
static void iniciar_tablero(void);
static void mostrar(void);
static bool cavar(int y, int x);
static void liberar_tablero(void);
static void liberar_parcial(int filas);

#define MAX_ALTO 1000
#define MAX_ANCHO 1000

static char **TABLERO, **MAP_TABLERO;
static char DICC_GRAFICO[9];
static int ALTO, ANCHO, MINAS;
static int COLOR_FONDO, COLOR_TEXTO, COLOR_MINA, COLOR_BANDERA, COLOR_CURSOR;
static int ARRIBA, ABAJO, IZQUIERDA, DERECHA, CAVAR, MARCAR, SALIR, POS[2];
static bool GANAR=false, PERDER=false;

int jugar_buscaminas(void){
	// Iniciar juego
	int inicio=iniciar_variables();
	if (inicio!=0){
		fprintf(stderr, "ERROR: Sucedió un error al iniciar el buscaminas\n");
		return inicio;
	}
	iniciar_tablero();
	
	while (!PERDER && !GANAR)
    {
		mostrar();
		int y=POS[0], x=POS[1];
		
		int k=getch();
		if (k==ARRIBA)    	   y--;
		else if (k==ABAJO)     y++;
		else if (k==IZQUIERDA) x--;
		else if (k==DERECHA)   x++;
		else if (k==CAVAR)cavar(POS[0], POS[1]);
		else if (k==MARCAR && MAP_TABLERO[y][x]==0) MAP_TABLERO[y][x]=2;
		else if (k==MARCAR && MAP_TABLERO[y][x]==2) MAP_TABLERO[y][x]=0;
		else if (k==SALIR) return 0;
		
		if (0<=y && y<ALTO && 0<=x && x<ANCHO){
			POS[0]=y;
			POS[1]=x;
		}
	}
    mostrar();

	if (PERDER) printf("Has perdido!\n");
	else if (GANAR) printf("Has ganado!\n");

	liberar_tablero();

    return 0;
}

static int iniciar_variables(void){
	//Menu
    char titulo[STR_LEN]="Elige una dificultad", subtitulo[STR_LEN]="";
    char opciones[][STR_LEN]={"FACIL", "MEDIO", "DIFICIL"};
    char len=3;
    int alto=16, ancho=16, minas=40;

    char eleccion=menu(titulo, subtitulo, opciones, len);
    switch(eleccion)
    {
        case 0: //Facil
            alto=8;
            ancho=8;
            minas=10;
            break;
        case 1: //Medio
            break; //(por defecto)
        case 2: //Dificil
            alto=16;
            ancho=30;
            minas=99;
            break;
        case 3: //Salir
			return 0;
    }

	//Errores
	size_t total_cells = (size_t) alto * (size_t) ancho;
	if (alto<=0 || ancho<=0){
		fprintf(stderr, "ERROR: Alto y ancho deben ser mayores a 0.\nAlto: %d | Ancho: %d\n", alto, ancho);
		return 1;
	} else if (alto>MAX_ALTO || ancho>MAX_ANCHO){
		fprintf(stderr, "ERROR: Alto no debe ser mayor a %d y ancho no debe ser mayor a %d\nAlto: %d | Ancho: %d\n", MAX_ALTO, MAX_ANCHO, alto, ancho);
		return 1;
	} else if (total_cells == 0 || total_cells > (size_t)INT_MAX){
		fprintf(stderr, "ERROR: Tamaño del tablero inválido o demasiado grande\n");
        return 1;
	} else if (minas<0 || minas>alto*ancho){
		fprintf(stderr, "ERROR: Minas debe ser mayor a 0 y menor a la cantidad de casillas.\nMinas: %d | Total casillas: %d\n", minas, alto*ancho);
		return 1;
	}
		
	ALTO=alto;
	ANCHO=ancho;
	MINAS=minas;
	
	//Controles
	ARRIBA=1000;
	ABAJO=1001;
	DERECHA=1002;
	IZQUIERDA=1003;
	
	CAVAR='z';
	MARCAR='x';
	SALIR='q';
	
	// Iniciar tablero
	TABLERO = malloc((size_t)ALTO * sizeof(char *));
    MAP_TABLERO = malloc((size_t)ALTO * sizeof(char *));
    if (!TABLERO || !MAP_TABLERO) {
        liberar_parcial(0);
		fprintf(stderr, "ERROR: ");
        perror("malloc");
        return 1;
    }
	
	int filas=0;
    for (int y=0; y<ALTO; ++y){
        TABLERO[y]=calloc((size_t)ANCHO, sizeof(char));
        if (!TABLERO[y]){ 
			fprintf(stderr, "ERROR: ");
        	perror("calloc");
			liberar_parcial(filas); 
			return 1; 
		}
        MAP_TABLERO[y]=calloc((size_t)ANCHO, sizeof(char));
        if (!MAP_TABLERO[y]){ 
			fprintf(stderr, "ERROR: ");
        	perror("calloc");
			liberar_parcial(filas); 
			return 1; 
		}
        filas++;
    }
	
	//Iniciar diccioinario grafico
    //0-8
	DICC_GRAFICO[0]=90; //Gris
	DICC_GRAFICO[1]=34; //Azul oscuro
	DICC_GRAFICO[2]=32; //Verde
	DICC_GRAFICO[3]=31; //Rojo
	DICC_GRAFICO[4]=35; //Magenta
	DICC_GRAFICO[5]=33; //Marron chungo
	DICC_GRAFICO[6]=36; //Cian
	DICC_GRAFICO[7]=37; //Blanco
	DICC_GRAFICO[8]=34; //azul
	
	//Interfaz
	COLOR_FONDO=40; 	//Negro
	COLOR_TEXTO=93; 	//Amarillo
	COLOR_MINA=91; 		//Rojo
	COLOR_BANDERA=92; 	//verde
	COLOR_CURSOR=46;	//cian
	
	//Variabes de juego
	POS[0]=0;
	POS[1]=0;
	PERDER=false;
	GANAR=false;
		
	return 0;
}

static void iniciar_tablero(void){
	int colocadas=0;
	int y=0, x=0;
	while (colocadas<MINAS)
	{
		y=rand()%ALTO;
		x=rand()%ANCHO;
		if (TABLERO[y][x]==0){
			TABLERO[y][x]=-1;
			colocadas++;
		}
	}
	
	for (y=0; y<ALTO; y++){
		for (x=0; x<ANCHO; x++){
			if (TABLERO[y][x]==-1) continue;
			int m=0;
			for (int dy=-1; dy<2; dy++){
				for (int dx=-1; dx<2; dx++){
					int ny=y+dy, nx=x+dx;
					if (0<=ny && ny<ALTO && 0<=nx && nx<ANCHO && TABLERO[ny][nx]==-1) m++;
				}
			}
			TABLERO[y][x]=m;
		}
	}
}

static void mostrar(void){
	limpiar_pantalla();

    int marcadas=0;
    for (int y=0; y<ALTO; y++){
        for (int x=0; x<ANCHO; x++){
            if (MAP_TABLERO[y][x]==2) marcadas++;
        }
    }

    printf("\033[%d;%dmMinas restantes: %d\n", COLOR_TEXTO, COLOR_FONDO, MINAS-marcadas);
    for (int idx=0; idx<ANCHO+2; idx++) printf("#");
    printf("\n");
    for (int y=0; y<ALTO; y++){
        printf("\033[%d;%dm#", COLOR_TEXTO, COLOR_FONDO);
        for (int x=0; x<ANCHO; x++){
            int bg=COLOR_FONDO;
            int fg=COLOR_TEXTO;
            char c=' ';
            switch(MAP_TABLERO[y][x])
            {
				case 0: 
					break;
				case 1:
					if (TABLERO[y][x]==-1){
						c='X';
						fg=COLOR_MINA;
					} else {
						c=TABLERO[y][x]+'0';
						fg=DICC_GRAFICO[TABLERO[y][x]];
					}
					break;
				case 2:
					c='P';
					fg=COLOR_BANDERA;
					break;
			}
			if (POS[0]==y && POS[1]==x) bg=COLOR_CURSOR;
			printf("\033[%d;%dm%c\033[0m", fg, bg, c);
		}
        printf("\033[%d;%dm#\n", COLOR_TEXTO, COLOR_FONDO);
    }
    for (int i=0; i<ANCHO+2; i++) printf("\033[%d;%dm#", COLOR_TEXTO, COLOR_FONDO);
    printf("\033[0m\n");
}

static bool cavar(int y, int x){
	if (y<0 || y>=ALTO || x<0 || x>=ANCHO) return true;
	if (MAP_TABLERO[y][x]!=0) return true;

	MAP_TABLERO[y][x]=1;

	if (TABLERO[y][x]==-1){
		mostrar();
		PERDER=true;
		return false;
	}
	else if (TABLERO[y][x]==0){
		for (int dy=-1; dy<=1; dy++){
			for (int dx=-1; dx<=1; dx++){
				int ny=y+dy, nx=x+dx;
				if (ny<0 || ny>=ALTO || nx<0 || nx>=ANCHO) continue;
				if (MAP_TABLERO[ny][nx]!=0) continue;
				cavar(ny, nx);
			}
		}
	}
	
	GANAR=true;
	for (int y=0; y<ALTO; y++){
        for (int x=0; x<ANCHO; x++){
            if (MAP_TABLERO[y][x]==0){
				GANAR=false;
				break;
			}
        }
    }

	return true;
}

static void liberar_tablero(void){
	if (TABLERO && MAP_TABLERO){
		for (int i=0; i<ALTO; i++){
			free(TABLERO[i]);
			free(MAP_TABLERO[i]);
		}
		free(TABLERO);
		free(MAP_TABLERO);
		TABLERO=NULL;
		MAP_TABLERO=NULL;
	}
}

static void liberar_parcial(int filas){
	int idx=0;
    if (TABLERO){
        for (idx=0; idx<filas; idx++) free(TABLERO[idx]);
        free(TABLERO);
        TABLERO=NULL;
    }
    
    if (MAP_TABLERO){
        for (idx=0; idx<filas; idx++) free(MAP_TABLERO[idx]);
        free(MAP_TABLERO);
        MAP_TABLERO=NULL;
    }
}
