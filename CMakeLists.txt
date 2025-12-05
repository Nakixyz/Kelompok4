cmake_minimum_required(VERSION 4.0)
project(Tampilan_Login_Super_Admin C)

set(CMAKE_C_STANDARD 23)

add_executable(Tampilan_Login_Super_Admin main.c
        Console.c
        Console.h
        karyawan.c
        karyawan.h
)
