//Optimal strategy for Rock-Paper-Scissors ( A 2 Player, Zero-Sum, Incomplete Information Game) using Counterfactual Regret Minimization

//Link to paper: http://modelai.gettysburg.edu/2013/cfr/cfr.pdf

#include<bits/stdc++.h>

using namespace std;

// Returns the adjusted strategy after an iteration
vector<vector<float>> getStrategy(vector<float> &regretSum, vector<float> &strategySum){
    int actions = 3;
    float normalizingSum =0;
    vector<float> strategy = {0,0,0};
    //NormalizingSum is the sum of positive regrets.
    //This ensures do not 'over-adjust' and coverage to equilibrium
    for(int i=0; i< actions; i++){
        if(regretSum[i]> 0)
            strategy[i] = regretSum[i];
        else 
            strategy[i] = 0;
        normalizingSum += strategy[i];
    }
    // This loop normalizes our update strategy
    for(int i=0; i< actions; i++) {
        if(normalizingSum > 0)
            strategy[i] = strategy[i]/normalizingSum;
        else {
            // Default to 33%
            strategy[i] = 1.0 / actions;
        }
        strategySum[i] += strategy[i]; 
    }

    vector<vector<float>> strategyStrategySum;
    strategyStrategySum.push_back(strategy);
    strategyStrategySum.push_back(strategySum);
    return strategyStrategySum;
}

//sum function
float sum(vector<float> &arraySum){
    float s=0;
    for(int i=0;i<arraySum.size();i++){
        s+=arraySum[i];
    }
    return s;
}

// Returns a random action according to the strategy

int getAction(vector<float> &strategy){
    float r = (float)rand() / (float)RAND_MAX;
    if(r>= 0 && r< strategy[0])
        return 0;
    else if (r>=strategy[0] && r<(strategy[0]+strategy[1]))
        return 1;
    else if (r>=strategy[0] + strategy[1] && r<sum(strategy)) // Using the implemeted sum function
        return 2;
    else 
        return 0;
}


vector<float> train(int iterations, vector<float> &regretSum, vector<float> &oppStrategy) {
    vector<int> actionUtility = {0,0,0};
    vector<float> strategySum = {0,0,0};
    int actions = 3;
    for (int i = 0; i< iterations; i++) {
        // Retrieve Actions
        vector<vector<float>> t = getStrategy(regretSum, strategySum);
        vector<float> strategy = t[0];
        vector<float> strategySum = t[1];
        
        float myaction = getAction(strategy);
        // Define an arbitary opponent strategy from which to adjust
        float otherAction = getAction(oppStrategy);
        
        // Opponent Chooses Scissors
        if(otherAction == actions-1) {
            // Utility(Rock) = 1
            actionUtility[0] = 1;
            // Utility(Paper) = -1
            actionUtility[1] = -1;
        // Opponent Chooses Rock
        }
        else if (otherAction == 0) {
            // Utility(Scissors) = -1
            actionUtility[actions -1] = -1;
            // Utility(Paper) = 1
            actionUtility[1] = 1;
        // Opponent Chooses Paper
        }
        else {
            // Utility(Rock) = -1
            actionUtility[0] = -1;
            // Utility(Scissors) = 1
            actionUtility[2] = 1;
        }

        // Add the regrets from this decision
        for(int i=0; i< actions; i++) {
            regretSum[i] += actionUtility[i] - actionUtility[myaction];
        }
    }
    return strategySum;
}

vector<float> getAverageStrategy(int iterations, vector<float> &oppStrategy) {
    int actions = 3;
    vector<float> regretSum={0,0,0};
    vector<float> strategySum= train(iterations, regretSum , oppStrategy);
    vector<float> avgStrategy= {0,0,0};
    float normalizingSum = 0;
    for(int i=0; i<actions; i++){
        normalizingSum += strategySum[i];
    }
    for(int i=0; i<actions; i++) {
        if(normalizingSum > 0) {
            avgStrategy[i] = strategySum[i] / normalizingSum;
        }
        else {
            avgStrategy[i] = 1.0 / actions;
        }
    }
    return avgStrategy;
}


// Two player training function
vector<vector<float>> train2Player(int iterations, vector<float> &regretSum1, vector<float> &regretSum2, vector<float> &p2Strat) {
    // Adapt train function for two players
    int actions = 3;
    vector<int> actionUtility= {0,0,0};
    vector<float> strategySum1 = {0,0,0};
    vector<float> strategySum2 = {0,0,0};
    for(int i=0; i< iterations; i++) {
        // Retrieve Actions
        vector<vector<float>>t1 = getStrategy(regretSum1, strategySum1);
        vector<float> strategy1 = t1[0];
        strategySum1 = t1[1];
        float myaction = getAction(strategy1);
        vector<vector<float>>t2 = getStrategy(regretSum2, p2Strat);
        vector<float> strategy2 = t2[0];
        strategySum2 = t2[1];
        float otherAction = getAction(strategy2);

        // Opponent Chooses scissors
        if (otherAction == actions -1) {
            // Utility(Rock) = 1
            actionUtility[0] = 1;
            // Utility(Paper) = -1
            actionUtility[1] = -1;
        }
        // Opponent chooses Rock
        else if (otherAction == 0) {
            // Utility(Scissors) = -1
            actionUtility[actions-1] = -1;
            // Utility(Paper) = 1
            actionUtility[1] = 1;
        }
        // Opponent Chooses Paper
        else {
            // Utility(Rock) = -1
            actionUtility[0] = -1;
            // Utility(Scissors) = 1
            actionUtility[2] = 1;
        }

        // Add the regrets from this decision 
        for(int i=0 ; i< actions; i++){
            regretSum1[i] += actionUtility[i] - actionUtility[myaction];
            regretSum2[i] += -(actionUtility[i] - actionUtility[myaction]);
        }
    }

    vector<vector<float>> strategySum12;
    strategySum12.push_back(strategySum1);
    strategySum12.push_back(strategySum2);
    return strategySum12;
}

// Returns the Nash Equilibrium reached by two opponents throught Counterfactual Regret Minimisation:

vector<vector<float>> RPStoNash(int iterations, vector<float> &oppStrat) {
    vector<float> regretSum1 ={0,0,0};
    vector<float> regretSum2 ={0,0,0};
    vector<vector<float>> strats = train2Player(iterations, regretSum1, regretSum2, oppStrat);
    
    float s1 = sum(strats[0]);
    float s2 = sum(strats[1]);

    for(int i=0; i<3; i++) {
        if(s1>0){
            strats[0][i] = strats[0][i]/s1;
        }
        if(s2>0){
            strats[1][i] = strats[1][i]/s2;
        }
    }

    return strats;
}

int main(){

vector<float> oppStrat = {0.4,0.3,0.3}; clock_t time_req;
cout<<"Opponent's Strategy: ";
for(auto itr:oppStrat){
    cout<<itr<<" ";
}
time_req = clock();
vector<float> ans = getAverageStrategy(1000000,oppStrat);
cout<<"\nMaximally Exploitative Strategy: ";

for(auto itr:ans){
    cout<<itr<<" ";
}

vector<vector<float>> rpsToNash = RPStoNash(1000000,oppStrat);
cout<<"\nNash Equilibrium at: ";

for(auto itr:rpsToNash[0]){
    cout<<itr<<" ";
}
for(auto itr:rpsToNash[1]){
    cout<<itr<<" ";
}
time_req = clock() - time_req;
cout<<"\nProgram Execution Time: "<<time_req*1000/CLOCKS_PER_SEC<<" ms"<<endl;

return 0;
}