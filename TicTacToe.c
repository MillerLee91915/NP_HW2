#include <stdio.h>
#include <stdlib.h>

/* 0 repersernt O , 1 represent X*/
int player; 
int winner;
/* Every position on table only have three situation : empty , O , X*/
char board[3][3];
/* When left_empty is zero Game Over!*/
int left_empty;
int num;
int colum;
int row;

void gamestart();
void gameinit();
void show_current_board();
void win_check();

int main()
{

    printf("Let's play the fucking Tic-Tac-Toe together!\n");

    gamestart();

    
}

void gamestart()
{
    gameinit();
    
    while(left_empty > 0)
    {   
        /*show current board*/
        show_current_board();

        /*player's input*/
        if(player == 1)
        {
            printf("Please enter number of the position you want to place X :\n");
        }
        else if(player == 0)
        {
            printf("Please enter number of the position you want to place O :\n");
        }
        int input_is_unvaild = 1;

        while (input_is_unvaild)
        {
            scanf("%d", &num);

            switch (num)
            {
                case 9:
                    row = 0;
                    colum = 2;
                    break;
                case 8:
                    row = 0;
                    colum = 1;
                    break;
                case 7:
                    row = 0;
                    colum = 0;
                    break;
                case 6:
                    row = 1;
                    colum = 2;
                    break;
                case 5:
                    row = 1;
                    colum = 1;
                    break;
                case 4:
                    row = 1;
                    colum = 0;
                    break;
                case 3:
                    row = 2;
                    colum = 2;
                    break;
                case 2:
                    row = 2;
                    colum = 1;
                    break;
                case 1:
                    row = 2;
                    colum = 0;
                    break;
            }

            if(num < 1 || num > 9)
            {
                printf("Invaild Input: please enter the number 1~9.\n");
                continue;
            }
            else if (board[row][colum] != ' ')
            {
                printf("Invaild Input: the position chosen is not empty.\n");
                continue;
            }
            
            input_is_unvaild = 0;
        }

        if(player == 1)
        {
            board[row][colum] = 'X';
            player = 0;
        }
        else if(player == 0)
        {
            board[row][colum] = 'O';
            player = 1;
        }
        
        win_check();
        

        left_empty--;

    }

    show_current_board();
    printf("Tie! No winner.\n");
    exit(0);
    
}

void gameinit()
{
    winner = -1;
    player = 1;
    for(int i = 0 ; i < 3 ; i++)
    {
        for(int j =0; j <3; j++)
        {
            board[i][j] = ' ';
        }
    }
    left_empty = 9;
}


void show_current_board()
{
    
        printf(" _________________\n");
        printf("|     |     |     | \n");
        printf("|  %c  |  %c  |  %c  |\n",board[0][0],board[0][1],board[0][2]);
        printf("|_____|_____|_____|\n");
        printf("|     |     |     |\n");
        printf("|  %c  |  %c  |  %c  |\n",board[1][0],board[1][1],board[1][2]);
        printf("|_____|_____|_____|\n");
        printf("|     |     |     |\n");
        printf("|  %c  |  %c  |  %c  |\n",board[2][0],board[2][1],board[2][2]);
        printf("|_____|_____|_____|\n");

}

void win_check()
{
	  int i;
	  char key = ' ';

	  // Check Rows
	  for (i=0; i<3;i++)
	  if (board [i][0] == board [i][1] && board [i][0] == board [i][2] && board [i][0] != ' ') key = board [i][0];	
	  // check Columns
	  for (i=0; i<3;i++)
	  if (board [0][i] == board [1][i] && board [0][i] == board [2][i] && board [0][i] != ' ') key = board [0][i];
	  // Check Diagonals
	  if (board [0][0] == board [1][1] && board [1][1] == board [2][2] && board [1][1] != ' ') key = board [1][1];
	  if (board [0][2] == board [1][1] && board [1][1] == board [2][0] && board [1][1] != ' ') key = board [1][1];

	  //Declare Winner if any

	  if (key == 'X')
	  {
        show_current_board();
	    printf ("Player 1 Wins\n");
	    winner=1;
	    exit (0);
	  }

	  if (key == 'O')
	  {
        show_current_board();
	    printf ("Player 2 Wins\n");
	    winner=2;
	    exit (0);
	  }
	
	  	

}
