#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include "io.h"
#include "robots.h"

static int fase(void);
static int inicializar_variables(void);
static void  mostrar(void);
static void teletransporte(void);
static void manejar_colisiones(void);
static int coordenada_aleatoria(int vieja_coordenada[2], int store[2]);
static bool in_array(int elemento[2], int array[][2], int len);

#define MAX_ALTO 1000
#define MAX_ANCHO 1000
#define MAX_ROBOTS 1600
#define MAX_BASURA 1600
#define INCREMENTO_PUNTOS 5
#define INCREMENTO_FASE 10
#define INCREMENTO_ROBOTS 16

static int POS_ACTUAL[2]={0, 0}, ROBOTS[MAX_ROBOTS][2], PUNTOS=0, N_ROBOTS=0, N_BASURA=0, ALTO=24, ANCHO=80;
static int BASURA[MAX_BASURA][2];
static char FASE_ACTUAL=0;
static bool GAME_OVER=false;

int jugar_robots(void){
    inicializar_variables();
	
	while (!GAME_OVER)
	{
		FASE_ACTUAL++;
		if (PUNTOS>0) PUNTOS+=rand()%(INCREMENTO_FASE*2)+INCREMENTO_FASE;
		fase();
	}
	mostrar();
	printf("Perdiste con %d puntos después de %d fases\n",PUNTOS, FASE_ACTUAL);


    return 0;
}

static int inicializar_variables(void){
    ALTO=24;
    ANCHO=80;

    N_ROBOTS=0;
    N_BASURA=0;
    PUNTOS=0;
    FASE_ACTUAL=0;
    GAME_OVER=false;

    POS_ACTUAL[0]=rand()%ALTO;
    POS_ACTUAL[1]=rand()%ANCHO;

    return 0;
}

static int fase(void){
    //Inicializar robots
    int cantidad_robots=FASE_ACTUAL*INCREMENTO_ROBOTS;

    while (cantidad_robots)
    {
        int coord[2]={rand()%ALTO, rand()%ANCHO};
        if (!in_array(coord, ROBOTS, N_ROBOTS))
        {
            ROBOTS[N_ROBOTS][0]=coord[0];
            ROBOTS[N_ROBOTS][1]=coord[1];
            N_ROBOTS++;
            cantidad_robots--;
        }
    }
    
    //Bucle principal
    while (true)
    {
        mostrar();
        int y=POS_ACTUAL[0], x=POS_ACTUAL[1];
        int k=getch();

        //Control de jugador
        switch (k)
        {
            case 'q':
                x--;
                y--;
                break;
            case 'e':
                x++;
                y--;
                break;
            case 'w':
                y--;
                break;

            case 'a':
                x--;
                break;
            case 'd':
                x++;
                break;

            case 'z':
                x--;
                y++;
                break;
            case 'x':
                y++;
                break;
            case 'c':
                y++;
                x++;
                break;

            case 't':
                teletransporte();
                y=-1;
                x=-1;
                break;
        }

        int n_pos[2]={y, x};
        if (0<=y && y<ALTO && 0<=x && x<ANCHO && !in_array(n_pos, BASURA, N_BASURA)) 
        {
            POS_ACTUAL[0]=y;
            POS_ACTUAL[1]=x;
        }

        //Movimiento robots
        for (int r=0; r<N_ROBOTS; r++)
        {
            if (ROBOTS[r][0]<POS_ACTUAL[0]){
                ROBOTS[r][0]++;
            } else if (ROBOTS[r][0]>POS_ACTUAL[0]){
                ROBOTS[r][0]--;
            }

            if (ROBOTS[r][1]<POS_ACTUAL[1]){
                ROBOTS[r][1]++;
            } else if (ROBOTS[r][1]>POS_ACTUAL[1]){
                ROBOTS[r][1]--;
            }
        }

        manejar_colisiones();
        
        if (in_array(POS_ACTUAL, ROBOTS, N_ROBOTS) || in_array(POS_ACTUAL, BASURA, N_BASURA)) {
			GAME_OVER=true; //perder
			return 0;
		} else if (N_ROBOTS==0) return 0;

    }
    return 1;
}

static void mostrar(void){
    limpiar_pantalla();

    printf("Puntos: %d\n", PUNTOS);
    for (int i=0; i<ANCHO+2; i++) printf("#");
    printf("\n");

    for (int y=0; y<ALTO; y++)
    {
        printf("#");
        for (int x=0; x<ANCHO; x++)
        {
            int coord[2]={y, x};
            if (POS_ACTUAL[0]==y && POS_ACTUAL[1]==x)
            {
                printf("@"); 
            } else if (in_array(coord, ROBOTS, N_ROBOTS))
            {
                printf("+");
            } else if (in_array(coord, BASURA, N_BASURA))
            {
                printf("*");
            } else
            {
                printf(" ");
            }
        }
        printf("#");
        printf("\n");
    }

    for (int i=0; i<ANCHO+2; i++) printf("#");
    printf("\n");
    printf("R: %d | B: %d \n", N_ROBOTS, N_BASURA);
}

static void teletransporte(void){
    int coord[2];
    int x=1;

    do
    {
        coord[0]=rand()%ALTO;
        coord[1]=rand()%ANCHO;
    } while (in_array(coord, ROBOTS, N_ROBOTS) || in_array(coord, BASURA, N_BASURA) || (coord[0]==POS_ACTUAL[0] && coord[1]==POS_ACTUAL[1]));
    
    while (POS_ACTUAL[0]!=coord[0] || POS_ACTUAL[1]!=coord[1])
    {
        if (POS_ACTUAL[0]<coord[0]){
            POS_ACTUAL[0]++;
        } else if (POS_ACTUAL[0]>coord[0]){
            POS_ACTUAL[0]--;
        }

        if (POS_ACTUAL[1]<coord[1]){
            POS_ACTUAL[1]++;
        } else if (POS_ACTUAL[1]>coord[1]){
            POS_ACTUAL[1]--;
        }

        mostrar();
        sleep(0.5);
    }
}

static void manejar_colisiones(void){
    int nuevos_robots[MAX_ROBOTS][2];
    int n_nuevos=0;

    for (int r=0; r<N_ROBOTS; r++) {
        int y=ROBOTS[r][0];
        int x=ROBOTS[r][1];

        // Colision con otro robot
        int colision=0;
        for (int j=r+1;j<N_ROBOTS; j++) {
            if (ROBOTS[j][0]==y && ROBOTS[j][1]==x) {
                colision = 1;
            }
        }

        if (colision){
            // Añadir a basura
            int coord[2]={y, x};
            if (! in_array(coord, BASURA, N_BASURA)) {
                BASURA[N_BASURA][0]=y;
                BASURA[N_BASURA][1]=x;
                N_BASURA++;
            }
            PUNTOS+=rand()%5+1;
           
        } else {
            // Colisiones con basura
            int sobre_basura=0;
            for (int b=0; b<N_BASURA; b++) {
                if (BASURA[b][0]==y && BASURA[b][1]==x) {
                    sobre_basura=1;
                    break;
                }
            }
            if (!sobre_basura) {
                // Sobrevive
                nuevos_robots[n_nuevos][0]=y;
                nuevos_robots[n_nuevos][1]=x;
                n_nuevos++;
            } else {
				PUNTOS+=rand()%INCREMENTO_PUNTOS+1;
			}
        }
    }

    for (int i=0; i<n_nuevos; i++){
        ROBOTS[i][0]=nuevos_robots[i][0];
        ROBOTS[i][1]=nuevos_robots[i][1];
   }
    N_ROBOTS=n_nuevos;
}

static bool in_array(int elemento[2], int array[][2], int len){
    for (int idx=0; idx<len; idx++)
    {
        if (elemento[0]==array[idx][0] && elemento[1]==array[idx][1]) return true;
    }
    return false;
}

