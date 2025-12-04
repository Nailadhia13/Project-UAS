#include <curses.h>
#include <iostream>
#include <fstream>
#include <cstdlib> //untuk fungsi umum kek rand() dan srand()
#include <ctime> //untuk fungsi ngatur waktu
using namespace std;

// ================== variabel keseluruhan game ==================
const int LEBAR = 40;
const int TINGGI = 20;

int ularX[100], ularY[100]; // posisi setiap bagian tubuh uler
int panjangUlar = 3;

int makanX, makanY;
int skor = 0;
int skorTertinggi = 0;

int arahX = 1, arahY = 0; // arah awal ke kanan

int nyawa = 3;


// ================== function untuk buat file text / nyimpen skor ==================
void muatSkorTertinggi() {
    ifstream file("skortertinggi.txt");
    if (file.is_open()) {
        file >> skorTertinggi;
        file.close();
    }
}

void simpanSkorTertinggi() {
    ofstream file("skortertinggi.txt");
    if (file.is_open()) {
        file << skorTertinggi;
        file.close();
    }
}

// ================== pause game ==================
void pauseGame() {
    mvprintw(TINGGI + 2, 0, "[PAUSED] Tekan 'P' lagi untuk lanjut...");
    refresh();

    int tombol;
    while (true) {
        tombol = getch();
        if (tombol == 'p' || tombol == 'P') {
            mvprintw(TINGGI + 2, 0, "                                   ");
            refresh();
            break;
        }
    }
}


// ================== setup game ==================
void mulaiGame() {
    panjangUlar = 3;
    skor = 0;

    // posisi awal tubuh uler
    for (int i = 0; i < panjangUlar; i++) {
        ularX[i] = 10 - i;
        ularY[i] = 10;
    }

    // spawn makanan
    makanX = rand() % LEBAR;
    makanY = rand() % TINGGI;
}


// ================== gambar map ==================
void gambar() {
    clear();

    // dinding
    for (int i = 0; i <= LEBAR; i++) {
        mvprintw(0, i, "#");
        mvprintw(TINGGI, i, "#");
    }
    for (int i = 0; i <= TINGGI; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, LEBAR, "#");
    }

    // gambar uler (kepala = @, badan = o)
    for (int i = 0; i < panjangUlar; i++) {
        mvprintw(ularY[i], ularX[i], (i == 0 ? "@" : "o"));
    }

    // gambar makanan
    mvprintw(makanY, makanX, "*");

    // nganuin skor
    mvprintw(TINGGI + 1, 0, "Skor : %d  Skor Tertinggi: %d", skor, skorTertinggi);

    refresh();
}


// ================== control input ==================
void input() {
    timeout(100); // kecepatan gerakan uler
    int tombol = getch();

    if (tombol == 'p' || tombol == 'P') { pauseGame(); return; } // tekan P buat pause

    switch (tombol) {
        case KEY_UP: if (arahY != 1) { arahX = 0; arahY = -1; } break;
        case KEY_DOWN: if (arahY != -1) { arahX = 0; arahY = 1; } break;
        case KEY_LEFT: if (arahX != 1) { arahX = -1; arahY = 0; } break;
        case KEY_RIGHT: if (arahX != -1) { arahX = 1; arahY = 0; } break;
    }
}


// ================== kehilangan nyawa (kalo nabrak dinding / badan) ==================
bool kehilanganNyawa() {
    nyawa--;
    if (nyawa > 0) {
        // tampilkan pesan singkat
        mvprintw(TINGGI + 2, 0, "Anda kehilangan nyawa! Sisa nyawa: %d", nyawa);
        refresh();
        napms(1000); // delay 1 detik (ncurses)
        mvprintw(TINGGI + 2, 0, "                                       ");
        refresh();
        // reset posisi ular (tetap pertahankan skor & skor tertinggi)
        mulaiGame();
        return true; // lanjutkan game
    } else {
        // nyawa habis -> game over
        return false;
    }
}


// ================== function logika gerakan si uler ==================
bool updateGame() {
    // ini buat ngegeser biar tu tubuh uler ngikutin kepalanya pergi 
    for (int i = panjangUlar - 1; i > 0; i--) {
        ularX[i] = ularX[i - 1];
        ularY[i] = ularY[i - 1];
    }

    // gerakin kepala ulernya
    ularX[0] += arahX;
    ularY[0] += arahY;

    // kalo nabrak dinding ngurangin nyawa 
    if (ularX[0] <= 0 || ularX[0] >= LEBAR || ularY[0] <= 0 || ularY[0] >= TINGGI) {
        return false;
    }

    // kalo nabrak badan sendiri ngurangin nyawa uga
    for (int i = 1; i < panjangUlar; i++) {
        if (ularX[0] == ularX[i] && ularY[0] == ularY[i]) {
            bool masihHidup = kehilanganNyawa();
            return masihHidup;
        }
    }

    // makan makanan
    if (ularX[0] == makanX && ularY[0] == makanY) {
        skor++;
        panjangUlar++;

        // spawn makanan baru
        makanX = rand() % (LEBAR - 1) + 1;
        makanY = rand() % (TINGGI - 1) + 1;
    }

    return true; // lanjutin gamenya
}


// ================== program utamanya ini ==================
int main() {
    srand(time(0));

    muatSkorTertinggi();

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    mulaiGame();

    // looping game yang utama
    while (true) {
        gambar();
        input();

        if (!updateGame()) break;
    }

    // nyimpen skor tertinggi
    if (skor > skorTertinggi) {
        skorTertinggi = skor;
        simpanSkorTertinggi();
    }

    clear();
    mvprintw(10, 10, "GAME OVER!");
    mvprintw(11, 10, "Skor Anda : %d", skor);
    mvprintw(12, 10, "Skor Tertinggi : %d", skorTertinggi);
    mvprintw(14, 10, "Tekan tombol apapun untuk keluar!!");
    getch();

    endwin();
    return 0;
} 