/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdio.h> /*for sprintf*/

extern volatile int currentX;
extern volatile int currentY;
extern volatile int currentPiece;
extern volatile int currentRotation;

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
/*********************************************************************************************************
** Function name:       TIMER0_IRQHandler
** Description:         Gestisce SOLO la gravità (scende in automatico)
*********************************************************************************************************/
void TIMER0_IRQHandler (void)
{
    // Qui non servono contatori, perché questo timer scatta già lentamente!
    
    // Prova a scendere di 1
    if (Check_Collision(currentX, currentY + 1, currentPiece, currentRotation) == 0) {
        // Se c'è spazio: cancella, scendi, disegna
        Delete_Piece(currentX, currentY, currentPiece, currentRotation);
        currentY++;
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red);
    } 
    else {
        // Se tocca il fondo o un altro pezzo:
        // 1. Logica di "Lock" (fissa il pezzo nella griglia statica)
        // 2. Controllo righe complete
        // 3. Spawn nuovo pezzo
        
        // Esempio basico (quello che avevi tu):
        Delete_Piece(currentX, currentY, currentPiece, currentRotation);
        currentY = 0; 
        currentX = 4;
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red);
    }

    // Reset flag Timer0
    LPC_TIM0->IR = 1;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
