#ifndef CONSOLE_H
#define CONSOLE_H

// Batas panjang username & password
#define MAX_USERNAME 50
#define MAX_PASSWORD 50

// Ukuran console aktual
extern int gCols;
extern int gRows;

// Posisi input username & password di layar
extern int gUserCol, gUserRow;
extern int gPassCol, gPassRow;

// ---------- UTIL KONSOLE ----------
void clearScreen(void);
void updateConsoleSize(void);
void gotoxy(int col, int row);
void initConsole(void);
void drawBox(int x, int y, int w, int h);

// ---------- INPUT HELPER ----------
void clearInputBuffer(void);
void inputTextAt(int col, int row, int maxLen, char *buffer, int *exitFlag);
void inputPasswordAt(int col, int row, int maxLen, char *password);

// ---------- UI / MENU ----------
void drawLoginLayout(void);
void tampilkanMenuUtama(void);

#endif // CONSOLE_H
