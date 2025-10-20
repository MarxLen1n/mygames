# mygames

**mygames** es una colección de mini-juegos clásicos desarrollados en C, ejecutables directamente desde la terminal.  
Incluye títulos como **Buscaminas**, **Robots**, **Ahorcado**, **Juego de la Vida**, **Piedra, Papel o Tijera** y **Tres en Raya**.

---

## DISCLAIMER

Este proyecto se ha desarrollado **con fines educativos**.  
No está pensado para distribución comercial ni incluye ninguna funcionalidad en red o recopilación de datos.  
El objetivo principal del paquete es el **aprendizaje** del creador.

---

## Características

- 🎯 Múltiples juegos en un único binario (`mygames`)
- 🧭 Menú interactivo para seleccionar juegos
- 🧰 Código modular: cada juego se compila de forma independiente
- 🖥️ Compatible con Linux / BSD / macOS (terminal ANSI)

---

## Estructura del proyecto

```yaml
mygames/
├── bin/ # Binarios compilados
│ └── mygames
├── etc/ # Configuración y recursos
│ └── palabras.txt
├── include/ # Cabeceras comunes (.h)
│ └── io.h
├── src/ # Código fuente principal
│ ├── ahorcado/
│ │ ├── ahorcado.c
│ │ └── ahorcado.h
│ ├── buscaminas/
│ │ ├── buscaminas.c
│ │ └── buscaminas.h
│ ├── commons/
│ │ └── io.c
│ ├── juego_delaVida/
│ │ ├── juego_delaVida.c
│ │ └── juego_delaVida.h
│ ├── piedra_papel_tijera/
│ │ ├── piedra_papel_tijera.c
│ │ └── piedra_papel_tijera.h
│ ├── robots/
│ │ ├── robots.c
│ │ └── robots.h
│ ├── tres_enRaya/
│ │ ├── tres_enRaya.c
│ │ └── tres_enRaya.h
│ └── main.c
├── compile.sh # Script alternativo de compilación
└── README.md # Este documento
```

---

## Compilación

Requiere un compilador compatible con **C99** (por ejemplo `gcc` o `clang`).

```bash
# Esto generará el binario bin/mygames.
make
# clean para una compilación limpia
make clean
```

## Ejecución

Ejecuta en el menú principal:

```bash
mygames
```

O ejecuta un juego directamente:

```bash
mygames --ahorcado
mygames --buscaminas
mygames --juego_delaVida
mygames --piedra_papel_tijera
mygames --robots
mygames --tres_enRaya

mygames --help
```

## Juegos incluidos

- Ahorcado
- Buscaminas
- Juego de la vida de Conway
- Piedra, papel o tijera
- Robots
- Tres en raya

## Licencia

Este proyecto está bajo GPLv3.
Consulta los archivos LICENSE <https://www.gnu.org/licenses/gpl-3.0.html> (original en inglés) y LICENCIA <https://lslspanish.github.io/translation_GPLv3_to_spanish/> (traducción al español).
