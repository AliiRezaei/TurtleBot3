#ifndef _ReinforcementLearning_
#define _ReinforcementLearning_

#include "Toolbox.h"
#include <iostream>

class QLearning {
  private:

    int   n_actions;
    float epsilon;

    minFind minInfo;
    maxFind maxInfo;


    float x_space_precise;
    float y_space_precise;
    float theta_space_precise;

    int x_space_size;
    int y_space_size;
    int theta_space_size;

    int x_lower;
    int x_upper;

    int y_lower;
    int y_upper;

    int theta_lower;
    int theta_upper;

    std::size_t states_row_size;
    std::size_t states_col_size;

    
  public:

    int *actions = new int[5];
    randomGen randGenerator;
    
    QLearning(int n_actions, float epsilon);

    int    *create_actions(int nactions);
    float **create_states(stateInfo x, stateInfo y, stateInfo theta);
    float **create_state_action_pairs(float *all_states[], int *a);
    int     get_random_action(int *actions_set);
    int     get_best_action(float *action_values, int *actions_set);
    int     epsilon_greedy(double reward[], int *actions_set, double actions_count[]);
    int    *get_random_policy(int num);
    void    improve_policy(float *qtable, std::size_t qtable_len, int *last_policy);
    float  *zeros_init_Qtable(int num);
    float  *random_init_Qtable(int num, float min_rand, float max_rand);
    void    do_action(float *state, float *state_new, int action);
    float   get_reward(float *all_states[], float *current_state, float *previous_state, float *goal_state, int action);
    float  *get_action_values(double reward[], double actions_count[]);
    
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
    
    // create_actions();
    // create_states();
}

int *QLearning::create_actions(int n_actions) {
    for(int i=0; i<n_actions; i++) {
        *(this->actions + i) = i;
    }
    return this->actions;
}

float **QLearning::create_states(stateInfo x, stateInfo y, stateInfo theta) {
  // extract info :
  float x_min = x.min;
  float x_max = x.max;
  int   x_n   = x.n;
  this->x_space_precise = (x_max - x_min) / (x_n - 1.0);

  float y_min = y.min;
  float y_max = y.max;
  int   y_n   = y.n;
  this->y_space_precise = (y_max - y_min) / (y_n - 1.0);

  float theta_min = theta.min;
  float theta_max = theta.max;
  int   theta_n   = theta.n;
  this->theta_space_precise = (theta_max - theta_min) / (theta_n - 1.0);

  // x space :
  float *x_ = linspace<float>(x_min, x_max, x_n);
  // this->x_space = x_;
  this->x_space_size = x_n;
  this->x_lower = x_min;
  this->x_upper = x_max;
  
  // y space :
  float *y_ = linspace<float>(y_min, y_max, y_n);
  // this->y_space = y_;
  this->y_space_size = y_n;
  this->y_lower = y_min;
  this->y_upper = y_max;

  // theta space :
  float *theta_ = linspace<float>(theta_min, theta_max, theta_n);
  // this->theta_space = theta_;
  this->theta_space_size = theta_n;
  this->theta_lower = theta_min;
  this->theta_upper = theta_max;

  // create augmented states matrix :
  std::size_t all_states_row = x_n * y_n * theta_n;
  std::size_t all_states_col = 3;

  // allocate memory for the result matrix :
  float **augmented_states = new float*[all_states_row];
  for (std::size_t i = 0; i < all_states_row; i++) {
      augmented_states[i] = new float[all_states_col];
  }

  for(std::size_t i = 0; i < x_n; i++) {
    for(std::size_t j = 0; j < y_n; j++) {
      for(std::size_t k = 0; k < theta_n; k++) {
        std::size_t index = i * (y_n * theta_n) + j * theta_n + k;
        *(*(augmented_states + index) + 0) = x_[i];
        *(*(augmented_states + index) + 1) = y_[j];
        *(*(augmented_states + index) + 2) = theta_[k];
      }
    }
  }
  this->states_row_size = all_states_row;
  this->states_col_size = all_states_col;
  delete[] x_;
  delete[] y_;
  delete[] theta_;
  return augmented_states;
}

float **QLearning::create_state_action_pairs(float *all_states[], int *a) {
    std::size_t states_row_size = x_space_size * y_space_size * theta_space_size;
    std::size_t states_col_size = 3;
    std::size_t result_row_size = states_row_size * n_actions;
    std::size_t result_col_size = states_col_size + 1;

    // allocate memory for the result matrix :
    float **s_a_pairs = new float*[result_row_size];
    for (std::size_t i = 0; i < result_row_size; i++) {
        s_a_pairs[i] = new float[result_col_size];
    }

    for (std::size_t i = 0; i < states_row_size; ++i) {
        for (std::size_t j = 0; j < n_actions; ++j) {
            std::size_t index = i * n_actions + j;
            for (std::size_t k = 0; k < states_col_size; ++k) {
                *(*(s_a_pairs + index) + k) = all_states[i][k];
            }
            *(*(s_a_pairs + index) + states_col_size) = *(a + j);
        }
    }
    return s_a_pairs;
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
    action = get_random_action(this->actions);
  }
   return action; 
}

int *QLearning::get_random_policy(int num) {
  minInfo.minFunc(this->actions, n_actions);
  int min_actions =  minInfo.minVal;
  
  maxInfo.maxFunc(this->actions, n_actions);
  int max_actions =  maxInfo.maxVal;
  int *random_policy = new int[num];

  for(int i=0; i<num; i++) {
    *(random_policy + i) = randGenerator.randInteger(min_actions, max_actions);
  }
  return random_policy;
}

void QLearning::improve_policy(float *qtable, std::size_t qtable_len, int *last_policy) {
  maxFind maxInfo;
  float *qtable_window = new float[n_actions];
  for(std::size_t i=0; i<qtable_len/n_actions; i++) {
    std::size_t index = 0;
    for(std::size_t j=n_actions*i; j<n_actions*(i+1); j++) {
      *(qtable_window + index++) = *(qtable + j);
    }
    maxInfo.maxFunc(qtable_window, n_actions);
    *(last_policy + i) = maxInfo.maxIdx;
  }
  delete[] qtable_window;
}

float *QLearning::zeros_init_Qtable(int num) {
  float *q = new float[num];
  for(int i=0; i<num; i++) {
    *(q + i) = 0.0;
  }
  return q;
}

float *QLearning::random_init_Qtable(int num, float min_rand, float max_rand) {
  float *q = new float[num];
  for(int i=0; i<num; i++) {
    *(q + i) = randGenerator.randUniform(min_rand, max_rand);
  }
  return q;
}

void QLearning::do_action(float *state, float *state_new, int action) {
  
  float x = *(state + 0);
  float y = *(state + 1);
  float theta = *(state + 2);
  
  float x_new;
  float y_new;
  float theta_new;
  
  switch (action)
  {
  case 0:
    // move forward in theta direcction :
    x_new = x + x_space_precise * round(cos(theta));
    y_new = y + y_space_precise * round(sin(theta));
    theta_new = theta;
    break;
  
  case 1:
    // move backward in theta direcction :
    x_new = x - x_space_precise * round(cos(theta));
    y_new = y - y_space_precise * round(sin(theta));
    theta_new = theta;
    break;

  case 2:
    // turn cw :
    x_new = x;
    y_new = y;
    theta_new = theta - theta_space_precise;
    break;

  case 3:
    // turn ccw :
    x_new = x;
    y_new = y;
    theta_new = theta + theta_space_precise;
    break;

  default:
    x_new = x;
    y_new = y;
    theta_new = theta;
    break;
  }

  float reward = 0.0;

  *(state_new + 0) = x_new;
  *(state_new + 1) = y_new;
  *(state_new + 2) = theta_new;
}

float QLearning::get_reward(float *all_states[], float *current_state, float *previous_state, float *goal_state, int action) {
  // interact_with_env.state_next = previous_state;
  float reward = 0.0;
  int current_state_status = ismember<float>(current_state, all_states, this->states_row_size, this->states_col_size);
  // std::cout << current_state_status << std::endl;
  if(current_state_status != -1) {
    if(*(current_state + 0) == *(goal_state + 0) && *(current_state + 1) == *(goal_state + 1)) {
      reward = +10.0;
    }
    else {
      reward = -1.0;
    }
  }
  else {
    *(current_state + 0) = *(previous_state + 0);
    *(current_state + 1) = *(previous_state + 1);
    *(current_state + 2) = *(previous_state + 2);
    reward = -10.0;
  }
  return reward;
}

float *QLearning::get_action_values(double reward[], double actions_count[]){
  float *action_values = new float[n_actions];

  /// calculate action_values :
  for(int i=0; i<n_actions; i++){
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