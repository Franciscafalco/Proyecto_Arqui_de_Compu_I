#include <stdio.h>
#include "funciones_arm.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>

#include "EasyPIO.h" // <----DESCOMENTAR CUANDO SE USE EN LA RASPBERRY

/*-----niveles lógicos para EasyPIO------*/
#ifndef HIGH
#define HIGH 1
#endif
#ifndef LOW
#define LOW 0
#endif

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
void leds(uint8_t value);
int leer_password(char *buffer, size_t max_len);
bool revisar_entrada(int ch);
void leds_init(void);

uint32_t delay_ticks = 300; // Velocidad inicial de la secuencia

const int led_pins[8] = {14, 15, 18, 23, 24, 25, 8, 7};

int main(void)
{
    pioInit(); // Descomentar cuando se use en la Raspberry

    leds_init();           // Descomentar cuando se use en la Raspberry
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
                leds(0x00);        // Descomentar cuando se use en la Raspberry
                return;            // sale de la secuencia
            }
            disp_binary(1u << i);
            delay(delay_ticks);
            leds(1u << i); //<---- Descomentar cuando se use en la Raspberry
            delayMillis(300);
        }

        for (int i = 6; i > 0; --i)
        {
            ch = getch();
            if (revisar_entrada(ch))
            {
                disp_binary(0x00); // apaga
                leds(0x00);        // Descomentar cuando se use en la Raspberry
                return;            // sale de la secuencia
            }
            disp_binary(1u << i);
            delay(delay_ticks);
            leds(1u << i); //  <---- Descomentar cuando se use en la Raspberry
            delayMillis(300);
        }
    }
    disp_binary(0x00);
    leds(0x00); // Descomentar cuando se use en la Raspberry;
}

void secuencia_choque(void)
{
    printf("Presione 'u' para aumentar la velocidad y 'd' para disminuirla");
    int ch;
    while (1)
    {
        ch = getch();
        if (revisar_entrada(ch))
        {
            disp_binary(0x00);
            leds(0x00);
            return;
        }
        /* Patrón simétrico: dos luces se acercan, chocan y se alejan */
        uint8_t tabla[8] = {0x81, 0x42, 0x24, 0x18,
                            0x18, 0x24, 0x42, 0x81};

        for (size_t i = 0; i < 8; ++i)
        {
            ch = getch();
            if (revisar_entrada(ch))
            {
                disp_binary(0x00);
                leds(0x00);
                return;
            }
            disp_binary(tabla[i]);
            leds(tabla[i]);
            delay(delay_ticks);
            delayMillis(delay_ticks);
        }
    }
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
    if (ch == 'u' || ch == 'U')
    {
        if (delay_ticks > 10)
            delay_ticks -= 50;
        return false;
    }
    else if (ch == 'd' || ch == 'D')
    {
        if (delay_ticks < 1000)
            delay_ticks += 50;
        return false;
    }
    else if (ch != ERR)
    {
        return true; // cualquier otra tecla: salir
    }
    return false;
}

void leds(uint8_t value)
{
    for (int i = 0; i < 8; ++i)
    {
        int bit = (value >> i) & 0x01; // extrae bit i (LSB a MSB)
        digitalWrite(led_pins[i], bit ? HIGH : LOW);
    }
}

void leds_init(void)
{
    pioInit(); // Descomentar cuando se use en la Raspberry
    for (int i = 0; i < 8; ++i)
    {
        pinMode(led_pins[i], OUTPUT);   // Configura pines como salida
        digitalWrite(led_pins[i], LOW); // Apaga todos los LEDs
    }
}
