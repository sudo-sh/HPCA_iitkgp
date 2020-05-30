## Question 2

![image-20200531002055925](/home/sudarshan/.config/Typora/typora-user-images/image-20200531002055925.png)

We need to add the above two variables in the stats file.



For adding  BTBMissPct 

Steps 

1. Go the file $gem5/src/cpu/pred/bpred unit.hh

2. Add the line in following line in the private section where all Stats::<variables> are mentioned

   ```C
       Stats::Formula BTBMissPct;
   ```

   

3. Go to the file $gem5/src/cpu/pred/bpred unit.cc

4. Add the following line in the function void BPredUnit::regStats()

   ```C
   
       BTBMissPct
           .name(name() + ".BTBMissPct")
           .desc("BTB Miss Percentage")
           .precision(6);
       BTBMissPct = (1-(BTBHits / BTBLookups)) * 100;      
   
   ```

   

   

For adding BranchMispredPercent

Steps

1. Go the file gem5/src/cpu/simple/exec context.hh

2. Add the line in following line in the public section where all Stats::<variables> are mentioned

   ```C
      Stats::Formula numBranchMispredPercent;
   ```

   

3. Go to the file gem5/src/cpu/simple/base.cc

4. Add the following lines in the function void BaseSimpleCPU::regStats()

   ```C
       t_info.numBranchMispredPercent = (t_info.numBranchMispred/t_info.numBranches)*100;
   
           t_info.numBranchMispredPercent
               .name(thread_str + ".BranchMispredPercent")
               .desc("Number of branch mispredictions percentage")
               .prereq(t_info.numBranchMispred);    
   
   ```

   Save all the files.

   

   

Now run the following command in the gem5 installation directory.
    ```bash
    scons build/X86/gem5.opt -j5
    ```

Now run the hello program to generate the status.txt in the md5out folder using the following command.

```bash
./build/X86/gem5.opt ./configs/example/se.py -c
./tests/test-progs/hello/bin/x86/linux/hello --cpu-type=TimingSimpleCPU
```

