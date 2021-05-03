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
#ifndef MONTECARLO_TREE_SEARCH
#define MONTECARLO_TREE_SEARCH
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
class MontecarloTreeSearch {
public:
	/*--------------------------------[ Montecarlo ���ڵ� ]--------------------------------*/
	struct TreeNode {
		int   winTime = 0, 
			visitTime = 0;
		State*    state  = NULL;
		TreeNode* parent = NULL;
		bool isChildFull = false;
		std::vector<TreeNode*> child;
		// ��������
		~TreeNode() {
			//delete state;
			for (int i = 0; i < child.size(); i++)
				delete child[i];
		}
	};
	/*--------------------------------[ �������� ]--------------------------------*/
	TreeNode root;
	const int maxSearchTimes = 20000;
	bool(*newStateRandFunc)	(State&, State&, bool);			//������״̬
	char(*judgeWin)	        (State&);						//�ж���Ӯ
	/*--------------------------------[ ����/�������� ]--------------------------------*/
	MontecarloTreeSearch(
		bool(*_newStateRandFunc)(State&, State&, bool),
		char(*_judgeWin)		(State&)
	) :
		newStateRandFunc(_newStateRandFunc),
		judgeWin		(_judgeWin)
	{ ; }
	/*----------------------------------------------------------------------------
				Montecarlo Search Tree
	*The process of MCTS is split up into four steps:
	*	[1] selection,
		[2] expansion,
		[3] simulation,
		[4] backpropagation.
	*---------------------------------------------------------------------------*/
	State* run(State* state) {
		root.state = state;
		for (int i = 0; i < maxSearchTimes; i++) {
			TreeNode* expandNode = TreePolicy(&root);		//[1][2]
			if (expandNode == NULL) break;
			Backpropagation(								//[4]
				expandNode, 
				Simulation(expandNode->state)				//[3]
			);
		}
		return Select(&root, false)->state;					//Ans
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
		while ( node != NULL && !judgeWin(*node->state)) {
			if (node->isChildFull) {
				node = Select(node, true); continue;
			}
			if (Expand(node, newNode))return newNode;		//������չ�ӽڵ㣬�����У���õ�ȫ��
			else {
				node->isChildFull = true;
				node = Select(node, true);
			}
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
	bool Expand(TreeNode* node, TreeNode*& newNode) {
		//New State
		State* newState = new State;
		if (!newStateRandFunc(*(node->state), *newState, false)) {
			delete newState; return false;
		}
		//New Node
		newNode = new TreeNode;
		newNode->state  = newState;
		newNode->parent = node;
		node   ->child.push_back(newNode);
		return true;
	}
	/*--------------------------------[ [3]Simulation ģ�� ]--------------------------------
	��[node]�ڵ��״̬[state]������ģ�⣬ֱ��ʤ��������
	���ظ�״̬ģ���[Reward]����ֵ��
	**----------------------------------------------------------------------------*/
	int Simulation(State* state) {
		State* newState = new State;
		(*newState) = (*state);
		/*---- ��ʼģ�� ----*/
		int reward = 0;
		while ((reward = judgeWin(*newState)) == 0) 
			if (!newStateRandFunc(*newState, *newState, true))
				break;
		delete newState;
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