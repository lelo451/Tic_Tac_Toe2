#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
int board[25] = {
	:,:,:,:,:,
	:,-,-,-,:,
	:,-,-,-,:,
	:,-,-,-,:,
	:,:,:,:,:

	 0, 1, 2, 3, 4,
	 5, 6, 7, 8, 9,
	10,11,12,13,14,
	15,16,17,18,19,
	20,21,22,23,24
}
*/

enum {NOUGHTS, CROSSES, BORDER, EMPTY};
enum {HUMANWIN, COMPWIN, DRAW};

const int Directions[4] = {1, 5, 4, 6};

const int ConvertTo25[9] = {
        6, 7, 8,
        11, 12, 13,
        16, 17, 18
};

const int InMiddle = 4;
const int Corners[4] = {0, 2, 6, 8};

int ply = 0;
int positions = 0;
int maxPly = 0;

int GetNumberForDir(int startSq, const int dir, const int *board, const int us){
    int found = 0;
    while(board[startSq] != BORDER){
        if(board[startSq] != us){
            break;
        }
        found++;
        startSq += dir;
    }

    return found;
}

int FindThreeInArow(const int *board, const int ourindex, const int us){
    int DirIndex = 0;
    int Dir = 0;
    int threeCount = 1;

    for(DirIndex = 0; DirIndex < 4; ++DirIndex){
        Dir = Directions[DirIndex];
        threeCount += GetNumberForDir(ourindex + Dir, Dir, board, us);
        threeCount += GetNumberForDir(ourindex  + Dir * -1, Dir * -1, board, us);
        if(threeCount == 3){
            break;
        }
        threeCount = 1;
    }
    return threeCount;
}

int FindThreeInArowAllBoard(const int *board, const int us){
    int threeFound = 0;
    int index;

    for(index = 0; index < 9; ++index){
        if(board[ConvertTo25[index]] == us){
            if(FindThreeInArow(board, ConvertTo25[index], us) == 3){
                threeFound = 1;
                break;
            }
        }
    }
    return threeFound;
}

int EvalForWin(const int *board, const int us){
    if(FindThreeInArowAllBoard(board, us) != 0){
        return 1;
    }
    if(FindThreeInArowAllBoard(board, us ^ 1) != 0){
        return -1;
    }
    return 0;
}

int MinMax(int *board, int side){
    int MoveList[9];
    int MoveCount = 0;
    int bestScore = -2;
    int score = -2;
    int bestMove = -1;
    int Move;
    int index;

    if(ply > maxPly) maxPly = ply;
    positions++;

    if(ply > 0){
        score = EvalForWin(board, side);
        if(score != 0){
            return score;
        }
    }

    //preenche Move List
    for(index = 0; index < 9; ++index){
        if(board[ConvertTo25[index]] == EMPTY){
            MoveList[MoveCount++] = ConvertTo25[index];
        }
    }

    //coloca todos os movimentos em loop
    for(index = 0; index < MoveCount; ++index){
        Move = MoveList[index];
        board[Move] = side;

        ply++;
        score = -MinMax(board, side ^ 1);
        if(score > bestScore){
            bestScore = score;
            bestMove = Move;
        }
        board[Move] = EMPTY;
        ply--;
    }

    if(MoveCount == 0){
        bestScore = FindThreeInArowAllBoard(board, side);
    }

    if(ply != 0){
        return bestScore;
    } else{
        return bestMove;
    }
}

void InitialiseBoard(int *board){
    int index = 0;

    for(index = 0; index < 25; ++index){
        board[index] = BORDER;
    }
    for(index = 0; index < 9; ++index){
        board[ConvertTo25[index]] = EMPTY;
    }
}

void PrintBoard(const int *board){
    int index = 0;
    char pceChars[] = "OX|-";
    printf("\n\nTabuleiro:\n\n");
    for(index = 0; index < 9; ++index){
        if(index != 0 && index % 3 == 0){
            printf("\n\n");
        }
        printf("%4c", pceChars[board[ConvertTo25[index]]]);
    }
    printf("\n");
}

int HasEmpty(const int *board){
    int index = 0;
    for(index = 0; index < 9; ++index){
        if(board[ConvertTo25[index]] == EMPTY){
            return 1;
        }
    }
    return 0;
}

void MakeMove(int *board, const int sq, const side){
    board[sq] = side;
}

int GetNextBest(const int *board){
    int ourMove = ConvertTo25[InMiddle];
    if(board[ourMove] == EMPTY){
        return ourMove;
    }

    int index = 0;
    ourMove = -1;

    for(index = 0; index < 4; index++){
        ourMove = ConvertTo25[Corners[index]];
        if(board[ourMove] == EMPTY){
            break;
        }
        ourMove = -1;
    }
    return ourMove;
}

int GetWinningMove(int *board, const int side){
    int ourMove = -1;
    int winFound = 0;
    int index = 0;

    for(index = 0; index < 9; ++index){
        if(board[ConvertTo25[index]] == EMPTY){
            ourMove = ConvertTo25[index];
            board[ourMove] = side;

            if(FindThreeInArow(board, ourMove, side) == 3){
                winFound = 1;
            }
            board[ourMove] = EMPTY;
            if(winFound == 1){
                break;
            }
            ourMove = -1;
        };
    }
    return ourMove;
}

int GetComputerHMove(int *board, const int side){
    ply = 0;
    positions = 0;
    maxPly = 0;
    int best = MinMax(board, side);
    printf("Terminada a busca de posicoes: %d Profundidade maxima: %d Melhor movimento: %d\n",positions,maxPly,best);
    return best;
}

int GetComputerMMove(int *board, const int side){
    int index = 0;
    int numFree = 0;
    int availableMoves[9];
    int randMove = 0;

    randMove = GetWinningMove(board, side);
    if(randMove != -1) {
        return randMove;
    }

    randMove = GetWinningMove(board, side ^ 1 );
    if(randMove != -1) {
        return randMove;
    }

    randMove = GetNextBest(board);
    if(randMove != -1) {
        return randMove;
    }

    randMove = 0;
    for(index = 0; index < 9; ++index) {
        if( board[ConvertTo25[index]] == EMPTY) {
            availableMoves[numFree++] = ConvertTo25[index];
        };
    }

    randMove = (rand() % numFree);
    return availableMoves[randMove];
}

int GetComputerBMove(int *board, const int side) {
    int index = 0;
    int numFree = 0;
    int availableMoves[9];
    int randMove = 0;

    randMove = GetWinningMove(board, side);
    if(randMove != -1) {
        return randMove;
    }

    randMove = 0;
    for(index = 0; index < 9; ++index) {
        if( board[ConvertTo25[index]] == EMPTY) {
            availableMoves[numFree++] = ConvertTo25[index];
        };
    }

    randMove = (rand() % numFree);
    return availableMoves[randMove];
}

int GetComputerMove(const int *board){
    int index = 0;
    int numFree = 0;
    int availableMoves[9];
    int randMove = 0;
    int move = 0;

    /*
    2, 4, 8
        availableMoves[0] = 2 numFree++ -> 1;
        availableMoves[numFree] = 4 numFree++ -> 2;
        availableMoves[numFree] = 8 numFree -> 3;

        rand() % numFree retorna de 0 a 2;

        rand de 0 a 2, retorna availableMoves[rand];
    */

    for(index = 0; index < 9; ++index){
        if(board[ConvertTo25[index]] == EMPTY){
            availableMoves[numFree++] = ConvertTo25[index];
        };
    }

    randMove = (rand() % numFree);
    return availableMoves[randMove];
}

int GetHumanMove(const int *board){
    char userInput[4];

    int moveOk = 0;
    int move = -1;

    while(moveOk == 0){
        printf("Entre com um movimento para a O de 1 a 9: ");
        fgets(userInput, 3, stdin);
        fflush(stdin);

        if(strlen(userInput) != 2){
            printf("invalido\n");
            continue;
        }

        if(sscanf(userInput, "%d", &move) != 1){
            move = -1;
            printf("invalido\n");
            continue;
        }

        if(move < 1 || move > 9){
            move = -1;
            printf("Numero invalido\n");
            continue;
        }

        move--; //index zero

        if(board[ConvertTo25[move]] != EMPTY){
            move = -1;
            printf("Posicao invalida\n");
            continue;
        }

        moveOk = 1;
    }

    printf("Fazendo movimento de O...%d\n", (move + 1));
    return ConvertTo25[move];
}

int GetHuman2Move(const int *board){
    char userInput[4];

    int moveOk = 0;
    int move = -1;

    while(moveOk == 0){
        PrintBoard(&board[0]);
        printf("Entre com um movimento para o X de 1 a 9: ");
        fgets(userInput, 3, stdin);
        fflush(stdin);

        if(strlen(userInput) != 2){
            printf("invalido\n");
            continue;
        }

        if(sscanf(userInput, "%d", &move) != 1){
            move = -1;
            printf("invalido\n");
            continue;
        }

        if(move < 1 || move > 9){
            move = -1;
            printf("Numero invalido\n");
            continue;
        }

        move--; //index zero

        if(board[ConvertTo25[move]] != EMPTY){
            move = -1;
            printf("Posicao invalida\n");
            continue;
        }

        moveOk = 1;
    }

    printf("Fazendo movimento de X...%d\n", (move + 1));
    return ConvertTo25[move];
}

void RunGame(){

    int GameOver = 0;
    int Side = NOUGHTS;
    int LastMoveMade = 0;
    int board[25];

    InitialiseBoard(&board[0]);
    PrintBoard(&board[0]);

    while(!GameOver){
        if(Side == NOUGHTS){
            LastMoveMade = GetHumanMove(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = CROSSES;
        } else{
            LastMoveMade = GetComputerMove(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(&board[0]);
        }

        if(FindThreeInArow(board, LastMoveMade, Side ^ 1) == 3){
            printf("Fim de jogo!\n");
            GameOver = 1;
            if(Side == NOUGHTS){
                printf("Computador ganhou\n");
            } else{
                printf("Voce ganhou\n");
            }
        }

        if(!HasEmpty(board)){
            printf("Fim de jogo!\n");
            GameOver = 1;
            printf("Deu velha!\n");
        }
    }
    PrintBoard(&board[0]);
    tryAgain();
}

void RunPvPGame(){

    int GameOver = 0;
    int Side = NOUGHTS;
    int LastMoveMade = 0;
    int board[25];

    InitialiseBoard(&board[0]);
    PrintBoard(&board[0]);

    while(!GameOver){
        if(Side == NOUGHTS){
            LastMoveMade = GetHumanMove(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = CROSSES;
        } else{
            LastMoveMade = GetHuman2Move(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(&board[0]);
        }

        if(FindThreeInArow(board, LastMoveMade, Side ^ 1) == 3){
            printf("Fim de jogo!\n");
            GameOver = 1;
            if(Side == NOUGHTS){
                printf("Player 2 ganhou\n");
            } else{
                printf("Player 1 ganhou\n");
            }
        }

        if(!HasEmpty(board)){
            printf("Fim de jogo!\n");
            GameOver = 1;
            printf("Deu velha!\n");
        }
    }
    PrintBoard(&board[0]);
    tryAgain();
}

void RunBGame(){

    int GameOver = 0;
    int Side = NOUGHTS;
    int LastMoveMade = 0;
    int board[25];

    InitialiseBoard(&board[0]);
    PrintBoard(&board[0]);

    while(!GameOver){
        if(Side == NOUGHTS){
            LastMoveMade = GetHumanMove(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = CROSSES;
        } else{
            LastMoveMade = GetComputerBMove(&board[0], Side);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(&board[0]);
        }

        if(FindThreeInArow(board, LastMoveMade, Side ^ 1) == 3){
            printf("Fim de jogo!\n");
            GameOver = 1;
            if(Side == NOUGHTS){
                printf("Computador ganhou\n");
            } else{
                printf("Voce ganhou\n");
            }
        }

        if(!HasEmpty(board)){
            printf("Fim de jogo!\n");
            GameOver = 1;
            printf("Deu velha!\n");
        }
    }
    PrintBoard(&board[0]);
    tryAgain();
}

void RunMGame(){

    int GameOver = 0;
    int Side = NOUGHTS;
    int LastMoveMade = 0;
    int board[25];

    InitialiseBoard(&board[0]);
    PrintBoard(&board[0]);

    while(!GameOver){
        if(Side == NOUGHTS){
            LastMoveMade = GetHumanMove(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = CROSSES;
        } else{
            LastMoveMade = GetComputerMMove(&board[0], Side);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(&board[0]);
        }

        if(FindThreeInArow(board, LastMoveMade, Side ^ 1) == 3){
            printf("Fim de jogo!\n");
            GameOver = 1;
            if(Side == NOUGHTS){
                printf("Computador ganhou\n");
            } else{
                printf("Voce ganhou\n");
            }
        }

        if(!HasEmpty(board)){
            printf("Fim de jogo!\n");
            GameOver = 1;
            printf("Deu velha!\n");
        }
    }
    PrintBoard(&board[0]);
    tryAgain();
}

void RunHGame(){

    int GameOver = 0;
    int Side = NOUGHTS;
    int LastMoveMade = 0;
    int board[25];

    InitialiseBoard(&board[0]);
    PrintBoard(&board[0]);

    while(!GameOver){
        if(Side == NOUGHTS){
            LastMoveMade = GetHumanMove(&board[0]);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = CROSSES;
        } else{
            LastMoveMade = GetComputerHMove(&board[0], Side);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(&board[0]);
        }

        if(FindThreeInArow(board, LastMoveMade, Side ^ 1) == 3){
            printf("Fim de jogo!\n");
            GameOver = 1;
            if(Side == NOUGHTS){
                printf("Computador ganhou\n");
            } else{
                printf("Voce ganhou\n");
            }
        }

        if(!HasEmpty(board)){
            printf("Fim de jogo!\n");
            GameOver = 1;
            printf("Deu velha!\n");
        }
    }
    PrintBoard(&board[0]);
    tryAgain();
}

void tryAgain(){
    char again;
    printf("Deseja jogar novamente? (Y/N) ");
    scanf(" %c", &again);
    switch (again){
        case 'Y' :
            main();
            break;
        case 'y' :
            main();
            break;
        default:
            exit(0);
    }
}

int main(){

    int choice, dif;
    system("cls");
    srand(time(0));
    printf("JOGO DA VELHA\n");
    printf("Escolha contra quem voce quer jogar\n");
    printf("1) Computador\n2) Player vs Player\n3) Sair\nEscolha: ");
    scanf("%d", &choice);
    if(choice == 1){
        printf("Escolha a dificuldade:\n1) Movimentos aleatorios da maquina\n2) Facil\n3) Medio\n4) Dificil\n5) Voltar ao menu\nEscolha: ");
        scanf("%d", &dif);
        switch (dif){
            case 1 :
                RunGame();
                break;
            case 2 :
                RunBGame();
                break;
            case 3 :
                RunMGame();
                break;
            case 4 :
                RunHGame();
                break;
            case 5 :
                main();
                break;
            default:
                main();
        }
    } else if(choice == 2){
        RunPvPGame();
    } else if(choice == 3){
        exit(0);
    } else{
        printf("\nEscolha invalida, tente novamente!\n");
        main();
    }

    return 0;
}
