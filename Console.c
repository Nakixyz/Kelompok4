#include "console.h"
#include "karyawan.h"   // untuk menuKelolaKaryawan

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

// posisi input login
int gUserCol, gUserRow;
int gPassCol, gPassRow;

// posisi input pilihan menu superadmin
int gMenuPromptCol, gMenuPromptRow;

// ASCII art kecil di panel kanan menu superadmin
// ASCII art untuk panel kanan menu superadmin
static const char *gSuperAdminArt[] = {
"                         ..----.",
"                    -#### .#########",
"                  .######.########.  -###+",
"                 -##-.  +.     ##+  #########+",
"                 #-     +     .##  +#############-.",
"                -#     #.     ##-  ###### -+#########-.",
"                #-    .#     -##. #######     ##+########+.",
"               -###############+ .#######     #+   .#########.",
"               ################. +#######     #+    #.  .#####- --",
"              -################ .#########+.  #-    #.   ++  -+.#####.",
"              ##.  ######- ###  +################+. #.   ++  -+.##+######",
"              +##.-######  -#+  ########################+#+  -+.##-  +.-####.",
"              .##############  ###############################+.##-  +. -+ -+-##.",
"             +. +#########+. .################################+.##########.-+-+ ++#-",
"              ##-.         -##################################+.############+-###+--",
"              -###############################################+.############+-######",
"               ####--++#################.+..# -#+ ###### +--+-+#.+++-+###+##-######-",
"               -##+ ......-############ ### + ### ###### #++#.##.-+--#####.+. .####.",
"                ##+...-+#############+ +### # ### . ####-. ..#..##+. ..+######-",
"                .###################.-- ##..##  .#--.#####+- ..-######---+",
"                         ..          .#+--#####+-    .--+######--+++-",
"              .---+##############+---   ---.  --+##########-. .-.",
"      +#############---+####++-. .-.  -++#######+######-",
"       ####-.. ..-######-     .+################+.   .",
"        .+######-      +################-     ...",
"               -#############+      .+####-",
"                   -#+. .+########-",
"                             .-++."
};

static const int gSuperAdminArtLines =
    sizeof(gSuperAdminArt) / sizeof(gSuperAdminArt[0]);

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
        ShowWindow(h, SW_MAXIMIZE);
    }
    system("color 0b");
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

// Input teks (username) dengan deteksi '0' sebagai exit
void inputTextAt(int col, int row, int maxLen, char *buffer, int *exitFlag) {
    int i = 0;
    char ch;

    gotoxy(col, row);

    while (1) {
        ch = getch();

        if (ch == 13) {           // ENTER
            break;
        } else if (ch == '0' && i == 0) {
            *exitFlag = 1;       // keluar program
            return;
        } else if (ch == 8) {    // BACKSPACE
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (ch >= 32 && ch <= 126 && i < maxLen - 1) {
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
            for (int j = 0; j < maxLen - 1; j++) {
                printf(" ");
            }
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

// ------------- LAYOUT LOGIN -------------

void drawLoginLayout() {
    clearScreen();
    updateConsoleSize();

    int UI_W = gCols - 4;
    int UI_H = gRows - 4;
    if (UI_W < 80) UI_W = (gCols > 80) ? 80 : gCols - 2;
    if (UI_H < 20) UI_H = (gRows > 20) ? 20 : gRows - 2;

    int outerX = (gCols - UI_W) / 2 + 1;
    int outerY = (gRows - UI_H) / 2 + 1;

    drawBox(outerX, outerY, UI_W, UI_H);

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

    int contentTop = outerY + logoLines + 2;

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

    int sepX = leftX + leftW + 1;
    for (int y = contentTop - 1; y < outerY + UI_H - 1; y++) {
        gotoxy(sepX, y);
        printf("|");
    }

    gotoxy(sepX + 2, contentTop - 1);
    printf("Login >");

    int cardX = sepX + 2;
    int cardY = contentTop;
    int cardW = outerX + UI_W - 2 - cardX;
    int cardH = outerY + UI_H - 3 - cardY;
    if (cardW < 40) cardW = 40;
    if (cardH < 12) cardH = 12;
    drawBox(cardX, cardY, cardW, cardH);

    const char *welcome = "Welcome to Fourtrain";
    int welLen = (int)strlen(welcome);
    int welX = cardX + (cardW - welLen) / 2;
    gotoxy(welX, cardY + 1);
    printf("%s", welcome);

    int userLabelX = cardX + 4;
    int userLabelY = cardY + 3;
    gotoxy(userLabelX, userLabelY);
    printf("Nama Pengguna");

    int userBoxX = userLabelX;
    int userBoxY = userLabelY + 1;
    int userBoxW = cardW - 8;
    int userBoxH = 3;
    drawBox(userBoxX, userBoxY, userBoxW, userBoxH);

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

    int btnW = 20;
    int btnH = 3;
    if (btnW > cardW - 8) btnW = cardW - 8;
    int btnX = cardX + (cardW - btnW) / 2;
    int btnY = cardY + cardH - 4;
    drawBox(btnX, btnY, btnW, btnH);
    gotoxy(btnX + (btnW - 5) / 2, btnY + 1);
    printf("ENTER");

    gUserCol = userBoxX + 2;
    gUserRow = userBoxY + 1;
    gPassCol = passBoxX + 2;
    gPassRow = passBoxY + 1;
}

// ------------- LAYOUT MENU SUPERADMIN -------------

void drawSuperAdminMenuLayout() {
    clearScreen();
    updateConsoleSize();

    int UI_W = gCols - 4;
    int UI_H = gRows - 4;
    if (UI_W < 80) UI_W = (gCols > 80) ? 80 : gCols - 2;
    if (UI_H < 20) UI_H = (gRows > 20) ? 20 : gRows - 2;

    int outerX = (gCols - UI_W) / 2 + 1;
    int outerY = (gRows - UI_H) / 2 + 1;

    drawBox(outerX, outerY, UI_W, UI_H);

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

    int contentTop = outerY + logoLines + 2;

    // panel kiri
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

    gotoxy(menuX + 4, menuY + 1);
    printf("Menu");

    int mLine = menuY + 3;
    gotoxy(menuX + 2, mLine++); printf("[1] Kelola Data Penumpang");
    gotoxy(menuX + 2, mLine++); printf("[2] Kelola Data Karyawan");
    gotoxy(menuX + 2, mLine++); printf("[3] Keluar");

    int sepX = leftX + leftW + 1;
    for (int y = contentTop - 1; y < outerY + UI_H - 1; y++) {
        gotoxy(sepX, y);
        printf("|");
    }

    gotoxy(sepX + 2, contentTop - 1);
    printf("Super Admin > Menu");

    int cardX = sepX + 2;
    int cardY = contentTop;
    int cardW = outerX + UI_W - 2 - cardX;
    int cardH = outerY + UI_H - 3 - cardY;
    if (cardW < 40) cardW = 40;
    if (cardH < 12) cardH = 12;
    drawBox(cardX, cardY, cardW, cardH);

    const char *title = "Menu Utama Super Admin";
    int titleLen = (int)strlen(title);
    int titleX = cardX + (cardW - titleLen) / 2;
    gotoxy(titleX, cardY + 1);
    printf("%s", title);

    // ASCII art kecil di card kanan
    int artStartY = cardY + 3;
    for (int i = 0; i < gSuperAdminArtLines; i++) {
        int len = (int)strlen(gSuperAdminArt[i]);
        int artX = cardX + (cardW - len) / 2;
        int artY = artStartY + i;
        if (artY >= cardY + cardH - 3) break;
        gotoxy(artX, artY);
        printf("%s", gSuperAdminArt[i]);
    }

    gotoxy(outerX + 3, outerY + UI_H - 2);
    printf("Masukkan nomor menu (1-3) lalu tekan ENTER");

    const char *prompt = "Pilih menu (1-3): ";
    int promptRow = cardY + cardH - 3;
    int promptCol = cardX + 4;
    gotoxy(promptCol, promptRow);
    printf("%s", prompt);

    gMenuPromptRow = promptRow;
    gMenuPromptCol = promptCol + (int)strlen(prompt);
}

// ------------- LOOP MENU SUPERADMIN -------------

void tampilkanMenuUtama() {
    int pilihan = 0;

    do {
        drawSuperAdminMenuLayout();

        gotoxy(gMenuPromptCol, gMenuPromptRow);
        scanf("%d", &pilihan);
        clearInputBuffer();

        switch (pilihan) {
            case 1:
                clearScreen();
                printf("Kelola Data Penumpang belum diimplementasi.\n");
                printf("Tekan sembarang tombol...");
                getch();
                break;

            case 2:
                menuKelolaKaryawan();
                break;

            case 3:
                break;

            default:
                gotoxy(gMenuPromptCol, gMenuPromptRow + 2);
                printf("Pilihan tidak valid. Tekan sembarang tombol...");
                getch();
                break;
        }

    } while (pilihan != 3);
}
