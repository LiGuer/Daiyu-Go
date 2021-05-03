/*
Copyright 2020,2021 LiGuer. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef CHESS_H
#define CHESS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include "../../LiGu_AlgorithmLib/Mat.h"
#include "MontecarloTreeSearch.h"
/******************************************************************************
*                    ��������AI  Chess-AI
******************************************************************************/
namespace Chess_AI {
#define BOARDSIZE 8
#define BLACK  1
#define WHITE -1
typedef char Piece;										//����
enum { King = 1, Queen, Bishop, Knight, Rook, Pawn };	//��,��,��,��,��,��
/******************************************************************************
*                    ״̬
*	[����]: [1]����λ��		[2]����		[3]����
******************************************************************************/
struct State {
	int st, ed;
	Piece player = 0;
	Mat<Piece> board{ BOARDSIZE , BOARDSIZE };
	Mat<int>   mark { BOARDSIZE , BOARDSIZE };
	Piece& operator[](int i)		{ return board[i]; }
	Piece& operator()(int x, int y) { return board(x, y); }
};
/******************************************************************************
*                    ��������
******************************************************************************/
//��������
bool	newStateRand	(State& state, State& newState, bool isSimulation);
char	judgeWin		(State& state);
/*--------------------------------[ ���� ]--------------------------------*/
void run(State* board, int& JiePos) {
	typedef MontecarloTreeSearch<State> AI;
	AI ai(
		newStateRand,
		judgeWin
	);
	*board = *ai.run(board);
}
/*--------------------------------------------------------------------------------
						�����״̬���� New State Rand
*	[�㷨]: ��������[����]���ڿ��ܵĶ����£����ѡ��һ������
*	[����]:
		[1] ȷ��״̬�ռ�����
			[1.1] ������Ϊ��, ����ʧ��
		[2] ���ѡ��һ��״̬, ������
--------------------------------------------------------------------------------*/
// ����Montecarlo[2. ��չ]
bool newStateRand(State& state, State& newState, bool isSimulation) {
	if (isSimulation) return newStateRand(state);
	newState.board = state.board;
	newState.player = -state.player;
	//[1]
	int num = 0;
	std::vector<int> numList;
	for (int i = 0; i < state.board.size(); i++)
		if (state.board[i] == state.player) {
			int t = getNextStep(state.board, i);
			numList.push_back(t);
			num += t;
		}
	if (num == 0) return false;
	//[2]
	int index = rand() % num + 1;
	for (int i = 0; i < state.board.size(); i++) {
	}
	return true;
}
// ����Montecarlo[3. ģ��]
bool newStateRand(State& state) {
	state.player = -state.player;
	//[1]
	int num = 0;
	std::vector<int> numList;
	for (int i = 0; i < state.board.size(); i++)
		if (state.board[i] == state.player) {
			int t = getNextStep(state.board, i);
			numList.push_back(t);
			num += t;
		}
	if (num == 0) return false;
	//[2]
	int index = rand() % num + 1;
	for (int i = 0; i < state.board.size(); i++) {
	}
	return true;
}
/*--------------------------------[ �ж��Ƿ���� ]--------------------------------*/
bool judgeOut(int x, int y) {
	return (x >= 0 && x < BOARDSIZE
		 && y >= 0 && y < BOARDSIZE) ? false : true;
}
/******************************************************************************
*                   �������������
******************************************************************************/
/*--------------------------------[ �ж���Ӯ ]--------------------------------*/
char judgeWin(State& state) {
	bool flagB = 1, flagW = 1;
	for (int i = 0; i < state.board.size; i++) {
		if (state[i] ==  King) flagW = 0;
		if (state[i] == -King) flagB = 0;
	}
	return flagB ? BLACK : (flagW ? WHITE : 0);
}
/*--------------------------------[ ��һ��״̬���� ]--------------------------------*/
int getNextStep(Mat<Piece>& board, int pos) {
	Piece piece = board[pos];
	int x = board.i2x(pos),
		y = board.i2y(pos),
	static const int 
		x_step[] = { 1,-1, 0, 0, 1,-1, 1,-1 },
		y_step[] = { 0, 0, 1,-1, 1, 1,-1,-1 };
	int NextStepNum = 0;
	switch (abs(piece)) {
	King: {													// ��
		for (int i = 0; i < 8; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			if (judgeOut(xt, yt) || board(xt, yt) * piece > 0) continue;
			NextStepNum++;
		}
	}break;
	Queen: {												// Ů��
		for (int i = 0; i < 8; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			while (!judgeOut(xt, yt) && board(xt, yt) * piece <= 0) {
				xt += x_step[i]; 
				yt += y_step[i];
				NextStepNum++;
			}
		}
	}break;
	Bishop: {												// ��
		for (int i = 4; i < 8; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			while (!judgeOut(xt, yt) && board(xt, yt) * piece <= 0) {
				xt += x_step[i]; 
				yt += y_step[i];
				NextStepNum++;
			}
		}
	}break;
	Knight: {												// ��
		for (int i = 0; i < 8; i++) {
			int xt = x + x_step[i % 4 + 4] + i <= 4 ? 1 : 0,
				yt = y + y_step[i % 4 + 4] + i >  4 ? 1 : 0;
			if (judgeOut(xt, yt)) continue;
			NextStepNum++;
		}
	}break;
	Rook: {													// ��
		for (int i = 0; i < 4; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			while (!judgeOut(xt, yt) && board(xt, yt) * piece <= 0) {
				xt += x_step[i]; 
				yt += y_step[i];
				NextStepNum++;
			}
		}
	}break;
	Pawn: {													// ��
		int t = piece > 0 ? 1 : -1;
		if (!judgeOut(x,y+t) && board(x, y + t) * piece < 0)
			NextStepNum++;
		if (((piece > 0 && y == 1)
		  || (piece < 0 && y == 6))
			&& !judgeOut(x, y + 2 * t) && board(x, y + 2 * t) * piece < 0)
			NextStepNum++;
	}
	}
	return NextStepNum;
}
};
#endif