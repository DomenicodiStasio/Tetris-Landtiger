#include "tetris.h"

// Matrice logica del campo di gioco
// 0 = vuoto, 1 = occupato (in futuro useremo l'ID del colore)
volatile int board[ROWS][COLS]; 

// Variabili globali di gioco
int currentPiece = 0;
int currentRotation = 0;
int currentX = 0;
int currentY = 0;

// Definizione dei 7 pezzi con le 4 rotazioni
// Formato: { {x1,y1}, {x2,y2}, {x3,y3}, {x4,y4} }
const int TETROMINOES[7][4][4][2] = {
    // I (Linea)
    {
        {{0, 1}, {1, 1}, {2, 1}, {3, 1}}, // Rotazione 0
        {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, // Rotazione 1
        {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, // Rotazione 2
        {{1, 0}, {1, 1}, {1, 2}, {1, 3}}  // Rotazione 3
    },
    // J
    {
        {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
        {{1, 0}, {1, 1}, {0, 2}, {1, 2}}
    },
    // L
    {
        {{2, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {0, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {1, 2}}
    },
    // O (Quadrato - non ruota davvero, ma ripetiamo per semplicità)
    {
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}}
    },
    // S
    {
        {{1, 0}, {2, 0}, {0, 1}, {1, 1}},
        {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
        {{1, 1}, {2, 1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
    },
    // T
    {
        {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {2, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
        {{1, 0}, {0, 1}, {1, 1}, {1, 2}}
    },
    // Z
    {
        {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
        {{2, 0}, {1, 1}, {2, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
        {{1, 0}, {0, 1}, {1, 1}, {0, 2}}
    }
};

void Tetris_Init(void) {
    int i, j;
    
    // 1. Pulisce lo schermo
    LCD_Clear(Black); 
    
    // 2. Inizializza la matrice vuota
    for(i=0; i<ROWS; i++){
        for(j=0; j<COLS; j++){
            board[i][j] = 0;
        }
    }
    
    // 3. Disegna i bordi dell'area di gioco (Linee bianche)
    // Sinistra
    for(i=0; i < (ROWS*BLOCK_SIZE); i++) 
        LCD_SetPoint(BOARD_X - 1, BOARD_Y + i, White);
    // Destra
    for(i=0; i < (ROWS*BLOCK_SIZE); i++) 
        LCD_SetPoint(BOARD_X + (COLS*BLOCK_SIZE), BOARD_Y + i, White);
    // Fondo
    for(i=0; i <= (COLS*BLOCK_SIZE); i++) 
        LCD_SetPoint(BOARD_X + i -1, BOARD_Y + (ROWS*BLOCK_SIZE), White);
}

// Funzione per disegnare un "quadratino" di Tetris
void Draw_Block(int row, int col, uint16_t color) {
    int x_start = BOARD_X + (col * BLOCK_SIZE);
    int y_start = BOARD_Y + (row * BLOCK_SIZE);
    int i, j;

    // Disegna un quadrato pieno
    for(i = 0; i < BLOCK_SIZE - 1; i++) {       // -1 per lasciare un bordino nero
        for(j = 0; j < BLOCK_SIZE - 1; j++) {
            LCD_SetPoint(x_start + j, y_start + i, color);
        }
    }
}

void Draw_Piece(int x, int y, int type, int rotation, uint16_t color) {
    int i;
    int blockX, blockY;
    
    // Un pezzo è fatto da 4 blocchi: cicliamo su ognuno
    for(i = 0; i < 4; i++) {
        // Calcoliamo la posizione assoluta sulla griglia
        // TETROMINOES[tipo][rotazione][n_blocco][0 = coordinata X]
        blockX = x + TETROMINOES[type][rotation][i][0]; 
        blockY = y + TETROMINOES[type][rotation][i][1];
        
        // Disegniamo il singolo blocco
        Draw_Block(blockY, blockX, color); // Occhio: Draw_Block vuole (row, col) -> (y, x)
    }
}