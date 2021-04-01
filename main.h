#define COUNT_MINE 10

#define POS_KOPALNIA_X 0
#define POS_KOPALNIA_Y 3
#define POS_KOPALNIA_LVL_X 12
#define POS_KOPALNIA_LVL_Y 3
#define POS_KOPALNIA_WYD_X 30
#define POS_KOPALNIA_WYD_Y 3
#define POS_KOPALNIA_RUDA_X 55
#define POS_KOPALNIA_RUDA_Y 3

#define POS_USER_NAME_X 0
#define POS_USER_NAME_Y 0
#define POS_ZLOTO_X 20
#define POS_ZLOTO_Y 0
#define POS_RUDA_X 40
#define POS_RUDA_Y 0

#define POS_CMD_X_0 0
#define POS_CMD_X 9
#define POS_CMD_Y 8 + COUNT_MINE
#define POS_INFO_X 0
#define POS_INFO_Y 6 + COUNT_MINE

#define POS_LEGEND_X 0
#define POS_LEGEND_Y_0 11 + COUNT_MINE
#define POS_LEGEND_Y 13 + COUNT_MINE

#define POS_SAVES_X 0
#define POS_SAVES_Y 15 + COUNT_MINE

#define COL_ZLOTO 0x01
#define COL_RUDA 0x02
#define COL_KOPALNIA 0x03
#define COL_WYD 0x04
#define COL_FULL 0x05
#define COL_LVL_UP 0x06
#define COL_BIALY 0x0F

#define WZOR_WYD(l) l*900
#define WZOR_RUDA(l) l*9000
#define WZOR_KOSZT_LVL(l) l*200
#define WZOR_CZAS_LVL(l) l*600
#define WZOR_SOLD_RUDA(l) l/100



void drawStats(struct player *p, COORD *c, HANDLE *h);

void drawMine(struct mine* m, COORD* c, HANDLE* h);

void drawMineLvl(struct mine* m, COORD* c, HANDLE* h, time_t t);

void drawMineWyd(struct mine* m, COORD* c, HANDLE* h);

void drawMineRuda(struct mine* m, COORD* c, HANDLE* h, time_t t);

long int countUrobek(time_t t, struct mine* m);

void zbierzUrobek(struct mine* m, struct player* p, time_t t, COORD* c, HANDLE* h);

void lvlUpMine(struct mine* m, time_t t, struct player* p, COORD* c, HANDLE* h, char bud);

void soldRuda(struct player* p, COORD* c, HANDLE* h, long int ile, char sold);

void __cdecl ThreadProc(void* args);

char komenda[20];	// bufor wpisanych s³ów, dzielony pomiêdzy dwa w¹tki

void saveGame(struct mine* m, struct player* p, HANDLE* h, COORD* c);

void loadGame(struct mine* m, struct player* p, char* n, HANDLE* h, COORD* c);

void drawLegend(COORD* c, HANDLE* h);

void newGame(struct mine* m, struct player* p, HANDLE* h, COORD* c);

void clearLine(HANDLE* h, int lineNumber, int lineCount);