#include<iostream>
#include<algorithm>
#include<vector>
#include<cmath>
#include<cstdlib>
#include<random>

using namespace std;

//TODO continuous decision space, history, copy constructor issue on populationSize passing
int populationSize = 1;
class Agent {
    public:
        vector<int> strategy;
        vector<int> score;
        vector<int> history;
        //int populationSize;

        vector<int> connectionLeft;
        vector<int> connectionRight;
        Agent* leftNeighbor;
        Agent* rightNeighbor;

        Agent() {
            this->strategy = vector<int>(0);
            for(int i=0;i<populationSize;i++) {
                strategy.push_back(rand()%2);
                connectionLeft.push_back(rand()%populationSize);
                connectionRight.push_back(rand()%populationSize);
            }
            history = vector<int>(0);
            //cerr<<this->strategy[0]<<"isStrat";
        }
        int getDecision(int sampleIndividual) {
            int decision = strategy[sampleIndividual];
            history.push_back(decision);
            return decision;
        }
};

// Scoring as decribed in the paper
int score(int decision1, int decision2, int decision3) {
    //cerr<<decision1<<decision2<<decision3<<"Wololo";
    if(decision1 && decision2 && decision3 == 1)
        return 70;
    else if(decision1 && decision2 && decision3 == 0)
        return 20;
    else if((decision1 && decision3 == 1) && decision2 == 0)
        return 90;
    else 
        return 0;
}

// Scoring which has no temptation to defect
int score2(int decision1, int decision2, int decision3) {
    if(decision1 && decision2 && decision3 == 1)
        return 70;
    else if(decision1 || decision2 || decision3 == 0)
        return 20;
    else if((decision1 && decision3 == 1) && decision2 == 0)
        return 50;
    else 
        return 0;
}

class IPD {
    public:
        //Number of agents
        int nAgents;
        //Number of strategies per agent/population size
        int nStrategiesPerAgent;
        //Number of iterations
        int nIterations;
        //Agents
        vector<Agent> agents;
        //Score per run of IPD
        int totalScore;
        int averageScore;

        IPD(int nAgents, int nStrategiesPerAgent, int nIterations, vector<Agent> agents) : nAgents(nAgents), nStrategiesPerAgent(nStrategiesPerAgent), nIterations(nIterations), agents(agents) {
            totalScore = 0;
            averageScore = 0;
        }

        /**
         * Run the iterated prisoner's dilemma with the specified parameters
         */
        void runIPD() {
            for(int sampleIndividual=0;sampleIndividual<nStrategiesPerAgent;sampleIndividual++) {
                for(int round=0;round<nIterations;round++) {
                    for(int i=0;i<nAgents;i++) {
                        if(i==0)
                            agents[i].score.push_back(score(agents[nAgents-1].getDecision(sampleIndividual),agents[i].getDecision(sampleIndividual),agents[i+1].getDecision(sampleIndividual))); 
                        else if(i==nAgents-1)
                            agents[i].score.push_back(score(agents[i-1].getDecision(sampleIndividual),agents[i].getDecision(sampleIndividual),agents[0].getDecision(sampleIndividual))); 
                        else
                            agents[i].score.push_back(score(agents[i-1].getDecision(sampleIndividual),agents[i].getDecision(sampleIndividual),agents[i+1].getDecision(sampleIndividual))); 
                        totalScore += agents[i].score[round];
                    }
                }
                averageScore = totalScore/nAgents;
            }
        }


        void printAgentScores() {
            cout<<"Agent Scores:"<<endl;
            for(vector<Agent>::iterator it = agents.begin();it!=agents.end();++it) {
                int agentScore = 0;
                for(int n : (*it).score)
                    agentScore += n;
                cout<<agentScore<<endl;
            }
        }
        void printGlobalScore() {
            cout<<"Average Score: "<<averageScore<<endl;
        }
};


//Future; generality; TODO;
class ringIPD: public IPD {
    public:
        ;
};

///TODO
void produceNewGeneration() {
    ;
}

//TODO
void selectBestPopulation() {
    ;
}

int main() {
    srand(time(NULL));
    int nAgents = 2;
    int populationSize = 1;
    vector<Agent> agents(0);
    for(int i=0;i<nAgents;i++) {
        Agent a;
        agents.push_back(a);
    }
    IPD ipd(nAgents,populationSize,100,agents);
    ipd.runIPD();
    ipd.printAgentScores();
    ipd.printGlobalScore();
    return 0;
}
