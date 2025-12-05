#ifndef CONSOLE_H
#define CONSOLE_H

// Ukuran console aktual
extern int gCols;
extern int gRows;

// Posisi input login
extern int gUserCol;
extern int gUserRow;
extern int gPassCol;
extern int gPassRow;

// Posisi input pilihan menu superadmin
extern int gMenuPromptCol;
extern int gMenuPromptRow;

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

// ---------- UI ----------
void drawLoginLayout(void);             // tampilan login
void drawSuperAdminMenuLayout(void);    // tampilan menu superadmin
void tampilkanMenuUtama(void);          // loop menu superadmin

#endif // CONSOLE_H
