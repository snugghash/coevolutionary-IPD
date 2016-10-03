#include<iostream>
#include<algorithm>
#include<vector>
#include<cmath>
#include<cstdlib>
#include<random>

using namespace std;

//TODO continuous decision space, history, copy constructor issue on populationSize passing
int populationSize = 10;
int totalGenerations = 100;
int nIterations = 100;
int nAgents = 10;
class Agent {
    public:
        vector<int> strategy;
        vector<int> score;
        vector<int> totalScoreOfIndividual;
        vector<int> history;
        //int populationSize;

        vector<int> connectionLeft;
        vector<int> connectionRight;
        //Agent* leftNeighbor;
        //Agent* rightNeighbor;

        Agent() {
            this->strategy = vector<int>(0);
            for(int i=0;i<populationSize;i++) {
                strategy.push_back(rand()%2);
                connectionLeft.push_back(rand()%populationSize);
                connectionRight.push_back(rand()%populationSize);
            }
            history = vector<int>(populationSize);
            totalScoreOfIndividual = vector<int>(populationSize);
            score = vector<int>(nIterations);
            //cerr<<this->strategy[0]<<"isStrat";
        }
        int getDecision(int sampleIndividual) {
            int decision = strategy[sampleIndividual];
            //TODO proper history for all individuals
            history[sampleIndividual] = decision;
            return decision;
        }
};

// Scoring as decribed in the paper
int score2(int decision1, int decision2, int decision3) {
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
int score(int decision1, int decision2, int decision3) {
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
        int populationSize;
        //Number of iterations
        int nIterations;
        //Agents
        vector<Agent> agents;
        //Score per run of IPD
        long long totalScore;
        long long averageScore;

        IPD(int nAgents, int populationSize, int nIterations, vector<Agent> agents) : nAgents(nAgents), populationSize(populationSize), nIterations(nIterations), agents(agents) {
            totalScore = 0;
            averageScore = 0;
        }

        /**
         * Run the iterated prisoner's dilemma with the specified parameters
         */
        void runIPD() {
            for(int generationCount=0;generationCount<totalGenerations;generationCount++) {
                averageScore = 0;
                cout<<"Generation "<<generationCount<<" starting"<<endl;
                for(int i=0;i<nAgents;i++) {
                    agents[i].totalScoreOfIndividual = vector<int>(populationSize);
                }
                totalScore = 0;
                for(int sampleIndividual=0;sampleIndividual<populationSize;sampleIndividual++) {
                    //cerr<<"Running IPD for individual "<<sampleIndividual<<endl;
                    for(int round=0;round<nIterations;round++) {
                        //cerr<<"Running IPD for round "<<round<<endl;
                        for(int i=0;i<nAgents;i++) {
                            //cerr<<"Running IPD for agent "<<i<<endl;
                            if(i==0)
                                agents[i].score[round] = score(agents[nAgents-1].getDecision(agents[i].connectionLeft[sampleIndividual]),agents[i].getDecision(sampleIndividual),agents[i+1].getDecision(agents[i].connectionRight[sampleIndividual])); 
                            else if(i==nAgents-1)
                                agents[i].score[round] = score(agents[i-1].getDecision(agents[i].connectionLeft[sampleIndividual]),agents[i].getDecision(sampleIndividual),agents[0].getDecision(agents[i].connectionRight[sampleIndividual])); 
                            else
                                agents[i].score[round] = score(agents[i-1].getDecision(agents[i].connectionLeft[sampleIndividual]),agents[i].getDecision(sampleIndividual),agents[i+1].getDecision(agents[i].connectionRight[sampleIndividual])); 
                            totalScore += agents[i].score[round];
                            agents[i].totalScoreOfIndividual[sampleIndividual] += agents[i].score[round];
                        }
                    }
                    averageScore += totalScore/nAgents;
                }
                averageScore /= populationSize;
                produceNewGeneration(agents);
                cout<<"Generation "<<generationCount<<" done"<<endl;
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
        ///TODO Mutation, uniqueness of connections, selection count
        void produceNewGeneration(vector<Agent> a) {
            for(int i=0;i<nAgents;i++) {
                //cerr<<"Selecting population for agent "<<i<<endl;
                vector<int> selector(populationSize);
                vector<int> successful_individuals;
                int sum = 0;
                for(int j=0;j<populationSize;j++)
                    sum+=a[i].totalScoreOfIndividual[j];
                if(sum)
                    for(int selectionCount=0;selectionCount<populationSize/2;selectionCount++) {
                        int fitness_selector = rand()%sum;
                        int j=0;
                        while(fitness_selector>0) {
                            fitness_selector -= a[i].totalScoreOfIndividual[j];
                            j++;
                        }
                        selector[j-1] = 1;
                        successful_individuals.push_back(j-1);
                    }
                int countCopiedSuccessfulIndividials = 0;
                for(int j=0;j<populationSize;j++) {
                    if(!selector[j]) {
                        if(sum) {
                            agents[i].strategy[j] = agents[i].strategy[successful_individuals[countCopiedSuccessfulIndividials]];
                            countCopiedSuccessfulIndividials++;
                            if(countCopiedSuccessfulIndividials>=successful_individuals.size())
                                countCopiedSuccessfulIndividials = rand()%successful_individuals.size();
                        }
                        else agents[i].strategy[j] = rand()%2;
                        agents[i].connectionLeft[j] = rand()%populationSize;
                        agents[i].connectionRight[j] = rand()%populationSize;
                    }
                }
            }
        }
};


//Future; generality; TODO;
class ringIPD: public IPD {
    public:
        ;
};


//TODO
void selectBestPopulation() {
    ;
}

int main() {
    srand(time(NULL));
    vector<Agent> agents(0);
    for(int i=0;i<nAgents;i++) {
        Agent a;
        agents.push_back(a);
    }
    IPD ipd(nAgents,populationSize,nIterations,agents);
    ipd.runIPD();
    //ipd.printAgentScores();
    ipd.printGlobalScore();
    return 0;
}
