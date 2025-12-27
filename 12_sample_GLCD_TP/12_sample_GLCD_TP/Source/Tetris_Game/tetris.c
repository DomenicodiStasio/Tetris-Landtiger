#include "tetris.h"

// Matrice logica del campo di gioco
// 0 = vuoto, 1 = occupato (in futuro useremo l'ID del colore)
volatile int board[ROWS][COLS]; 

// Variabili globali di gioco
volatile int currentPiece = 0;
volatile int currentRotation = 0;
volatile int currentX = 0;
volatile int currentY = 0;

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

// Restituisce 1 se c'è collisione (mossa non valida), 0 se è tutto OK
int Check_Collision(int x, int y, int type, int rotation) {
    int i;
    int blockX, blockY;
    
    for(i = 0; i < 4; i++) {
        // Coordinate assolute del singolo blocco
        blockX = x + TETROMINOES[type][rotation][i][0];
        blockY = y + TETROMINOES[type][rotation][i][1];
        
        // 1. Uscita dai bordi laterali (Destra/Sinistra)
        if (blockX < 0 || blockX >= COLS) return 1;
        
        // 2. Uscita dal fondo (Il pezzo è arrivato giù)
        if (blockY >= ROWS) return 1;
        
        // 3. Collisione con altri pezzi già fissati nella griglia
        // (Nota: controlliamo board solo se blockY >= 0, perché i pezzi nascono un po' fuori in alto)
        if (blockY >= 0 && board[blockY][blockX] != 0) return 1;
    }
    return 0; // Nessuna collisione
}

void Delete_Piece(int x, int y, int type, int rotation) {
    Draw_Piece(x, y, type, rotation, Black);
}

// Funzioni di movimento
void Move_Left(void) {
    // Controllo: Posso andare a sinistra (currentX - 1)?
    if (Check_Collision(currentX - 1, currentY, currentPiece, currentRotation) == 0) {
        Delete_Piece(currentX, currentY, currentPiece, currentRotation);
        currentX--; // Sposto a sinistra
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red); // Ridisegno (per ora rosso fisso)
    }
}

void Move_Right(void) {
    if (Check_Collision(currentX + 1, currentY, currentPiece, currentRotation) == 0) {
        Delete_Piece(currentX, currentY, currentPiece, currentRotation);
        currentX++;
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red);
    }
}

void Rotate_Piece(void) {
    // Calcola la prossima rotazione (0 -> 1 -> 2 -> 3 -> 0)
    int nextRotation = (currentRotation + 1) % 4;
    
    if (Check_Collision(currentX, currentY, currentPiece, nextRotation) == 0) {
        Delete_Piece(currentX, currentY, currentPiece, currentRotation);
        currentRotation = nextRotation;
        Draw_Piece(currentX, currentY, currentPiece, currentRotation, Red);
    }
}

void Lock_Piece(int x, int y, int piece, int rotation) {
    int k;
    for(k = 0; k < 4; k++) {
        // Leggi le coordinate relative del k-esimo blocchetto
        int relX = TETROMINOES[piece][rotation][k][0];
        int relY = TETROMINOES[piece][rotation][k][1];
        
        // Calcola posizione assoluta nella Board
        int boardX = x + relX;
        int boardY = y + relY;
        
        // Salva nella Board (se dentro i limiti)
        if(boardY >= 0 && boardY < 20 && boardX >= 0 && boardX < 10) {
            board[boardY][boardX] = 1; // 1 = Occupato
        }
    }
}

void Update_Field(void) {
    int i, j;
    
    // Scorre tutte le celle della Board (20 righe x 10 colonne)
    for(i = 0; i < 20; i++) {
        for(j = 0; j < 10; j++) {
            
            if(board[i][j] != 0) {
                // Se c'è un blocco (occupato), disegnalo VERDE (o il colore che vuoi per i pezzi morti)
                // Nota: La tua funzione accetta (row, col), quindi passiamo (i, j)
                Draw_Block(i, j, Green); 
            } else {
                // Se è vuoto, disegna NERO per cancellare eventuali residui
                Draw_Block(i, j, Black);
            }
        }
    }
}

void Check_Lines(void) {
    int i, j, k;
    int count;
    int lines_cleared = 0;

    // Scansioniamo dalla riga più bassa (19) a quella più alta (0)
    for(i = 19; i >= 0; i--) {
        count = 0;
        
        // Conta quanti blocchi ci sono in questa riga
        for(j = 0; j < 10; j++) {
            if(board[i][j] != 0) {
                count++;
            }
        }

        // Se la riga è PIENA (10 blocchi)
        if(count == 10) {
            lines_cleared++; // Hai fatto punto!

            // 1. Sposta tutte le righe sopra di questa verso il basso
            for(k = i; k > 0; k--) {
                for(j = 0; j < 10; j++) {
                    board[k][j] = board[k-1][j];
                }
            }

            // 2. Pulisci la riga 0 (la cima del campo ora è vuota)
            for(j = 0; j < 10; j++) {
                board[0][j] = 0;
            }

            // 3. IMPORTANTE: Rimaniamo sulla stessa riga 'i' al prossimo giro!
            // Perché ora alla riga 'i' c'è il contenuto che prima era a 'i-1'.
            // Potrebbe essere un'altra riga piena (es. hai fatto Tetris, 4 righe insieme).
            i++; 
        }
    }

    // Se abbiamo cancellato qualcosa, ridisegniamo il campo
    if(lines_cleared > 0) {
        Update_Field(); 
        
        // Qui puoi aggiungere punti se hai una variabile score
        // score += 100 * lines_cleared;
    }
}