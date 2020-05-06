#include "tetris.h"

static struct sigaction act, oact;

int main() {
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));
	createRankList();

	while (!exit) {
		clear();
		switch (menu()) {
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
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
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline() {
	int i, j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0, 0, HEIGHT, WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);
	DrawBox(10, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17, WIDTH + 10);
	printw("SCORE");
	DrawBox(18, WIDTH + 10, 1, 8);
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
	move(19, WIDTH + 11);
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
		move(11 + i, WIDTH + 13);
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

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			//블락중에 바뀌는 거 일때 체크
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (f[i + blockY][j + blockX] == 1) return 0;
				if (i + blockY >= HEIGHT || i + blockY < 0) return 0;
				if (j + blockX >= WIDTH || j + blockX < 0) return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다.
	int preR = blockRotate;
	int preX = blockX;
	int preY = blockY;
	int flag = 1;

	//이전 블럭의 형태 계산
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

	while (flag) {
		flag = CheckToMove(field, currentBlock, preR, ++preY, preX);
	}
	preY--;

	//기존의 그림자 삭제
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (block[currentBlock][preR][i][j] == 1 && i + preY >= 0) {
				move(i + preY + 1, j + preX + 1);
				printw(".");
			}
		}
	}

	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	//move(HEIGHT, WIDTH + 10);
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code

	int touched = 0;

	//Block이 추가된 영역의 필드값을 바꾼다.
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				f[i + blockY][j + blockX] = 1;
				if ((blockY + i + 1) == HEIGHT) touched++;
			}
		}
	}
	return touched * 10;
}

void BlockDown(int sig) {
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		move(HEIGHT, WIDTH + 10);
	}
	else {
		if (blockY <= -1) {
			gameOver = 1;
			return;
		}

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;

		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawField();

		DrawNextBlock(nextBlock);
		PrintScore(score);
	}
	timed_out = 0;
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
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

void DrawBlockWithFeatures(int blockY, int blockX, int blockID, int blockRotate) {
	DrawShadow(blockY, blockX, blockID, blockRotate);
	DrawBlock(blockY, blockX, blockID, blockRotate, ' ');
}

void DrawShadow(int y, int x, int blockID, int blockRotate) {
	int avail = 1;

	while (avail) avail = CheckToMove(field, blockID, blockRotate, ++y, x);

	--y;
	DrawBlock(y, x, blockID, blockRotate, '/');
}

void createRankList() {
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE* fp;
	int i, j;
	Node* newNode;
	Node* cNode;

	head = (Node*)malloc(sizeof(Node));
	head->link = NULL;
	cNode = head;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	//freopen("rank.txt", "r", stdin);
	fscanf(fp, "%d", &score_number);

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문

	for (int i = 0; i < score_number; ++i) {
		newNode = (Node*)malloc(sizeof(Node));
		fscanf(fp, "%s %d", newNode->name, &(newNode->score));
		newNode->link = NULL;
		cNode->link = newNode;
		cNode = newNode;
	}
	fclose(fp);
}

void rank() {
	// user code
	int X = 1, Y = score_number, ch, i, j;
	Node* cNode = head;
	clear();

	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	ch = wgetch(stdscr);

	if (ch == '1') {
		printw("X: ");
		echo();
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		noecho();

		printw("	name   	|	score	\n");
		printw("---------------------------------\n");
		if (X > Y || score_number == 0 || X > score_number)
			mvprintw(8, 0, "search failure: no rank in the list\n");
		else {
			//채워 넣기
			for (i = 0; i < X; ++i) {
				cNode = cNode->link;
			}

			while (i++ <= Y) {
				printw("%15s | %d\n", cNode->name, cNode->score);
				cNode = cNode->link;
			}
		}
	}
	else if (ch == '2') {

	}
	else if (ch == '3') {

	}
	wgetch(stdscr);
}

void writeRankFile() {
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	FILE * fp = fopen("rank.txt", "w");
	Node* cNode = head->link;

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d", score_number);

	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	while (cNode) {
		fprintf(fp, "%s %d\n", cNode->name, cNode->score);
		cNode = cNode->link;
	}

	fclose(fp);
}

void newRank(int score) {
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN + 1];
	int i, j;
	Node* newNode = (Node*)malloc(sizeof(Node));
	Node* pNode = head, *cNode;
	clear();

	echo();
	//1. 사용자 이름을 입력받음
	printw("Input your name: ");
	scanw("%s", str);
	strcpy(newNode->name, str);
	noecho();

	newNode->score = score;
	newNode->link = NULL;

	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	if (score_number == 0) {
		head->link = newNode;
	}
	else {
		while(pNode->link) {
			cNode = pNode;
			pNode = pNode->link;
			if (score > pNode->score) {
				cNode->link= newNode;
				newNode->link = pNode;
				break;
			}
		}

		if (!(pNode->link)) {
			if (score > pNode->score) {
				cNode->link = newNode;
				newNode->link = pNode;
			}
			else {
				pNode->link = newNode;
			}
		}
	}
	score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID, int blockRotate) {
	// user code
}

int recommend(RecNode* root) {
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay() {
	// user code
}
