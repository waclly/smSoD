Author: Ryoji Tanabe
Date: 9/Jun/2014

These C++ files are based on the code of L-SHADE for Special Session & Competition on Real-Parameter Single Objective Optimization at CEC-2014.
The version submitted to the competition (version 1.0.0) had a bug in the archive update mechanism which resulted in a slight performance degradation.
Version 1.0.1 fixes the bug and achieves slightly better performance compared to 1.0.0.
See the details of L-SHADE in the following paper:

  * Ryoji Tanabe and Alex Fukunaga: Improving the Search Performance of SHADE Using Linear Population Size Reduction,  Proc. IEEE Congress on Evolutionary Computation (CEC-2014), Beijing, July, 2014.
  
System configurations in our experimental enviroment:

OS: Ubuntu 12.04 LTS
CPU: core i7 (2.20GHz)
RAM: 8GB
Language: C++
Compiler: g++ (gcc) with -O3 optimization flag

##############################
How to execute:

Step 1. #make
Step 2. #./lshade_exe

Dimension size and parameter settings of SHADE are easily changeable by rewriting source code in "main.cc".
If you have any questions, please feel free to contact me (rt.ryoji.tanabe [at] gmail.com).
