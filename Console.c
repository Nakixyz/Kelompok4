#include "console.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ukuran console aktual
int gCols = 120;
int gRows = 30;

// posisi input
int gUserCol, gUserRow;
int gPassCol, gPassRow;

// ------------- UTIL KONSOLE -------------

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void updateConsoleSize() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &info)) {
        gCols = info.srWindow.Right  - info.srWindow.Left + 1;
        gRows = info.srWindow.Bottom - info.srWindow.Top  + 1;
    }
#else
    gCols = 120;
    gRows = 30;
#endif
}

void gotoxy(int col, int row) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = (SHORT)(col - 1);
    pos.Y = (SHORT)(row - 1);
    SetConsoleCursorPosition(hOut, pos);
#else
    printf("\033[%d;%dH", row, col);
#endif
}

void initConsole() {
#ifdef _WIN32
    HWND h = GetConsoleWindow();
    if (h != NULL) {
        ShowWindow(h, SW_MAXIMIZE);   // maximize kalau di cmd klasik
    }
    system("color 0b");               // background terang, teks gelap
#endif
    clearScreen();
    updateConsoleSize();
}

// gambar box umum
void drawBox(int x, int y, int w, int h) {
    int i;
    if (w < 2 || h < 2) return;

    gotoxy(x, y);                 printf("+");
    gotoxy(x + w - 1, y);         printf("+");
    gotoxy(x, y + h - 1);         printf("+");
    gotoxy(x + w - 1, y + h - 1); printf("+");

    for (i = x + 1; i < x + w - 1; i++) {
        gotoxy(i, y);             printf("-");
        gotoxy(i, y + h - 1);     printf("-");
    }
    for (i = y + 1; i < y + h - 1; i++) {
        gotoxy(x, i);             printf("|");
        gotoxy(x + w - 1, i);     printf("|");
    }
}

// ------------- INPUT HELPER -------------

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// Input username dengan deteksi tombol '0' untuk keluar
void inputTextAt(int col, int row, int maxLen, char *buffer, int *exitFlag) {
    int i = 0;
    char ch;

    gotoxy(col, row);

    while (1) {
        ch = getch();

        // ENTER -> selesai input
        if (ch == 13) {
            break;
        }
        // Kalau tombol pertama yang ditekan adalah '0' -> keluar program
        else if (ch == '0' && i == 0) {
            *exitFlag = 1;
            return;
        }
        // BACKSPACE
        else if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        // karakter normal
        else if (ch >= 32 && ch <= 126 && i < maxLen - 1) {
            buffer[i++] = ch;
            printf("%c", ch);
        }
    }

    buffer[i] = '\0';
}

// Input password dengan TAB = SEE/UNSEE
void inputPasswordAt(int col, int row, int maxLen, char *password) {
    int i = 0;
    char ch;
    int visible = 0;

    gotoxy(col, row);

    while (1) {
        ch = getch();

        if (ch == 13) {          // ENTER
            break;
        } else if (ch == 9) {    // TAB -> toggle
            visible = !visible;
            gotoxy(col, row);
            for (int j = 0; j < i; j++) {
                printf("%c", visible ? password[j] : '*');
            }
        } else if (ch == 8) {    // BACKSPACE
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (ch >= 32 && ch <= 126 && i < maxLen - 1) {
            password[i++] = ch;
            printf("%c", visible ? ch : '*');
        }
    }

    password[i] = '\0';
}

// ------------- MENU UTAMA -------------

void tampilkanMenuUtama() {
    int pilihan;
    do {
        clearScreen();
        gotoxy(5, 2);  printf("=== MENU UTAMA SUPER ADMIN ===");
        gotoxy(5, 4);  printf("1. Kelola User (dummy)");
        gotoxy(5, 5);  printf("2. Laporan (dummy)");
        gotoxy(5, 6);  printf("3. Setting (dummy)");
        gotoxy(5, 7);  printf("4. Keluar");
        gotoxy(5, 9);  printf("Pilih menu (1-4): ");
        scanf("%d", &pilihan);
        clearInputBuffer();
    } while (pilihan != 4);
}

// ------------- LAYOUT LOGIN (ASCII LOGO) -------------

void drawLoginLayout() {
    clearScreen();
    updateConsoleSize();

    // ukuran UI hampir penuh, sisakan margin
    int UI_W = gCols - 4;
    int UI_H = gRows - 4;
    if (UI_W < 80) UI_W = (gCols > 80) ? 80 : gCols - 2;
    if (UI_H < 20) UI_H = (gRows > 20) ? 20 : gRows - 2;

    // posisi kiri-atas supaya center
    int outerX = (gCols - UI_W) / 2 + 1;
    int outerY = (gRows - UI_H) / 2 + 1;

    // frame utama
    drawBox(outerX, outerY, UI_W, UI_H);

    // ASCII logo "FOURTRAIN"
    const char *logo[] = {
        "    ______                ______           _     ",
        "   / ____/___  __  ______/_  __/________ _(_)___ ",
        "  / /_  / __ \\/ / / / ___// / / ___/ __ `/ / __ \\",
        " / __/ / /_/ / /_/ / /   / / / /  / /_/ / / / / /",
        "/_/    \\____/\\__,_/_/   /_/ /_/   \\__,_/_/_/ /_/ "
    };
    int logoLines = 5;

    for (int i = 0; i < logoLines; i++) {
        int len = (int)strlen(logo[i]);
        int x = outerX + (UI_W - len) / 2;
        int y = outerY + 1 + i;
        gotoxy(x, y);
        printf("%s", logo[i]);
    }

    // baris pertama konten di bawah logo
    int contentTop = outerY + logoLines + 2;

    // panel kiri (menu)
    int leftW = UI_W / 4;
    if (leftW < 20) leftW = 20;
    int leftX = outerX + 2;
    int leftY = contentTop;
    int leftH = outerY + UI_H - 2 - leftY;
    if (leftH < 10) leftH = 10;
    drawBox(leftX, leftY, leftW, leftH);

    gotoxy(leftX + 3, leftY + 1);
    printf("- - Kelompok 4 - -");

    int menuX = leftX + 2;
    int menuY = leftY + 3;
    int menuW = leftW - 4;
    int menuH = leftH / 2;
    if (menuH < 8) menuH = 8;
    drawBox(menuX, menuY, menuW, menuH);
    gotoxy(menuX + 4, menuY + 1); printf("Menu");
    gotoxy(menuX + 2, menuY + 3); printf("[0] Keluar");

    // garis pemisah vertikal
    int sepX = leftX + leftW + 1;
    for (int y = contentTop - 1; y < outerY + UI_H - 1; y++) {
        gotoxy(sepX, y);
        printf("|");
    }

    // breadcrumb "Login >"
    gotoxy(sepX + 2, contentTop - 1);
    printf("Login >");

    // card login di kanan
    int cardX = sepX + 2;
    int cardY = contentTop;
    int cardW = outerX + UI_H - 2; // placeholder, lalu dihitung ulang
    cardW = outerX + UI_W - 2 - cardX;
    int cardH = outerY + UI_H - 3 - cardY;
    if (cardW < 40) cardW = 40;
    if (cardH < 12) cardH = 12;
    drawBox(cardX, cardY, cardW, cardH);

    const char *welcome = "Welcome to Fourtrain";
    int welLen = (int)strlen(welcome);
    int welX = cardX + (cardW - welLen) / 2;
    gotoxy(welX, cardY + 1);
    printf("%s", welcome);

    // username
    int userLabelX = cardX + 4;
    int userLabelY = cardY + 3;
    gotoxy(userLabelX, userLabelY);
    printf("Nama Pengguna");

    int userBoxX = userLabelX;
    int userBoxY = userLabelY + 1;
    int userBoxW = cardW - 8;
    int userBoxH = 3;
    drawBox(userBoxX, userBoxY, userBoxW, userBoxH);

    // password
    int passLabelX = cardX + 4;
    int passLabelY = userBoxY + 4;
    gotoxy(passLabelX, passLabelY);
    printf("Kata Sandi");

    int passBoxX = passLabelX;
    int passBoxY = passLabelY + 1;
    int passBoxW = cardW - 8;
    int passBoxH = 3;
    drawBox(passBoxX, passBoxY, passBoxW, passBoxH);

    gotoxy(outerX + 3, outerY + UI_H - 2);
    printf("Tekan TAB untuk melihat password");

    // tombol ENTER
    int btnW = 20;
    int btnH = 3;
    if (btnW > cardW - 8) btnW = cardW - 8;
    int btnX = cardX + (cardW - btnW) / 2;
    int btnY = cardY + cardH - 4;
    drawBox(btnX, btnY, btnW, btnH);
    gotoxy(btnX + (btnW - 5) / 2, btnY + 1);
    printf("ENTER");

    // posisi input
    gUserCol = userBoxX + 2;
    gUserRow = userBoxY + 1;
    gPassCol = passBoxX + 2;
    gPassRow = passBoxY + 1;
}
