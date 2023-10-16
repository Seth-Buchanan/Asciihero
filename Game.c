#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

#define WIDTH 50
#define WALL_CHARACTER "\U00002591" /* really a three byte string */
#define EMPTY_CHARACTER ' '

void setUp();
void printWorld();
bool move();
char* myStrCat (char*, char*);
char* repeatStr (char*, size_t);
int getch();


typedef struct Hero {
  char name[10];
  char symbol;
  int x;
  int y;
  int score;
} Hero;

typedef struct Bindings {
  char up;
  char left;
  char down;
  char right;
  char exit;
} Bindings;


int main() {
  Hero pmc;
  Bindings keys;
  bool keepGoing = true;

  setUp(&pmc, &keys);

  while (keepGoing) {
    keepGoing = move(&pmc, &keys);
    printWorld(&pmc);
  }

  printf("\e[?25h");		/* show cusor */
  return 0;
}

void setUp(Hero *pmc, Bindings *keys) {
  srand(time(0));

  pmc->symbol = '@';
  printf("What is the name of your hero?: ");

  if (scanf("%s", pmc->name) != 1)
    printf("name invalid\n");

  pmc->x = rand() % WIDTH;
  pmc->y = rand() % WIDTH;
  pmc->score = 0;

  keys->up    = 'W';
  keys->left  = 'A';
  keys->down  = 'S';
  keys->right = 'D';
  keys->exit  = 'Q';

  printf("\e[?25l");		/* hide cursor */
}

void printWorld(Hero *pmc) {
  printf("\e[1;1H\e[2J");	/* return cursor to 1,1 and clear whole screen */
  
  char* top_and_bottom = repeatStr(WALL_CHARACTER, WIDTH + 2);
  printf("%s\n", top_and_bottom);

  for (int row = 0; row < WIDTH; row++) {
    printf("%s", WALL_CHARACTER); /* left border */

    for (int col = 0; col < WIDTH; col++) {
      if (row == pmc->y && col == pmc->x) {
	printf("%c", pmc->symbol);
      } else {
	printf("%c", EMPTY_CHARACTER);
      }
    }
    printf("%s\n", WALL_CHARACTER); /* right border */
  }
  printf("%s\n", top_and_bottom);
}

bool move(Hero *pmc, Bindings *keys) {
  printf("Move, %s [%c%c%c%c] \t %c to quit.\t SCORE: %d",
	 pmc->name, keys->up, keys->left, keys->down, keys->right, keys->exit, pmc->score);

  char keypress = toupper(getch());

  if (keypress == keys->up) {
    if (pmc->y - 1 >= 0)
      pmc->y--;
  } else if (keypress == keys->left) {
    if (pmc->x -1 >= 0)
      pmc->x--;
  } else if (keypress == keys->down) {
    if (pmc->y + 1 < WIDTH)
      pmc->y++;
  } else if (keypress == keys->right) {
    if (pmc->x + 1 < WIDTH)
      pmc->x++;
  } else if (keypress == keys->exit) {
    return false;
  }
  return true;
}

char* myStrCat (char *s, char *a) {
    while (*s != '\0') s++;
    while (*a != '\0') *s++ = *a++;
    *s = '\0';
    return s;
}

char* repeatStr (char *str, size_t count) {
    if (count == 0) return NULL;
    char *ret = malloc (strlen (str) * count + count);
    if (ret == NULL) return NULL;
    *ret = '\0';
    char *tmp = myStrCat (ret, str);
    while (--count > 0) {
        tmp = myStrCat (tmp, str);
    }
    return ret;
}

int getch() {
  /* Saves terminal settings, sets to non-cannonical mode,
   gets the next char typed, resets to old settings*/
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
