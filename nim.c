#include <stdio.h>
#include <stdlib.h>
#include "nim.h"

/*
WRITTEN BY: Haifaa Abushaaban
DATE: November 30, 2021
*/

int *new_board (int board_size){

    int *newBoard = malloc(sizeof(int) * board_size);
    if (newBoard == NULL){
        fprintf(stderr, "Unable to malloc() for board in function new_board\n");
        exit(-1);
    }

    return newBoard;
}

struct node *mk_nim_hash (int max_hash, int board_size, int *start_board){

    struct node *nim_hash = malloc(sizeof(struct node) * max_hash); // board for each index
    if (nim_hash == NULL){
        fprintf(stderr, "Unable to malloc() for node in function mk_nim_hash\n");
        exit(-1);
    }
    // each node has 4 values to initialize:
    for (int i = 0; i < max_hash; ++i){ // max_hash is size of the hash table
        nim_hash[i].moves = -1;
        nim_hash[i].move = NULL;
        nim_hash[i].nimsum = -1;
        nim_hash[i].board = hash2board (board_size, start_board, i);
    }

    return nim_hash;
}

void free_board (int *board){

    free(board);
}

void free_nim_hash (int max_hash, struct node *nim_hash){

    for (int i = 0; i < max_hash; ++i){
        free(nim_hash[i].board); // hash2board calls new_board which mallocs a board and returns it in the board variable
        free(nim_hash[i].move); // move is malloc'd in the join_board function
    }
    free(nim_hash);
}

int *board_from_argv (int board_size, char **argv){

    int *board = new_board (board_size);
    for (int i = 0; i < board_size; ++i){
        board[i] = atoi(argv[i]);
    }

    return board;
}

int *copy_board (int board_size, int *board){ // for user to play again

    int *newBoard = new_board (board_size);
    for (int i = 0; i < board_size; ++i){
        newBoard[i] = board[i];
    }

    return newBoard;
}

int game_over (int board_size, int *board){

    int count = 0;
    for (int i = 0; i < board_size; ++i){
        count += board[i];
    }

    if (count == 1) return 1; // one match
    return 0;
}

void join_graph (struct node *nim_hash, int hash, int board_size, int *start_board){
    // connects the nodes in the hash table together
    // nim_hash is max_hash nodes initialized in the mk_nim_hash function
    // board size rows
    // start_board is beginning board from command line
    // nim_hash[hash] is the current board

    // since there is more than one way to reach the same board, the hash index will already be initialized for the hash score of a board
    if (nim_hash[hash].moves != -1){ // not initialized, non-recursive case
        return;
    }

    nim_hash[hash].nimsum = compute_nimsum (board_size, nim_hash[hash].board);

    int moves = 0; // this is how many struct moves for a given board, that will be in the array
    for (int b = 0; b < board_size; ++b){
        moves += nim_hash[hash].board[b];
    }

    // initialize the number of moves
    nim_hash[hash].moves = moves;

    // allocate a move array within nim_hash of moves size to store move
    nim_hash[hash].move = malloc(sizeof(struct move) * moves);
    if (nim_hash[hash].move == NULL){
        fprintf(stderr, "Unable to malloc() for moves in function join_graph\n");
        exit(-1);
    }

    int i = 0;
    for (int r = 0; r < board_size; ++r){
        for (int m = 1; m <= nim_hash[hash].board[r]; ++m){

            // initialize each possible move with row and match values
            nim_hash[hash].move[i].row = r; // from which row matches will be removed
            nim_hash[hash].move[i].matches = m; // how many matches will be removed, must be 1+

            // compute the outcome of that board after matches are taken from row of current
            // board (subtract matches from board) and place in new board
            int *new_board = copy_board (board_size, nim_hash[hash].board);
            // subtract values in the integer array
            new_board[r] = nim_hash[hash].board[r] - m;

            // outcome for the move, destination for the edge
            int board_hash = board2hash(board_size, start_board, new_board);
            // new current board has a hash value, which is stored in move
            nim_hash[hash].move[i].hash = board_hash; // hash is destination of edge

            for (int b = 0; b < board_size; ++b){
                nim_hash[board_hash].board[b] = new_board[b]; // copy new contents into board
            }

            free_board (new_board);

            // add moves (edges) to the graph recursively for all moves WHICH ONE?!
            join_graph (nim_hash, board_hash, board_size, start_board);

            // i is the number of moves (in the end, should be same as moves value)
            ++i;
        }
    }
}

int compute_nimsum (int board_size, int *board){

    int nimsum = 0;
    int negate = 0;

    for (int b = 0; b < board_size; ++b){
        nimsum ^= board[b]; // bitwise XOR is calculated using '^' in C
        if (board[b] <= 1){
            ++negate; // count number of values less or equal 1
        }
    }

    if (negate == board_size) return !nimsum; // if all values are less equal 1, return negation of nimsum
    return nimsum;
}
