#include <curses.h>
#include <iostream>
#include <fstream>
#include <cstdlib> //untuk fungsi umum kek rand() dan srand()
#include <ctime> //untuk fungsi ngatur waktu
using namespace std;

// ================== VARIABEL GLOBALNYAA ==================

const int LEBAR = 40; // untuk arena game
const int TINGGI = 20; // ini juga

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
 	makanX = (rand() % (LEBAR - 2)) + 1;
	makanY = (rand() % (TINGGI - 2)) + 1;

}

// ================== gambar map ==================

void gambar() {
    erase();

   // DINDING ATAS & BAWAH (HORIZONTAL)
	attron(COLOR_PAIR(1));
	for (int x = 0; x <= LEBAR; x++) {
    	mvprintw(0, x, "#");
    	mvprintw(TINGGI, x, "#");
	}
	attroff(COLOR_PAIR(1));

// DINDING KIRI & KANAN (VERTIKAL)
	attron(COLOR_PAIR(1));
	for (int y = 0; y <= TINGGI; y++) {
   		mvprintw(y, 0, "#");
    	mvprintw(y, LEBAR, "#");
	}	
	attroff(COLOR_PAIR(1));


    // gambar uler (kepala = $, badan = o)
    for (int i = 0; i < panjangUlar; i++) {
		attron(COLOR_PAIR(2));
		mvprintw(ularY[i], ularX[i], (i == 0 ? "$" : "o"));
		attroff(COLOR_PAIR(2));
    }

    // gambar makanan
	attron(COLOR_PAIR(3));
	mvprintw(makanY, makanX, "*");
	attroff(COLOR_PAIR(3));

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
            bool masihHidup = kehilanganNyawa(); //Panggil kehilanganNyawa() dan simpan hasilnya (true/false) ke variabel masihHidup
            return masihHidup;
        }
    }

    // makan makanan
	if (ularX[0] == makanX && ularY[0] == makanY) {
    skor++;
    
    // update skor tertinggi kalo dapet skor yg lebih tinggi
    if (skor > skorTertinggi) {
        skorTertinggi = skor;
        simpanSkorTertinggi(); // supaya langsung tersimpan ke file
    }

    panjangUlar++;

    // spawn mamaman baru
    makanX = rand() % (LEBAR - 1) + 1;
    makanY = rand() % (TINGGI - 1) + 1;
}

    return true; // lanjutin gamenya
}
//===================function untuk tampiln game over================
void gameOverScreen() {
	timeout(-1);
    clear();
    mvprintw(TINGGI/2 - 1, (LEBAR/2) - 5, "GAME OVER!");
    mvprintw(TINGGI/2, (LEBAR/2) - 12, "Tekan tombol apa saja untuk keluar...");
    refresh();
    getch(); // tunggu pemain pencet tombol
}

// ================== program utamanya ini ==================
int main() {
    srand(time(0));

    muatSkorTertinggi();

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    
    if (has_colors()) {
    start_color();
    use_default_colors();

    init_pair(1, COLOR_GREEN, -1);   // dinding IJOKK soft
    init_pair(2, COLOR_BLACK, -1);   // ular hitam
    init_pair(3, COLOR_BLUE, -1);    // makanan biru
}

    mulaiGame();

    // looping game yang utama
    while (true) {
        input();                    // baca tombol

        if (!updateGame()) break;   // kalau false = game over

        gambar();                   // gambar setelah update
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
    
    refresh();
    timeout(-1);

    getch();

    endwin();
    return 0;
}

