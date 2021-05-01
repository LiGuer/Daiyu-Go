#ifndef MINIMAX_H
#define MINIMAX_H
template<class State>
class MiniMax {
public:
	int MaxLevel;
	int		(*Evaluate)		(State*);					//���ۺ���
	State*	(*newStateFunc)	(State*);					//������״̬
	/*----------------[ ����/�������� ]----------------*/
	MiniMax(int(*_Evaluate)(State*), State* (*_newStateFunc)(State*),int _MaxLevel = 5) {
		Evaluate     = _Evaluate;
		newStateFunc = _newStateFunc;
		MaxLevel	 = _MaxLevel;
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
	int Policy(char level, State* state, int alpha, int beta) {
		//[0]
		if (level > MaxLevel)
			return Evaluate(state);
		int max = -0x7fffffff, 
			min =  0x7fffffff;
		//[1]
		State* newState;
		for ((newState = newStateFunc(state)) != NULL) {
			//[2]
			int score = Policy(level + 1, newState, alpha, beta);
			//[3]
			level % 2 == 1 ?
				min = min < score ? min : score, beta  = beta  < score ? beta  : score:
				max = max > score ? max : score, alpha = alpha > score ? alpha : score;
			if (alpha >= beta) 
				return level % 2 == 0 ? max : min;
		}
		return level % 2 == 0 ? max : min;
	}
};
#endif