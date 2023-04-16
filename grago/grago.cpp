#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<stdio.h>
#include"conio2.h"
#include <windows.h>

#define ENTER 13
#define BACKSPACE 8
#define ESC 27

//wymiary konsoli
#define WIDTH 120
#define HEIGHT 30

//pozycja x legendy
#define X_LEGEND 3
#define Y_LEGEND 6

//ASCII znaku 0
#define ZERO 48

//kody ASCII do stworzenia planszy
#define LEWY_GORNY 218
#define PRAWY_GORNY 191
#define GORNY 194
#define LEWY_DOLNY 192
#define PRAWY_DOLNY 217
#define DOLNY 193
#define LEWY 195
#define PRAWY 180
#define SRODEK 197

//wspolrzedne poczatka planszy
#define X_PLANSZY 60
#define Y_PLANSZY 6

#define MAX_PLANSZA_Y 25
#define MAX_PLANSZA_X 61

//struktura opisujaca stan gry
struct game {
	//tworzymy pusta tablice na znaki planszy i tablicy do dodawania kamieni
	int BOARD_SIZE = 0;
	char** empty_board = NULL; //tablica z pusta plansza
	char** current_board = NULL; //tablica po kazdym ruchu
	char** previous_board = NULL; //tablica z plansza ruch wczesniej, do zasady ko
	char queue = 'o'; //o-kolej czarnego, x-kolej bialego
	int x = X_PLANSZY, y = Y_PLANSZY;
	int black_score = 0;
	int white_score = 0;
};

void DrawLegend(const game* g); //funkcja wyrysowuje menu z lewej strony
void FreeData(game* g); //funkcja czysci pamiec dynamiczna i przywraca wartosci poczatkowe
void SetBoards(game* g); //funkcja wyrysowuje plansze z prawej strony
void DrawBoard(const game* g); //funkcja rysujaca plansze
void Move(const char code, game* g); //funkcja przesuwa kursor sprawdzajac, czy nie wyjechal poza plansze 
char ChangeQueue(char queue); //funkcja zmieniajaca kolejke miedzy graczami
int CheckForcing(game* g, int** chain); //funkcja sprawdza, czy zbito pionka
bool KoRule(game* g, char** temp, int score); //funkcja sprawdzajaca zasade ko
void CheckChain(const game* g, int** chain, int y, int x, char team);
void DeleteChain(const game* g, int** chain); //funkcja zeruje tablice z lancuchem
bool PutStone(game* g); //funkcja ustawia kamyk na zadane pole
void WriteFileName(char* nazwa); //funkcja pozwala na wpisanie nazwy pliku
void SaveGame(char* nazwa, game* g); //funkcja zapisuje stan gry
bool LoadGame(char* nazwa, game* g); //funkcja odczytuje plik
bool ChooseBoardSize(int* size); //funkcja ustawia rozmiar planszy
bool Handicap(game* g); //funkcja dodajaca czarnym kamienie
char Akcja(); // zatwierdzanie akcji enter lub cofniecie escape

// !!!! POCZATEK PROGRAMU !!!!! 
int main() {
	game obecna_gra;
	int attr = 7, back = 0, zero = 0;
	int zn = 0;
	char txt[32];

#ifndef __cplusplus
	Conio2_Init();
#endif

	settitle("Hubert, Malinowski");
	textbackground(BLACK);
	textcolor(WHITE);

	do {
		_setcursortype(_NOCURSOR);

		DrawLegend(&obecna_gra);
		if (obecna_gra.BOARD_SIZE == 0) {
			ChooseBoardSize(&(obecna_gra.BOARD_SIZE));
			clrscr();
			SetBoards(&obecna_gra);
			Handicap(&obecna_gra);
			clrscr();
			_setcursortype(_NOCURSOR);
			DrawLegend(&obecna_gra);
		}

		// print out the code of the last key pressed
		if (zero) sprintf_s(txt, "key code: 0x00 0x%02x", zn);
		else sprintf_s(txt, "key code: 0x%02x", zn);
		gotoxy(X_LEGEND, Y_LEGEND + 12);
		cputs(txt);
		textbackground(DARKGRAY);
		textcolor(LIGHTMAGENTA);
		DrawBoard(&obecna_gra);
		if (obecna_gra.queue == 'o') textcolor(BLACK);
		if (obecna_gra.queue == 'x') textcolor(WHITE);
		int y = (obecna_gra.y < HEIGHT) ? obecna_gra.y : HEIGHT;
		int x = (obecna_gra.x < WIDTH) ? obecna_gra.x : WIDTH;
		gotoxy(x, y);
		putch(obecna_gra.queue);
		textbackground(BLACK);
		textcolor(WHITE);
		
		zero = 0;
		zn = getch();
		if (zn == 'n') {
			zn = Akcja();
			if (zn == ENTER) FreeData(&obecna_gra);
			clrscr();
		}

		if (zn == 0) {
			zero = 1;		
			zn = getch();	
			Move(zn, &obecna_gra);
		}

		if (zn == 'i') { //postawienie pionka
			zn = Akcja();
			if (zn == ENTER) PutStone(&obecna_gra);
		}

		if (zn == 's') { //zapis gry
			zn = Akcja();
			if (zn == ENTER) {
				gotoxy(X_LEGEND, Y_LEGEND + 15);
				cputs("Zapis gry do pliku.");
				char file_name[25];
				WriteFileName(file_name);
				clrscr();
				SaveGame(file_name, &obecna_gra);
			}
		}

		if (zn == 'l') { //zaladowanie gry
			zn = Akcja();
			if (zn == ENTER) {
				gotoxy(X_LEGEND, Y_LEGEND + 15);
				cputs("Odczyt gry z pliku.");
				char file_name[25];
				WriteFileName(file_name);
				LoadGame(file_name, &obecna_gra);
				clrscr();
			}
		}
	} while (zn != 'q');
	clrscr();
	textbackground(BLACK);
	textcolor(WHITE);
	gotoxy(1, 1);
	return 0;
}

//funkcja wyrysowuje menu z lewej strony
void DrawLegend(const game* g) {
	gotoxy(X_LEGEND, Y_LEGEND);
	cputs("Hubert Malinowski");
	gotoxy(X_LEGEND, Y_LEGEND + 2);
	cputs("arrows  = move");
	gotoxy(X_LEGEND, Y_LEGEND + 3);
	cputs("q       = exit");
	gotoxy(X_LEGEND, Y_LEGEND + 4);
	cputs("n       = start");
	gotoxy(X_LEGEND, Y_LEGEND + 5);
	cputs("enter   = confirm");
	gotoxy(X_LEGEND, Y_LEGEND + 6);
	cputs("esp     = cancel");
	gotoxy(X_LEGEND, Y_LEGEND + 7);
	cputs("i       = place a stone");
	gotoxy(X_LEGEND, Y_LEGEND + 8);
	cputs("s       = save");
	gotoxy(X_LEGEND, Y_LEGEND + 9);
	cputs("l       = load");
	gotoxy(X_LEGEND, Y_LEGEND + 10);
	cputs("f       = finish");
	gotoxy(X_LEGEND, Y_LEGEND + 11);
	char buffer[40];
	sprintf_s(buffer, "Cursor position (x, y): (%d, %d)", g->x, g->y);
	cputs(buffer);
}

//funkcja czysci pamiec dynamiczna i przywraca wartosci poczatkowe
void FreeData(game* g) {
	if (g->empty_board != NULL) {
		for (int i = 0; i < g->BOARD_SIZE; i++)
			free(g->empty_board[i]);
		free(g->empty_board);
	}
	if (g->current_board != NULL) {
		for (int i = 0; i < g->BOARD_SIZE; i++)
			free(g->current_board[i]);
		free(g->current_board);
	}
	if (g->previous_board != NULL) {
		for (int i = 0; i < g->BOARD_SIZE; i++)
			free(g->previous_board[i]);
		free(g->previous_board);
	}
	g->queue = 'o';
	g->x = X_PLANSZY;
	g->y = Y_PLANSZY;
	g->black_score = 0;
	g->white_score = 0;
	g->BOARD_SIZE = 0;
}

//funkcja wyrysowuje plansze z prawej strony
void SetBoards(game* g) {
	g->empty_board = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE));
	for (int i = 0; i < g->BOARD_SIZE; i++)
		g->empty_board[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
	g->current_board = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE));
	for (int i = 0; i < g->BOARD_SIZE; i++)
		g->current_board[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
	g->previous_board = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE));
	for (int i = 0; i < g->BOARD_SIZE; i++)
		g->previous_board[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		for (int j = 0; j < g->BOARD_SIZE; j++) {
			if (i == 0) {
				if (j == 0) {
					g->empty_board[i][j] = char(LEWY_GORNY);
					g->current_board[i][j] = char(LEWY_GORNY);
					g->previous_board[i][j] = char(LEWY_GORNY);
				}
				else if (j == g->BOARD_SIZE - 1) {
					g->empty_board[i][j] = char(PRAWY_GORNY);
					g->current_board[i][j] = char(PRAWY_GORNY);
					g->previous_board[i][j] = char(PRAWY_GORNY);
				}
				else {
					g->empty_board[i][j] = char(GORNY);
					g->current_board[i][j] = char(GORNY);
					g->previous_board[i][j] = char(GORNY);
				}
			}
			else if (i == g->BOARD_SIZE - 1) {
				if (j == 0) {
					g->empty_board[i][j] = char(LEWY_DOLNY);
					g->current_board[i][j] = char(LEWY_DOLNY);
					g->previous_board[i][j] = char(LEWY_DOLNY);
				}
				else if (j == g->BOARD_SIZE - 1) {
					g->empty_board[i][j] = char(PRAWY_DOLNY);
					g->current_board[i][j] = char(PRAWY_DOLNY);
					g->previous_board[i][j] = char(PRAWY_DOLNY);
				}
				else {
					g->empty_board[i][j] = char(DOLNY);
					g->current_board[i][j] = char(DOLNY);
					g->previous_board[i][j] = char(DOLNY);
				}
			}
			else {
				if (j == 0) {
					g->empty_board[i][j] = char(LEWY);
					g->current_board[i][j] = char(LEWY);
					g->previous_board[i][j] = char(LEWY);
				}
				else if (j == g->BOARD_SIZE - 1) {
					g->empty_board[i][j] = char(PRAWY);
					g->current_board[i][j] = char(PRAWY);
					g->previous_board[i][j] = char(PRAWY);
				}
				else {
					g->empty_board[i][j] = char(SRODEK);
					g->current_board[i][j] = char(SRODEK);
					g->previous_board[i][j] = char(SRODEK);
				}
			}
		}
	}
}

//funkcja rysujaca plansze
void DrawBoard(const game* g) {
	int y = g->y;
	int x = g->x;
	int maxy = (g->BOARD_SIZE <= MAX_PLANSZA_Y) ? g->BOARD_SIZE : MAX_PLANSZA_Y;
	int maxx = (g->BOARD_SIZE <= MAX_PLANSZA_X) ? g->BOARD_SIZE : MAX_PLANSZA_X;
	if (HEIGHT < g->y) y = HEIGHT;
	if (WIDTH < g->x) x = WIDTH;
	for (int i = g->y - y; i < maxy + g->y - y; i++) {
		for (int j = g->x - x; j < maxx + g->x - x; j++) {
			if (g->current_board[i][j] == 'o') textcolor(BLACK);
			else if (g->current_board[i][j] == 'x') textcolor(WHITE);
			gotoxy(X_PLANSZY + j - g->x + x, Y_PLANSZY + i - g->y + y);
			putch(g->current_board[i][j]);
			textcolor(LIGHTMAGENTA);
		}
	}
}

//funkcja przesuwa kursor sprawdzajac, czy nie wyjechal poza plansze 
void Move(const char code, game* g) {
	if (code == 0x48 && (g->y) != Y_PLANSZY) (g->y)--;
	else if (code == 0x50 && (g->y) != Y_PLANSZY + g->BOARD_SIZE - 1) (g->y)++;
	else if (code == 0x4b && (g->x) != X_PLANSZY) (g->x)--;
	else if (code == 0x4d && (g->x) != X_PLANSZY + g->BOARD_SIZE - 1) (g->x)++;
}

//funkcja zmieniajaca kolejke miedzy graczami
char ChangeQueue(char queue) {
	if (queue == 'o') return 'x';
	else return 'o';
}

//funkcja sprawdza, czy zbito pionka
int CheckForcing(game* g, int** chain) { //teraz queue to opponent w tej funkcji
	int ilosc = 0;
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		for (int j = 0; j < g->BOARD_SIZE; j++) {
			if (chain[i][j] == 1)
			{
				if (i - 1 >= 0 && g->current_board[i - 1][j] == g->empty_board[i - 1][j]) return false;
				if (i + 1 < g->BOARD_SIZE && g->current_board[i + 1][j] == g->empty_board[i + 1][j]) return false;
				if (j - 1 >= 0 && g->current_board[i][j - 1] == g->empty_board[i][j - 1]) return false;
				if (j + 1 < g->BOARD_SIZE && g->current_board[i][j + 1] == g->empty_board[i][j + 1]) return false;
				ilosc++;
			}
		}
	}
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		for (int j = 0; j < g->BOARD_SIZE; j++) {
			if (chain[i][j] == 1) {
				g->current_board[i][j] = g->empty_board[i][j];
			}
		}
	}
	if (g->queue == 'o')
		g->black_score += ilosc;
	else if (g->queue == 'x')
		g->white_score += ilosc;
	return ilosc;
}

//funkcja sprawdzajaca zasade ko
bool KoRule(game* g, char** temp, int score) {
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		for (int j = 0; j < g->BOARD_SIZE; j++) {
			if (g->current_board[i][j] != g->previous_board[i][j])
				return false;
		}
	}
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		for (int j = 0; j < g->BOARD_SIZE; j++) {
			g->current_board[i][j] = temp[i][j];
		}
	}
	if (g->queue == 'o') g->black_score -= score;
	else if (g->queue == 'x') g->white_score -= score;
	return true;
}

void CheckChain(const game* g, int** chain, int y, int x, char team) {
	if ((y - 1 >= 0) && g->current_board[y - 1][x] == team && chain[y - 1][x] == 0) {
		chain[y - 1][x] = 1;
		CheckChain(g, chain, y - 1, x, team);
	}
	if ((y + 1 < g->BOARD_SIZE) && g->current_board[y + 1][x] == team && chain[y + 1][x] == 0) {
		chain[y + 1][x] = 1;
		CheckChain(g, chain, y + 1, x, team);
	}
	if ((x - 1 >= 0) && g->current_board[y][x - 1] == team && chain[y][x - 1] == 0) {
		chain[y][x - 1] = 1;
		CheckChain(g, chain, y, x - 1, team);
	}
	if ((x + 1 < g->BOARD_SIZE) && g->current_board[y][x + 1] == team && chain[y][x + 1] == 0) {
		chain[y][x + 1] = 1;
		CheckChain(g, chain, y, x + 1, team);
	}
}

//funkcja zeruje tablice z lancuchem
void DeleteChain(const game* g, int** chain) {
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		for (int j = 0; j < g->BOARD_SIZE; j++) {
			if (chain[i][j] == 1) chain[i][j] = 0;
		}
	}
}

//funkcja ustawia kamyk na zadane pole
bool PutStone(game* g) {
	if (g->current_board[g->y - Y_PLANSZY][g->x - X_PLANSZY] == g->empty_board[g->y - Y_PLANSZY][g->x - X_PLANSZY]) {
		char** temp = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE)); //plansza tymczasowa, jesli zajdzie powtorzenie pozycji
		for (int i = 0; i < g->BOARD_SIZE; i++)
			temp[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
		for (int i = 0; i < g->BOARD_SIZE; i++) {
			for (int j = 0; j < g->BOARD_SIZE; j++) {
				temp[i][j] = g->current_board[i][j];
			}
		}

		char opponent = ChangeQueue(g->queue);
		g->current_board[g->y - Y_PLANSZY][g->x - X_PLANSZY] = g->queue;

		int** chain = (int**)malloc(sizeof(int*) * (g->BOARD_SIZE)); //utworzenie tablicy z 0 i 1, 1 bedzie oznaczac lancuch kamieni
		for (int i = 0; i < g->BOARD_SIZE; i++)
			chain[i] = (int*)malloc(sizeof(int) * g->BOARD_SIZE);
		for (int i = 0; i < g->BOARD_SIZE; i++) {
			for (int j = 0; j < g->BOARD_SIZE; j++) {
				chain[i][j] = 0;
			}
		}

		int score1 = 0;
		if ((g->y - Y_PLANSZY - 1 >= 0) && g->current_board[g->y - Y_PLANSZY - 1][g->x - X_PLANSZY] == opponent) {
			chain[g->y - Y_PLANSZY - 1][g->x - X_PLANSZY] = 1;
			CheckChain(g, chain, g->y - Y_PLANSZY - 1, g->x - X_PLANSZY, opponent); //czy kamien tworzy lancuch
			score1 += CheckForcing(g, chain); //czy lancuch zostanie zbity
			DeleteChain(g, chain); //usuwa lancuch z tablicy, zeby sprawdzic inne lancuchy
		}
		if ((g->y - Y_PLANSZY + 1 < g->BOARD_SIZE) && g->current_board[g->y - Y_PLANSZY + 1][g->x - X_PLANSZY] == opponent) {
			chain[g->y - Y_PLANSZY + 1][g->x - X_PLANSZY] = 1;
			CheckChain(g, chain, g->y - Y_PLANSZY + 1, g->x - X_PLANSZY, opponent);
			score1 += CheckForcing(g, chain);
			DeleteChain(g, chain);
		}
		if ((g->x - X_PLANSZY - 1 >= 0) && g->current_board[g->y - Y_PLANSZY][g->x - X_PLANSZY - 1] == opponent) {
			chain[g->y - Y_PLANSZY][g->x - X_PLANSZY - 1] = 1;
			CheckChain(g, chain, g->y - Y_PLANSZY, g->x - X_PLANSZY - 1, opponent);
			score1 += CheckForcing(g, chain);
			DeleteChain(g, chain);
		}
		if ((g->x - X_PLANSZY + 1 < g->BOARD_SIZE) && g->current_board[g->y - Y_PLANSZY][g->x - X_PLANSZY + 1] == opponent) {
			chain[g->y - Y_PLANSZY][g->x - X_PLANSZY + 1] = 1;
			CheckChain(g, chain, g->y - Y_PLANSZY, g->x - X_PLANSZY + 1, opponent);
			score1 += CheckForcing(g, chain);
			DeleteChain(g, chain);
		}

		chain[g->y - Y_PLANSZY][g->x - X_PLANSZY] = 1;
		CheckChain(g, chain, g->y - Y_PLANSZY, g->x - X_PLANSZY, g->queue); //sprawdza lancuch kamienia postawionego
		int score = CheckForcing(g, chain); //sprawdza pionka samobojce
		if (score > 0) {
			for (int i = 0; i < g->BOARD_SIZE; i++) {
				for (int j = 0; j < g->BOARD_SIZE; j++) {
					g->current_board[i][j] = temp[i][j]; //uzycie tablicy tymczasowej do cofniecia ustawienie kamienia (jesli samobojca)
				}
			}
			//w funkcji CheckForcing efektem ubocznym bylo dodanie punktow zawodnikowi przy sprawdzaniu pionka samobojcy
			//nalezy odpowiednio odjac wyniki
			if (g->queue == 'o') g->black_score -= score;
			else if (g->queue == 'x') g->white_score -= score;
			for (int j = 0; j < g->BOARD_SIZE; j++)
				free(temp[j]);
			free(temp);
			for (int i = 0; i < g->BOARD_SIZE; i++)
				free(chain[i]);
			free(chain);
			return false;
		}

		//sprawdza zasade go
		if (KoRule(g, temp, score1)) {
			for (int i = 0; i < g->BOARD_SIZE; i++)
				free(temp[i]);
			free(temp);
			for (int i = 0; i < g->BOARD_SIZE; i++)
				free(chain[i]);
			free(chain);
			return false;
		}
		g->queue = ChangeQueue(g->queue);
		for (int i = 0; i < g->BOARD_SIZE; i++) {
			for (int j = 0; j < g->BOARD_SIZE; j++) {
				g->previous_board[i][j] = temp[i][j]; //aktualizuje poprzednia tablice
			}
		}
		for (int i = 0; i < g->BOARD_SIZE; i++)
			free(temp[i]);
		free(temp);
		for (int i = 0; i < g->BOARD_SIZE; i++)
			free(chain[i]);
		free(chain);
		return true;
	}
	return false;
}

//funkcja pozwala na wpisanie nazwy pliku
void WriteFileName(char* nazwa) {
	char zn;
	gotoxy(X_LEGEND, Y_LEGEND + 14);
	cputs("Wpisz nazwe pliku bez podawania rozszerzenia,");
	gotoxy(X_LEGEND, Y_LEGEND + 15);
	cputs("maksymalnie 20 znakow,");
	gotoxy(X_LEGEND, Y_LEGEND + 16);
	cputs("dozwolone tylko male litery i cyfry:");
	gotoxy(X_LEGEND, Y_LEGEND + 17);
	_setcursortype(_NORMALCURSOR);
	zn = getch();
	int i;
	for (i = 0; i < 20; i++) {
		if ((zn > 47 && zn < 58) || (zn > 96 && zn < 123)) {  //kody ascii liter i cyfr
			nazwa[i] = zn;
		}
		else if (zn == ENTER && i != 0) {
			break;
		}
		else if (zn == BACKSPACE && i != 0) {
			gotoxy(X_LEGEND + i - 1, Y_LEGEND + 17);
			putch(NULL);
			gotoxy(X_LEGEND + i - 1, Y_LEGEND + 17);
			i -= 2;
			zn = getch();
			continue;
		}
		else {
			i--;
			zn = getch();
			continue;
		}
		gotoxy(X_LEGEND + i, Y_LEGEND + 17);
		putch(zn);
		zn = getch();
	}
	nazwa[i] = '.';
	nazwa[++i] = 't';
	nazwa[++i] = 'x';
	nazwa[++i] = 't';
	nazwa[++i] = NULL;
}

//funkcja zapisuje stan gry
void SaveGame(char* nazwa, game* g) {
	FILE* plik;
	plik = fopen(nazwa, "w");
	char board_s[20];
	int board_size = sprintf_s(board_s, "%d ", g->BOARD_SIZE);
	fwrite(board_s, sizeof(char), board_size, plik);
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		fwrite(g->empty_board[i], sizeof(char), g->BOARD_SIZE, plik);
		fwrite(g->current_board[i], sizeof(char), g->BOARD_SIZE, plik);
		fwrite(g->previous_board[i], sizeof(char), g->BOARD_SIZE, plik);
	}
	const char* queue = &(g->queue);
	fwrite(queue, sizeof(char), 1, plik);
	char buffer1[20];
	char buffer2[20];
	int buffer_size1 = sprintf_s(buffer1, "%d %d ", g->x, g->y);
	int buffer_size2 = sprintf_s(buffer2, "%d %d ", g->black_score, g->white_score);
	fwrite(buffer1, sizeof(char), buffer_size1, plik);
	fwrite(buffer2, sizeof(char), buffer_size2, plik);
	fclose(plik);
}

//funkcja odczytuje plik
bool LoadGame(char* nazwa, game* g) {
	FILE* plik;
	plik = fopen(nazwa, "r");
	if (plik == NULL) {
		gotoxy(X_LEGEND, Y_LEGEND + 20);
		cputs("Plik nie istnieje.");
		Sleep(1000);
		return false;
	}
	FreeData(g);
	fscanf(plik, "%d ", &(g->BOARD_SIZE));
	g->empty_board = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE));
	for (int i = 0; i < g->BOARD_SIZE; i++)
		g->empty_board[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
	g->current_board = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE));
	for (int i = 0; i < g->BOARD_SIZE; i++)
		g->current_board[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
	g->previous_board = (char**)malloc(sizeof(char*) * (g->BOARD_SIZE));
	for (int i = 0; i < g->BOARD_SIZE; i++)
		g->previous_board[i] = (char*)malloc(sizeof(char) * g->BOARD_SIZE);
	for (int i = 0; i < g->BOARD_SIZE; i++) {
		fread(g->empty_board[i], sizeof(char), g->BOARD_SIZE, plik);
		fread(g->current_board[i], sizeof(char), g->BOARD_SIZE, plik);
		fread(g->previous_board[i], sizeof(char), g->BOARD_SIZE, plik);
	}
	char temp[1] = { NULL };
	fread(temp, sizeof(char), 1, plik);
	g->queue = temp[0];
	fscanf(plik, "%d ", &(g->x));
	fscanf(plik, "%d ", &(g->y));
	fscanf(plik, "%d ", &(g->black_score));
	fscanf(plik, "%d ", &(g->white_score));
	fclose(plik);
	return true;
}

//funkcja ustawia rozmiar planszy
bool ChooseBoardSize(int* size) {
	gotoxy(X_PLANSZY, Y_PLANSZY);
	cputs("Wybierz numer rozmiaru planszy i wcisnij ENTER:");
	gotoxy(X_PLANSZY, Y_PLANSZY + 1);
	cputs("1. 9x9");
	gotoxy(X_PLANSZY, Y_PLANSZY + 2);
	cputs("2. 13x13");
	gotoxy(X_PLANSZY, Y_PLANSZY + 3);
	cputs("3. 19x19");
	gotoxy(X_PLANSZY, Y_PLANSZY + 4);
	cputs("4. Inny (wpisz numer 4 i po spacji rozmiar):");
	_setcursortype(_SOLIDCURSOR);
	char zn;
	int y = Y_PLANSZY + 1;
	do {
		gotoxy(X_PLANSZY, y);
		zn = getch();
		if (zn == 0) {
			zn = getch();
			if (zn == 0x48 && y > Y_PLANSZY + 1) y--;
			if (zn == 0x50 && y < Y_PLANSZY + 4) y++;
		}
	} while (zn != ENTER);

	if (y == Y_PLANSZY + 1) {
		*size = 9;
		return true;
	}
	else if (y == Y_PLANSZY + 2) {
		*size = 13;
		return true;
	}
	else if (y == Y_PLANSZY + 3) {
		*size = 19;
		return true;
	}
	else if (y == Y_PLANSZY + 4) {
		gotoxy(X_PLANSZY, Y_PLANSZY + 5);
		cputs("Wpisz rozmiar planszy (tylko szerokosc):");
		gotoxy(X_PLANSZY, Y_PLANSZY + 6);
		_setcursortype(_NORMALCURSOR);
	}
	int x = X_PLANSZY;
	do
	{
		zn = getch();
		if (zn > 47 && zn < 58) {

			gotoxy(x++, Y_PLANSZY + 6);
			putch(zn);
		}
		else if (zn == BACKSPACE && x > X_PLANSZY) {
			gotoxy(--x, Y_PLANSZY + 6);
			putch(NULL);
			gotoxy(--x, Y_PLANSZY + 6);
		}
	} while (zn != ENTER || (zn == ENTER && x == X_PLANSZY));
	char* pt = (char*)malloc(sizeof(char) * (x - X_PLANSZY) * 2);
	gettext(X_PLANSZY, Y_PLANSZY + 6, x - 1, Y_PLANSZY + 6, pt);
	gotoxy(X_PLANSZY, Y_PLANSZY + 5);
	clreol();
	gotoxy(X_PLANSZY, Y_PLANSZY + 6);
	clreol();
	int i = 0;
	int liczba = 0;
	int potega = 1;
	for (int i = 0; i < x - X_PLANSZY; i++) {
		potega *= 10;
	}
	while (i < (x - X_PLANSZY) * 2 && pt[i] != NULL) {
		sscanf(pt + i, "%d", &liczba);
		potega /= 10;
		*size += liczba * potega;
		i += 2;
	}
	free(pt);
	return true;
}

//funckja dodajaca czarnym kamienie
bool Handicap(game* g) {
	gotoxy(X_PLANSZY, Y_PLANSZY);
	cputs("Czy chcesz skorzystac z handicapu dla koloru czarnego?");
	gotoxy(X_PLANSZY, Y_PLANSZY + 1);
	cputs("1.Tak.");
	gotoxy(X_PLANSZY, Y_PLANSZY + 2);
	cputs("2.Nie.");
	_setcursortype(_SOLIDCURSOR);
	char zn;
	int y = Y_PLANSZY + 1;
	do {
		gotoxy(X_PLANSZY, y);
		zn = getch();
		if (zn == 0) {
			zn = getch();
			if (zn == 0x48 && y == Y_PLANSZY + 2) y--;
			if (zn == 0x50 && y == Y_PLANSZY + 1) y++;
		}
	} while (zn != ENTER);
	if (y == Y_PLANSZY + 2) {
		return false;
	}
	else {
		clrscr();
		DrawLegend(g);
		_setcursortype(_NOCURSOR);
		gotoxy(X_PLANSZY, Y_PLANSZY - 4);
		cputs("Kamien postaw wciskajac \"i\",");
		gotoxy(X_PLANSZY, Y_PLANSZY - 3);
		cputs("usun, wciskajac \"Esc\" na danym kamieniu,");
		gotoxy(X_PLANSZY, Y_PLANSZY - 2);
		cputs("zatwierdz wszystko, wciskajac ENTER");
		textbackground(DARKGRAY);
		DrawBoard(g);
		do
		{
			int y = (g->y < HEIGHT) ? g->y : HEIGHT;
			int x = (g->x < WIDTH) ? g->x : WIDTH;
			gotoxy(x, y);
			textcolor(BLACK);
			putch('o');
			zn = getch();
			Move(zn, g);
			if (zn == 'i') {
				g->current_board[g->y - Y_PLANSZY][g->x - X_PLANSZY] = 'o';
				g->previous_board[g->y - Y_PLANSZY][g->x - X_PLANSZY] = 'o';
			}
			else if (zn == ESC) {
				putch(g->empty_board[g->y - Y_PLANSZY][g->y - Y_PLANSZY]);
				g->current_board[g->y - Y_PLANSZY][g->x - X_PLANSZY] = g->empty_board[g->y - Y_PLANSZY][g->x - X_PLANSZY];
				g->previous_board[g->y - Y_PLANSZY][g->x - X_PLANSZY] = g->empty_board[g->y - Y_PLANSZY][g->x - X_PLANSZY];
			}
			textcolor(LIGHTMAGENTA);
			DrawBoard(g);
		} while (zn != ENTER);
	}
	textcolor(WHITE);
	textbackground(BLACK);
	return true;
}

// zatwierdzanie akcji enter lub cofniecie escape
char Akcja() {
	char zn;
	do
	{
		zn = getch();
	} while (zn != ENTER && zn != ESC);
	return zn;
}