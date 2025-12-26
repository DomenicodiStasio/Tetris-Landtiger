#ifndef TETRIS_H
#define TETRIS_H

#include "LPC17xx.h"
#include "../GLCD/GLCD.h"

/* --- CONFIGURAZIONE GRIGLIA --- */
#define ROWS 20
#define COLS 10
#define BLOCK_SIZE 12   
#define BOARD_X 60      
#define BOARD_Y 10 

/* --- PEZZI TETRIS --- */
// 7 Pezzi, 4 Rotazioni, 4 Blocchi per pezzo, 2 Coordinate (X, Y)
// Usiamo 'extern' perché la definizione vera sarà nel .c
extern const int TETROMINOES[7][4][4][2]; 

// Variabili di stato del gioco
extern int currentPiece;    // Indice del pezzo (0-6)
extern int currentRotation; // 0, 1, 2, 3
extern int currentX;        // Posizione X sulla griglia
extern int currentY;        // Posizione Y sulla griglia

/* --- NUOVI PROTOTIPI --- */
void Draw_Piece(int x, int y, int type, int rotation, uint16_t color);

/* --- PROTOTIPI --- */
void Tetris_Init(void);
void Draw_Block(int row, int col, uint16_t color);

#endif