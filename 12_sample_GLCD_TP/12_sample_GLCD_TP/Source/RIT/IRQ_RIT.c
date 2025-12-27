#include "LPC17xx.h"
#include "RIT.h"
#include "../Tetris_Game/tetris.h"

volatile static int gravity_counter = 0;
volatile static int down_wait = 0;
volatile static int left_wait = 0;
volatile static int right_wait = 0;
volatile static int rotate_wait = 0;

/*********************************************************************************************************
** Function name:       RIT_IRQHandler
** Description:         Gestisce SOLO gli input (Joystick)
*********************************************************************************************************/
void RIT_IRQHandler(void) {
  // --- JOYSTICK DOWN (Hard Drop) - P1.26 ---
    if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
        
        // Usiamo un wait per evitare che lo faccia 50 volte se tieni premuto
        if(down_wait == 0) {
            
            // 1. Cancella il pezzo dove si trova ora
            Delete_Piece(currentX, currentY, currentPiece, currentRotation);
            
            // 2. Calcola la posizione più bassa possibile (senza disegnare)
            // Continua a scendere finché non trovi un ostacolo
            while(Check_Collision(currentX, currentY + 1, currentPiece, currentRotation) == 0) {
                currentY++;
            }
            
            // 3. Disegna il pezzo nella nuova posizione sul fondo
            Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red);
            
            // Imposta un'attesa lunga per evitare rimbalzi
            down_wait = 20; 
        } else {
            down_wait--;
        }
    } else {
        down_wait = 0;
    }  
	// --- JOYSTICK LEFT (P1.27) ---
    if((LPC_GPIO1->FIOPIN & (1<<27)) == 0) { 
        if(left_wait == 0) {
            Move_Left(); // TUA FUNZIONE: Sposta X-- e ridisegna
            left_wait = 1; // Valore basso = movimento veloce ma controllato
        } else {
            left_wait--;
        }
    } else {
        left_wait = 0;
    }
    
    // --- JOYSTICK RIGHT (P1.28) ---
    if((LPC_GPIO1->FIOPIN & (1<<28)) == 0) {
        if(right_wait == 0) {
            Move_Right(); // TUA FUNZIONE: Sposta X++ e ridisegna
            right_wait = 1; 
        } else {
            right_wait--;
        }
    } else {
        right_wait = 0;
    }
    
    // --- JOYSTICK UP - RUOTA (P1.29) ---
    if((LPC_GPIO1->FIOPIN & (1<<29)) == 0) {
        if(rotate_wait == 0) {
            Rotate_Piece(); // TUA FUNZIONE: Ruota e ridisegna
            rotate_wait = 10; // Pausa più lunga per evitare rotazioni multiple indesiderate
        } else {
            rotate_wait--;
        }
    } else {
        rotate_wait = 0;
    }

    // Reset flag RIT
    LPC_RIT->RICTRL |= 0x1; 
}