#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#include "karyawan.h"
#include "console.h"   // pakai clearScreen, clearInputBuffer, drawBox, gotoxy, gCols/gRows

#define KARYAWAN_FILE "karyawan.dat"

// storage in-memory
static Karyawan karyawanList[MAX_KARYAWAN];
static int karyawanCount = 0;

/* =========================================================================
   ASCII ART: KERETA untuk panel kanan MENU Kelola Data Karyawan
   ========================================================================= */

static const char *gKaryawanTrainArt[] = {
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

static const int gKaryawanTrainArtLines =
    sizeof(gKaryawanTrainArt) / sizeof(gKaryawanTrainArt[0]);

/* =========================================================================
   ASCII ART: TABEL HEADER untuk fitur READ (Lihat Data Karyawan)
   ========================================================================= */

static const char *gKaryawanTableArt[] = {
"+----------------------------------------------------------------------------------------------+",
"|                                   TABEL DATA KARYAWAN                                       |",
"+-----------------+----------------------+---------------------------+------------+------------+",
"| ID_Karyawan     | Nama_Karyawan        | Email                     | No_Telp    | Jabatan    |",
"+-----------------+----------------------+---------------------------+------------+------------+"
};

static const int gKaryawanTableArtLines =
    sizeof(gKaryawanTableArt) / sizeof(gKaryawanTableArt[0]);

/* ============================== HELPER UMUM ============================== */

// baca satu baris string (boleh spasi), hapus '\n' di akhir
static void inputLine(const char *label, char *buffer, int maxLen) {
    printf("%s: ", label);
    if (fgets(buffer, maxLen, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

static void pauseScreen() {
    printf("\nTekan sembarang tombol untuk melanjutkan...");
    getch();
}

// cari index by ID, -1 jika tidak ada
static int findKaryawanIndexById(const char *id) {
    for (int i = 0; i < karyawanCount; ++i) {
        if (strcmp(karyawanList[i].id_karyawan, id) == 0) {
            return i;
        }
    }
    return -1;
}

/* ============================ FILE PROCESSING =========================== */

static void loadFromFile() {
    FILE *f = fopen(KARYAWAN_FILE, "rb");
    if (!f) {
        karyawanCount = 0;
        return;
    }

    karyawanCount = 0;
    Karyawan temp;
    while (karyawanCount < MAX_KARYAWAN &&
           fread(&temp, sizeof(Karyawan), 1, f) == 1) {
        karyawanList[karyawanCount++] = temp;
    }

    fclose(f);
}

static void saveToFile() {
    FILE *f = fopen(KARYAWAN_FILE, "wb");
    if (!f) {
        printf("Gagal membuka file %s untuk menulis.\n", KARYAWAN_FILE);
        pauseScreen();
        return;
    }

    fwrite(karyawanList, sizeof(Karyawan), karyawanCount, f);
    fclose(f);
}

// dipanggil dari main sekali di awal
void initKaryawan() {
    loadFromFile();
}

/* =============================== CRUD =================================== */

// CREATE
static void createKaryawan() {
    if (karyawanCount >= MAX_KARYAWAN) {
        clearScreen();
        printf("Data karyawan penuh (maks %d).\n", MAX_KARYAWAN);
        pauseScreen();
        return;
    }

    clearScreen();
    printf("=== TAMBAH DATA KARYAWAN ===\n\n");

    Karyawan k;

    inputLine("ID Karyawan (max 10)", k.id_karyawan, sizeof(k.id_karyawan));
    if (strlen(k.id_karyawan) == 0) {
        printf("ID tidak boleh kosong.\n");
        pauseScreen();
        return;
    }

    if (findKaryawanIndexById(k.id_karyawan) != -1) {
        printf("ID sudah digunakan, data tidak disimpan.\n");
        pauseScreen();
        return;
    }

    inputLine("Nama Karyawan (max 50)", k.nama_karyawan, sizeof(k.nama_karyawan));
    inputLine("Email (max 100)", k.email, sizeof(k.email));

    printf("No. Telp (angka saja): ");
    scanf("%d", &k.no_telp);
    clearInputBuffer();

    inputLine("Jabatan (max 20)", k.jabatan, sizeof(k.jabatan));

    karyawanList[karyawanCount++] = k;
    saveToFile();

    printf("\nData karyawan berhasil ditambahkan.\n");
    pauseScreen();
}

// READ (LIST) – sekarang pakai layout tabel + frame
static void listKaryawan() {
    clearScreen();
    updateConsoleSize();

    int UI_W = gCols - 4;
    int UI_H = gRows - 4;
    if (UI_W < 80) UI_W = (gCols > 80) ? 80 : gCols - 2;
    if (UI_H < 20) UI_H = (gRows > 20) ? 20 : gRows - 2;

    int outerX = (gCols - UI_W) / 2 + 1;
    int outerY = (gRows - UI_H) / 2 + 1;

    // frame utama
    drawBox(outerX, outerY, UI_W, UI_H);

    // logo di atas
    const char *logo[] = {
        "    ______                ______           _     ",
        "   / ____/___  __  ______/_  __/________ _(_)___ ",
        "  / /_  / __ \\/ / / / ___// / / ___/ __ `/ / __ \\",
        " / __/ / /_/ / /_/ / /   / / / /  / /_/ / / / / /",
        "/_/    \\____/\\__,_/_/   /_/ /_/   \\__,_/_/_/ /_/ "
    };
    int logoLines = 5;
    for (int i = 0; i < logoLines; ++i) {
        int len = (int)strlen(logo[i]);
        int x = outerX + (UI_W - len) / 2;
        int y = outerY + 1 + i;
        gotoxy(x, y);
        printf("%s", logo[i]);
    }

    int contentTop = outerY + logoLines + 2;

    // breadcrumb
    gotoxy(outerX + 2, contentTop - 1);
    printf("Super Admin > Kelola Karyawan > Lihat Data");

    // card untuk tabel
    int cardX = outerX + 2;
    int cardY = contentTop;
    int cardW = UI_W - 4;
    int cardH = outerY + UI_H - 3 - cardY;
    if (cardW < 40) cardW = 40;
    if (cardH < 10) cardH = 10;
    drawBox(cardX, cardY, cardW, cardH);

    const char *title = "Daftar Data Karyawan";
    int titleLen = (int)strlen(title);
    int titleX = cardX + (cardW - titleLen) / 2;
    gotoxy(titleX, cardY + 1);
    printf("%s", title);

    int row = cardY + 3;

    if (karyawanCount == 0) {
        gotoxy(cardX + 4, row + 1);
        printf("Belum ada data karyawan.");
    } else {
        // gambar header tabel (gKaryawanTableArt)
        for (int i = 0; i < gKaryawanTableArtLines; ++i) {
            int len = (int)strlen(gKaryawanTableArt[i]);
            int x = cardX + (cardW - len) / 2;
            int y = row + i;
            gotoxy(x, y);
            printf("%s", gKaryawanTableArt[i]);
        }

        // posisi awal data di bawah header
        int borderLen = (int)strlen(gKaryawanTableArt[2]);   // garis +----+
        int tableX = cardX + (cardW - borderLen) / 2;
        int dataStartY = row + gKaryawanTableArtLines + 1;

        // cetak setiap baris data dengan format kolom yang match header
        for (int i = 0; i < karyawanCount; ++i) {
            int y = dataStartY + i;
            gotoxy(tableX, y);
            printf("| %-15s | %-20s | %-25s | %-10d | %-10s |",
                   karyawanList[i].id_karyawan,
                   karyawanList[i].nama_karyawan,
                   karyawanList[i].email,
                   karyawanList[i].no_telp,
                   karyawanList[i].jabatan);
        }

        // garis bawah tabel (pakai border yang sama)
        int bottomY = dataStartY + karyawanCount;
        gotoxy(tableX, bottomY);
        printf("+-----------------+----------------------+---------------------------+------------+------------+");
    }

    // hint bawah
    gotoxy(outerX + 3, outerY + UI_H - 2);
    printf("Tekan sembarang tombol untuk kembali ke menu karyawan");

    getch();
}

// UPDATE
static void updateKaryawan() {
    if (karyawanCount == 0) {
        clearScreen();
        printf("Belum ada data karyawan untuk diupdate.\n");
        pauseScreen();
        return;
    }

    char id[11];
    clearScreen();
    printf("=== UBAH DATA KARYAWAN ===\n\n");
    inputLine("Masukkan ID Karyawan yang akan diubah", id, sizeof(id));

    int idx = findKaryawanIndexById(id);
    if (idx == -1) {
        printf("\nData dengan ID '%s' tidak ditemukan.\n", id);
        pauseScreen();
        return;
    }

    Karyawan *k = &karyawanList[idx];

    printf("\nData saat ini:\n");
    printf("ID      : %s\n", k->id_karyawan);
    printf("Nama    : %s\n", k->nama_karyawan);
    printf("Email   : %s\n", k->email);
    printf("No.Telp : %d\n", k->no_telp);
    printf("Jabatan : %s\n\n", k->jabatan);

    printf("Masukkan data baru (kosongkan untuk tetap memakai yang lama):\n\n");

    char buffer[101];

    // Nama
    inputLine("Nama Karyawan baru", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(k->nama_karyawan, buffer, sizeof(k->nama_karyawan));
        k->nama_karyawan[sizeof(k->nama_karyawan) - 1] = '\0';
    }

    // Email
    inputLine("Email baru", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(k->email, buffer, sizeof(k->email));
        k->email[sizeof(k->email) - 1] = '\0';
    }

    // No Telp
    printf("No. Telp baru (kosongkan jika tidak diubah): ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (buffer[0] != '\n' && buffer[0] != '\0') {
            int telp;
            if (sscanf(buffer, "%d", &telp) == 1) {
                k->no_telp = telp;
            }
        }
    }

    // Jabatan
    inputLine("Jabatan baru", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(k->jabatan, buffer, sizeof(k->jabatan));
        k->jabatan[sizeof(k->jabatan) - 1] = '\0';
    }

    saveToFile();

    printf("\nData karyawan berhasil diupdate.\n");
    pauseScreen();
}

// DELETE
static void deleteKaryawan() {
    if (karyawanCount == 0) {
        clearScreen();
        printf("Belum ada data karyawan untuk dihapus.\n");
        pauseScreen();
        return;
    }

    char id[11];
    clearScreen();
    printf("=== HAPUS DATA KARYAWAN ===\n\n");
    inputLine("Masukkan ID Karyawan yang akan dihapus", id, sizeof(id));

    int idx = findKaryawanIndexById(id);
    if (idx == -1) {
        printf("\nData dengan ID '%s' tidak ditemukan.\n", id);
        pauseScreen();
        return;
    }

    printf("\nData yang akan dihapus:\n");
    printf("ID      : %s\n", karyawanList[idx].id_karyawan);
    printf("Nama    : %s\n", karyawanList[idx].nama_karyawan);
    printf("Email   : %s\n", karyawanList[idx].email);
    printf("No.Telp : %d\n", karyawanList[idx].no_telp);
    printf("Jabatan : %s\n\n", karyawanList[idx].jabatan);

    printf("Yakin hapus? (y/n): ");
    int c = getchar();
    clearInputBuffer();

    if (c == 'y' || c == 'Y') {
        for (int i = idx; i < karyawanCount - 1; ++i) {
            karyawanList[i] = karyawanList[i + 1];
        }
        karyawanCount--;
        saveToFile();
        printf("\nData karyawan berhasil dihapus.\n");
    } else {
        printf("\nPenghapusan dibatalkan.\n");
    }

    pauseScreen();
}

/* ======================= LAYOUT MENU KELOLA KARYAWAN ===================== */

// gambar UI full-screen untuk menu kelola karyawan, mirip menu super admin
// mengembalikan posisi prompt input lewat outPromptRow & outPromptCol
static void drawKaryawanMenuLayout(int *outPromptRow, int *outPromptCol) {
    clearScreen();
    updateConsoleSize();

    int UI_W = gCols - 4;
    int UI_H = gRows - 4;
    if (UI_W < 80) UI_W = (gCols > 80) ? 80 : gCols - 2;
    if (UI_H < 20) UI_H = (gRows > 20) ? 20 : gRows - 2;

    int outerX = (gCols - UI_W) / 2 + 1;
    int outerY = (gRows - UI_H) / 2 + 1;

    // frame utama
    drawBox(outerX, outerY, UI_W, UI_H);

    // logo FOURTRAIN di atas
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

    // panel kiri: menu CRUD karyawan
    int leftW = UI_W / 4;
    if (leftW < 20) leftW = 20;
    int leftX = outerX + 2;
    int leftY = contentTop;
    int leftH = outerY + UI_H - 2 - leftY;
    if (leftH < 10) leftH = 10;
    drawBox(leftX, leftY, leftW, leftH);

    gotoxy(leftX + 3, leftY + 1);
    printf("- - Karyawan - -");

    int menuX = leftX + 2;
    int menuY = leftY + 3;
    int menuW = leftW - 4;
    int menuH = leftH / 2;
    if (menuH < 8) menuH = 8;
    drawBox(menuX, menuY, menuW, menuH);

    gotoxy(menuX + 4, menuY + 1);
    printf("Menu");

    int mLine = menuY + 3;
    gotoxy(menuX + 2, mLine++); printf("[1] Tambah Data Karyawan");
    gotoxy(menuX + 2, mLine++); printf("[2] Lihat Data Karyawan");
    gotoxy(menuX + 2, mLine++); printf("[3] Ubah Data Karyawan");
    gotoxy(menuX + 2, mLine++); printf("[4] Hapus Data Karyawan");
    gotoxy(menuX + 2, mLine++); printf("[5] Kembali");

    // garis pemisah vertikal
    int sepX = leftX + leftW + 1;
    for (int y = contentTop - 1; y < outerY + UI_H - 1; y++) {
        gotoxy(sepX, y);
        printf("|");
    }

    // breadcrumb
    gotoxy(sepX + 2, contentTop - 1);
    printf("Super Admin > Kelola Karyawan");

    // card kanan
    int cardX = sepX + 2;
    int cardY = contentTop;
    int cardW = outerX + UI_W - 2 - cardX;
    int cardH = outerY + UI_H - 3 - cardY;
    if (cardW < 40) cardW = 40;
    if (cardH < 12) cardH = 12;
    drawBox(cardX, cardY, cardW, cardH);

    const char *title = "Kelola Data Karyawan";
    int titleLen = (int)strlen(title);
    int titleX = cardX + (cardW - titleLen) / 2;
    gotoxy(titleX, cardY + 1);
    printf("%s", title);

    // ASCII ART KERETA di panel kanan (bukan tabel lagi)
    int artStartY = cardY + 3;
    for (int i = 0; i < gKaryawanTrainArtLines; i++) {
        int len = (int)strlen(gKaryawanTrainArt[i]);
        int artX = cardX + (cardW - len) / 2;
        int artY = artStartY + i;
        if (artY >= cardY + cardH - 3) break;
        gotoxy(artX, artY);
        printf("%s", gKaryawanTrainArt[i]);
    }

    // hint di bawah frame utama
    gotoxy(outerX + 3, outerY + UI_H - 2);
    printf("Masukkan nomor menu (1-5) lalu tekan ENTER");

    // prompt input di dalam card kanan
    const char *prompt = "Pilih menu (1-5): ";
    int promptRow = cardY + cardH - 3;
    int promptCol = cardX + 4;
    gotoxy(promptCol, promptRow);
    printf("%s", prompt);

    if (outPromptRow) *outPromptRow = promptRow;
    if (outPromptCol) *outPromptCol = promptCol + (int)strlen(prompt);
}

/* ====================== MENU KELOLA KARYAWAN (PUBLIC) ==================== */

void menuKelolaKaryawan() {
    int pilih = 0;

    do {
        int promptRow, promptCol;
        drawKaryawanMenuLayout(&promptRow, &promptCol);

        gotoxy(promptCol, promptRow);
        scanf("%d", &pilih);
        clearInputBuffer();

        switch (pilih) {
            case 1:
                createKaryawan();
                break;
            case 2:
                listKaryawan();      // sekarang menampilkan tabel + data
                break;
            case 3:
                updateKaryawan();
                break;
            case 4:
                deleteKaryawan();
                break;
            case 5:
                // kembali ke menu super admin
                break;
            default:
                gotoxy(promptCol, promptRow + 2);
                printf("Pilihan tidak valid. Tekan sembarang tombol...");
                getch();
                break;
        }

    } while (pilih != 5);
}
