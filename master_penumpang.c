#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ===================== KONFIG =====================
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define SUPERADMIN_USER "superadmin"
#define SUPERADMIN_PASS "superadmin"
#define COLOR_THEME "color 0B"

// ===================== GLOBAL UKURAN CONSOLE =====================
static int gCols = 120;
static int gRows = 30;

// ===================== UTIL CONSOLE =====================
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void updateConsoleSize() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &info)) {
        gCols = info.srWindow.Right  - info.srWindow.Left + 1;
        gRows = info.srWindow.Bottom - info.srWindow.Top  + 1;
    } else {
        gCols = 120; gRows = 30;
    }
#else
    gCols = 120; gRows = 30;
#endif
}

static void gotoxy(int col, int row) {
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

static void hideCursor(int hide) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hOut, &ci);
    ci.bVisible = hide ? FALSE : TRUE;
    SetConsoleCursorInfo(hOut, &ci);
#else
    (void)hide;
#endif
}

static void initConsole() {
#ifdef _WIN32
    HWND h = GetConsoleWindow();
    if (h) ShowWindow(h, SW_MAXIMIZE);
    system(COLOR_THEME);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hIn, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode |= ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(hIn, mode);
#endif
    clearScreen();
    updateConsoleSize();
    hideCursor(0);
}

static void clearArea(int x, int y, int w, int h) {
    for (int r = 0; r < h; r++) {
        gotoxy(x, y + r);
        for (int c = 0; c < w; c++) putchar(' ');
    }
}

static void drawBox(int x, int y, int w, int h) {
    int i;
    if (w < 2 || h < 2) return;

    gotoxy(x, y); putchar('+');
    gotoxy(x + w - 1, y); putchar('+');
    gotoxy(x, y + h - 1); putchar('+');
    gotoxy(x + w - 1, y + h - 1); putchar('+');

    for (i = x + 1; i < x + w - 1; i++) {
        gotoxy(i, y); putchar('-');
        gotoxy(i, y + h - 1); putchar('-');
    }
    for (i = y + 1; i < y + h - 1; i++) {
        gotoxy(x, i); putchar('|');
        gotoxy(x + w - 1, i); putchar('|');
    }
}

static void printCentered(int x, int w, int y, const char *text) {
    int len = (int)strlen(text);
    int start = x + (w - len) / 2;
    gotoxy(start, y);
    printf("%s", text);
}

static void inputLineAt(int col, int row, char *buf, int maxLen) {
    gotoxy(col, row);
    for (int i = 0; i < maxLen - 1; i++) putchar(' ');
    gotoxy(col, row);
    fgets(buf, maxLen, stdin);
    buf[strcspn(buf, "\n")] = '\0';
}

static void inputPasswordAt(int col, int row, int maxLen, char *password) {
    int i = 0, visible = 0;
    char ch;
    gotoxy(col, row);
    while (1) {
        ch = getch();
        if (ch == 13) break;
        else if (ch == 9) {
            visible = !visible;
            gotoxy(col, row);
            for (int j = 0; j < i; j++)
                putchar(visible ? password[j] : '*');
        }
        else if (ch == 8) {
            if (i > 0) {
                i--;
                gotoxy(col + i, row); putchar(' ');
                gotoxy(col + i, row);
            }
        }
        else if (ch >= 32 && ch <= 126 && i < maxLen - 1) {
            password[i++] = ch;
            putchar(visible ? ch : '*');
        }
    }
    password[i] = '\0';
}

// ===================== DATA KERETA =====================
typedef struct {
    char id[16];
    char nama[64];
    char kelas[8];
    int  kapasitas;
    char status[16];
    char rute[80];
} Kereta;

static Kereta gKereta[100];
static int gKeretaN = 0;

static int findKeretaIndexById(const char *id) {
    for (int i = 0; i < gKeretaN; i++)
        if (strcmp(gKereta[i].id, id) == 0) return i;
    return -1;
}

static void seedKereta() {
    if (gKeretaN > 0) return;
    Kereta a[] = {
        {"KR001","Argo Parahyangan","BIS",360,"AKTIF","Bandung-Gambir"},
        {"KR002","Lodaya","EKO",720,"AKTIF","Bandung-Solo Balapan"},
        {"KR003","Mutiara Selatan","EKO",720,"AKTIF","Bandung-Malang"},
        {"KR004","Serayu","EKO",864,"AKTIF","Pasar Senen-Purwokerto"},
        {"KR005","Taksaka","BIS",360,"AKTIF","Yogyakarta-Gambir"}
    };
    for (int i = 0; i < 5; i++)
        gKereta[gKeretaN++] = a[i];
}

// ===================== DATA PENUMPANG =====================
typedef struct {
    char id[16];
    char nama[64];
    char nik[32];
    char nohp[32];
    char email[64];
} Penumpang;

static Penumpang gPenumpang[200];
static int gPenumpangN = 0;

static int findPenumpangIndexById(const char *id) {
    for (int i = 0; i < gPenumpangN; i++)
        if (strcmp(gPenumpang[i].id, id) == 0) return i;
    return -1;
}

static void seedPenumpang() {
    if (gPenumpangN > 0) return;

    Penumpang p[] = {
        {"PN001","Andi Prasetyo","3201011111","0812341001","andi@mail.com"},
        {"PN002","Budi Santoso","3201011112","0812341002","budi@mail.com"},
        {"PN003","Citra Dewi","3201011113","0812341003","citra@mail.com"},
        {"PN004","Dewi Lestari","3201011114","0812341004","dewi@mail.com"},
        {"PN005","Eka Pramana","3201011115","0812341005","eka@mail.com"},
        {"PN006","Fajar Nugraha","3201011116","0812341006","fajar@mail.com"},
        {"PN007","Gita Marlina","3201011117","0812341007","gita@mail.com"},
        {"PN008","Hanif Setiawan","3201011118","0812341008","hanif@mail.com"},
        {"PN009","Indah Purwati","3201011119","0812341009","indah@mail.com"},
        {"PN010","Joko Prayitno","3201011120","0812341010","joko@mail.com"},
        {"PN011","Kiki Rizky","3201011121","0812341011","kiki@mail.com"},
        {"PN012","Lala Septiani","3201011122","0812341012","lala@mail.com"},
        {"PN013","Maman Firmansyah","3201011123","0812341013","maman@mail.com"},
        {"PN014","Nina Amelia","3201011124","0812341014","nina@mail.com"},
        {"PN015","Oki Handoko","3201011125","0812341015","oki@mail.com"},
        {"PN016","Putri Ayu","3201011126","0812341016","putri@mail.com"},
        {"PN017","Qori Rahma","3201011127","0812341017","qori@mail.com"},
        {"PN018","Rian Hidayat","3201011128","0812341018","rian@mail.com"},
        {"PN019","Sari Ramadhani","3201011129","0812341019","sari@mail.com"},
        {"PN020","Tono Suharjo","3201011130","0812341020","tono@mail.com"}
    };

    int n = sizeof(p)/sizeof(p[0]);
    for (int i = 0; i < n; i++) gPenumpang[gPenumpangN++] = p[i];
}

// ===================== UI LAYOUT =====================
static int OUT_X, OUT_Y, OUT_W, OUT_H;
static int LEFT_X, LEFT_Y, LEFT_W, LEFT_H;
static int RIGHT_X, RIGHT_Y, RIGHT_W, RIGHT_H;

static void layoutBaseFrame() {
    clearScreen();
    updateConsoleSize();

    OUT_X = 1; OUT_Y = 1;
    OUT_W = gCols; OUT_H = gRows;
    drawBox(OUT_X, OUT_Y, OUT_W, OUT_H);

    LEFT_W = OUT_W / 4;
    if (LEFT_W < 26) LEFT_W = 26;
    if (LEFT_W > 34) LEFT_W = 34;

    LEFT_X = OUT_X + 2;
    LEFT_Y = OUT_Y + 5;
    LEFT_H = OUT_H - 7;

    RIGHT_X = LEFT_X + LEFT_W + 2;
    RIGHT_Y = OUT_Y + 5;
    RIGHT_W = OUT_X + OUT_W - 2 - RIGHT_X;
    RIGHT_H = OUT_H - 7;

    printCentered(OUT_X, OUT_W, OUT_Y + 2, "FOURTRAIN - SUPERADMIN");

    gotoxy(OUT_X + 2, OUT_Y + 3);
    for (int i = 0; i < OUT_W - 4; i++) putchar('-');

    drawBox(LEFT_X, LEFT_Y, LEFT_W, LEFT_H);
    drawBox(RIGHT_X, RIGHT_Y, RIGHT_W, RIGHT_H);

    gotoxy(LEFT_X + 2, LEFT_Y + 1); printf("- - Kelompok 4 - -");
    gotoxy(LEFT_X + 2, LEFT_Y + 3); printf("Menu");
    gotoxy(LEFT_X + 2, LEFT_Y + 5); printf("[A] Tambah");
    gotoxy(LEFT_X + 2, LEFT_Y + 6); printf("[E] Edit");
    gotoxy(LEFT_X + 2, LEFT_Y + 7); printf("[H] Hapus");
    gotoxy(LEFT_X + 2, LEFT_Y + 8); printf("[S] Search");
    gotoxy(LEFT_X + 2, LEFT_Y + 10); printf("[ESC] Kembali");
}

static void clearRightPanel() {
    clearArea(RIGHT_X + 1, RIGHT_Y + 1, RIGHT_W - 2, RIGHT_H - 2);
    drawBox(RIGHT_X, RIGHT_Y, RIGHT_W, RIGHT_H);
}

static void statusMessage(const char *msg) {
    int msgY = RIGHT_Y + RIGHT_H - 2;
    clearArea(RIGHT_X + 2, msgY, RIGHT_W - 4, 1);
    gotoxy(RIGHT_X + 3, msgY);
    printf("%s", msg);
}

// ===================== TAMPILAN KERETA =====================
static void rightHeaderKereta() {
    printCentered(RIGHT_X, RIGHT_W, RIGHT_Y + 1, "MASTER KERETA");
    gotoxy(RIGHT_X + 2, RIGHT_Y + 2);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');
}

static void drawTableKereta() {
    int x = RIGHT_X + 2;
    int y = RIGHT_Y + 4;

    gotoxy(x, y);
    printf("ID     | Nama Kereta                 | Kls | Kap  | Status  | Rute");

    gotoxy(x, y+1);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');

    int rows = RIGHT_H - 8;
    int line = 0;

    for (int i = 0; i < gKeretaN && line < rows; i++, line++) {
        gotoxy(x, y + 2 + line);
        printf("%-6s | %-26s | %-3s | %-4d | %-7s | %-30s",
               gKereta[i].id,
               gKereta[i].nama,
               gKereta[i].kelas,
               gKereta[i].kapasitas,
               gKereta[i].status,
               gKereta[i].rute);
    }

    gotoxy(RIGHT_X + 2, RIGHT_Y + RIGHT_H - 4);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');
}

// ===================== CRUD KERETA =====================
static void runTambahKereta() {
    clearRightPanel();
    rightHeaderKereta();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char id[16], nama[64], kelas[16], kap[16], status[16], rute[80];

    gotoxy(x, y); printf("Tambah Kereta");
    gotoxy(x, y+3); printf("ID Kereta (KRxxx) :");
    inputLineAt(inCol, y+3, id, sizeof(id));
    if (!id[0]) return;

    if (findKeretaIndexById(id) != -1) { statusMessage("ID sudah ada!"); getch(); return; }

    gotoxy(x, y+5); printf("Nama Kereta       :");
    gotoxy(x, y+7); printf("Kelas (EKO/BIS)   :");
    gotoxy(x, y+9); printf("Kapasitas         :");
    gotoxy(x, y+11); printf("Status            :");
    gotoxy(x, y+13); printf("Rute              :");

    inputLineAt(inCol, y+5, nama, sizeof(nama));
    inputLineAt(inCol, y+7, kelas, sizeof(kelas));
    inputLineAt(inCol, y+9, kap, sizeof(kap));
    inputLineAt(inCol, y+11, status, sizeof(status));
    inputLineAt(inCol, y+13, rute, sizeof(rute));

    Kereta k;
    strncpy(k.id, id, 15);
    strncpy(k.nama, nama, 63);
    strncpy(k.kelas, kelas, 7);
    k.kapasitas = atoi(kap);
    strncpy(k.status, status, 15);
    strncpy(k.rute, rute, 79);

    gKereta[gKeretaN++] = k;
    statusMessage("Berhasil tambah!");
    getch();
}

static void runEditKereta() {
    clearRightPanel();
    rightHeaderKereta();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char id[16], buf[128];

    gotoxy(x, y); printf("Edit Kereta");
    gotoxy(x, y+3); printf("ID Kereta         :");
    inputLineAt(inCol, y+3, id, sizeof(id));
    if (!id[0]) return;

    int idx = findKeretaIndexById(id);
    if (idx == -1) { statusMessage("Tidak ditemukan!"); getch(); return; }

    Kereta *k = &gKereta[idx];

    gotoxy(x, y+5); printf("Nama Kereta       :");
    gotoxy(x, y+7); printf("Kelas             :");
    gotoxy(x, y+9); printf("Kapasitas         :");
    gotoxy(x, y+11); printf("Status            :");
    gotoxy(x, y+13); printf("Rute              :");

    inputLineAt(inCol, y+5, buf, sizeof(buf)); if (buf[0]) strncpy(k->nama, buf, 63);
    inputLineAt(inCol, y+7, buf, sizeof(buf)); if (buf[0]) strncpy(k->kelas, buf, 7);
    inputLineAt(inCol, y+9, buf, sizeof(buf)); if (buf[0]) k->kapasitas = atoi(buf);
    inputLineAt(inCol, y+11, buf, sizeof(buf)); if (buf[0]) strncpy(k->status, buf, 15);
    inputLineAt(inCol, y+13, buf, sizeof(buf)); if (buf[0]) strncpy(k->rute, buf, 79);

    statusMessage("Berhasil edit!");
    getch();
}

static void runHapusKereta() {
    clearRightPanel();
    rightHeaderKereta();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char id[16];

    gotoxy(x, y); printf("Hapus Kereta");
    gotoxy(x, y+3); printf("ID Kereta         :");
    inputLineAt(inCol, y+3, id, sizeof(id));
    if (!id[0]) return;

    int idx = findKeretaIndexById(id);
    if (idx == -1) { statusMessage("Tidak ditemukan!"); getch(); return; }

    gotoxy(x, y+5);
    printf("Yakin hapus %s? (Y/N): ", id);
    int c = toupper(getch());
    if (c == 'Y') {
        for (int i = idx; i < gKeretaN - 1; i++)
            gKereta[i] = gKereta[i + 1];
        gKeretaN--;
        statusMessage("Berhasil hapus!");
    }
    getch();
}

static void runSearchKereta() {
    clearRightPanel();
    rightHeaderKereta();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char key[64];

    gotoxy(x, y); printf("Search Kereta");
    gotoxy(x, y+3); printf("Kata kunci        :");

    inputLineAt(inCol, y+3, key, sizeof(key));
    if (!key[0]) return;

    gotoxy(x, y+5); printf("Hasil:");
    gotoxy(x, y+6);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');

    int line = 0;
    for (int i = 0; i < gKeretaN; i++) {
        if (strstr(gKereta[i].id, key) || strstr(gKereta[i].nama, key)) {
            gotoxy(x, y + 7 + line);
            printf("%s | %s | %s | %d | %s | %s",
                   gKereta[i].id, gKereta[i].nama, gKereta[i].kelas,
                   gKereta[i].kapasitas, gKereta[i].status, gKereta[i].rute);
            line++;
        }
    }

    if (!line) {
        gotoxy(x, y+7); printf("(Tidak ada hasil)");
    }

    getch();
}

// ===================== TAMPILAN PENUMPANG =====================
static void rightHeaderPenumpang() {
    printCentered(RIGHT_X, RIGHT_W, RIGHT_Y + 1, "MASTER PENUMPANG");
    gotoxy(RIGHT_X + 2, RIGHT_Y + 2);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');
}

static void drawTablePenumpang() {
    int x = RIGHT_X + 2;
    int y = RIGHT_Y + 4;

    gotoxy(x, y);
    printf("ID     | Nama Lengkap              | NIK          | No HP       | Email");

    gotoxy(x, y+1);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');

    int line = 0, rows = RIGHT_H - 8;

    for (int i = 0; i < gPenumpangN && line < rows; i++, line++) {
        gotoxy(x, y + 2 + line);
        printf("%-6s | %-24s | %-12s | %-11s | %-30s",
               gPenumpang[i].id,
               gPenumpang[i].nama,
               gPenumpang[i].nik,
               gPenumpang[i].nohp,
               gPenumpang[i].email);
    }

    gotoxy(RIGHT_X + 2, RIGHT_Y + RIGHT_H - 4);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');
}

// ===================== CRUD PENUMPANG =====================
static void runTambahPenumpang() {
    clearRightPanel();
    rightHeaderPenumpang();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;

    char id[16], nama[64], nik[32], hp[32], email[64];

    gotoxy(x, y); printf("Tambah Penumpang");
    gotoxy(x, y+3); printf("ID Penumpang (PNxxx) :");
    inputLineAt(inCol, y+3, id, sizeof(id));
    if (!id[0]) return;

    if (findPenumpangIndexById(id) != -1) { statusMessage("ID sudah ada!"); getch(); return; }

    gotoxy(x, y+5); printf("Nama Lengkap        :");
    gotoxy(x, y+7); printf("NIK                 :");
    gotoxy(x, y+9); printf("No HP               :");
    gotoxy(x, y+11); printf("Email               :");

    inputLineAt(inCol, y+5, nama, sizeof(nama));
    inputLineAt(inCol, y+7, nik, sizeof(nik));
    inputLineAt(inCol, y+9, hp, sizeof(hp));
    inputLineAt(inCol, y+11, email, sizeof(email));

    Penumpang p;
    strncpy(p.id, id, 15);
    strncpy(p.nama, nama, 63);
    strncpy(p.nik, nik, 31);
    strncpy(p.nohp, hp, 31);
    strncpy(p.email, email, 63);

    gPenumpang[gPenumpangN++] = p;
    statusMessage("Berhasil tambah!");
    getch();
}

static void runEditPenumpang() {
    clearRightPanel();
    rightHeaderPenumpang();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char id[16], buf[128];

    gotoxy(x, y); printf("Edit Penumpang");
    gotoxy(x, y+3); printf("ID Penumpang        :");
    inputLineAt(inCol, y+3, id, sizeof(id));
    if (!id[0]) return;

    int idx = findPenumpangIndexById(id);
    if (idx == -1) { statusMessage("Tidak ditemukan!"); getch(); return; }

    Penumpang *p = &gPenumpang[idx];

    gotoxy(x, y+5); printf("Nama Lengkap        :");
    gotoxy(x, y+7); printf("NIK                 :");
    gotoxy(x, y+9); printf("No HP               :");
    gotoxy(x, y+11); printf("Email               :");

    inputLineAt(inCol, y+5, buf, sizeof(buf)); if (buf[0]) strncpy(p->nama, buf, 63);
    inputLineAt(inCol, y+7, buf, sizeof(buf)); if (buf[0]) strncpy(p->nik, buf, 31);
    inputLineAt(inCol, y+9, buf, sizeof(buf)); if (buf[0]) strncpy(p->nohp, buf, 31);
    inputLineAt(inCol, y+11, buf, sizeof(buf)); if (buf[0]) strncpy(p->email, buf, 63);

    statusMessage("Berhasil edit!");
    getch();
}

static void runHapusPenumpang() {
    clearRightPanel();
    rightHeaderPenumpang();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char id[16];

    gotoxy(x, y); printf("Hapus Penumpang");
    gotoxy(x, y+3); printf("ID Penumpang        :");
    inputLineAt(inCol, y+3, id, sizeof(id));
    if (!id[0]) return;

    int idx = findPenumpangIndexById(id);
    if (idx == -1) { statusMessage("Tidak ditemukan!"); getch(); return; }

    gotoxy(x, y+5);
    printf("Yakin hapus %s? (Y/N): ", id);
    int c = toupper(getch());

    if (c == 'Y') {
        for (int i = idx; i < gPenumpangN - 1; i++)
            gPenumpang[i] = gPenumpang[i + 1];
        gPenumpangN--;
        statusMessage("Berhasil hapus!");
    }
    getch();
}

static void runSearchPenumpang() {
    clearRightPanel();
    rightHeaderPenumpang();

    int x = RIGHT_X + 2, y = RIGHT_Y + 4, inCol = x + 22;
    char key[64];

    gotoxy(x, y); printf("Search Penumpang");
    gotoxy(x, y+3); printf("Kata kunci          :");

    inputLineAt(inCol, y+3, key, sizeof(key));
    if (!key[0]) return;

    gotoxy(x, y+5); printf("Hasil:");
    gotoxy(x, y+6);
    for (int i = 0; i < RIGHT_W - 4; i++) putchar('-');

    int line = 0;
    for (int i = 0; i < gPenumpangN; i++) {
        if (strstr(gPenumpang[i].id, key) || strstr(gPenumpang[i].nama, key)) {
            gotoxy(x, y + 7 + line);
            printf("%s | %s | %s | %s | %s",
                   gPenumpang[i].id, gPenumpang[i].nama,
                   gPenumpang[i].nik, gPenumpang[i].nohp, gPenumpang[i].email);
            line++;
        }
    }

    if (!line) {
        gotoxy(x, y+7); printf("(Tidak ada hasil)");
    }

    getch();
}

// ===================== HALAMAN MASTER =====================
static void masterKeretaPage() {
    seedKereta();
    layoutBaseFrame();
    rightHeaderKereta();
    drawTableKereta();

    while (1) {
        int c = toupper(getch());
        if (c == 27) break;

        switch (c) {
            case 'A': runTambahKereta(); break;
            case 'E': runEditKereta(); break;
            case 'H': runHapusKereta(); break;
            case 'S': runSearchKereta(); break;
            default: statusMessage("Gunakan A/E/H/S atau ESC."); break;
        }

        clearRightPanel();
        rightHeaderKereta();
        drawTableKereta();
    }
}

static void masterPenumpangPage() {
    seedPenumpang();
    layoutBaseFrame();
    rightHeaderPenumpang();
    drawTablePenumpang();

    while (1) {
        int c = toupper(getch());
        if (c == 27) break;

        switch (c) {
            case 'A': runTambahPenumpang(); break;
            case 'E': runEditPenumpang(); break;
            case 'H': runHapusPenumpang(); break;
            case 'S': runSearchPenumpang(); break;
            default: statusMessage("Gunakan A/E/H/S atau ESC."); break;
        }

        clearRightPanel();
        rightHeaderPenumpang();
        drawTablePenumpang();
    }
}

// ===================== MENU UTAMA =====================
static void menuUtama() {
    while (1) {
        clearScreen();
        updateConsoleSize();
        drawBox(2, 2, gCols - 3, gRows - 3);

        printCentered(2, gCols - 3, 4, "MENU UTAMA SUPER ADMIN");

        gotoxy(6, 7);  printf("1. Master Kereta");
        gotoxy(6, 8);  printf("2. Master Penumpang");
        gotoxy(6, 9);  printf("0. Logout");

        gotoxy(6, 11); printf("Pilihan: ");
        int c = getch();

        if (c == '1') masterKeretaPage();
        else if (c == '2') masterPenumpangPage();
        else if (c == '0') break;
    }
}

// ===================== LOGIN =====================
static int gUserCol, gUserRow, gPassCol, gPassRow;

static void drawLoginLayout() {
    clearScreen();
    updateConsoleSize();

    int UI_W = gCols - 4;
    int UI_H = gRows - 4;

    int outerX = (gCols - UI_W) / 2 + 1;
    int outerY = (gRows - UI_H) / 2 + 1;

    drawBox(outerX, outerY, UI_W, UI_H);

    int cardX = outerX + 20;
    int cardY = outerY + 6;
    int cardW = UI_W - 40;
    int cardH = UI_H - 12;

    drawBox(cardX, cardY, cardW, cardH);
    printCentered(cardX, cardW, cardY + 1, "Welcome to FOURTRAIN");

    gotoxy(cardX + 4, cardY + 4); printf("Nama Pengguna");
    drawBox(cardX + 4, cardY + 5, cardW - 8, 3);

    gotoxy(cardX + 4, cardY + 9); printf("Kata Sandi");
    drawBox(cardX + 4, cardY + 10, cardW - 8, 3);

    gUserCol = cardX + 6;
    gUserRow = cardY + 6;
    gPassCol = cardX + 6;
    gPassRow = cardY + 11;
}

// ===================== MAIN =====================
int main() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    initConsole();
    int attempts = 0;

    while (attempts < 3) {
        drawLoginLayout();

        gotoxy(gUserCol, gUserRow);
        inputLineAt(gUserCol, gUserRow, username, sizeof(username));
        if (strcmp(username, "0") == 0) return 0;

        gotoxy(gPassCol, gPassRow);
        inputPasswordAt(gPassCol, gPassRow, sizeof(password), password);

        if (strcmp(username, SUPERADMIN_USER) == 0 &&
            strcmp(password, SUPERADMIN_PASS) == 0) {
            gotoxy(gUserCol, gPassRow + 3);
            printf("Login berhasil! Tekan tombol apa saja...");
            getch();
            menuUtama();
            break;
        } else {
            attempts++;
            gotoxy(gUserCol, gPassRow + 3);
            printf("Login gagal (%d/3). Tekan tombol...", attempts);
            getch();
        }
    }

    clearScreen();
    return 0;
}
