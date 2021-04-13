#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 테두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 테두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	/* next next block을 보여주는 공간의 테두리를 그린다.*/
	move(8,WIDTH+10);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 테두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
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

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	for(i=0; i < 4; i++){
		move(10+i, WIDTH+13);
		for(j=0; j < 4; j++){
			if(block[nextBlock[2]][0][i][j] == 1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int y, x, y_cord, x_cord;

	// 
	for(y=0; y < BLOCK_HEIGHT; y++){
		for(x=0; x < BLOCK_WIDTH; x++){
			if(block[currentBlock][blockRotate][y][x] == 1){
				y_cord = blockY+y;
				x_cord = blockX+x;
				if((y_cord < 0) || (y_cord >= HEIGHT) || (x_cord < 0) || (x_cord >= WIDTH)) return 0;
				if(f[y_cord][x_cord] == 1) return 0;
			}
		}
	}
	
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	
	DrawField(); // 필드를 다시 그려 화면에 그려진 블록과 그림자를 지운다

	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate); // 새로운 블록과 그 그림자를 화면에 그린다

	/*  그림자가 몇칸 아래에 있는지 DrawBlockWithFeatures 함수가 반환한다면 중복되는 연산 없이 블록과 그림자를 독립적으로 지울 수 있을 것 같다.
		하지만 위의 함수는 그렇지 않기 때문에 DrawField() 함수로 블록을 지우는 작업을 수행했다.*/
}

void BlockDown(int sig){
	int moveFlag; // 블록이 아래로 이동할 수 있는지 표시하는 flag
	moveFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX); // 아래의 칸으로 이동이 가능한지 확인

	// 아래로 이동이 가능하면 블록의 y좌표에 1을 더하고 변경된 위치에 블록을 다시 그린다
	if(moveFlag){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else{
		// 블록의 y좌표가 -1이면 현재 블록이 필드를 벗어난 것이기 때문에 게임을 종료
		if(blockY == -1){
			gameOver = 1;
			return;
		}

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX); // 블록을 필드에 저장하고 함수에서 반환된 점수를 score에 더한다
		blockY = -1, blockX = (WIDTH)/2-2, blockRotate=0; // 블록을 초기화
		score += DeleteLine(field); // 필드에서 가득찬 줄들을 지우고 함수에서 반환된 점수를 score에 더한다
		PrintScore(score);
		nextBlock[0] = nextBlock[1]; // 현재 블록을 다음 블록으로 지정
		nextBlock[1] = nextBlock[2]; // 다음 블록을 다다음 블록으로 지정
		nextBlock[2] = rand()%7; // 새로운 다다음 블록을 랜덤하게 지정
		DrawField();
		DrawNextBlock(nextBlock); // 앞으로 나올 블록들을 각자의 상자에 출력
	}

	timed_out=0; // 다음 시간에 BlockDown이 실행될 수 있도록 timed_out을 0으로 초기화
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j, y_cord, x_cord, touched=0;

	for(j=0; j < BLOCK_HEIGHT; j++){
		for(i=0; i < BLOCK_WIDTH; i++){
			// 4*4 블록의 좌측상단의 좌표를 기준으로 배열을 돈다
			y_cord = blockY+j;
			x_cord = blockX+i;
			if(block[currentBlock][blockRotate][j][i] == 1){
				// 만약 block의 element가 1이면 해당 element 아래에 필드가 접하는지 확인한다. 접하면 touched에 1을 더한다.
				if(f[y_cord+1][x_cord] == 1 || y_cord+1 == HEIGHT) touched++;
				// block의 element가 1이면 element와 겹치는 필드 위치의 값을 1로 바꾼다
				f[y_cord][x_cord] = 1;
			}
		}
	}

	return touched*10; // 점수를 반환
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int y, x, temp_y, temp_x;
	int isfull, line_removed;
	
	line_removed = 0;

	for(y=0; y < HEIGHT; y++){ // 필드 맨 밑줄에서 윗줄까지 확인
		isfull = 0;
		// 각 줄이 가득차있는지 확인하고 빈칸이 있으면 반복문에서 escape
		for(x=0; x < WIDTH; x++){
			if(f[y][x] != 1) break;
			isfull += 1; // 반복문을 돌며 isfull에 1을 더한다
		}
		// 줄이 차있으면 줄을 삭제하고 위의 줄들을 내리는 작업을 수행
		if(isfull == WIDTH){
			line_removed++; // 줄을 지울 때마다 line_removed에 1을 더한다
			// 지워야 하는 줄에서 시작하여 위의 줄들을 한개씩 내린다
			for(temp_y=y; temp_y > 0; temp_y--){
				for(temp_x=0; temp_x < WIDTH; temp_x++){
					f[temp_y][temp_x] = f[temp_y-1][temp_x];
				}
			}
			for(temp_x=0; temp_x < WIDTH; temp_x++) f[temp_y][temp_x] = '.'; // 맨 위의 줄은 비어있는 줄이 되기 때문에 '.'으로 채운다
		}
	}

	return (line_removed * line_removed) * 100;	// 점수를 계산하여 반환
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// 블록이 내려갈 수 있는지 반복하여 확인하며 내려갈 수 있을 때마다 y에 1을 더한다
	while(CheckToMove(field, blockID, blockRotate, y+1, x)) y++;

	DrawBlock(y, x, blockID, blockRotate, '/'); // 그림자는 위 반복문을 통해 구한 y와 '/' 기호를 사용해 그린다
}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}
