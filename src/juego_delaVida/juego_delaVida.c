#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include "io.h"
#include "juego_delaVida.h"

int jugar_juego_delaVida(void);
static int iniciar_variables(void);
static void logica(void);
static void aleatorio(int cantidad);
static void mostrar(void);
static void liberar_parcial(int filas);

char POS[2], CUENTA=0;
int ALTO, ANCHO;
bool **TABLERO;

int jugar_juego_delaVida(void){
	int res=iniciar_variables();
	if (res!=0){
		fprintf(stderr, "ERROR: Hubo un error al iniciar el juego de la vida");
		return 1;
	}
			
	//Lógica
	char c;
	mostrar();
	while (true)		
	{
		c=getch();
		if (c=='q') break;
		logica();
		CUENTA++;
		mostrar();
	}
	
	return 0;
}

static int iniciar_variables(void){
	io_restaurar();
	// Menu
	printf("Alto: ");
	scanf("%d", &ALTO);
	printf("Ancho: ");
	scanf("%d", &ANCHO);

	POS[0]=ALTO/2;
	POS[1]=ANCHO/2;
	io_iniciar();

	// Iniciar el tablero
	TABLERO = malloc((size_t)ALTO * sizeof(bool *));
    if (!TABLERO) {
        liberar_parcial(0);
		fprintf(stderr, "ERROR: ");
        perror("malloc");
        return 1;
    }

	int filas=0;
    for (int y=0; y<ALTO; ++y){
        TABLERO[y]=calloc((size_t)ANCHO, sizeof(bool));
        if (!TABLERO[y]){ 
			fprintf(stderr, "ERROR: ");
        	perror("calloc");
			liberar_parcial(filas); 
			return 1; 
		}
		filas++;

		for (int x=0; x<ANCHO; x++){
			TABLERO[y][x]=false;
		}
	}

	//colocar celulas
	bool salir=false;
	while (!salir)
	{
		mostrar();

		int y=POS[0], x=POS[1];
		int k = getch();

		switch (k)
		{
			case 1000:
			case 'w':
				y--;
				break;
			case 1001:
			case 's':
				y++;
				break;
			case 1002:
			case 'd':
				x++;
				break;
			case 1003:
			case 'a':
				x--;
				break;

			case 10:
				if (TABLERO[y][x]==false) TABLERO[y][x]=true;
				else if (TABLERO[y][x]==false) TABLERO[y][x]=true;
				break;
			case 'r':				
				int c=0;
				printf("Cantidad de células: ");
				scanf("%d", &c);
				aleatorio(c);
				break;
			case 32:
				salir=true;
				break;
		}
		if (0<=y && y<ALTO && 0<=x && x<ANCHO){
			POS[0]=y;
			POS[1]=x;
		}
	}
	POS[0]=-1;
	POS[1]=-1;

	return 0;
}

static void mostrar(void){
	limpiar_pantalla();

	for (int idx=0; idx<ANCHO+2; idx++) printf("#");
	printf("\n");

	for (int y=0; y<ALTO; y++){
		printf("#");
		for (int x=0; x<ANCHO; x++){
			if (y==POS[0] && x==POS[1]) printf("\x1b[46m");
			if (TABLERO[y][x]) printf("O");
			else printf(" ");
			printf("\x1b[0m");
		}
		printf("#\n");
	}
	for (int idx=0; idx<ANCHO+2; idx++) printf("#");
	printf("\n");
	printf("%d\n", CUENTA);
}

static void logica(){
	char nuevo_tablero[ALTO][ANCHO];
	for (int y=0; y<ALTO; y++){
		for (int x=0; x<ANCHO; x++){
			int derredor=0;
			for (int dy=-1; dy<2; dy++){
				for (int dx=-1; dx<2; dx++){
					if (dy==0 && dx==0) continue;
					int ny=y+dy, nx=x+dx;
					if (0<=ny && ny<ALTO && 0<=nx && nx<ANCHO && TABLERO[ny][nx]) derredor++;
				}
			}
			//Nacimiento
			if (!TABLERO[y][x] && derredor==3){
				nuevo_tablero[y][x]=1;
			} else if (!TABLERO[y][x]){
				nuevo_tablero[y][x]=0;
				continue;
			//Muertes
			} else if (derredor==2 || derredor==3) nuevo_tablero[y][x]=1;
			else nuevo_tablero[y][x]=0;
		}
	}
			
	for (int y=0; y<ALTO; y++){
		for (int x=0; x<ANCHO; x++) TABLERO[y][x]=nuevo_tablero[y][x];
	}
}

static void aleatorio(int cantidad){
	if (ALTO*ANCHO<cantidad){
		printf("%d es un número demasiado grande, %d es el límite.\nPulsa enter para continuar.\n", cantidad, ALTO*ANCHO);
		while (getchar()=='\n');
		getchar();
		return;
	}
	
	while (cantidad){
		int y=rand()%ALTO;
		int x=rand()%ANCHO;
		if (!TABLERO[y][x]){
			TABLERO[y][x]=true;
			cantidad--;
		}
	}
}

static void liberar_parcial(int filas){
	int idx=0;
    if (TABLERO){
        for (idx=0; idx<filas; idx++) free(TABLERO[idx]);
        free(TABLERO);
        TABLERO=NULL;
    }
}