#include <stdio.h>				// obs³ga wyjœcia/wejœcia
#include <Windows.h>			// kolorowanie sk³adni w konsoli i przesuwanie kursora
#include <time.h>				// czas, zegar POSIX
#include <locale.h>				// polskie znaki
#include <process.h>			// w¹tki
#include "main.h"				// lokalny plik nag³ówkowy

struct player 
{
	long int zloto;
	long int ruda;
	char userName[11];
};

struct mine
{
	unsigned char id;
	unsigned char lvl;
	time_t startTime;
	time_t startBuild;
	time_t endBuild;
};

int main()
{
	setlocale(LC_CTYPE, "Polish");		// polskie znaki w konsoli

	// zmina kolorów sk³adni

	CONSOLE_SCREEN_BUFFER_INFOEX bi;
	bi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);		// pobranie konsoli

	GetConsoleScreenBufferInfoEx(hcon, &bi);

	// definiowanie kolorów 0xBBGGRR odwrócone RGB

	bi.ColorTable[1] = 0x00A3CC;		// z³oto
	bi.ColorTable[2] = 0x0000CC;		// ruda
	bi.ColorTable[3] = 0x00FF00;		// kopalnia
	bi.ColorTable[4] = 0x800080;		// wydajnoœæ
	bi.ColorTable[5] = 0x0099FF;		// full stan
	bi.ColorTable[6] = 0xFFFF66;		// lvl up


	SetConsoleScreenBufferInfoEx(hcon, &bi);		// zapisanie kolorów do konsoli

	// ukrycie wyœwietlania kursora

	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hcon, &info);

	//definiowanie zmiennych pod wykrywanie klawiszy

	

	// definiowanie koordynatów (kursor)

	COORD cord;
	cord.X = 0;
	cord.Y = 0;

	time_t czas;
	czas = time(NULL);

	// definiowanie gracza

	struct player gracz;
	struct mine kopalnia[COUNT_MINE];


	gracz.zloto = 0;
	gracz.ruda = 0;
	gracz.userName[0] = '\0';

	// wartoœci pocz¹tkowe dla kopalnii

	
	for (int i = 0; i < COUNT_MINE; i++)
	{
		kopalnia[i].id = (unsigned char)i;
		kopalnia[i].lvl = 0;
		kopalnia[i].startTime = 0;
		kopalnia[i].startBuild = 0;
		kopalnia[i].endBuild = 0;

		drawMine(&kopalnia[i], &cord, &hcon);				// wyœwietlanie kopalnii
		drawMineLvl(&kopalnia[i], &cord, &hcon, czas);		// wyœwietlanie poziomu kopalnii
		drawMineWyd(&kopalnia[i], &cord, &hcon);			// wyœwietlanie wydajnoœci danej kopalnii
		drawMineRuda(&kopalnia[i], &cord, &hcon, czas);		// wyœwietlanie wydobywania rudy w kopalnii
	}

	// rysowanie statystyk gracza

	drawStats(&gracz, &cord, &hcon); 
	drawLegend(&cord, &hcon);

	// Komenda powitalna

	cord.X = POS_INFO_X;
	cord.Y = POS_INFO_Y;

	clearLine(&hcon, POS_INFO_Y, 1);

	SetConsoleCursorPosition(hcon, cord);
	SetConsoleTextAttribute(hcon, COL_BIALY);
	printf("Witaj w grze. Rozpocznij now¹ grê wpisuj¹c: ");

	SetConsoleTextAttribute(hcon, COL_KOPALNIA);
	printf("Nowa Gra ");

	SetConsoleTextAttribute(hcon, COL_LVL_UP);
	printf("nazwa");

	SetConsoleTextAttribute(hcon, COL_BIALY);
	printf(" lub wczytaj grê wpisuj¹c: ");

	SetConsoleTextAttribute(hcon, COL_KOPALNIA);
	printf("Wczytaj ");

	SetConsoleTextAttribute(hcon, COL_LVL_UP);
	printf("nazwa");

	// *****************************************

	cord.X = POS_CMD_X_0;
	cord.Y = POS_CMD_Y;

	SetConsoleCursorPosition(hcon, cord);
	SetConsoleTextAttribute(hcon, COL_BIALY);
	printf("Komenda:");

	cord.X = POS_CMD_X;
	cord.Y = POS_CMD_Y;

	SetConsoleCursorPosition(hcon, cord);
	SetConsoleTextAttribute(hcon, COL_BIALY);
	


	// Uruchomienie w¹tku obs³uguj¹cego klawisze;

	HANDLE hThread = (HANDLE)_beginthread(ThreadProc, 0, NULL);

	char* ptr;
	int komenda_index;



	while (1)
	{
		Sleep(10);

		cord.X = POS_CMD_X;
		cord.Y = POS_CMD_Y;

		SetConsoleCursorPosition(hcon, cord);
		SetConsoleTextAttribute(hcon, COL_BIALY);
		printf("%s                    ", komenda);

		ptr = strchr(komenda, '\0');
		komenda_index = (int)(ptr - &komenda);

		if (komenda[komenda_index - 1] == 24)
		{
			komenda[komenda_index - 1] = '\0';

			if (!strncmp(komenda, "Zbierz", strlen("Zbierz")))								// wykrywanie okreœlonej komendy
			{
				int zbieranie = atoi(komenda + strlen("Zbierz"));
				if (zbieranie >= 0 && zbieranie < COUNT_MINE)
				{
					czas = time(NULL);
					zbierzUrobek(&kopalnia[zbieranie], &gracz, czas, &cord, &hcon);
					drawStats(&gracz, &cord, &hcon);
				}
			}
			else if (!strncmp(komenda, "Ulepsz", strlen("Ulepsz")))
			{
				int ulepszanie = atoi(komenda + strlen("Ulepsz"));

				if (ulepszanie >= 0 && ulepszanie < COUNT_MINE)
				{
					if (komenda[komenda_index - 2] == 'U')
					{
						czas = time(NULL);
						lvlUpMine(&kopalnia[ulepszanie], czas, &gracz, &cord, &hcon, 1);
						drawStats(&gracz, &cord, &hcon);
					}
					else
					{
						czas = time(NULL);
						lvlUpMine(&kopalnia[ulepszanie], czas, &gracz, &cord, &hcon, 0);
						drawStats(&gracz, &cord, &hcon);
					}
				}
			}
			else if (!strncmp(komenda, "Sprzedaj", strlen("Sprzedaj")))
			{
				long int sprzedawanie = atoi(komenda + strlen("Sprzedaj"));
				if (sprzedawanie >= 0)
				{
					if (komenda[komenda_index - 2] == 'S')
					{
						soldRuda(&gracz, &cord, &hcon, sprzedawanie, 1);
						drawStats(&gracz, &cord, &hcon);
					}
					else
					{
						soldRuda(&gracz, &cord, &hcon, sprzedawanie, 0);
						drawStats(&gracz, &cord, &hcon);
					}
				}
			}
			else if (!strncmp(komenda, "Zapisz", strlen("Zapisz")))
			{
				if (strlen(gracz.userName) > 0)
				{
					saveGame(kopalnia, &gracz, &hcon, &cord);
				}
				else
				{
					cord.X = POS_INFO_X;
					cord.Y = POS_INFO_Y;

					clearLine(&hcon, POS_INFO_Y, 1);

					SetConsoleCursorPosition(hcon, cord);
					SetConsoleTextAttribute(hcon, COL_BIALY);

					printf("Nie mo¿na zapisaæ. Stwórz now¹ grê.");
				}
			}
			else if (!strncmp(komenda, "Wczytaj", strlen("Wczytaj")))
			{
				if (strlen(komenda) > strlen("Wczytaj "))
				{
					loadGame(kopalnia, &gracz, (komenda + strlen("Wczytaj ")), &hcon, &cord);

					drawStats(&gracz, &cord, &hcon);
					for (int i = 0; i < COUNT_MINE; i++)
					{
						drawMineWyd(&kopalnia[i], &cord, &hcon);
					}
				}
				else
				{
					cord.X = POS_SAVES_X;
					cord.Y = POS_SAVES_Y;

					clearLine(&hcon, POS_SAVES_Y, 7);

					SetConsoleCursorPosition(hcon, cord);
					SetConsoleTextAttribute(hcon, COL_LVL_UP);

					system("dir /B *.kop");

					cord.X = POS_INFO_X;
					cord.Y = POS_INFO_Y;

					clearLine(&hcon, POS_INFO_Y, 1);

					SetConsoleCursorPosition(hcon, cord);
					SetConsoleTextAttribute(hcon, COL_BIALY);

					printf("Wypisano zapisy gier.");
				}
			}
			else if (!strncmp(komenda, "Nowa Gra", strlen("Nowa Gra")))
			{
				if (strlen(komenda) > strlen("Nowa Gra ") && strlen(komenda) <= strlen("Nowa Gra ######"))
				{
					strcpy_s(gracz.userName, 11, (komenda + strlen("Nowa Gra ")));
					strcat_s(gracz.userName, 11, ".kop");

					newGame(kopalnia, &gracz, &hcon, &cord);

					drawStats(&gracz, &cord, &hcon);

					for (int i = 0; i < COUNT_MINE; i++)
					{
						drawMineWyd(&kopalnia[i], &cord, &hcon);
					}
				}
				else
				{
					cord.X = POS_INFO_X;
					cord.Y = POS_INFO_Y;

					clearLine(&hcon, POS_INFO_Y, 1);

					SetConsoleCursorPosition(hcon, cord);
					SetConsoleTextAttribute(hcon, COL_BIALY);

					printf("B³¹d nazwy, nazwa powinna zawieraæ iloœæ znaków od 1 do 6.");

				}
			}
			else
			{
				cord.X = POS_INFO_X;
				cord.Y = POS_INFO_Y;

				clearLine(&hcon, POS_INFO_Y, 1);

				SetConsoleCursorPosition(hcon, cord);
				SetConsoleTextAttribute(hcon, COL_BIALY);
				
				printf("Nie ma takiej komendy.");
			}
		}

		czas = time(NULL);
		for (int k = 0; k < COUNT_MINE; k++)
		{
			drawMineRuda(&kopalnia[k], &cord, &hcon, czas);
			drawMineLvl(&kopalnia[k], &cord, &hcon, czas);
		}
	}

	return 0;
}


void drawStats(struct player* p, COORD* c, HANDLE* h)
{
	c->X = POS_USER_NAME_X;
	c->Y = POS_USER_NAME_Y;

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_LVL_UP);
	printf("%s        ", p->userName);

	c->X = POS_ZLOTO_X;
	c->Y = POS_ZLOTO_Y;

	SetConsoleCursorPosition(*h, *c);			// ustawienie koordynatów kursora w konsoli
	SetConsoleTextAttribute(*h, COL_ZLOTO);		// ustawienie koloru
	printf("Z³oto %d        ", p->zloto);				// wypisanie wartoœci

	c->X = POS_RUDA_X;
	c->Y = POS_RUDA_Y;

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_RUDA);
	printf("Ruda %d                                  ", p->ruda);


}

void drawMine(struct mine* m, COORD* c, HANDLE* h)
{
	c->X = POS_KOPALNIA_X;
	c->Y = POS_KOPALNIA_Y + m->id;			// Umiejscowienie napisów / Ka¿da nastêpna kopalnia w nastêpnej linii

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_KOPALNIA);
	printf("Kopalnia %d", m->id);			// kopalnia z jej numerem
}

void drawMineLvl(struct mine* m, COORD* c, HANDLE* h, time_t t)
{
	c->X = POS_KOPALNIA_LVL_X;
	c->Y = POS_KOPALNIA_LVL_Y + m->id;

	SetConsoleCursorPosition(*h, *c);
	if (t >= m->startBuild && t < m->endBuild)
	{
		SetConsoleTextAttribute(*h, COL_LVL_UP);
		printf("(%d %%)          ", ((t - m->startBuild) * 100) / (m->endBuild - m->startBuild));

	}
	else
	{
		SetConsoleTextAttribute(*h, COL_KOPALNIA);
		printf("(Lvl %d)        ", m->lvl);
	}
}

void drawMineWyd(struct mine* m, COORD* c, HANDLE* h)
{
	c->X = POS_KOPALNIA_WYD_X;
	c->Y = POS_KOPALNIA_WYD_Y + m->id;

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_WYD);
	printf("%d RUDA/h         ", WZOR_WYD(m->lvl));			// wypisanie iloœæ wydobywanych surowców na godzinê
}

void drawMineRuda(struct mine* m, COORD* c, HANDLE* h, time_t t)
{
	c->X = POS_KOPALNIA_RUDA_X;
	c->Y = POS_KOPALNIA_RUDA_Y + m->id;

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_RUDA);
	
	long int urobek = countUrobek(t, m);

	if (urobek == WZOR_RUDA(m->lvl)) SetConsoleTextAttribute(*h, COL_FULL);

	printf("%ld/%d                  ", urobek, WZOR_RUDA(m->lvl));
}

void drawLegend(COORD* c, HANDLE* h)
{
	c->X = POS_LEGEND_X;
	c->Y = POS_LEGEND_Y_0;

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_BIALY);

	printf("Legenda:");

	c->X = POS_LEGEND_X;
	c->Y = POS_LEGEND_Y;

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_KOPALNIA);

	printf("| Zbierz # | Ulepsz # | Sprzedaj ### | Zapisz | Wczytaj ######.kop | Nowa Gra ###### |");

}

long int countUrobek(time_t t, struct mine* m)
{
	long int urobek;																								// wydobyte surowce
	if (m->startTime >= m->endBuild)																						// jeœli wydobycie zaczê³o sie po rozbudowanie
	{
		urobek = (t - m->startTime) * (WZOR_WYD((double)m->lvl) / 3600);													// to urobek wynosi iloœæ zebranych materia³ów od startu
	}
	else if (m->startTime < m->startBuild)																				// jeœli ruda by³a wydobywana, a nastêpnie rozpoczê³a siê rozbudowa
	{
		if (t < m->endBuild && t >= m->startBuild)																		// to jeœli nie zakoñczy³a siê rozbudowa
		{
			urobek = (m->startBuild - m->startTime) * (WZOR_WYD(((double)m->lvl - 1)) / 3600);										// to urobek jest taki jak w czasie rozpoczêcia rozbudowy na ni¿szym poziomie
		}
		else
		{
			urobek = (m->startBuild - m->startTime) * (WZOR_WYD(((double)m->lvl - 1)) / 3600) + (t - m->endBuild) * (WZOR_WYD((double)m->lvl) / 3600);					// czas wydobywania na ni¿szym poziomie plus czas wydobywania na wy¿szym poziomie, nie bior¹ pod uwagê czasu budowy
		}
	}
	else if (m->startTime < m->endBuild && m->startTime >= m->startBuild)														// je¿eli rozpoczêcie wydobycia zaczê³o siê w trakcie rozbudowy (czyli zebrany zosta³ urobek)
	{
		if (t < m->endBuild && t >= m->startBuild)																		// oraz rozbudowa aktualnie trwa to urobek równy jest 0
		{
			urobek = 0;
		}
		else
		{
			urobek = (t - m->startTime - (m->endBuild - m->startTime)) * (WZOR_WYD((double)m->lvl) / 3600);						// kiedy jest ju¿ po rozbudowie to urobek ma stan od zakoñczenia budowy do czasu aktualnego
		}
	}

	if (urobek > WZOR_RUDA(m->lvl))						// jeœli urobek przekroczy ³adownosæ magazynu
	{
		urobek = WZOR_RUDA(m->lvl);						// przestaje wydobywaæ i magazyn zostaje zape³niony
	}

	return urobek;
}

void zbierzUrobek(struct mine* m, struct player* p, time_t t, COORD* c, HANDLE* h)
{
	long int urobek = countUrobek(t, m);

	m->startTime = t;
	p->ruda += urobek;

	c->X = POS_INFO_X;
	c->Y = POS_INFO_Y;

	clearLine(h, POS_INFO_Y, 1);

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_BIALY);
	printf("Zebrano ");
	SetConsoleTextAttribute(*h, COL_RUDA);
	printf("%ld RUDY", urobek);
}

void __cdecl ThreadProc(void* args)
{
	HANDLE hcon_i = GetStdHandle(STD_INPUT_HANDLE);			// pobranie wejœcia konsoli
	INPUT_RECORD irInBuf[1];								// bufory przetrzymuj¹cy informacje o przechwyconym klawiszu
	DWORD cNumRead;											// okreœla liczbê przechowywanych klawiszy
	char* ptr;												// wskaŸnik do poszukiwanego znaku
	int komenda_index;										// index znalezionego znaku (któr¹ pozycjê w tablicy zajmuje)
	

	while (1)
	{

		Sleep(10);

		//********* obs³uga klawiatury/komend **********

		ReadConsoleInput(hcon_i, irInBuf, 1, &cNumRead);										// zczytanie klawisza i dodanie do buforu irInBuf

		ptr = strchr(komenda, '\0');															// wyszukanie koñca linii
		komenda_index = (int)(ptr - &komenda);													// wyliczenie pozycji znaku

		if (irInBuf[0].Event.KeyEvent.bKeyDown)													// je¿eli klawisz jest wciœniêty
		{
			if (irInBuf[0].Event.KeyEvent.uChar.AsciiChar == 8)									// je¿eli znak to backspace
			{
				if (komenda_index > 0)															// jeœli s¹ jakieœ znaki w buforze komenda
				{
					komenda[komenda_index - 1] = '\0';											// podmiana ostatniego znaku niebia³ego na pusty (kasowanie)
				}
			}
			 else if (irInBuf[0].Event.KeyEvent.uChar.AsciiChar == 13)							// je¿eli znak to enter
			 {
				if (komenda_index < 19)
				{
					komenda[komenda_index] = 24;
				}
			 }
			else
			{
				if (komenda_index < 18)															// komenda blokuj¹ca wyjœcie poza bufor
				{
					komenda[komenda_index] = irInBuf[0].Event.KeyEvent.uChar.AsciiChar;			// wstawienie do naszego bufora na odpowiedniej pozycji z bufora irInBuf
				}
			}
		}

		//**********************************************
	}

	_endthread();
}

void lvlUpMine(struct mine* m, time_t t, struct player* p, COORD* c, HANDLE* h, char bud)
{
	if (bud)
	{
		if (p->zloto >= WZOR_KOSZT_LVL((m->lvl + 1)))
		{
			if ((m->startTime >= m->endBuild && countUrobek(t, m) < WZOR_RUDA(m->lvl)) || m->lvl == 0)
			{
				m->lvl++;
				m->startBuild = t;
				m->endBuild = t + (time_t)WZOR_CZAS_LVL(m->lvl);
				p->zloto -= WZOR_KOSZT_LVL(m->lvl);


				c->X = POS_INFO_X;
				c->Y = POS_INFO_Y;

				clearLine(h, POS_INFO_Y, 1);

				SetConsoleCursorPosition(*h, *c);
				SetConsoleTextAttribute(*h, COL_BIALY);
				printf("Ulepszanie ");
				SetConsoleTextAttribute(*h, COL_KOPALNIA);
				printf("Kopalni %d", m->id);
				drawMineWyd(m, c, h);
			}
			else
			{
				c->X = POS_INFO_X;
				c->Y = POS_INFO_Y;

				clearLine(h, POS_INFO_Y, 1);

				SetConsoleCursorPosition(*h, *c);
				SetConsoleTextAttribute(*h, COL_BIALY);
				printf("Nie mo¿na ulepszyæ, pierwsze opró¿nij magazyn kopalni lub poczekaj a¿ kopalnia zostanie ukoñczona.");
			}
		}
		else
		{
			c->X = POS_INFO_X;
			c->Y = POS_INFO_Y;

			clearLine(h, POS_INFO_Y, 1);

			SetConsoleCursorPosition(*h, *c);
			SetConsoleTextAttribute(*h, COL_BIALY);
			printf("Masz zbyt ma³o ");
			SetConsoleTextAttribute(*h, COL_ZLOTO);
			printf("z³ota");
		}
	}
	else
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("Rozbudowa ");
		SetConsoleTextAttribute(*h, COL_KOPALNIA);
		printf("Kopalnia %d", m->id);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf(". Koszt ");
		SetConsoleTextAttribute(*h, COL_ZLOTO);
		printf("%d sztuk z³ota", WZOR_KOSZT_LVL((m->lvl + 1)));
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf(". Czas rozbudowy ");
		SetConsoleTextAttribute(*h, COL_WYD);
		printf("%d", WZOR_CZAS_LVL((m->lvl + 1)) / 60);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf(" minut. Aby ulepszyæ wpisz: ");
		SetConsoleTextAttribute(*h, COL_LVL_UP);
		printf("Ulepsz %dU", m->id);
	}
}

void soldRuda(struct player* p, COORD* c, HANDLE* h, long int ile, char sold)
{
	long int piniondz = WZOR_SOLD_RUDA(ile);

	if (sold)
	{
		if (ile <= p->ruda)
		{
			p->ruda -= ile;
			p->zloto += piniondz;

			c->X = POS_INFO_X;
			c->Y = POS_INFO_Y;

			clearLine(h, POS_INFO_Y, 1);

			SetConsoleCursorPosition(*h, *c);
			SetConsoleTextAttribute(*h, COL_BIALY);
			printf("Sprzedano ");
			SetConsoleTextAttribute(*h, COL_RUDA);
			printf("%d rudy", ile);
			SetConsoleTextAttribute(*h, COL_BIALY);
			printf(" za ");
			SetConsoleTextAttribute(*h, COL_ZLOTO);
			printf("%d z³ota.", piniondz);
		}
		else
		{
			c->X = POS_INFO_X;
			c->Y = POS_INFO_Y;

			clearLine(h, POS_INFO_Y, 1);

			SetConsoleCursorPosition(*h, *c);
			SetConsoleTextAttribute(*h, COL_BIALY);
			printf("Nie masz na tyle ");
			SetConsoleTextAttribute(*h, COL_RUDA);
			printf("rudy.");
		}
	}
	else
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("Mo¿esz sprzedaæ ");
		SetConsoleTextAttribute(*h, COL_RUDA);
		printf("%d rudy", ile);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf(" za ");
		SetConsoleTextAttribute(*h, COL_ZLOTO);
		printf("%d z³ota", piniondz);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf(". Aby sprzedaæ wpisz: ");
		SetConsoleTextAttribute(*h, COL_LVL_UP);
		printf("Sprzedaj %dS", ile);
	}
}


/*

Struktura pliku
*************************

= 4 Bajty informacyjne o pocz¹tku ramki danych kopalni ('\x01' 'K' 'O' 'P')
= 4 Bajty okreœlaj¹ce iloœæ kopalni (int)  KOP
<
= 4 Bajty okreœlaj¹ce id kopalni (int) 
= 4 Bajty okreœlaj¹ce lvl kopalni (int)
= 8 Bajtów startTime (time_t)
= 8 Bajtów startBuild (time_t)
= 8 Bajtów endBuild (time_t)
> * KOP

= 4 Bajty informacyjne o pocz¹tku ramki danych gracza ('\x01' 'U' 'S' 'R')
= 4 Bajty okreœlaj¹ce iloœæ graczy ( (int) 1 - zawsze, bo jest jeden gracz)
= 8 Bajtów okreœlaj¹cych iloœæ z³ota gracza (long long int)
= 8 Bajtow okreœlaj¹cych iloœæ rudy gracza (long long int)

*/

void saveGame(struct mine* m, struct player* p, HANDLE* h, COORD* c)
{
	char info[4];
	int countKop[1];

	int infoKop[2];
	time_t times[3];

	long long int statsUsr[2];

	FILE* fp;
	fopen_s(&fp, p->userName, "wb");

	info[0] = '\x01';
	info[1] = 'K';
	info[2] = 'O';
	info[3] = 'P';

	countKop[0] = COUNT_MINE;

	fwrite(info, sizeof(char), 4, fp);								// funkcja fwrite(wskaŸnik do tablicy z danymi, ile bajtów posiada pojedyñcza dana, iloœæ danych, wskaŸnik do pliku)
	fwrite(countKop, sizeof(int), 1, fp);

	for (int i = 0; i < COUNT_MINE; i++)
	{
		infoKop[0] = m[i].id;
		infoKop[1] = m[i].lvl;

		times[0] = m[i].startTime;
		times[1] = m[i].startBuild;
		times[2] = m[i].endBuild;

		fwrite(infoKop, sizeof(int), 2, fp);
		fwrite(times, sizeof(time_t), 3, fp);
	}

	info[0] = '\x01';
	info[1] = 'U';
	info[2] = 'S';
	info[3] = 'R';

	countKop[0] = 1;

	statsUsr[0] = p->zloto;
	statsUsr[1] = p->ruda;

	fwrite(info, sizeof(char), 4, fp);
	fwrite(countKop, sizeof(int), 1, fp);
	fwrite(statsUsr, sizeof(long long int), 2, fp);

	fclose(fp);

	c->X = POS_INFO_X;
	c->Y = POS_INFO_Y;

	clearLine(h, POS_INFO_Y, 1);

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_BIALY);
	printf("Pomyœlnie zapisano: ");

	SetConsoleTextAttribute(*h, COL_LVL_UP);
	printf("%s", p->userName);

}

void loadGame(struct mine* m, struct player* p, char* n, HANDLE* h, COORD* c)
{
	char info[4];
	int countKop[1];

	int infoKop[2];
	time_t times[3];

	long long int statsUsr[2];

	errno_t blad;

	FILE* fp;
	blad = fopen_s(&fp, n, "rb");

	if (blad)
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("B³¹d wczytywania, nie ma takiego pliku.");

		return 0;
	}

	fread(info, sizeof(char), 4, fp);
	
	if (strncmp(info, "\x01KOP", 4))
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("B³¹d wczytywania, nie znaleziono kopalni.");

		return 0;
	}
	
	fread(countKop, sizeof(int), 1, fp);

	if (countKop[0] != COUNT_MINE)
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("B³¹d wczytywania, niepoprawna iloœæ kopalni.");

		return 0;
	}

	for (int i = 0; i < COUNT_MINE; i++)
	{
		fread(infoKop, sizeof(int), 2, fp);

		if (infoKop[0] != i)
		{
			c->X = POS_INFO_X;
			c->Y = POS_INFO_Y;

			clearLine(h, POS_INFO_Y, 1);

			SetConsoleCursorPosition(*h, *c);
			SetConsoleTextAttribute(*h, COL_BIALY);
			printf("B³¹d wczytywania, niepoprawny numer kopalni %d.", i);

			return 0;
		}

		fread(times, sizeof(time_t), 3, fp);

		m[i].id = infoKop[0];
		m[i].lvl = infoKop[1];

		m[i].startTime = times[0];
		m[i].startBuild = times[1];
		m[i].endBuild = times[2];
	}

	fread(info, sizeof(char), 4, fp);

	if (strncmp(info, "\x01USR", 4))
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("B³¹d wczytywania, nie znaleziono gracza.");

		return 0;
	}

	fread(countKop, sizeof(int), 1, fp);

	if (countKop[0] != 1)
	{
		c->X = POS_INFO_X;
		c->Y = POS_INFO_Y;

		clearLine(h, POS_INFO_Y, 1);

		SetConsoleCursorPosition(*h, *c);
		SetConsoleTextAttribute(*h, COL_BIALY);
		printf("B³¹d wczytywania, niepoprawna iloœæ kopalni.");

		return 0;
	}

	fread(statsUsr, sizeof(long long int), 2, fp);

	p->zloto = statsUsr[0];
	p->ruda = statsUsr[1];

	fclose(fp);

	strcpy_s(p->userName, 11, n);

	c->X = POS_INFO_X;
	c->Y = POS_INFO_Y;

	clearLine(h, POS_INFO_Y, 1);

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_BIALY);
	printf("Pomyœlnie wczytano: ");

	SetConsoleTextAttribute(*h, COL_LVL_UP);
	printf("%s", p->userName);
}

void newGame(struct mine* m, struct player* p, HANDLE* h, COORD* c)
{
	p->zloto = 200;
	p->ruda = 0;

	for (int i = 0; i < COUNT_MINE; i++)
	{
		m[i].id = (unsigned char)i;
		m[i].lvl = 0;
		m[i].startTime = 0;
		m[i].startBuild = 0;
		m[i].endBuild = 0;
	}

	c->X = POS_INFO_X;
	c->Y = POS_INFO_Y;

	clearLine(h, POS_INFO_Y, 1);

	SetConsoleCursorPosition(*h, *c);
	SetConsoleTextAttribute(*h, COL_BIALY);

	printf("Stworzono now¹ grê: ");

	SetConsoleTextAttribute(*h, COL_LVL_UP);

	printf("%s", p->userName);

}

void clearLine(HANDLE* h, int lineNumber, int lineCount)
{
	DWORD cCharsWritten;
	COORD cor;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	TCHAR znak = ' ';
	DWORD dwConeSize;

	cor.X = 0;
	cor.Y = lineNumber;

	GetConsoleScreenBufferInfo(*h, &csbi);

	dwConeSize = lineCount * csbi.dwSize.X;

	FillConsoleOutputCharacter(*h, znak, dwConeSize, cor, &cCharsWritten);

	SetConsoleCursorPosition(*h, cor);
}