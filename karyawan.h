#ifndef KARYAWAN_H
#define KARYAWAN_H

#define MAX_KARYAWAN 100

typedef struct {
    char id_karyawan[11];    // VARCHAR(10) + '\0'
    char nama_karyawan[51];  // VARCHAR(50) + '\0'
    char email[101];         // VARCHAR(100) + '\0'
    int  no_telp;            // INT
    char jabatan[21];        // VARCHAR(20) + '\0'
} Karyawan;

// Inisialisasi modul karyawan (load dari file)
void initKaryawan(void);

// Menu CRUD karyawan (CREATE, READ, UPDATE, DELETE)
void menuKelolaKaryawan(void);

#endif // KARYAWAN_H
