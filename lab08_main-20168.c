/*
 * File:   lab08_main-20168.c
 * Author: luisg
 *
 * Created on April 19, 2022, 8:32 PM
 */
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include<stdio.h>
#include <stdint.h>

//DEFINICION DE FRECUENCIA PARA DELAY
#define _XTAL_FREQ 4000000

//PROTO FUNCIONES
void setup(void);

//CONFIGURACION PRINCIPAL
void setup(void){
    ANSEL = 0b00000001;         // PORTA PIN0 COMO ANALOGICO, RESTO COMO DIGITALES
    ANSELH = 0;                 // DEMAS PUERTOS COMO DIGITALES
    
    TRISA = 0b00000001;         // PORTA PIN0 COMO ENTRADA, RESTO COMO SALIDA
    PORTA = 0;                  // LIMPIEZA DEL PORTA
    
    TRISC = 0;                  // PORTC COMO SALIDA
    PORTC = 0;                  // LIMPIEZA DEL PORTC
    
    //OSCCONFIC
    OSCCONbits.IRCF = 0b0110;   // FRECUENCIA DE OSCILADOR INTERNO (4MHz)
    OSCCONbits.SCS  = 1;        // RELOJ INTERNO
    
    //ADC config
    ADCON0bits.ADCS = 0b01;     // Fosc/8
    
    ADCON1bits.VCFG0 = 0;       // USO DE VDD COMO VOLTAJE DE REFERENCIA INTERNO
    ADCON1bits.VCFG1 = 1;       // USO DE VSS COMO VOLTAJE DE REFERENCIA INTERNO
    
    ADCON0bits.CHS = 0b0000;    // SELECCION DE PORTA PIN0 (AN0) COMO ENTRADA DE ADC
    ADCON1bits.ADFM = 0;        // FORMATO DE BITS JUSTIFICADOS A LA IZQUIERDA
    ADCON0bits.ADON = 1;        // HABILITACION DE MODULO DE ADC
    __delay_us(40);
    
    //CONFIG DE INTERRUPCIONES
    INTCONbits.GIE = 1;         // HABILITAR INTERRUPCIONES GLOBALES
    INTCONbits.PEIE = 1;        // HABILITAR INTERRUPCIONES EN PERIFERICOS
    PIE1bits.ADIE = 1;          // HABILITAR INTERRUPCION DE ADC
    PIR1bits.ADIF = 0;          // LIMPIEZA DE BANDERA DE INTERRUPCION DE ADC
}

//INTERRUPCIONES
void __interrupt() isr(void){
    if (PIR1bits.ADIF){         // REVISAR INTERRUPCION DE ADC
        if (ADCON0bits.CHS == 0){   // REVISAR SI LA INTERRUPCION FUE EN AN0
            PORTC = ADRESH;         // CARGAR BITS MAS SIGNIFICATIVOS AL PORTC
        }
        PIR1bits.ADIF = 0;          // LIMPIEZA DE BANDERA DE INTERRUPCION ADC
    }
    return;
}

//CICLO MAIN
void main(void) {
    //EJECUCION CONFIG
    setup();    
    
    //LOOP MAIN
    while(1){
        if (ADCON0bits.GO == 0){    // REVISAR SI HAY PROCESO DE CONVERSION
            ADCON0bits.GO = 1;      // DE NO HABER, INICIAR PROCESO DE CONVERSION
        }   
    }
    return;
}


