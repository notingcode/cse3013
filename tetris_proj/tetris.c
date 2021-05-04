#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC: recommendedPlay(); break;
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

	for(i=0; i<VISIBLE_BLOCKS; i++){
		nextBlock[i] = rand()%7;
	}
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
	InitTree();

	// 게임 실행시간 측정
	gameStart = time(NULL);
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
	gameStop = time(NULL);

	game_time = (double)difftime(gameStop, gameStart);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	if(play_auto){
		clear();
		printw("Play Time t: %lf\n", game_time);
		printw("score(t): %d\n", score);
		printw("time(t): %lf\n", tree_time);
		temp_space = (max_tree_space+min_tree_space)/2;
		printw("space(t): %ld\n", temp_space);
		getch();
		clear();
	}
	else newRank(score);
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

	// 블록의 좌표를 기준으로 블록 이동할 위치가 필드를 벗어나는지 확인
	for(y=0; y < BLOCK_HEIGHT; y++){
		for(x=0; x < BLOCK_WIDTH; x++){
			// 4*4 블록의 값들 중에 1이 있는 곳의 위치가 필드를 벗어난 지점에 있는지 확인
			if(block[currentBlock][blockRotate][y][x] == 1){
				y_cord = blockY+y;
				x_cord = blockX+x;
				// 블록의 필드를 벗어나거나 블록이 이동할 위치가 필드 위의 블록과 겹치는 경우 0을 반환
				if((y_cord < 0) || (y_cord >= HEIGHT) || (x_cord < 0) || (x_cord >= WIDTH)) return 0;
				if(f[y_cord][x_cord] == 1) return 0;
			}
		}
	}
	// 위에 해당되는 상황이 없으면 1을 반환하여 이동이 가능함을 알림
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	DrawField(); // 필드를 다시 그려 화면에 그려진 블록과 그림자를 지운다

	DrawRecommend(recommendY, recommendX, currentBlock, recommendR);

	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate); // 새로운 블록과 그 그림자를 화면에 그린다

	/*  그림자가 몇칸 아래에 있는지 DrawBlockWithFeatures 함수가 반환한다면 중복되는 연산 없이 블록과 그림자를 독립적으로 지울 수 있을 것 같다.
		하지만 위의 함수는 그렇지 않기 때문에 DrawField() 함수로 블록을 지우는 작업을 수행했다.*/
}

void BlockDown(int sig){
	int moveFlag; // 블록이 아래로 이동할 수 있는지 표시하는 flag
	moveFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX); // 아래의 칸으로 이동이 가능한지 확인

	// 아래로 이동이 가능하면 블록의 y좌표에 1을 더하고 변경된 위치에 블록을 다시 그린다
	if(moveFlag){
		// 자동 플레이 시 실행
		if(play_auto){
			blockX = recommendX;
			blockY = recommendY;
			blockRotate = recommendR;
		}
		else blockY++;
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
		for(int i=1; i<VISIBLE_BLOCKS; i++){
			nextBlock[i-1] = nextBlock[i];
		}
		nextBlock[VISIBLE_BLOCKS-1] = rand()%7;
		DrawField();
		clearTree(recRoot);
		CopyField(field, recRoot->recField);
		treeStart = time(NULL);
		// recommend(recRoot);
		modified_recommend(recRoot);
		treeStop = time(NULL);
		tree_time += (double)difftime(treeStop, treeStart);
		temp_space = evalSize(recRoot);
		max_tree_space = MAX(max_tree_space, temp_space);
		min_tree_space = MIN(min_tree_space, temp_space);
		findMaxScorePos();
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
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int idx;
	Node *new, *curr;

	// 파일 열기
	fp = fopen("rank.txt", "r");

	// 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문

	if(fscanf(fp, "%d", &num_items) > 0){
		nodeRoot = (Node*)malloc(sizeof(Node));
		curr = nodeRoot;
		fscanf(fp, "%s %d", curr->name, &curr->score);
		curr->link = NULL;
		for(idx=1; idx < num_items; idx++){
			new = (Node*)malloc(sizeof(Node));
			if(new == NULL){
				clear();
				printw("New node not allocated!\n");
				return;
			}
			else{
				fscanf(fp, "%s %d", new->name, &new->score);
				new->link=NULL;
				curr->link = new;
				curr = new;
			}
		}
	}
	
	// 파일닫기
	fclose(fp);

}

void rank(){
	// 목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	// 문자열 초기화
	int X=1, Y=num_items, ch, i;
	Node* curr;

	clear();

	// printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	// wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	echo();

	// 각 메뉴에 따라 입력받을 값을 변수에 저장
	// 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		printw("X: ");
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		printw("        name        |   score   \n");
		printw("--------------------------------\n");

		if(X < 1 || Y > num_items || X > Y){
			printw("Search Failure: No rank in the given range\n");
		}
		else{
			i = 1;
			curr = nodeRoot;
			while(i <= Y){
				if(i >= X){
					printw("%-20s|  %d\n", curr->name, curr->score);
				}
				curr = curr->link;
				i++;
			}
		}
	}

	// 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;

		printw("input the name: ");
		scanw("%s", str);
		curr = nodeRoot;

		printw("        name        |   score   \n");
		printw("--------------------------------\n");
		while(curr != NULL){
			if(!strcmp(curr->name, str)){
				printw("%-20s|   %d\n", curr->name, curr->score);
				check=1;
			}
			curr = curr->link;
		}

		if(!check){
			printw("search failure: no name is the list\n");
		}
	}

	// 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num, idx=1;
		Node* prev;

		printw("input the rank\n");
		scanw("%d", &num);
		curr = nodeRoot;
		prev=NULL;

		if(num > num_items || num < 1) printw("search failure: the rank not in the list\n");
		else{
			num_items--;
			if(num == 1){
				nodeRoot = curr->link;
				free(curr);
			}
			else{
				while(curr != NULL){
					if(idx == num){
						prev->link = curr->link;
						free(curr);

						break;
					}
					prev = curr;
					curr = curr->link;
					idx++;
				}	
			}
			printw("result: the rank deleted\n");
			writeRankFile();
		}
	}

	noecho();

	getch();
}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int i;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");
	Node *curr;

	curr = nodeRoot;

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", num_items); 
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	while(curr != NULL){
		fprintf(fp, "%s %d\n", curr->name, curr->score);
		curr = curr->link;
	}

	fclose(fp);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	Node *new, *curr, *prev;
	
	clear();
	echo();

	printw("Your Name: ");
	scanw("%s", str);

	//1. 사용자 이름을 입력받음
	//2. 새로운 노드를 생성해 이름과 점수를 저장
	new = (Node*)malloc(sizeof(Node));
	strcpy(new->name, str);
	new->score = score;
	new->link = NULL;

	// nodeRoot가 null인 경우 지금까지 저장된 랭크 데이터가 없는 것이기 때문에 nodeRoot에 새로운 노드 지정
	if(nodeRoot == NULL){
		nodeRoot = new;
	}
	// 그 외에는 노드를 한 개씩 이동하며 삽입할 위치를 찾는다.
	else{
		curr = nodeRoot;
		prev = NULL;
		while(curr != NULL){
			// 현재 노드의 점수가 새로운 랭크 데이터의 점수보다 작을 때 삽입 위치를 찾음
			if(curr->score < score) break;
			// 그것이 아니면 계속 다음 노드들을 탐색
			prev = curr;
			curr = curr->link;
		}
		/* 이전 노드의 포인터 값이 null이면 첫번째 노드에서 탐색이 끝난 것.
		   따라서 새로운 노드를 nodeRoot으로 지정하고 기존의 root를 nodeRoot의 다음 노드로 지정*/
		if(prev == NULL){
			new->link = nodeRoot;
			nodeRoot = new;
		}
		// 그 외에는 이전 노드의 다음 노드로 새로운 노드를 지정하고 현재 노드를 새로운 노드의 다음 노드로 지정
		else{
			prev->link = new;
			new->link = curr;
		}
	}

	noecho();

	num_items++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int recommend(RecNode *root){
	int max=0, num_childAdded=0, max_rotation, curr_lv, temp_y, temp_score; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	if(root->lv == -1) curr_lv=0;
	else curr_lv=root->lv;
	findMaxRotation(nextBlock[curr_lv], &max_rotation);
	root->child = (RecNode**)calloc(MAX_CHILDREN, sizeof(RecNode*));

	for(int rot=0; rot < max_rotation; rot++){
		for(int pos_X=-2; pos_X < WIDTH; pos_X++){
			if(CheckToMove(root->recField, nextBlock[curr_lv], rot, -1, pos_X)){
				temp_score=0;
				temp_y = -1;
				while(CheckToMove(root->recField, nextBlock[curr_lv], rot, temp_y+1, pos_X)) temp_y++;

				root->child[num_childAdded] = (RecNode*)calloc(1, sizeof(RecNode));
				root->child[num_childAdded]->lv = curr_lv + 1;
				root->child[num_childAdded]->recBlockRotate = rot;
				root->child[num_childAdded]->recBlockX = pos_X;
				root->child[num_childAdded]->recBlockY = temp_y;
				CopyField(root->recField, root->child[num_childAdded]->recField);
				temp_score = AddBlockToField(root->child[num_childAdded]->recField, nextBlock[curr_lv], rot, temp_y, pos_X);
				temp_score += DeleteLine(root->child[num_childAdded]->recField);
				temp_score += (temp_y*temp_y);
				if(root->child[num_childAdded]->lv < VISIBLE_BLOCKS) temp_score += recommend(root->child[num_childAdded]);
				root->child[num_childAdded]->score = temp_score;
				num_childAdded++;
			}
		}
	}

	for(int i=0; i < num_childAdded; i++){
		max = MAX(max, root->child[i]->score);
	}

	return max;
}

int modified_recommend(RecNode *root){
	int max=0, max_idx=0, num_childAdded=0, max_rotation, curr_lv, temp_y, temp_score, end_idx;

	if(root->lv == -1) curr_lv=0;
	else curr_lv=root->lv;

	findMaxRotation(nextBlock[curr_lv], &max_rotation);
	end_idx=max_rotation*9;
	root->child = (RecNode**)calloc(end_idx+1, sizeof(RecNode*));

	root->child[end_idx] = (RecNode*)calloc(1, sizeof(RecNode));
	for(int rot=0; rot < max_rotation; rot++){
		for(int pos_X=-2; pos_X < WIDTH; pos_X++){
			if(CheckToMove(root->recField, nextBlock[curr_lv], rot, -1, pos_X)){
				temp_score=0;
				temp_y = -1;
				while(CheckToMove(root->recField, nextBlock[curr_lv], rot, temp_y+1, pos_X)) temp_y++;

				root->child[end_idx]->lv = curr_lv + 1;
				root->child[end_idx]->recBlockRotate = rot;
				root->child[end_idx]->recBlockX = pos_X;
				root->child[end_idx]->recBlockY = temp_y;
				CopyField(root->recField, root->child[end_idx]->recField);
				temp_score = AddBlockToField(root->child[end_idx]->recField, nextBlock[curr_lv], rot, temp_y, pos_X);
				temp_score += DeleteLine(root->child[end_idx]->recField);
				temp_score += (temp_y*temp_y);
				root->child[end_idx]->score = temp_score;
				if(temp_score > max){
					max = temp_score;
					root->child[num_childAdded++] = root->child[end_idx];
					root->child[end_idx] = (RecNode*)calloc(1, sizeof(RecNode));
				}
				else if((num_childAdded <= end_idx/2) && (rand()%2)){
					root->child[num_childAdded++] = root->child[end_idx];
					root->child[end_idx] = (RecNode*)calloc(1, sizeof(RecNode));
				}
			}
		}
	}
	free(root->child[end_idx]);
	root->child[end_idx] = NULL;

	for(int i=0; i < num_childAdded; i++){
		if(root->child[i]->lv < VISIBLE_BLOCKS) root->child[i]->score += recommend(root->child[i]);
	}

	for(int i=0; i < num_childAdded; i++){
		if(root->child[i]->score > root->child[max_idx]->score) max_idx=i;
	}

	return root->child[max_idx]->score;
}

void recommendedPlay(){
	play_auto=1;
	play();
	play_auto=0;
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}

void CopyField(char f1[HEIGHT][WIDTH], char f2[HEIGHT][WIDTH]){
	for(int i=0; i < WIDTH; i++){
		for(int j=0; j < HEIGHT; j++){
			f2[j][i] = f1[j][i];
		}
	}
}

int clearTree(RecNode *root){
	if(root->child){
		for(int i=0; root->child[i]; i++){
			clearTree(root->child[i]);
			free(root->child[i]);
		}
		free(root->child);
	}

    return 0;
}

void InitTree(){
	recRoot = (RecNode*)calloc(1, sizeof(RecNode));
	recRoot->lv = -1;
	CopyField(field, recRoot->recField);
	// recommend(recRoot);
	modified_recommend(recRoot);
	min_tree_space = max_tree_space = evalSize(recRoot);
	findMaxScorePos();
}

void findMaxScorePos(){
	int max=0, i=0;
	if(recRoot->child){
		while((*(recRoot->child+i)) != NULL){
			if(max < (*(recRoot->child+i))->score){
				recommendY = (*(recRoot->child+i))->recBlockY;
				recommendX = (*(recRoot->child+i))->recBlockX;
				recommendR = (*(recRoot->child+i))->recBlockRotate;
				max = (*(recRoot->child+i))->score;
			}
			i++;
		}
	}
}

void findMaxRotation(int blockID, int* rotation){
	int size;

	switch(blockID){
		case 4:
			*rotation=1;
			break;
		case 0:
		case 5:
		case 6:
			*rotation=2;
			break;
		default:
			*rotation=4;
			break;
	}
}

/*이 기능을 구현하기 위해선 modified_recommend 함수 부분적으로 수정필요*/

// int CheckHeight(char h[WIDTH], int currentBlock, int blockRotate, int blockY, int blockX){
// 	int y, x, y_cord, x_cord;

// 	// 블록의 좌표를 기준으로 블록 이동할 위치가 필드를 벗어나는지 확인
// 	for(y=0; y < BLOCK_HEIGHT; y++){
// 		for(x=0; x < BLOCK_WIDTH; x++){
// 			// 4*4 블록의 값들 중에 1이 있는 곳의 위치가 필드를 벗어난 지점에 있는지 확인
// 			if(block[currentBlock][blockRotate][y][x] == 1){
// 				y_cord = blockY+y;
// 				x_cord = blockX+x;
// 				// 블록의 필드를 벗어나거나 블록이 이동할 위치가 높이와 겹치는 경우 0을 반환
// 				if((y_cord < 0) || (y_cord >= HEIGHT) || (x_cord < 0) || (x_cord >= WIDTH)) return 0;
// 				if(h[x_cord] >= y_cord) return 0;
// 			}
// 		}
// 	}
// 	// 위에 해당되는 상황이 없으면 1을 반환하여 이동이 가능함을 알림
// 	return 1;
// }

// int ChangeHeight(char h[WIDTH], int currentBlock, int blockRotate, int blockY, int blockX){
// 	int i, j, y_cord, x_cord, touched=0;

// 	for(j=0; j < BLOCK_HEIGHT; j++){
// 		for(i=0; i < BLOCK_WIDTH; i++){
// 			y_cord = blockY+j;
// 			x_cord = blockX+i;
// 			if(block[currentBlock][blockRotate][j][i] == 1){
// 				if(h[x_cord] == y_cord-1 || y_cord+1 == HEIGHT) touched++;
// 				h[x_cord] = y_cord;
// 			}
// 		}
// 	}

// 	return touched*10; // 점수를 반환
// }

// void CopyHeight(char h1[WIDTH], char h2[WIDTH]){
// 	for(int i=0; i<WIDTH; i++) h2[i] = h1[i];
// }

// int DeleteHeight(char h[WIDTH]){
// 	int y, x, temp_y, temp_x;
// 	int isfull, line_removed;
	
// 	line_removed = 0;

// 	return (line_removed * line_removed) * 100;	// 점수를 계산하여 반환
// }

long evalSize(RecNode *root){
	long size;
	size = sizeof(RecNode) + (HEIGHT * WIDTH * sizeof(char));
	if(root->child){
		for(int i=0; root->child[i]; i++){
			size += evalSize(root->child[i]);
		}
		return size;
	}
	else return 0;
}