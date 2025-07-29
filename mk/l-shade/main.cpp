/*
  L-SHADE implemented by C++ for Special Session & Competition on Real-Parameter Single Objective Optimization at CEC-2014
  See the details of L-SHADE in the following paper:

  * Ryoji Tanabe and Alex Fukunaga: Improving the Search Performance of SHADE Using Linear Population Size Reduction,  Proc. IEEE Congress on Evolutionary Computation (CEC-2014), Beijing, July, 2014.
  
  Version: 1.1  Date: 9/Jun/2014
  Written by Ryoji Tanabe (rt.ryoji.tanabe [at] gmail.com)
*/

#include "de.h"
#include <random>

double *OShift,*M,*y,*z,*x_bound;
int ini_flag=0,n_flag,func_flag,*SS;

int g_function_number;
int g_problem_size;
unsigned int g_max_num_evaluations;

int g_pop_size;
double g_arc_rate;
int g_memory_size;
double g_p_best_rate;


int g_k, g_m; // k is the number of variables in each subproblem, m is the number of subproblems

#include <fstream>
#include <vector>
#include <string>
#include <mutex>
std::mutex fileMutex;
void write_csv(long seed, double error){

  const std::vector<std::string> header = {"m", "k", "length", "mode",  "eva_fun", "seed", "error"};
  std::lock_guard<std::mutex> guard(fileMutex);

  const string file_name = "../mk.csv";

  std::ifstream infile(file_name);
  bool isEmpty = infile.peek() == std::ifstream::traits_type::eof();
  infile.close();

  if (isEmpty) 
  {
      std::ofstream outfile(file_name);
      if (!outfile.is_open()) 
      {
          return;
      }
      for (size_t i = 0; i < header.size(); ++i) 
      {
          outfile << header[i];
          if (i < header.size() - 1) 
          {
              outfile << ",";
          }
      }
      outfile << "\n";
      outfile.close();
  }

  std::ofstream file(file_name, std::ios::app);
  file << g_m << ",";
  file << g_k << ",";
  file << g_problem_size << ",";
  file << "l-shade" << ",";
  file << g_max_num_evaluations << ",";
  file << seed << ",";
  file << std::scientific << std::setprecision(16) << error << "\n";
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) 
  {
    string arg = argv[i];
    if (arg == "--g_m" && i + 1 < argc) {
      g_m = atoi(argv[++i]);
    }        
    else if (arg == "--g_k" && i + 1 < argc) {
      g_k = atoi(argv[++i]);
    }     
  }
  g_max_num_evaluations = 400000;
  g_problem_size = g_m * g_k;

  //number of runs
  // int num_runs = 51;
  //   //dimension size. please select from 10, 30, 50, 100
  // g_problem_size = 10;
  // //available number of fitness evaluations 
  // g_max_num_evaluations = g_problem_size * 10000;

  //random seed is selected based on time according to competition rules
  // srand((unsigned)time(NULL));
  
  random_device rd;  
  long seed = static_cast<long>(rd());  
  srand(seed);

  //L-SHADE parameters
  g_pop_size = (int)round(g_problem_size * 18);
  g_memory_size = 5;
  g_arc_rate = 1.4;
  g_p_best_rate = 0.11;

  Fitness *bsf_fitness_array = (Fitness*)malloc(sizeof(Fitness) * 1);
  Fitness mean_bsf_fitness = 0;
  Fitness std_bsf_fitness = 0;

  int j = 0;
  searchAlgorithm *alg = new LSHADE();
  bsf_fitness_array[j] = alg->run();
  // cout << j + 1 << "th run, " << "error value = " << bsf_fitness_array[j] << endl;
  write_csv(seed, bsf_fitness_array[j]);

//  for (int i = 0; i < 30; i++) {
//     g_function_number = i + 1;
//     cout << "\n-------------------------------------------------------" << endl;
//     cout << "Function = " << g_function_number << ", Dimension size = " << g_problem_size << "\n" << endl;

//     Fitness *bsf_fitness_array = (Fitness*)malloc(sizeof(Fitness) * num_runs);
//     Fitness mean_bsf_fitness = 0;
//     Fitness std_bsf_fitness = 0;

//     for (int j = 0; j < num_runs; j++) { 
//       searchAlgorithm *alg = new LSHADE();
//       bsf_fitness_array[j] = alg->run();
//       cout << j + 1 << "th run, " << "error value = " << bsf_fitness_array[j] << endl;
//     }
  
//     for (int j = 0; j < num_runs; j++) mean_bsf_fitness += bsf_fitness_array[j];
//     mean_bsf_fitness /= num_runs;

//     for (int j = 0; j < num_runs; j++) std_bsf_fitness += pow((mean_bsf_fitness - bsf_fitness_array[j]), 2.0);
//     std_bsf_fitness /= num_runs;
//     std_bsf_fitness = sqrt(std_bsf_fitness);

//     cout  << "\nmean = " << mean_bsf_fitness << ", std = " << std_bsf_fitness << endl;
//     free(bsf_fitness_array);
//   }

  return 0;
}
