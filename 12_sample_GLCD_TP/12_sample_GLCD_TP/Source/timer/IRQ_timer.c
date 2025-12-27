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

extern void Lock_Piece(int x, int y, int piece, int rotation);
extern void Check_Lines(void);
extern int Check_Collision(int x, int y, int piece, int rotation);
extern void Draw_Piece(int x, int y, int piece, int rotation, uint16_t color);
extern void Delete_Piece(int x, int y, int piece, int rotation);

// Variabili globali del gioco
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
    // 1. Prova a spostare il pezzo verso il BASSO
    if (Check_Collision(currentX, currentY + 1, currentPiece, currentRotation) == 0) {
        
        // Se c'è spazio: cancella vecchio -> aggiorna Y -> disegna nuovo
        Delete_Piece(currentX, currentY, currentPiece, currentRotation);
        currentY++;
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red); // O il colore che vuoi
    
    } else {
        // 2. SE TOCCA IL FONDO (o un altro pezzo)
        
        // A. Blocca il pezzo nella matrice (Board)
        Lock_Piece(currentX, currentY, currentPiece, currentRotation);
        
        // B. Controlla se abbiamo fatto righe piene (e aggiorna lo schermo)
        Check_Lines();
        
        // C. Genera il prossimo pezzo (Spawn)
        currentX = 4; // Centro
        currentY = 0; // Cima
        currentRotation = 0;
        
        // Qui dovresti scegliere un pezzo a caso.
        // Per ora facciamo un ciclo semplice: 0->1->2...->6->0
        currentPiece++;
        if(currentPiece > 6) currentPiece = 0;
        
        // D. Controllo GAME OVER
        // Se appena nato sbatte già contro qualcosa, il gioco è finito
        if (Check_Collision(currentX, currentY, currentPiece, currentRotation) != 0) {
            
            // FINE DEL GIOCO
            LCD_Clear(Red);            // Schermo rosso morte
            GUI_Text(100, 150, (uint8_t *)"GAME OVER", White, Red);
            
            // Blocca tutto in un loop infinito o resetta
            disable_timer(0); 
            return;
        }
        
        // E. Disegna il nuovo pezzo appena nato
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red);
    }

    // Reset del flag interrupt Timer0
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
