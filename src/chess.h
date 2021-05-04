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
#include <algorithm> 
#include "../../LiGu_AlgorithmLib/Mat.h"
#include "MontecarloTreeSearch.h"
/******************************************************************************
*                    国际象棋AI  Chess-AI
******************************************************************************/
namespace Chess_AI {
#define BOARDSIZE 8
#define BLACK  1
#define WHITE -1
typedef char Piece;										//棋子
enum { King = 1, Queen, Bishop, Knight, Rook, Pawn };	//王,后,象,马,车,兵
/******************************************************************************
*                    状态
*	[数据]: [1]下子位置		[2]棋手		[3]棋盘
******************************************************************************/
struct State {
	int   st, ed;
	char  player = 0;
	Piece eat = 0;
	Mat<Piece> board{ BOARDSIZE ,BOARDSIZE };
	int kidNum = -1;
	std::vector<int> kidIndex;
	Piece& operator[](int i)		{ return board[i]; }
	Piece& operator()(int x, int y) { return board(x, y); }
};
/******************************************************************************
*                    基础函数
******************************************************************************/
//函数声明
bool	newStateRand	(State& state, State& newState, bool isSimulation);
char	judgeWin		(State& state);
/*--------------------------------[ 运行 ]--------------------------------*/
void run(State* board, int& JiePos) {
	typedef MontecarloTreeSearch<State> AI;
	AI ai(
		newStateRand,
		judgeWin
	);
	*board = *ai.run(board);
}
/*--------------------------------------------------------------------------------
						随机新状态生成 New State Rand
*	[算法]: 基于输入[棋盘]，在可能的动作下，随机选择一个动作
*	[流程]:
		[1] 确认状态空间数量
			[1.1] 若数量为零, 返回失败
		[2] 随机选择一个状态, 并返回
--------------------------------------------------------------------------------*/
// 用于Montecarlo[2. 拓展]
bool newStateRand(State& state, State& newState, bool isSimulation) {
	if (isSimulation) return newStateRand(state);
	//[1]
	std::vector<int> numList;
	if (state.kidNum == -1) {
		state.kidNum = 0;
		for (int i = 0; i < state.board.size(); i++)
			if (state.board[i] ==   state.player) {
				int t = getNextStep(state.board, i);
				state.kidNum += t;
				numList.push_back(t);
			}
	}
	if (state.kidNum - state.kidIndex.size() == 0) return false;
	//[2]
	int index = rand() % (state.kidNum - state.kidIndex.size()) + 1, 
		cur   = 0;
	std::sort(state.kidIndex.begin(), state.kidIndex.end());
	for (int i = 0; i < state.kidIndex.size(); i++)
		if (state.kidIndex[i] == index)index++;
	state.kidIndex.push_back(index);

	for (int i = 0; i < newState.board.size(); i++)
		if (newState.board[i] == newState.player) {
			index -= numList[cur++];
			if (index <= 0) {
				newState.st = i;
				newState.ed = getNextStep(newState.board, i, index);
				break;
			}
		}
	newState.board			= state.board;
	newState.player			=-state.player;
	newState.eat			= state[newState.ed];
	newState[newState.ed]	= state[newState.st];
	return true;
}
// 用于Montecarlo[3. 模拟]
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
	int index = rand() % num + 1, cur = 0;
	for (int i = 0; i < state.board.size(); i++)
		if (state.board[i] == state.player) {
			index -= numList[cur++];
			if (index <= 0) {
				state.st = i;
				state.ed = getNextStep(state.board, i, index);
				break;
			}
		}
	state.eat		= state[state.ed];
	state[state.ed] = state[state.st];
	return true;
}
/*--------------------------------[ 判断是否过界 ]--------------------------------*/
bool judgeOut(int x, int y) {
	return (x >= 0 && x < BOARDSIZE
		 && y >= 0 && y < BOARDSIZE) ? false : true;
}
/******************************************************************************
*                   国际象棋规则函数
******************************************************************************/
/*--------------------------------[ 判断输赢 ]--------------------------------*/
char judgeWin(State& state) {
	bool flagB = 1, flagW = 1;
	for (int i = 0; i < state.board.size(); i++) {
		if (state[i] ==  King) flagW = 0;
		if (state[i] == -King) flagB = 0;
	}
	return flagB ? BLACK : (flagW ? WHITE : 0);
}
/*--------------------------------[ 下一步状态数量 ]--------------------------------*/
int getNextStep(Mat<Piece>& board, int pos, int index = -1) {
	Piece piece = board[pos];
	int x = board.i2x(pos),
		y = board.i2y(pos),
	static const int 
		x_step[] = { 1,-1, 0, 0, 1,-1, 1,-1 },
		y_step[] = { 0, 0, 1,-1, 1, 1,-1,-1 };
	int NextStepNum = 0;
	switch (abs(piece)) {
	King: {													// 王
		for (int i = 0; i < 8; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			if (judgeOut(xt, yt) || board(xt, yt) * piece > 0) continue;
			NextStepNum++;
			if (index-- == 0) return board.xy2i(xt, yt);
		}
	}break;
	Queen: {												// 女王
		for (int i = 0; i < 8; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			while (!judgeOut(xt, yt) && board(xt, yt) * piece <= 0) {
				xt += x_step[i]; 
				yt += y_step[i];
				NextStepNum++;
				if (index-- == 0) return board.xy2i(xt, yt);
			}
		}
	}break;
	Bishop: {												// 象
		for (int i = 4; i < 8; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			while (!judgeOut(xt, yt) && board(xt, yt) * piece <= 0) {
				xt += x_step[i]; 
				yt += y_step[i];
				NextStepNum++;
				if (index-- == 0) return board.xy2i(xt, yt);
			}
		}
	}break;
	Knight: {												// 马
		for (int i = 0; i < 8; i++) {
			int xt = x + x_step[i % 4 + 4] + i <= 4 ? 1 : 0,
				yt = y + y_step[i % 4 + 4] + i >  4 ? 1 : 0;
			if (judgeOut(xt, yt)) continue;
			NextStepNum++;
			if (index-- == 0) return board.xy2i(xt, yt);
		}
	}break;
	Rook: {													// 车
		for (int i = 0; i < 4; i++) {
			int xt = x + x_step[i], 
				yt = y + y_step[i];
			while (!judgeOut(xt, yt) && board(xt, yt) * piece <= 0) {
				xt += x_step[i]; 
				yt += y_step[i];
				NextStepNum++;
				if (index-- == 0) return board.xy2i(xt, yt);
			}
		}
	}break;
	Pawn: {													// 兵
		int t = piece > 0 ? 1 : -1;
		if (!judgeOut(x + t, y) && board(x + t, y) * piece < 0) {
			NextStepNum++;
			if (index-- == 0) return board.xy2i(x + t, y);
		}
		if (((piece > 0 && y == 1)
		  || (piece < 0 && y == 6))
			&& !judgeOut(x + 2 * t, y) && board(x + 2 * t, y) * piece < 0){
			NextStepNum++;
			if (index-- == 0) return board.xy2i(x + 2 * t, y);
		}
	}
	}
	return NextStepNum;
}
};
#endif