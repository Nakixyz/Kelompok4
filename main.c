#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "Console.h"

#define SUPERADMIN_USER "superadmin"
#define SUPERADMIN_PASS "superadmin"

int main() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 0;
    const int maxAttempts = 3;
    int berhasil = 0;
    int exitProgram = 0;   // flag kalau user tekan 0 di login

    initConsole();

    while (attempts < maxAttempts && !berhasil && !exitProgram) {
        drawLoginLayout();

        // username
        gotoxy(gUserCol, gUserRow);
        printf("                                        ");
        gotoxy(gUserCol, gUserRow);

        // input username dengan deteksi tombol '0' untuk keluar
        inputTextAt(gUserCol, gUserRow, MAX_USERNAME, username, &exitProgram);

        // Kalau user menekan '0' → keluar dari loop
        if (exitProgram) {
            break;
        }

        // password
        gotoxy(gPassCol, gPassRow);
        printf("                                        ");
        inputPasswordAt(gPassCol, gPassRow, MAX_PASSWORD, password);

        if (strcmp(username, SUPERADMIN_USER) == 0 &&
            strcmp(password, SUPERADMIN_PASS) == 0) {
            gotoxy(gUserCol, gPassRow + 3);
            printf("Login berhasil. Tekan sembarang tombol...");
            getch();
            berhasil = 1;
            } else {
                attempts++;
                gotoxy(gUserCol, gPassRow + 3);
                printf("Login gagal (%d/%d). Tekan sembarang tombol...",
                       attempts, maxAttempts);
                getch();
            }
    }

    if (exitProgram) {
        clearScreen();
        gotoxy(5, 5);
        printf("Keluar dari program dengan menekan 0.\n");
        return 0;
    }

    if (!berhasil) {
        clearScreen();
        gotoxy(5, 5);
        printf("Anda gagal login %d kali. Program selesai.\n", maxAttempts);
        return 0;
    }

    tampilkanMenuUtama();
    clearScreen();
    return 0;
}
