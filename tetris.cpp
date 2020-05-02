#include "tetris.h"

static struct sigaction act, oact;

int main() {
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while (!exit) {
		clear();
		switch (menu()) {
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit = 1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris() {
	int i, j;

	for (j = 0; j < HEIGHT; j++)
		for (i = 0; i < WIDTH; i++)
			field[j][i] = 0;

	nextBlock[0] = rand() % 7;
	nextBlock[1] = rand() % 7;
	nextBlock[2] = rand() % 7;
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline() {
	int i, j;
	/* ���� �������� ������ �µθ��� �׸���.*/
	DrawBox(0, 0, HEIGHT, WIDTH);

	/* next block�� �����ִ� ������ �µθ��� �׸���.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);

	/* score�� �����ִ� ������ �µθ��� �׸���.*/
	move(9, WIDTH + 10);
	printw("SCORE");
	DrawBox(10, WIDTH + 10, 1, 8);
}

int GetCommand() {
	int command;
	command = wgetch(stdscr);
	switch (command) {
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command) {
	int ret = 1;
	int drawFlag = 0;
	switch (command) {
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)))
			blockRotate = (blockRotate + 1) % 4;
		break;
	case KEY_DOWN:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)))
			blockX++;
		break;
	case KEY_LEFT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)))
			blockX--;
		break;
	default:
		break;
	}
	if (drawFlag) DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
	return ret;
}

void DrawField() {
	int i, j;
	for (j = 0; j < HEIGHT; j++) {
		move(j + 1, 1);
		for (i = 0; i < WIDTH; i++) {
			if (field[j][i] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score) {
	move(11, WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int* nextBlock) {
	int i, j;
	for (i = 0; i < 4; i++) {
		move(4 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[1]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	for (i = 0; i < 4; i++) {
		move(12 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile) {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT, WIDTH + 10);
}

void DrawBox(int y, int x, int height, int width) {
	int i, j;
	move(y, x);
	addch(ACS_ULCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for (j = 0; j < height; j++) {
		move(y + j + 1, x);
		addch(ACS_VLINE);
		move(y + j + 1, x + width + 1);
		addch(ACS_VLINE);
	}
	move(y + j + 1, x);
	addch(ACS_LLCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play() {
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();

	do {
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessCommand(command) == QUIT) {
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu() {
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////ù���� �ǽ����� �����ؾ� �� �Լ�/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			//����߿� �ٲ�� �� �϶� üũ
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (i + blockY >= HEIGHT || j + blockX >= WIDTH
					|| i + blockY < 0 || j + blockX < 0) return 0;
				if (field[i + blockY][j + blockX] == 1) return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code

	//1. ���� ��� ������ ã�´�. ProcessCommand�� switch���� ������ ��
	//2. ���� ��� ������ �����. DrawBlock�Լ� ������ ��.
	//3. ���ο� ��� ������ �׸���.
	int preR = blockRotate;
	int preX = blockX;
	int preY = blockY;

	//���� ���� ���� ���
	switch (command) {
	case KEY_UP:
		preR = (preR + 3) % 4;
		break;
	case KEY_DOWN:
		preY--;
		break;
	case KEY_LEFT:
		preX++;
		break;
	case KEY_RIGHT:
		preX--;
		break;
	default:
		break;
	}

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (block[currentBlock][preR][i][j] == 1 && i + preY >= 0) {
				move(i + preY + 1, j + preX + 1);
				printw(".");
			}
		}
	}

	DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');
	move(HEIGHT, WIDTH + 10);
}

void BlockDown(int sig) {
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)==1) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		move(HEIGHT, WIDTH + 10);
	}
	else {
		if (blockY <= -1) {
			gameOver = 1;
			return;
		}
		AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;

		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
		DrawField();

		DrawNextBlock(nextBlock);
		PrintScore(score);
	}
	timed_out = 0;
}



void AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code

	//Block�� �߰��� ������ �ʵ尪�� �ٲ۴�.
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (block[currentBlock][blockRotate][i][j] == 1) f[i + blockY][j + blockX] = 1;
		}
	}
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	// user code

	//1. �ʵ带 Ž���Ͽ�, �� �� ������ �ִ��� Ž���Ѵ�.
	//2. �� �� ������ ������ �ش� ������ �����. ��, �ش� �������� �ʵ尪�� ��ĭ�� ������.
	int cnt = 0;
	int j = 0;
	for (int i = 0; i < HEIGHT; ++i) {
		int flag = 1;
		for (int j = 0; j < WIDTH; ++j) {
			if (f[i][j] == 0) {
				flag = 0;
				break;
			}
		}
		if (flag == 1) {
			for (int j = i; j > 0; --j) {
				for (int k = 0; k < WIDTH; ++k) {
					f[j][k] = f[j - 1][k];
				}
			}
			cnt++;
		}
	}
	return (cnt * cnt * 100);
}

///////////////////////////////////////////////////////////////////////////

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void DrawShadow(int y, int x, int blockID, int blockRotate) {
	int avail = 1;

	while (avail) avail = CheckToMove(field, blockID, blockRotate, ++y, x);

	if(avail) DrawBlock(--y, x, blockID, blockRotate, '/');
}

void createRankList() {
	// user code
}

void rank() {
	// user code
}

void writeRankFile() {
	// user code
}

void newRank(int score) {
	// user code
}

void DrawRecommend(int y, int x, int blockID, int blockRotate) {
	// user code
}

int recommend(RecNode* root) {
	int max = 0; // �̸� ���̴� ���� ��õ ��ġ���� ������� �� ���� �� �ִ� �ִ� ����

	// user code

	return max;
}

void recommendedPlay() {
	// user code
}
