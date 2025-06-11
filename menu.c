#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>

// #include "EasyPIO.h"          <----DESCOMENTAR CUANDO SE USE EN LA RASPBERRY

#define MAX_PASSWORD 5
#define RETARDO_DEF 0x1500

void mostrar_menu(void);
void ejecutar_opcion(int op);

/* secuencias */
void ejecutar_secuencia(int secuencia);
void secuencia_auto_fantastico(void);
void secuencia_choque(void);
void secuencia_prop_alg(void);
void secuencia_tabla(void);

/* utilitarios */
void disp_binary(uint8_t value);
void delay(volatile uint32_t ticks);
int leer_password(char *buffer, size_t max_len);
bool revisar_entrada(int ch);

uint32_t delay_ticks = 300; // Velocidad inicial de la secuencia


int main(void)
{
    // pioInit();

    initscr();             // arranca ncurses
    cbreak();              // un solo carácter a la vez
    noecho();              // no “eco” de teclas
    nodelay(stdscr, TRUE); // getch() no bloquea

    /* 1. Autenticación  */
    char verificador = false;
    char lector_password[MAX_PASSWORD + 1];
    const char password[MAX_PASSWORD + 1] = "daleB";
    int cant_intentos = 0;

    do
    {
        printf("Ingrese su password (5 digitos): ");
        int leidos = leer_password(lector_password, MAX_PASSWORD);

        if (leidos != MAX_PASSWORD)
        {
            printf("Longitud incorrecta (deben ser 5 dígitos).\n");
            ++cant_intentos;
            continue;
        }

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
            printf("Password no válida\n");
            ++cant_intentos;
        }
    } while (cant_intentos < 3 && !verificador);

    if (!verificador)
    {
        puts("Se han excedido los intentos permitidos. Acceso denegado.");
        return 0;
    }

    // volvemos a reactivar cbreak/noecho/nodelay
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

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

    nodelay(stdscr, FALSE);
    echo();
    nocbreak();
    endwin();

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
        puts("\nAuto fantástico:\n");
        secuencia_auto_fantastico();
        break;
    case 2:
        puts("\nEl choque:\n");
        secuencia_choque();
        break;
    case 3:
        puts("\nSecuencia propia #1:\n");
        secuencia_prop_alg();
        break;
    case 4:
        puts("\nSecuencia propia #2:\n");
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
    printf("Presione 'u' para aumentar la velocidad y 'd' para disminuirla");
    int ch;
    while (1)
    {
        for (int i = 0; i < 8; ++i)
        {
            ch = getch();
            if (revisar_entrada(ch))
            {
                disp_binary(0x00); // apaga
                // leds(0x00); // Descomentar cuando se use en la Raspberry
                return; // sale de la secuencia
            }
            disp_binary(1u << i);
            delay(delay_ticks);
            // leds(1u << i);                         <---- Descomentar cuando se use en la Raspberry
            // delayMillis(300);
        }

        for (int i = 6; i > 0; --i)
        {
            ch = getch();
            if (revisar_entrada(ch))
            {
                disp_binary(0x00); // apaga
                // leds(0x00); // Descomentar cuando se use en la Raspberry
                return; // sale de la secuencia
            }
            disp_binary(1u << i);
            delay(delay_ticks);
            // leds(1u << i);                         <---- Descomentar cuando se use en la Raspberry
            // delayMillis(300);
        }
    }
    disp_binary(0x00);
    // leds(0x00); // Descomentar cuando se use en la Raspberry;
}

void secuencia_choque(void)
{
    printf("Presione 'u' para aumentar la velocidad y 'd' para disminuirla");
    int ch;
    while (1)
    {
        ch = getch();
        if(revisar_entrada(ch)){
            disp_binary(0x00);
            //leds(0x00)
            return;
        }
        /* Patrón simétrico: dos luces se acercan, chocan y se alejan */
        uint8_t tabla[8] = {0x81, 0x42, 0x24, 0x18,
                            0x18, 0x24, 0x42, 0x81};

        for (size_t i = 0; i < 8; ++i)
        {
            ch = getch();
            if (revisar_entrada(ch)){
            disp_binary(0x00);
            // leds(0x00);
            return;
        }
        disp_binary(tabla[i]);
        //leds(tabla[i]);
        delay(delay_ticks);
        //delayMillis(delay_ticks);
        }
    }
}

void secuencia_prop_alg(void)
{
    printf("Presione 'u' para aumentar la velocidad y 'd' para disminuirla");
    int ch;
    while (1)
    {
        ch = getch();
        if (revisar_entrada(ch)){
            disp_binary(0x00);
            // leds(0x00);
            return;
        }
        /* Parpadeo alternado pares↔impares 5 veces */
        for (int n = 0; n < 5; ++n)
        {
            ch = getch();
            if (revisar_entrada(ch)){
                disp_binary(0x00);
                // leds(0x00);
                return;
            }
            
            disp_binary(0xAA);
            // leds(0xAA);
            delay(delay_ticks);
            // delayMillis(delay_ticks);
            
            ch = getch();
            if (revisar_entrada(ch)){
                disp_binary(0x00);
                // leds(0x00);
                return;
            }
            disp_binary(0x55);
            // leds(0x55);
            delay(delay_ticks);
            // delayMillis(delay_ticks);
        }
    }
    disp_binary(0x00);
    // leds(0x00);
}

void secuencia_tabla(void)
{
    printf("Presione 'u' para aumentar la velocidad y 'd' para disminuirla");
    int ch;
    while (1)
    {
        // Onda desplazada
        uint8_t tabla[] = {0x11, 0x22, 0x44, 0x88,
                           0x44, 0x22};
        for (size_t i = 0; i < sizeof(tabla); ++i)
        {
            ch = getch();
            if (revisar_entrada(ch)){
                disp_binary(0x00);
                // leds(0x00);
                return;
            }
            
            // leds(tabla[i]);
            // delayMillis(delay_ticks);
            disp_binary(tabla[i]);
            delay(delay_ticks);
        }
    }
    disp_binary(0x00); // todos apagados
    // leds(0x00);
}

/* ====================== UTILITARIOS ====================== */

int leer_password(char *buffer, size_t len)
{
    system("stty -echo -icanon"); // sin eco y sin modo canónico

    int i = 0, ch;
    for (;;)
    {
        ch = getchar();               // llega tecla por tecla
        if (ch == '\n' || ch == '\r') // Enter → fin de lectura
            break;

        // Backspace
        if ((ch == 0x7F || ch == 0x08) && i > 0)
        {
            --i;
            printf("\b \b"); // borra un '*'
            fflush(stdout);
            continue;
        }

        // Solo guarda hasta len caracteres; extra quedarán descartados
        if (i < (int)len)
        {
            buffer[i++] = (char)ch;
            putchar('*'); // muestra el asterisco
            fflush(stdout);
        }
    }
    buffer[i] = '\0';
    putchar('\n');

    system("stty echo icanon"); // restaura la terminal
    return i;                   // devuelve cuántos dígitos guardó
}

bool revisar_entrada(int ch)
{
    if (ch == 'u' || ch == 'U') {
        if (delay_ticks > 10) delay_ticks -= 50;
        return false;
    }
    else if (ch == 'd' || ch == 'D') {
        if (delay_ticks < 1000) delay_ticks += 50;
        return false;
    }
    else if (ch != ERR) {
        return true; // cualquier otra tecla: salir
    }
    return false;
}


/* void leds(uint8_t value)
{
    // Enciende LEDs según bit (MSB→LED0)                           <----DESCOMENTAR CUANDO SE USE EN LA RASPBERRY
    for (int i = 0; i < 8; ++i) {
        int bit = (value & (1 << (7 - i))) != 0; // MSB primero
        digitalWrite(led_pins[i], bit ? HIGH : LOW);
    }
}*/

void disp_binary(uint8_t v)
{
    /* Imprime los 8 bits MSB→LSB */
    for (int b = 7; b >= 0; --b)
        putchar((v & (1u << b)) ? '*' : '_');
    putchar('\n');
}

void delay(volatile uint32_t ticks)
{
    const useconds_t us = ticks * 0x100;
    usleep(us);
}