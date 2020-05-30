## Question 1

Steps:

1. Go to the gem5/src/cpu/simple/BaseSimpleCPU.py

2. Add the line 

   ```python
   branchPred = Param.BranchPredictor(<branch_predictor>, "Branch Predictor")
   ```
   
   Inside the class BaseSimpleCPU(BaseCPU):
   
   <branch_predictor> from LocalBP(), TournamentBP(), and BiModeBP() one at a time.
   
3.  Now run the following command in the gem5 installation directory.
    ```bash
    scons build/X86/gem5.opt -j5
    ```
4. Now run the hello program to generate the configs.ini in the md5out folder using the following command.

   ```bash
   ./build/X86/gem5.opt ./configs/example/se.py -c
   ./tests/test-progs/hello/bin/x86/linux/hello --cpu-type=TimingSimpleCPU
   ```

   Repeat this for each of the three branch predictors and keep the generated config.ini files for each execution.

   

   

 








