#include "parameter.h"
#include <numeric>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <stdexcept>


long Parameter::seed = 12;          

// ecga parameter
int Parameter::pop_size = 200;             
int Parameter::tour_size = 8;              
double Parameter::cross_prob = 0.975;      
int Parameter::max_gen = 2000;            
int Parameter::len_chromo = 50;              

// msod parameter
vector<vector<int>> Parameter::dim_subproblems({{0}}); 

// discretization parameter
double Parameter::split_rate = 0.5;        
double Parameter::decrease_rate = 0.998;       
double Parameter::min_bound = 1e-5;        
double Parameter::lower = -100;           
double Parameter::upper = 100;            

// problem parameter
int Parameter::nfe = 0;
// int Parameter::maxFES;
int Parameter::m = 2;
int Parameter::k = 10;
int Parameter::funNum = 1;                   

// result output parameter
string Parameter::output_file = "../m_cec14.csv"; 


// ./your_program --seed 123 --pop_size 300 --funNum 10 --output_file results.csv

void Parameter::parseArguments(int argc, char* argv[]) 
{
    for (int i = 1; i < argc; ++i) 
    {
        string arg = argv[i];
        if (arg == "--funNum" && i + 1 < argc) {
            Parameter::funNum = atoi(argv[++i]);
        }
        else if (arg == "--m" && i + 1 < argc) {
            Parameter::m = atoi(argv[++i]);
        }               
        else {
            assert(false && "invalid arguement");
        }
    }
}

   
void Parameter::update() {
    Parameter::dim_subproblems.clear();
    Parameter::len_chromo = Parameter::m * Parameter::k;
    // 可分離函數單維劃分
    if (funNum == 8 || funNum == 10) {
        dim_subproblems.resize(k);
        for (int i = 0; i < k; ++i) {
            dim_subproblems[i] = {i};
        }
    }
    else {
        // 混合函數 (17-22) 子組合比例設定
        const std::map<int, std::vector<double>> hybridProbs = {
            {17, {0.3, 0.3, 0.4}},
            {18, {0.3, 0.3, 0.4}},
            {19, {0.2, 0.2, 0.3, 0.3}},
            {20, {0.2, 0.2, 0.3, 0.3}},
            {21, {0.1, 0.2, 0.2, 0.2, 0.3}},
            {22, {0.1, 0.2, 0.2, 0.2, 0.3}}
        };

        auto it = hybridProbs.find(funNum);
        if (it != hybridProbs.end()) {
            auto probs = it->second;
            dim_subproblems.resize(probs.size());

            // 打開 shuffle 文件
            std::stringstream ss;
            ss << "input_data/shuffle_data_" << funNum << "_D" << k << ".txt";
            std::ifstream infile(ss.str());
            if (!infile) {
                throw std::runtime_error("無法打開 shuffle 資料文件: " + ss.str());
            }

            // 計算各子組合應分配的維度數，並處理累計四捨五入誤差
            int allocated = 0;
            int segments = static_cast<int>(probs.size());
            for (int i = 0; i < segments; ++i) {
                int count = (i < segments - 1)
                    ? static_cast<int>(std::round(k * probs[i]))
                    : (k - allocated);
                allocated += count;

                for (int j = 0, idx; j < count; ++j) {
                    if (!(infile >> idx)) {
                        throw std::runtime_error("讀取維度索引失敗，請檢查 shuffle 文件");
                    }
                    dim_subproblems[i].push_back(idx - 1);
                }
            }
        } else {
            // 其他所有函數視為非可分離，全維度作為一個子問題
            dim_subproblems.assign(1, std::vector<int>(k));
            std::iota(dim_subproblems[0].begin(), dim_subproblems[0].end(), 0);
        }
    }

    if (Parameter::m > 1){
        size_t initial_state = dim_subproblems.size();
        for (int i = 1; i < m; ++i){
            for (size_t j = 0; j < initial_state; ++j){
                dim_subproblems.emplace_back(dim_subproblems[j]);
                for (size_t k = 0; k < dim_subproblems[j].size(); ++k){
                    dim_subproblems.back()[k] += i * Parameter::k;
                }
            }
        }
    }
    

    // 打印維度子問題資訊
    // std::cout << "Function " << funNum << ": "
    //           << dim_subproblems.size() << " subproblem(s)\n";
    // for (size_t i = 0; i < dim_subproblems.size(); ++i) {
    //     const auto &sub = dim_subproblems[i];
    //     std::cout << "  Subproblem " << i + 1 << ": "
    //               << sub.size() << " dimensions -> [";
    //     for (size_t j = 0; j < sub.size(); ++j) {
    //         std::cout << sub[j];
    //         if (j + 1 < sub.size()) std::cout << ", ";
    //     }
    //     std::cout << "]\n";
    // }
}
