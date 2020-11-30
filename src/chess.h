#ifndef CHESS_H
#define CHESS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
class Chess
{
public:
	typedef char Piece;					//����
	Piece* Board = NULL;				//���� (���ϰ���)(y��x��)
	enum { KING = 1, Queen, Bishop, Knight, Rook, Pawn };//��,��,��,��,��,��
	char MaxLevel = 4;
	char x0st, y0st, x0ed, y0ed;
	const char Size = 8;
	const char Black = 1, White = -1;

	Chess() {
		Board = (Piece*)malloc(sizeof(Piece) * Size * Size);
	}
	~Chess() { free(Board); }
	void ChessAI(char who, char& xst, char& yst, char& xed, char& yed) {
		
	}
	bool setMap(Piece* board, char xst, char yst, char xed, char yed) {		//����
		board[xed * Size + yed] = board[xst * Size + yst];
		board[xst * Size + yst] = 0;
	}
	Piece judgeWin(Piece* board);
	void getNextStep(Piece* board,char x, char y, char* NextStepX, char* NextStepY,char& NextStepNum) {
		Piece thisPiece = board[y * Size + x];
		static char x_step[] = { 1,-1,0,0,1,-1,1,-1 };
		static char y_step[] = { 0,0,1,-1,1,1,-1,-1 };
		NextStepX = (char*)malloc(sizeof(char) * 28);
		NextStepY = (char*)malloc(sizeof(char) * 28);
		NextStepNum = 0;
		switch (thisPiece) {
		KING: {													// ��
			for (char i = 0; i < 8; i++) {
				char xt = x + x_step[i], yt = y + y_step[i];
				if (xt < Size && yt && xt >= 0 && yt >= 0 && board[yt * Size + xt] * thisPiece < 0) {
					NextStepX[NextStepNum] = xt; NextStepY[NextStepNum] = yt; NextStepNum++;
				}
			}
		}break;
		Queen: {												// Ů��
			for (char i = 0; i < 8; i++) {
				char xt = x + x_step[i], yt = y + y_step[i];
				while (xt < Size && yt && xt >= 0 && yt >= 0 && board[yt * Size + xt] * thisPiece < 0) {
					NextStepX[NextStepNum] = xt; NextStepY[NextStepNum] = yt; NextStepNum++;
					xt += x_step[i]; yt += y_step[i];
				}
			}
		}break;
		Bishop: {												// ��
			for (char i = 4; i < 8; i++) {
				char xt = x + x_step[i], yt = y + y_step[i];
				while (xt < Size && yt && xt >= 0 && yt >= 0 && board[yt * Size + xt] * thisPiece < 0) {
					NextStepX[NextStepNum] = xt; NextStepY[NextStepNum] = yt; NextStepNum++;
					xt += x_step[i]; yt += y_step[i];
				}
			}
		}break;
		Knight: {												// ��
			static char x_Knight_step[] = { 1,1,-1,-1,2,2,-2,-2 };
			static char y_Knight_step[] = { 2,-2,2,-2,1,-1,1,-1 };
			for (char i = 0; i < 8; i++) {
				char xt = x + x_Knight_step[i], yt = y + y_Knight_step[i];
				if (xt < 0 || yt < 0 || xt >= Size || yt >= Size)continue;
				NextStepX[NextStepNum] = xt; NextStepY[NextStepNum] = yt; NextStepNum++;
			}
		}break;
		Rook: {													// ��
			for (char i = 0; i < 4; i++) {
				char xt = x + x_step[i], yt = y + y_step[i];
				while (xt < Size && yt && xt >=0 && yt >= 0 && board[yt * Size + xt] * thisPiece < 0) {
					NextStepX[NextStepNum] = xt; NextStepY[NextStepNum] = yt; NextStepNum++;
					xt += x_step[i]; yt += y_step[i];
				}
			}
		}break;
		Pawn: {													// ��
			char t = thisPiece > 0 ? 1 : -1;
			if (y + t >= 0 && y + t < Size && board[(y + t) * Size + x] * thisPiece < 0) {
				NextStepX[NextStepNum] = x; NextStepY[NextStepNum] = y + t; NextStepNum++;
			}
			if (((thisPiece > 0 && y == 1) || (thisPiece < 0 && y == 6))
				&& y + 2 * t >= 0 && y + 2 * t < Size && board[(y + 2 * t) * Size + x] * thisPiece < 0) {
				NextStepX[NextStepNum] = x; NextStepY[NextStepNum] = y + 2 * t; NextStepNum++;
			}
		}break;
		}
	}
	char Policy(char level, Piece* board, char alpha, char beta, char who)//������(����:���ߺ���)
	{
		if (level > MaxLevel || judgeWin(board) != 0) {		//�ﵽ�������������(�ݹ��յ�)
			return (who == -1 ? -1 : 1) * Evaluate(board);
		}
		//------�����С�㷨��alpha-beta��֦------
		char max = -0x7fffffff, min = 0x7fffffff;
		const static char step_x[] = { 0,0,1,-1,1,-1,1,-1 };
		const static char step_y[] = { 1,-1,0,0,1,-1,-1,1 };
		for (char y = 0; y < Size; y++) {
			for (char x = 0; x < Size; x++) {                           //��ÿ�������˼��
				if (board[y * Size + x] != 0 && who * board[y * Size + x] > 0) {	// �����ڵ�
					// ������䲽
					char* NextStepX, char* NextStepY;
					char NextStepNum;
					getNextStep(board, x, y, NextStepX, NextStepY, NextStepNum);
					//### ���Ľ���
					//### ������λ
					for (int stepCur = 0; stepCur < NextStepNum; stepCur++) {
						//��������
						Piece savetemp = board[NextStepX[stepCur] * Size + NextStepX[stepCur]];
						board[NextStepX[stepCur] * Size + NextStepX[stepCur]] = board[x * Size + y];
						board[x * Size + y] = 0;
						//��������
						char score = Policy(level + 1, board, alpha, beta, who);       //�ݹ�//score:��������ֵ
						//��ԭ����
						board[x * Size + y] = board[NextStepX[stepCur] * Size + NextStepX[stepCur]];
						board[NextStepX[stepCur] * Size + NextStepX[stepCur]] = savetemp;
						// ����������
						if (level % 2 == 1) {                                       //�������Ĳ�(���ֲ�)
							min = min < score ? min : score;                        //ȡ��С
							beta = beta < score ? beta : score;
						}
						else {                                                      //˫�����Ĳ�(�Ҳ�)
							if (level == 0 && max < score) { x0st = x, y0st = y; x0ed = NextStepX[stepCur]; y0ed = NextStepY[stepCur]; }
							max = max > score ? max : score;                        //ȡ���
							alpha = alpha > score ? alpha : score;
						}
						if (alpha >= beta)return level % 2 == 0 ? max : min;        //alpha-beta��֦
					}
				}
			}
		}
		return level % 2 == 0 ? max : min;
	}
	char Evaluate(Piece* board)									//���ۺ���
	{

	}

};
#endif