#ifndef _ReinforcementLearning_
#define _ReinforcementLearning_

#include "Toolbox.h"
#include <iostream>

class QLearning {
  private:
    int    n_actions;
    
    float  epsilon;

    minFind minInfo;
    maxFind maxInfo;

    randomGen randGenerator;

    void   set_actions();
    
  public:


    // int   *Actions = new int[n_actions];
    int   *Actions = new int[5];
    
    QLearning(int n_actions, float epsilon);
    int    get_random_action(int *actions_set);
    int    get_best_action(float *action_values, int *actions_set);
    int    epsilon_greedy(double reward[], int *actions_set, double actions_count[]);
    float  get_rewards(float current_position[], float previous_position[], float goal_position[], int action);
    float *get_action_values(double reward[], double actions_count[]);
    
};

QLearning::QLearning(int n_actions, float epsilon) {
    this->n_actions = n_actions;
    this->epsilon   = epsilon;

    randomGen randGenerator;
    this->randGenerator = randGenerator;

    minFind minInfo;
    this->minInfo = minInfo;

    maxFind maxInfo;
    this->maxInfo = maxInfo;
    
    set_actions();
}

void QLearning::set_actions() {
    for(int i=0; i<n_actions; i++) {
        *(Actions + i) = i;
    }
}

int QLearning::get_random_action(int *actions_set) {
  
  minInfo.minFunc(actions_set, n_actions);
  int min_actions =  minInfo.minVal;
  
  maxInfo.maxFunc(actions_set, n_actions);
  int max_actions =  maxInfo.maxVal;
  
  int random_action = randGenerator.randInteger(min_actions, max_actions);
  
  return random_action;
}

int QLearning::get_best_action(float *action_values, int *actions_set) {
  
  maxInfo.maxFunc(action_values, n_actions);
  int max_action_value =  maxInfo.maxVal;
  int max_action_index =  maxInfo.maxIdx;
  
  int best_action = *(actions_set + max_action_index);

  return best_action;
  // return max_action_index;
}

int QLearning::epsilon_greedy(double reward[], int *actions_set, double actions_count[]) {
  
  // uniform random number :
  float rand = randGenerator.randUniform(0.0, 1.0);
  
  // None action :
  int action = -1;

  // exploitation and exploration : 
  if(rand > epsilon) {
    float *action_values = get_action_values(reward, actions_count);
    action = get_best_action(action_values, actions_set);
  }
  else {
    action = get_random_action(this->Actions);
  }
   return action; 
}

float QLearning::get_rewards(float current_position[], float previous_position[], float goal_position[], int action) {
  
  float reward = 0.0;
  float previous_distance = eucliden_distance(previous_position, goal_position);
  float current_distance  = eucliden_distance(current_position, goal_position);

  if(action == 0 && current_distance > 0.05) {
    reward -= 10.0;
  }
  if(current_distance <= 0.05 && action == 0) {
    reward += 50.0;
  }
  if(current_distance < previous_distance) {
    reward += 1.0 * current_distance;
    // reward += 1.0;
  }
  else {
    reward -= 2.0 * current_distance;
  }
  if(action == 3 || action == 4) {
    reward -= 1.0;
  }
  return reward;
}

float *QLearning::get_action_values(double reward[], double actions_count[]){
  float *action_values = new float[5];

  /// calculate action_values :
  for(int i=0; i<5; i++){
    if(actions_count[i] == 0.0) {
      action_values[i] = 0.0;
    }
    else {
      action_values[i] = reward[i] / actions_count[i];
      // action_values[i] = reward[i];
    }
  }
  return action_values;
}

#endif