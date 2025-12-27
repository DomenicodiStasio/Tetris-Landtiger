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
extern volatile int currentPiece;    // Indice del pezzo (0-6)
extern volatile int currentRotation; // 0, 1, 2, 3
extern volatile int currentX;        // Posizione X sulla griglia
extern volatile int currentY;        // Posizione Y sulla griglia


/* --- PROTOTIPI --- */
void Tetris_Init(void);
void Draw_Block(int row, int col, uint16_t color);

void Draw_Piece(int x, int y, int type, int rotation, uint16_t color);
int Check_Collision(int x, int y, int type, int rotation);
void Delete_Piece(int x, int y, int type, int rotation);

void Move_Left(void);
void Move_Right(void);
void Rotate_Piece(void);

// Inizializzazione
void Reset_Board(void);

void Lock_Piece(int x, int y, int piece, int rotation);

// Gestione Righe e Aggiornamento
void Check_Lines(void);
void Update_Field(void);

#endif