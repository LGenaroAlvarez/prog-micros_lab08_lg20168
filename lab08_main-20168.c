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

//DEFINICIONES GENERALES
#define tmr0_val 248            // PARA INTERRUPCIONES CADA 2mS
#define disp1 PORTEbits.RE2
#define disp2 PORTEbits.RE1
#define disp3 PORTEbits.RE0  

//VARIABLES GLOBALES
uint8_t mod = 0;                // VARIABLE DE MODULO
int var;                        // VARIABLE GENERICA
int volt_val;                   // VARIABLE PARA VALOR DE VOLTAJE
int huns = 0;                   // VARIABLE PARA CENTENAS (HUNDREDS)
int tens = 0;                   // VARIABLE PARA DECENAS (TENTHS)
int ones = 0;                   // VARIABLE PARA UNIDADES (ONES)
int disp_flag = 0;              // BANDERA PARA DISPLAYS

//7SEG DISPLAY INDEX
char index[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

//PROTO FUNCIONES
void setup(void);

void tmr0_setup(void);

int digits(void);

//CONFIGURACION PRINCIPAL
void setup(void){
    ANSEL = 0b00000011;         // PORTA PIN0 COMO ANALOGICO, RESTO COMO DIGITALES
    ANSELH = 0;                 // DEMAS PUERTOS COMO DIGITALES
    
    TRISA = 0b00000011;         // PORTA PIN0 COMO ENTRADA, RESTO COMO SALIDA
    PORTA = 0;                  // LIMPIEZA DEL PORTA
    
    TRISC = 0;                  // PORTC COMO SALIDA
    TRISD = 0;                  // PORTD COMO SALIDA
    TRISE = 0;                  // PORTE COMO SALIDA
    PORTC = 0;                  // LIMPIEZA DEL PORTC
    PORTD = 0;                  // LIMPIEZA DEL PORTD
    PORTE = 0;                  // LIMPIEZA DEL PORTE
    
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
    INTCONbits.T0IE = 1;        // HABILITAR INTERRUPCIONES DE TMR0
    INTCONbits.T0IF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
    PIE1bits.ADIE = 1;          // HABILITAR INTERRUPCION DE ADC
    PIR1bits.ADIF = 0;          // LIMPIEZA DE BANDERA DE INTERRUPCION DE ADC
}

//CONFIGURACION TMR0
void tmr0_setup(void){
    OPTION_REGbits.T0CS = 0;    // UTILIZAR CICLO INTERNO
    OPTION_REGbits.PSA = 0;     // CAMBIAR PRESCALER A TMR0
    OPTION_REGbits.PS0 = 1;     // COLOCAR PRESCALER EN 1:256
    OPTION_REGbits.PS1 = 1;     // 
    OPTION_REGbits.PS2 = 1;     // 
    
    INTCONbits.T0IF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
    TMR0 = tmr0_val;            // VALOR DE TMR0
    return;
}

int digits(void){
    volt_val = var*((float)5/255)*(100);  // CONVERSION DE VALOR DE 255 A 500 PARA VOLTAJE
    mod = volt_val%100;        // CALCULO DEL MODULO DEL CONTADOR PARA AISLAR DECENAS Y UNIDADES
    huns = volt_val/100;       // DIVISION DE CONTADOR PARA DETERMINAR CENTENAS
    tens = mod/10;              // DIVISION DEL MODULO DEL CONTADOR PARA DETERMINAR DECENAS
    ones = mod%10;              // MODULO DEL MODULO DEL CONTADOR PARA DETERMINAR UNIDADES
}

//INTERRUPCIONES
void __interrupt() isr(void){
    if (PIR1bits.ADIF){         // REVISAR INTERRUPCION DE ADC
        if (ADCON0bits.CHS == 0){   // REVISAR SI ESTA ACTIVADO AN0
            var = ADRESH;         // CARGAR BITS MAS SIGNIFICATIVOS AL PORTC
        }
        else if (ADCON0bits.CHS == 1){  // REVISAR SI ESTA ACTIVADO AN1
            PORTD = ADRESH;             // CARGAR BITS MAS SIGNIFICATIVOS AL PORTD
        }
        PIR1bits.ADIF = 0;          // LIMPIEZA DE BANDERA DE INTERRUPCION ADC
    }
    
    else if(T0IF){              // INTERRUPCION DE TMR0 ACTIVADA
        INTCONbits.T0IF = 0;    // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
        TMR0 = tmr0_val;        // REINICIAR TMR0
        PORTE = 0;              // PREPARAR PORTE PARA SELECCION DE DISPLAYS
        if (disp_flag == 0){            //
            PORTC = (index[ones]);      // PASAR VALOR DE UNIDADES A DISPLAY USANDO INDICE
            disp3 = 0;                  // APAGAR DISPLAY3
            disp1 = 1;                  // ENCENDER DISPLAY1 (CURRENT DISP)
            disp_flag = 1;              // CAMBIAR DE DISPLAY
        }
        else if (disp_flag == 1){       // 
            PORTC = (index[tens]);      // PASAR VALOR DE DECENAS A DISPLAY USANDO INDICE
            disp1 = 0;                  // APAGAR DISPLAY1
            disp2 = 1;                  // ENCENDER DISPLAY2 (CURRENT DISP)
            disp_flag = 2;              // CAMBIAR DE DISPLAY
        }
        else if (disp_flag == 2){       //
            PORTC = (index[huns]);      // PASAR VALOR DE CENTENAS A DISPLAY USANDO INDICE
            PORTCbits.RC7 = 1;          // ACTIVAR PIN PARA DP
            disp2 = 0;                  // APAGAR DISPLAY 2
            disp3 = 1;                  // ENCENDER DISPLAY 3 (CURRENT DISP)
            disp_flag = 0;              // CAMBIAR DE DISPLAY
        }
    }
    return;
}

//CICLO MAIN
void main(void) {
    //EJECUCION CONFIG
    setup();  
    tmr0_setup();
    
    //LOOP MAIN
    while(1){
        digits();
        if (ADCON0bits.GO == 0){    // REVISAR SI HAY PROCESO DE CONVERSION
            if (ADCON0bits.CHS == 0){           // REVISAR SI SE ENCUENTRA EN EL CANAL 0
                ADCON0bits.CHS = 0b0001;        // SI SI, CAMBIAR A CANAL 1
            }
            else if (ADCON0bits.CHS == 1){      // REVISAR SI SE ENCUENTRA EN EL CANAL 1
                ADCON0bits.CHS = 0b0000;        // SI SI, CAMBIAR A CANAL 0
            }
            __delay_us(40);                     // TIEMPO DE ESPERA DE CAMBIO
            ADCON0bits.GO = 1;      // DE NO HABER, INICIAR PROCESO DE CONVERSION
        }   
    }
    return;
}


