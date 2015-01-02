#ifndef __FIVE_H
#define __FIVE_H

#include <vector>

typedef struct
{
	int num;
	int value;
}
ipair;

typedef struct
{
	int x1;int y1;
	int x2;int y2;
}
iquad;

enum Color{BLACK, WHITE, EMPTY};

typedef std::vector<int> Action;
typedef std::vector<Action> Actions;
typedef std::vector<std::vector<Color> > State;

const int m = 15;
const int n = 15;
const int win = 5;

Action action(int x, int y, Color player);
Actions actions(const State & st, Color player);
State emptyState();
State result(const State & st, const Action & act);
bool takeAction(State & st, Action act);
bool undo(State & st, Action act);
void printState(const State & st);
int utility(const State & st);
bool terminalTest(const State & st);

std::vector<ipair> bchecks;
std::vector<ipair> wchecks;
std::vector<iquad> bdanger;
std::vector<iquad> wdanger;

int minValue
(State & st, int alpha, int beta, Color WHITE, Color BLACK, 
Action & act, int depth, int maxBranch);

//Find the action to get the maximum utility, return the utility
int maxValue
(State & st, int alpha, int beta, Color BLACK, Color WHITE, 
Action & act, int depth, int maxBranch)
{
	int u = utility(st);
	int v = INT_MIN;
	Action tmp;
	act.clear();
	if(!bchecks.empty())
	{
		act = action(bchecks[0].num,bchecks[0].value,BLACK);
		return 100;
	}
	if(!wchecks.empty())
	{
		act = action(wchecks[0].num,wchecks[0].value,BLACK);
		u = utility(st);
		return u;
	}
	/*
	if(depth==0)
	{
		act = acts[0];
		return u;
	}*/
	if(!wdanger.empty())
	{
		Action act1 = action(wdanger[0].x1,wdanger[0].y1,BLACK);
		Action act2 = action(wdanger[0].x2,wdanger[0].y2,BLACK);
		Actions acts;acts.push_back(act1);acts.push_back(act2);
		
		for(int i = 0; i<acts.size() && i<maxBranch * depth; i++)
		{
			takeAction(st,acts[i]);
			//int start = clock();
			//std::cout<<"Start min value of depth "<<depth-1<<std::endl;
			int nv = minValue(st,alpha,beta,WHITE,BLACK,tmp,depth-1,maxBranch);
			//std::cout<<"end depth "<<depth-1<<" minValue:"<<clock()-start<<std::endl;
			undo(st,acts[i]);
			if(nv > v)
			{
				act = acts[i];
				v = nv;
			}
			if(v >= beta)
			{
				return v;
			}
			if(v > alpha) alpha = v;
		}
		return v;
	}
	if(terminalTest(st) || u==100 || u==-100)
	{
		return u;//Terminal state
	}
	Actions acts = actions(st,BLACK);
	std::vector<ipair> utis(acts.size());
	for(int i = 0; i<acts.size(); i++)
	{
		takeAction(st,acts[i]);
		utis[i].num = i;
		utis[i].value = utility(st);
		undo(st,acts[i]);
	}
	for(int i=1; i<utis.size(); i++)
	{
		int r = rand() % utis.size();
		ipair t = utis[i];
		utis[i] = utis[r];
		utis[r] = t;
	}
	for(int i=1; i<utis.size(); i++)
		for(int j=0; j<i; j++)
		{
			if(utis[j].value<utis[j+1].value)
			{
				ipair t = utis[j];
				utis[j] = utis[j+1];
				utis[j+1] = t;
			}
		}
	if(depth==0)
	{
		act = acts[utis[0].num];
		return u;
	}
	for(int i = 0; i<acts.size() && i<maxBranch * depth; i++)
	{
		takeAction(st,acts[utis[i].num]);
		//int start = clock();
		//std::cout<<"Start min value of depth "<<depth-1<<std::endl;
		int nv = minValue(st,alpha,beta,WHITE,BLACK,tmp,depth-1,maxBranch);
		//std::cout<<"end depth "<<depth-1<<" minValue:"<<clock()-start<<std::endl;
		undo(st,acts[utis[i].num]);
		if(nv > v)
		{
			act = acts[utis[i].num];
			v = nv;
		}
		if(v >= beta)
		{
			return v;
		}
		if(v > alpha) alpha = v;
	}
	return v;
}

int minValue
(State & st, int alpha, int beta, Color WHITE, Color BLACK,
 Action & act, int depth, int maxBranch)
{
	int u = utility(st);
	int v = INT_MAX;
	Action tmp;
	act.clear();
	if(!wchecks.empty())
	{
		act = action(wchecks[0].num,wchecks[0].value,BLACK);
		return -100;
	}
	if(!bchecks.empty())
	{
		act = action(bchecks[0].num,bchecks[0].value,WHITE);
		u = utility(st);
		return u;
	}
	/*
	if(depth==0)
	{
		act = acts[0];
		return u;
	}*/
	if(!bdanger.empty())
	{
		Action act1 = action(bdanger[0].x1,bdanger[0].y1,WHITE);
		Action act2 = action(bdanger[0].x2,bdanger[0].y2,WHITE);
		Actions acts;acts.push_back(act1);acts.push_back(act2);
		
		for(int i = 0; i<acts.size() && i<maxBranch * depth; i++)
		{
			takeAction(st,acts[i]);
			//int start = clock();
			//std::cout<<"Start max value of depth "<<depth-1<<std::endl;
			int nv = maxValue(st,alpha,beta,BLACK,WHITE,tmp,depth-1,maxBranch);
			//std::cout<<"end depth "<<depth-1<<" maxValue:"<<clock()-start<<std::endl;
			undo(st,acts[i]);
			if(nv < v) 
			{
				act = acts[i];
				v = nv;
			}
			if(v <= alpha) 
			{
				return v;
			}
			if(v < beta) beta = v;
		}
		return v;
	}
	if(terminalTest(st) || u==100 || u==-100)
	{
		return u;//Terminal state
	}
	Actions acts = actions(st,WHITE);
	std::vector<ipair> utis(acts.size());
	for(int i = 0; i<acts.size(); i++)
	{
		takeAction(st,acts[i]);
		utis[i].num = i;
		utis[i].value = utility(st);
		undo(st,acts[i]);
	}
	for(int i=1; i<utis.size(); i++)
	{
		int r = rand() % utis.size();
		ipair t = utis[i];
		utis[i] = utis[r];
		utis[r] = t;
	}
	for(int i=1; i<utis.size(); i++)
		for(int j=0; j<i; j++)
		{
			if(utis[j].value<utis[j+1].value)
			{
				ipair t = utis[j];
				utis[j] = utis[j+1];
				utis[j+1] = t;
			}
		}
	if(depth==0)
	{
		act = acts[utis[0].num];
		return u;
	}
	for(int i = 0; i<acts.size() && i<maxBranch * depth; i++)
	{
		takeAction(st,acts[utis[i].num]);
		//int start = clock();
		//std::cout<<"Start max value of depth "<<depth-1<<std::endl;
		int nv = maxValue(st,alpha,beta,BLACK,WHITE,tmp,depth-1,maxBranch);
		//std::cout<<"end depth "<<depth-1<<" maxValue:"<<clock()-start<<std::endl;
		undo(st,acts[utis[i].num]);
		if(nv < v) 
		{
			act = acts[utis[i].num];
			v = nv;
		}
		if(v <= alpha) 
		{
			return v;
		}
		if(v < beta) beta = v;
	}
	return v;
}



class Game
{
public:
	Game()
	:_Turn(BLACK)
	{
		_State = emptyState();
	}
	
	State & _state() {return _State;}
	State _result(const Action & act){return result(_State,act);}
	Actions _actions() {return actions(_State, _Turn);}
	bool _takeAction(Action act) 
	{
		if(takeAction(_State, act))
		{
			_Turn = (Color) (1-(int)_Turn);
			_actlist.push_back(act);
			return true;
		}
		return false;
	}
	
	bool _undo()
	{
		if(_actlist.empty())
			return false;
		else
		{
			undo(_State,_actlist.back());
			_actlist.pop_back();
			_Turn = (Color) (1-(int)_Turn);
		}
	}
	
	int _utility() {return utility(_State);}
	
	void _print() {printState(_State);}
	
	Action alphaBetaSearch(Color player);
	
	State _State;
	
	Color _Turn;
	
	Actions _actlist;
	
};

Action Game::alphaBetaSearch(Color player)
{
	int v; Action act;
	int depth = 3;
	int maxBranch = 5;
	if(player==BLACK)
		v = maxValue(_State, -100, 100, BLACK, WHITE, act, depth, maxBranch);
	else
		v = minValue(_State, -100, 100, WHITE, BLACK, act, depth, maxBranch);
	return act;
}

Action action(int x, int y, Color player)
{
	Action act(3);
	act[0] = x;
	act[1] = y;
	act[2] = (int)player;
	return act;
}
Actions actions(const State & st, Color player)
{
	int range = 1;
	Actions acts;
	for(int i = 0; i<m; i++)
		for(int j = 0; j<n; j++)
			if(st[i][j]==EMPTY)
			{
				bool valid = false;
				for(int ii=-range; ii<=range; ii++)
					for(int jj=-range; jj<=range; jj++)
						if(i+ii >= 0 && i+ii < m && j+jj >= 0 && j+jj < n)
							if(st[i+ii][j+jj]!=EMPTY) valid = true;
				if(valid) acts.push_back(action(i,j,player));
			}
	return acts;
}
State emptyState()
{
	State st(m);
	for(int i = 0; i<m; i++)
		for(int j = 0; j<n; j++)
			st[i].push_back(EMPTY);
	return st;
}
State result(const State & st, const Action & act)
{
	State nst = st;
	nst[act[0]][act[1]] = (Color) act[2];
	return nst;
}
bool takeAction(State & st, Action act)
{
	if(act[0]<0 || act[0]>m || act[1]<0 || act[1]>n || st[act[0]][act[1]]!=EMPTY)
		return false;
	st[act[0]][act[1]] = (Color) act[2];
	return true;
}
bool undo(State & st, Action act)
{
	st[act[0]][act[1]] = EMPTY;
	return true;
}

int utility(const State & st)
{
	//int start = clock();
	wchecks.clear();
	bchecks.clear();
	wdanger.clear();
	bdanger.clear();
	int u = 0;
	for(int i = 0; i<m; i++)
		for(int j = 0; j<n; j++)
		{
			if(i <= m - win)
			{
				int White = 0;
				int Black = 0;
				int theEmpty[2] = {-1,-1};
				for(int k = 0; k<win; k++)
				{
					if(st[i+k][j]==WHITE)
						White ++;
					else if(st[i+k][j]==BLACK)
						Black ++;
					else
					{
						if(theEmpty[0]==-1) theEmpty[0]=k;
						else if(theEmpty[1]==-1) theEmpty[1]=k;
					}
				}
				if(White == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return -100;
				}
				if(White == win-1 && Black==0)
				{
					u -= 20;
					ipair ip;
					ip.num = theEmpty[0]+i;
					ip.value = j;
					wchecks.push_back(ip);
				}
				if(White == win-2 && Black==0)
				{
					u -= 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i+win-1,j};
						wdanger.push_back(iq);
					}
				}
				if(Black == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return 100;
				}
				if(Black == win-1 && White==0)
				{
					u += 20;
					ipair ip;
					ip.num = theEmpty[0]+i;
					ip.value = j;
					bchecks.push_back(ip);
				}
				if(Black == win-2 && White==0)
				{
					u += 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i+win-1,j};
						bdanger.push_back(iq);
					}
				}
			}
			if(j <= n - win)
			{
				int White = 0;
				int Black = 0;
				int theEmpty[2] = {-1,-1};
				for(int k = 0; k<win; k++)
				{
					if(st[i][j+k]==WHITE)
						White ++;
					else if(st[i][j+k]==BLACK)
						Black ++;
					else
					{
						if(theEmpty[0]==-1) theEmpty[0]=k;
						else if(theEmpty[1]==-1) theEmpty[1]=k;
					}
				}
				if(White == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return -100;
				}
				if(White == win-1 && Black==0)
				{
					u -= 20;
					ipair ip;
					ip.num = i;
					ip.value = theEmpty[0]+j;
					wchecks.push_back(ip);
				}
				if(White == win-2 && Black==0)
				{
					u -= 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i,j+win-1};
						wdanger.push_back(iq);
					}
				}
				if(Black == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return 100;
				}
				if(Black == win-1 && White==0)
				{
					u += 20;
					ipair ip;
					ip.num = i;
					ip.value = theEmpty[0]+j;
					bchecks.push_back(ip);
				}
				if(Black == win-2 && White==0)
				{
					u += 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i,j+win-1};
						bdanger.push_back(iq);
					}
				}
			}
			if(i <= m - win && j <= n - win)
			{
				int White = 0;
				int Black = 0;
				int theEmpty[2] = {-1,-1};
				for(int k = 0; k<win; k++)
				{
					if(st[i+k][j+k]==WHITE)
						White ++;
					else if(st[i+k][j+k]==BLACK)
						Black ++;
					else
					{
						if(theEmpty[0]==-1) theEmpty[0]=k;
						else if(theEmpty[1]==-1) theEmpty[1]=k;
					}
				}
				if(White == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return -100;
				}
				if(White == win-1 && Black==0)
				{
					u -= 20;
					ipair ip;
					ip.num = theEmpty[0]+i;
					ip.value = theEmpty[0]+j;
					wchecks.push_back(ip);
				}
				if(White == win-2 && Black==0)
				{
					u -= 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i+win-1,j+win-1};
						wdanger.push_back(iq);
					}
				}
				if(Black == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return 100;
				}
				if(Black == win-1 && White==0)
				{
					u += 20;
					ipair ip;
					ip.num = theEmpty[0]+i;
					ip.value = theEmpty[0]+j;
					bchecks.push_back(ip);
				}
				if(Black == win-2 && White==0)
				{
					u += 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i+win-1,j+win-1};
						bdanger.push_back(iq);
					}
				}
			}
			if(i >= win - 1 && j <= n - win)
			{
				int White = 0;
				int Black = 0;
				int theEmpty[2] = {-1,-1};
				for(int k = 0; k<win; k++)
				{
					if(st[i-k][j+k]==WHITE)
						White ++;
					else if(st[i-k][j+k]==BLACK)
						Black ++;
					else
					{
						if(theEmpty[0]==-1) theEmpty[0]=k;
						else if(theEmpty[1]==-1) theEmpty[1]=k;
					}
				}
				if(White == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return -100;
				}
				if(White == win-1 && Black==0)
				{
					u -= 20;
					ipair ip;
					ip.num = -theEmpty[0]+i;
					ip.value = theEmpty[0]+j;
					wchecks.push_back(ip);
				}
				if(White == win-2 && Black==0)
				{
					u -= 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i-win+1,j+win-1};
						wdanger.push_back(iq);
					}
				}
				if(Black == win)
				{
					//std::cout<<"Utility:"<<clock()-start<<std::endl;
					return 100;
				}
				if(Black == win-1 && White==0)
				{
					u += 20;
					ipair ip;
					ip.num = -theEmpty[0]+i;
					ip.value = theEmpty[0]+j;
					bchecks.push_back(ip);
				}
				if(Black == win-2 && White==0) 
				{
					u += 10;
					if((theEmpty[0]==0 && theEmpty[1]==win-1) ||
						(theEmpty[0]==win-1 && theEmpty[1]==0))
					{
						iquad iq = {i,j,i-win+1,j+win-1};
						bdanger.push_back(iq);
					}
				}
			}
		}
	if(u>=100) u = 99;
	if(u<=-100) u = -99;
	return u;
}

bool terminalTest(const State & st)
{
	//int start = clock();
	for(int i = 0; i<m; i++)
		for(int j = 0; j<n; j++)
			if(st[i][j]==EMPTY)
			{
				//std::cout<<"Terminal Test:"<<clock()-start<<std::endl;
				return false;
			}
	//std::cout<<"Terminal Test:"<<clock()-start<<std::endl;
	return true;
}
#endif
