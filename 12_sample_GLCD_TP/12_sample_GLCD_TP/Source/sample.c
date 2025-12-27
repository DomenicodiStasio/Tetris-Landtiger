/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"         // <--- Aggiungi questo
#include "../Tetris_Game/tetris.h"

int main(void) {
    SystemInit();  
    LCD_Initialization();
    LCD_Clear(Black);
    
    // --- Configurazione JOYSTICK (P1.27, P1.28, P1.29) ---
    // Imposta i pin come INPUT
		LPC_GPIO1->FIODIR &= ~((1<<26)|(1<<27)|(1<<28)|(1<<29));

    // --- Inizializza Grafica e Variabili di Gioco ---
    Tetris_Init(); 
    
    // --- Configurazione LED (P2.0 ... P2.7) ---
    LPC_GPIO2->FIODIR |= 0x000000FF; // Output
    LPC_GPIO2->FIOSET = 0x000000FF;  // Spegni tutto (Active Low)

    // ============================================================
    //    ATTIVAZIONE DEI DUE TIMER (CUORE DEL SISTEMA)
    // ============================================================

    // 1. RIT per il JOYSTICK (Veloce - 50ms)
    // 0x004C4B40 = 50ms (con clock a 100MHz)
    init_RIT(0x000F4240); 
		//init_RIT(0x00030D40);
		
    enable_RIT();
    // Nota: solitamente enable_RIT() abilita già l'interrupt, 
    // ma se nel tuo lab serve esplicito, lascialo:
    NVIC_EnableIRQ(RIT_IRQn);

    // 2. TIMER0 per la GRAVITÀ (Lento - 1 secondo)
    // La LandTiger ha i timer periferici solitamente a 25MHz (CCLK/4).
    // 25.000.000 * (1/25MHz) = 1 secondo esatto.
    // Sintassi: init_timer(timer_num, prescale, match_value);
    init_timer(0, 600000); 
    enable_timer(0);

    // ============================================================
    
    // Loop infinito a basso consumo
    // Tutto il lavoro ora lo fanno RIT_IRQHandler e TIMER0_IRQHandler
    while(1) { 
        __ASM("wfi"); 
    }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
