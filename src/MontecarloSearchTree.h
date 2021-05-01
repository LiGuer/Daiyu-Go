#ifndef MONTECARLO_SEARCH_TREE
#define MONTECARLO_SEARCH_TREE
#include <vector>
/******************************************************************************
*					Montecarlo Search Tree ���ؿ���������
*	[�㷨]:
		[1] selection,
		[2] expansion,
		[3] simulation,
		[4] backpropagation.
*	[ע]:
		������[State]״̬�ĵط�ֻ������, Expand �� Simulation
		������[State]��������һState�ĸ���
		[State]���ڴ����ֻ�����乹��,�����������
******************************************************************************/
template<class State>
class MontecarloSearchTree {
public:
	/*--------------------------------[ Montecarlo ���ڵ� ]--------------------------------*/
	struct TreeNode {
		int   winTime = 0, 
			visitTime = 0;
		State*    state  = NULL;
		TreeNode* parent = NULL;
		std::vector<TreeNode*> child;
		// ��������
		~TreeNode() {
			delete state;
			for (int i = 0; i < child.size(); i++)
				delete child[i];
		}
	};
	/*--------------------------------[ �������� ]--------------------------------*/
	const int maxSearchTimes = 100000;
	State* (*newStateRandFunc)	(State*);					//������״̬
	State* (*judgeWin)	        (State*);					//�ж���Ӯ
	/*--------------------------------[ ���캯�� ]--------------------------------*/
	MontecarloSearchTree() { ; }
	/*----------------------------------------------------------------------------
				Montecarlo Search Tree
	*The process of MCTS is split up into four steps:
	*	[1] selection,
		[2] expansion,
		[3] simulation,
		[4] backpropagation.
	*---------------------------------------------------------------------------*/
	TreeNode* run(State* state) {
		TreeNode root; 
		root.state = state;
		for (int i = 0; i < maxSearchTimes; i++) {
			TreeNode* expandNode = TreePolicy(root);		//[1][2]
			if (expandNode == NULL) break;
			Backpropagation(								//[4]
				expandNode, 
				Simulation(expandNode->state)				//[3]
			);
		}
		return Select(root, false);							//Ans
	}
	/*------------------------------------------------------------------------------
						TreePolicy
		[1.Selection],[2.Expansion]�׶Σ�
		���뵱ǰ��ʼ�����Ľڵ�[node]��
		Selection������õ���Ҫexpend�Ľڵ㣬ע������ڵ���Ҷ�ӽ��ֱ�ӷ��ء�
		�������������ҵ�ǰδѡ������ӽڵ㣬����ж�������ѡ��
		�����ѡ�������Ȩ���exploration/exploitation��UCBֵ���ģ����UCBֵ��������ѡ��
	**----------------------------------------------------------------------------*/
	TreeNode* TreePolicy(TreeNode* node) {
		TreeNode* newNode = NULL;
		while (node != NULL && !judgeWin(node->state)) {
			if (node->isFullChildFlag) {
				node = Select(node, true); continue;
			}
			if (Expand(node, &newNode))return newNode;		//������չ�ӽڵ㣬�����У���õ�ȫ��
			node->isFullChildFlag = true;
			node = Select(node, true);
		}
		return node;
	}
	/*--------------------------------[ [1]Select ѡ�� ]--------------------------------
	*	[����]: [1] ���ڵ�	[2] ̽������
	*	[���]: [1] ���ڵ��������ӽڵ�
		��[UCB��ʽ]Ϊ�б��׼��
		ѡ��[node]�ڵ��£������ӽڵ���Ϊ�����
		��isExploration�رգ���ΪExploitation-Only������ģʽ��ֻ����Ŀǰ���ŵ�ҡ�ۡ�
		��isExploration��������̽�����ԵĻ���
	**----------------------------------------------------------------------------*/
	TreeNode* Select(TreeNode* node, bool isExplore) {
		double UcbConst = isExplore == true ? sqrt(2) : 0,
			   maxScore = -DBL_MAX;
		TreeNode* bestNode = NULL;
		for (int i = 0; i < node->child.size(); i++) {
			double t = UpperConfidenceBound(node->child[i], UcbConst);
			if (maxScore < t) {
				maxScore = t;
				bestNode = node->child[i];
			}
		}
		return bestNode;
	}
	/*--------------------------------[ UCB ]--------------------------------
	*Upper Confidence Bound (UCB)�����Ž��㷨:
	*                              -----------------------
	*             W(node)         /   In( N(parentNode) )
	*UCT(node) = --------- + c * / ----------------------
	*             N(node)       ��         N(node)
	**-----------------------------------------------------------------------*/
	double UpperConfidenceBound(TreeNode* node, double C) {
		double a = (double)	   node->winTime			/ node->visitTime,
			   b = (double)log(node->parent->visitTime) / node->visitTime;
		return a + C * sqrt(b);
	}
	/*--------------------------------[ [2]Expand ��չ ]--------------------------------
	*	��[node]�ڵ�����չһ���½ڵ㣬���ѡ����һ��Action
	*	[ע]: �豣֤�����ڵ��������ڵ�Action��ͬ��
	**----------------------------------------------------------------------------*/
	bool Expand(TreeNode* node, TreeNode*& newNode, bool(*nextStateRand)(TreeNode*, State*)) {
		/*---- New State ----*/
		State* newState;
		if ((newState = nextStateRand(node, state)) != NULL) return false;
		/*---- New Node ----*/
		newNode = new TreeNode;
		newNode->state  = newState;
		newNode->parent = node;
		node   ->child.push_back(*newNode);
		return true;
	}
	/*--------------------------------[ [3]Simulation ģ�� ]--------------------------------
	��[node]�ڵ��״̬[state]������ģ�⣬ֱ��ʤ��������
	���ظ�״̬ģ���[Reward]����ֵ��
	**----------------------------------------------------------------------------*/
	int Simulation(State* state0) {
		State* newState;
		/*---- ��ʼģ�� ----*/
		int reward = judgeWin(&state);
		while ((reward = judgeWin(newState)) == 0) {
			if ((newState = newStateRandFunc(&state)) != NULL) break;		//���ѡ����һ����
		}
		return reward;
	}
	/*--------------------------------[ [4]Backpropagation ���� ]--------------------------------
	��[3]ģ��ĵõ���[Reward]����ֵ��
	��Ҷ[node]�������ݣ�����ÿ����صĽڵ㡣
	**----------------------------------------------------------------------------*/
	void Backpropagation(TreeNode* node, int reward) {
		while (node != NULL) {
			node->visitTime++;
			node->  winTime += node->state->player == reward ? 1 : -1;
			node = node->parent;
		}
	}
};
#endif