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

void setup(void){
    ANSEL = 0b00000001;
    ANSELH = 0;
    
    TRISA = 0b00000001;
    PORTA = 0;
    
    TRISC = 0;
    PORTC = 0;
    
    //OSCCONFIC
    OSCCONbits.IRCF = 0b0110;   // FRECUENCIA DE OSCILADOR INTERNO (4MHz)
    OSCCONbits.SCS  = 1;        // RELOJ INTERNO
    
    //ADC config
    ADCON0bits.ADCS = 0b01;
    
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 1;
    
    ADCON0bits.CHS = 0b0000;
    ADCON1bits.ADFM = 0;
    ADCON0bits.ADON = 1;
    __delay_us(40);
    
    //CONFIG DE INTERRUPCIONES
    INTCONbits.GIE = 1;         // HABILITAR INTERRUPCIONES GLOBALES
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0; 
}

void __interrupt() isr(void){
    if (PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0){
            PORTC = ADRESH;
        }
        PIR1bits.ADIF = 0;
    }
    return;
}

void main(void) {
    setup();
    
    
    while(1){
        if (ADCON0bits.GO == 0){
            ADCON0bits.GO = 1;
        }   
    }
    return;
}


