    .syntax unified
    .cpu cortex-a7          @ ajusta si usás otro núcleo

    .text
    .global  disp_binary
    .global  delay
    .global  secuencia_prop_alg
    .global  secuencia_tabla

    /* ----- funciones/variables externas en C -------------------- */
    .extern putchar
    .extern leds                @ void leds(uint8_t)
    .extern getch               @ int  getch(void)
    .extern revisar_entrada     @ int  revisar_entrada(int)
    .extern delay_ticks         @ uint32_t delay_ticks


/* ───────────────────────── disp_binary ───────────────────────────
   R0 = byte → enciende LED y muestra '*' / '_' + '\n'             */
disp_binary:
    /* guardar r4-r7 y lr (5 registros) */
    SUB     sp, sp, #20
    STR     r4, [sp, #0]
    STR     r5, [sp, #4]
    STR     r6, [sp, #8]
    STR     r7, [sp, #12]
    STR     lr, [sp, #16]

    MOV     r4, r0              @ r4 = byte original
    MOV     r0, r4              @ arg para leds()
    BL      leds                @ ← enciende los 8 GPIO

    MOV     r5, #7              @ contador de bits 7‥0

bit_loop:
    MOV     r6, #1
    MOV     r6, r6, LSL r5      @ r6 = 1<<bit
    ANDS    r7, r4, r6          @ Z=1 si bit=0
    MOVEQ   r0, #'_'
    MOVNE   r0, #'*'
    BL      putchar

    SUBS    r5, r5, #1
    BPL     bit_loop

    MOV     r0, #'\n'
    BL      putchar

    /* restaurar */
    LDR     lr, [sp, #16]
    LDR     r7, [sp, #12]
    LDR     r6, [sp, #8]
    LDR     r5, [sp, #4]
    LDR     r4, [sp, #0]
    ADD     sp, sp, #20
    MOV     pc, lr


/* ───────────────────────── delay ────────────────────────────────
   R0 = ticks → bucle de ticks*256 iteraciones                    */
delay:
    SUB     sp, sp, #8
    STR     r4, [sp, #0]
    STR     lr, [sp, #4]

    MOV     r4, r0
    MOV     r4, r4, LSL #8      @ ×256

dly_loop:
    SUBS    r4, r4, #1
    BNE     dly_loop

    LDR     lr, [sp, #4]
    LDR     r4, [sp, #0]
    ADD     sp, sp, #8
    MOV     pc, lr


/* ───────────────────────── wait_delay ───────────────────────────
   Llama a delay(delay_ticks)                                     */
wait_delay:
    SUB     sp, sp, #4     @ ─ guarda LR
    STR     lr, [sp]

    LDR     r0, =delay_ticks
    LDR     r0, [r0]
    BL      delay           @ BL pisa LR

    LDR     lr, [sp]        @ ─ restaura LR del caller
    ADD     sp, sp, #4
    MOV     pc, lr



/* ───────────── secuencia_prop_alg – alterna 0xAA / 0x55 ───────── */
secuencia_prop_alg:
    SUB     sp, sp, #8
    STR     r4, [sp, #0]
    STR     lr, [sp, #4]

alg_loop:
    BL      getch
    MOV     r1, r0
    BL      revisar_entrada
    CMP     r0, #0
    BNE     alg_exit            @ salir si el usuario lo pidió

    LDR     r0, =0xAA
    BL      disp_binary
    BL      wait_delay

    BL      getch
    MOV     r1, r0
    BL      revisar_entrada
    CMP     r0, #0
    BNE     alg_exit

    LDR     r0, =0x55
    BL      disp_binary
    BL      wait_delay
    B       alg_loop

alg_exit:
    MOV     r0, #0
    BL      disp_binary
    LDR     lr, [sp, #4]
    LDR     r4, [sp, #0]
    ADD     sp, sp, #8
    MOV     pc, lr


/* ───────────── secuencia_tabla – “onda” en 6 bytes ────────────── */
secuencia_tabla:
    SUB     sp, sp, #12
    STR     r4, [sp, #0]
    STR     r5, [sp, #4]
    STR     lr, [sp, #8]

    LDR     r4, =tabla
    MOV     r5, #6

tab_loop:
    BL      getch
    MOV     r1, r0
    BL      revisar_entrada
    CMP     r0, #0
    BNE     tab_exit

    LDRB    r0, [r4], #1        @ r0 = *r4++; avanza
    BL      disp_binary
    BL      wait_delay

    SUBS    r5, r5, #1
    BNE     tab_loop

    LDR     r4, =tabla          @ reinicia tabla
    MOV     r5, #6
    B       tab_loop

tab_exit:
    MOV     r0, #0
    BL      disp_binary
    LDR     lr, [sp, #8]
    LDR     r5, [sp, #4]
    LDR     r4, [sp, #0]
    ADD     sp, sp, #12
    MOV     pc, lr


/* ───────────────────── Tabla de patrones ─────────────────────── */
tabla:
    .byte 0x11, 0x22, 0x44, 0x88, 0x44, 0x22
