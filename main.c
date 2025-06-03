#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_PASSWORD 5
#define RETARDO_DEF 0x1500

void mostrar_menu(void);
void ejecutar_opcion(int op);

/* secuencias */
void secuencia_auto_fantastico(void);
void secuencia_choque(void);
void secuencia_prop_alg(void);
void secuencia_tabla(void);

/* utilitarios */
void disp_binary(uint8_t value);
void delay(volatile uint32_t ticks);

int main(void)
{
    /* 1. Autenticación  */
    char verificador = false;
    char lector_password[MAX_PASSWORD + 1];
    const char password[MAX_PASSWORD + 1] = "daleR";
    int cant_intentos = 0;

    do
    {
        printf("Ingrese su password (5 digitos): ");
        scanf("%5s", lector_password); /* evita overflow */

        verificador = true;
        for (int i = 0; i < MAX_PASSWORD; ++i)
        {
            if (lector_password[i] != password[i])
            {
                verificador = false;
                break;
            }
        }

        if (verificador)
        {
            printf("\nBienvenido al sistema\n");
        }
        else
        {
            printf("Password no valida\n");
            ++cant_intentos;
        }
    } while (cant_intentos < 3 && !verificador);

    if (!verificador)
    {
        puts("Se han excedido los intentos permitidos. Acceso denegado.");
        return 0;
    }

    /* ─── 2. Menú principal ─── */
    int opcion;
    do
    {
        mostrar_menu();
        printf("Seleccione una opcion: ");
        if (scanf("%d", &opcion) != 1)
        {
            while (getchar() != '\n')
                ; /* limpia stdin */
            opcion = -1;
            continue;
        }
        ejecutar_opcion(opcion);
    } while (opcion != 0);

    puts("\nPrograma finalizado.");
    return 0;
}

/* MENÚ  */
void mostrar_menu(void)
{
    puts("========= MENU PRINCIPAL =========");
    puts("1) Auto fantastico");
    puts("2) El choque");
    puts("3) Secuencia propia (algoritmo)");
    puts("4) Secuencia propia (tabla de datos)");
    puts("0) Salir");
    puts("==================================");
}

void ejecutar_opcion(int op)
{
    switch (op)
    {
    case 1:
        puts("\nAuto fantástico...\n");
        secuencia_auto_fantastico();
        break;
    case 2:
        puts("\nEl choque...\n");
        secuencia_choque();
        break;
    case 3:
        puts("\nSecuencia propia-1...\n");
        secuencia_prop_alg();
        break;
    case 4:
        puts("\nSecuencia propia-2...\n");
        secuencia_tabla();
        break;
    case 0:
        puts("\nSaliendo al sistema...\n");
        break;
    default:
        puts("\nOpción invalida. Intente de nuevo.\n");
    }
}

/* SECUENCIAS DE LUCES */
void secuencia_auto_fantastico(void)
{
    /* TODO: desplazar un “1” de izquierda a derecha y regreso */
}

void secuencia_choque(void)
{
    /* Patrón simétrico: dos luces se acercan, chocan y se alejan */
    uint8_t tabla[8] = {0x81, 0x42, 0x24, 0x18,
                        0x18, 0x24, 0x42, 0x81};

    for (size_t i = 0; i < 8; ++i)
    {
        disp_binary(tabla[i]); /* sustitúyelo por salida a pines */
        delay(RETARDO_DEF);
    }
}

void secuencia_prop_alg(void)
{
    /* TODO: tu propia secuencia generada vía algoritmo */
}

void secuencia_tabla(void)
{
    /* Libre para otra tabla de datos o patrón distinto */
}

/* ====================== UTILITARIOS ====================== */
void disp_binary(uint8_t v)
{
    /* Imprime los 8 bits MSB→LSB */
    for (int b = 7; b >= 0; --b)
        putchar((v & (1u << b)) ? '*' : '_');
    putchar('\n');
}

void delay(volatile uint32_t ticks)
{
    /* Aproximadamente 10 µs por tick en un PC. Ajusta según tu HW */
    const useconds_t us = ticks * 0x100;
    usleep(us);
}