# Laboratorio 1: "myBash" - Grupo 1 - FaMAF (2024)

Los **autores** que colaboraron para la creación del mismo son: 
 - Juan Cruz Hermosilla Artico
 - Gaspar Saenz Valiente
 - Fernando Cabrera Luque

Los **comandos internos** de este bash son:
-  cd : Permite desplazarse entre los directorios del sistema.
- help : Muestra información de myBash.
- exit : Es para salir de la sesión.
-  :) : Printea una carita.
- cmds : Muestra los comandos builtin y sus descripciones.


## **Índice**
 1. [Introducción](#Introducción)
 2. [Dependencias](#Dependencias)
 3. [Compilación y ejecución](#Compilación-y-ejecución)
 4. [Módulos componentes de myBash](#Módulos-componentes-de-myBash)
 5. [Detalles de implementación](#Detalles-de-implementación)
    1. [Funcionalidad básica](#Funcionalidad-básica)
    2. [Puntos estrella](#Puntos-estrella)


## **Introducción**
Un shell es una interfaz entre el sistema operativo (SO) y el usuario, pues permite a este último acceder a los servicios del SO (como por ejemplo ejecutar comandos, redireccionar entradas y salidas, etc).<br>
En este proyecto de laboratorio impartido por la cátedra de **Sistemas Operativos** de **FaMAF** *(Facultad de Matemática, Astronomía, Física y Computación)* se busca programar un shell propio e hiper minimalista denominado **myBash**, indagando en el funcionamiento interno del mismo y afrontando los problemas que se presentan a lo largo de su implementación.


## **Dependencias**
Para poder compilar y ejecutar el proyecto, es necesario tener instaladas las siguientes herramientas:
 - Librería [`glib`](https://gitlab.gnome.org/GNOME/glib/) 
 - Librería [`check`](https://libcheck.github.io/check/)
 - Fuente monoespaciada con soporte de caracteres unicode: [`JetBrains Mono`](https://www.jetbrains.com/es-es/lp/mono/)


## **Compilación y ejecución**
Para compilar el proyecto, simplemente ejecutar:
```sh
make
```
Se generará un archivo ejecutable con el nombre del proyecto, el cual se puede ejecutar con:
```sh
./mybash
```
Finalmente, para eliminar todos los archivos ejecutables, debe utilizarse el comando:
```sh
make clean
```

## **Módulos componentes de myBash**
 - `mybash.c`: módulo principal del shell
 - `prompt.c`: implementación de un prompt estético con información relevante para el usuario
 - `parsing.c`: análisis de la entrada del usuario utilizando el parser
 - `parser.h`: implementación del TAD parser otorgado por la cátedra
 - `execute.c`: ejecución de comandos externos y administración de las llamadas al sistema operativo (syscalls)
 - `builtin.c`: implementación de interfaces para identificación e interacción con comandos internos
 - `command.c`: implementación del TAD que conforma los comandos simples y pipelines


## **Detalles de implementación**
### ***Funcionalidad básica***
Al momento de entrega, el proyecto cumple con todas las funcionalidades mínimas que fueron exigidas por la cátedra: 
| Objetivos | Status |
| --- | --- |
| Pasar el 100% del *unit-testing* (`make test`) dado para todo el proyecto | :heavy_check_mark: |
| Manejar pipelines de, al menos, dos comandos | :heavy_check_mark: |
| Manejar de manera adecuada la terminación de procesos lanzados en segundo plano con &, sin dejar procesos *zombies* | :heavy_check_mark: |
| Preservar las buenas prácticas de programación ya adquiridas durante los cursos anteriores | :heavy_check_mark: |

### ***Puntos estrella***
A su vez, se cumplen con ciertos objetivos *estrella* o extras:
| Objetivos | Status |
| --- | --- |
| Enunciado extra con suspenso (`obfuscated.c`) | :heavy_check_mark: |
| Generalizar el comando pipeline a una cantidad arbitraria de comandos simples | :heavy_check_mark: |
| Implementar el comando secuencial “&&” entre comandos simples | :x: |
| Imprimir un prompt con información relevante, por ejemplo, nombre del host, nombre de usuario y camino relativo | :heavy_check_mark: |
| Implementar toda la generalidad para aceptar la gramática de list según la sección SHELL GRAMMAR de man bash | :x: |
