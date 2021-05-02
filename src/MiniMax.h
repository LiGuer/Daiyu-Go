#ifndef MINIMAX_H
#define MINIMAX_H
#include <vector>
template<class State>
class MiniMax {
public:
	int MaxLevel = 5;
	int  (*Evaluate)		(State&);					//���ۺ���
	bool (*newStateFunc)	(State&, State&);			//������״̬
	char (*judgeWin)		(State&);					//�ж���Ӯ
	std::vector<int> ansScoreSet;
	State maxScoreState;
	/*----------------[ ����/�������� ]----------------*/
	MiniMax(int(*_Evaluate)(State&), bool(*_newStateFunc)(State&, State&), char(*_judgeWin)(State&),int _MaxLevel = 5) {
		Evaluate     = _Evaluate;
		newStateFunc = _newStateFunc;
		MaxLevel	 = _MaxLevel;
		judgeWin	 = _judgeWin;
	}
	/******************************************************************************
	*                    ���ߺ���  ������
	*	[����]:
			[0] ���������ֵ, ���ص�ǰ״̬������(���ۺ���)
			[1] ���ڵ�ǰ״̬, ������״̬, ����ÿһ�����ܵ���״̬
				[2] �ݹ���ߺ���, ���������������&����������
				[3] ��β
					[3,1] �������Ĳ�(���ֲ�) ȡ��С
					[3.2] ˫�����Ĳ�(��  ��) ȡ���
					[3.3] alpha-beta��֦
	******************************************************************************/
	int Policy(char level, State& state, int alpha, int beta) {
		//[0]
		if (level > MaxLevel || judgeWin(state) != 0)
			return Evaluate(state);
		int max = -0x7fffffff, 
			min =  0x7fffffff;
		//[1]
		State newState = state;
		while (newStateFunc(state, newState)) {
			//[2]
			int score = Policy(level + 1, newState, alpha, beta);
			//[3]
			if (level == 0) {
				for (char y = 0; y < 15; y++) {
					for (char x = 0; x < 15; x++) {
						printf("%d ", newState[x * 15 + y] + 1);
					}printf("\n");
				}
				ansScoreSet.push_back(score);
				if (score > max) maxScoreState = newState;
			}
			level % 2 == 0 ?
				max = max >= score ? max : score, alpha = alpha >= score ? alpha : score:
				min = min <= score ? min : score,  beta =  beta <= score ?  beta : score;
			if (alpha >= beta) 
				return level % 2 == 0 ? max : min;
		}
		return level % 2 == 0 ? max : min;
	}
};
#endif