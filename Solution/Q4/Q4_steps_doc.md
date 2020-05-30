# Simulation Report

## Question 4:

 *Running the benchmarks for comparison: In this part, you are required to compare between the performance of three branch predictors - 2bit local, Tournament, Gshare (that you just implemented) using a benchmark program. The benchmark program named blocked matmul.c has been provided in Moodle. Run the program using se.py script with the following cache parameters:*
--l1d size=128kB --l1i size=128kB --l2 size=1MB --l1d assoc=2 --l1i assoc=2 --l2 assoc=4. 

---

## Simulation

### Benchmark

The benchmark used for testing the performance of the predictor is a blocked matrix multiplication which takes in three random matrices A, B, C as input and results C+ AB. Following is the C code of the most important block.

```C

void blocked_matmul(myfloat * restrict A, myfloat * restrict B, myfloat * restrict C, int I, int K, int J) {
    assert(I % 2 == 0);
    assert(J % 2 == 0);
    assert(K % 2 == 0);
    for (int i = 0; i < I; i += 2) {
        for (int j = 0; j < J; j += 2) {
            myfloat Ci0j0 = C[(i + 0) * J + (j + 0)];
            myfloat Ci0j1 = C[(i + 0) * J + (j + 1)];
            myfloat Ci1j0 = C[(i + 1) * J + (j + 0)];
            myfloat Ci1j1 = C[(i + 1) * J + (j + 1)];
            for (int k = 0; k < K; k += 2) {
                myfloat Ai0k0 = A[(i + 0) * K + (k + 0)];
                myfloat Ai0k1 = A[(i + 0) * K + (k + 1)];
                myfloat Ai1k0 = A[(i + 1) * K + (k + 0)];
                myfloat Ai1k1 = A[(i + 1) * K + (k + 1)];
                myfloat Bk0j0 = B[(k + 0) * J + (j + 0)];
                myfloat Bk0j1 = B[(k + 0) * J + (j + 1)];
                myfloat Bk1j0 = B[(k + 1) * J + (j + 0)];
                myfloat Bk1j1 = B[(k + 1) * J + (j + 1)];

                Ci0j0 += Ai0k0 * Bk0j0 + Ai0k1 * Bk1j0;
                Ci1j0 += Ai1k0 * Bk0j0 + Ai1k1 * Bk1j0;
                Ci0j1 += Ai0k0 * Bk0j1 + Ai0k1 * Bk1j1;
                Ci1j1 += Ai1k0 * Bk0j1 + Ai1k1 * Bk1j1;
            }
            C[(i + 0) * J + (j + 0)] = Ci0j0;
            C[(i + 0) * J + (j + 1)] = Ci0j1;
            C[(i + 1) * J + (j + 0)] = Ci1j0;
            C[(i + 1) * J + (j + 1)] = Ci1j1;
        }
    }
}


```

There are three nest loops present in the code. The total number of iteration is I*J\*K. In one iteration

there is J+1 mispredictions if one bit predictor is used. The typical pattern of Taken(T) Not Taken(NT ) in one iterations is as follows:

TTTT (TTTTTTTTTTTT)*k NT TTTT NT 



We use the three popular branch predictor namely 2 Bit Local Predictor, Tournament Predictor and GShare Predictor to analyse which the prediction accuracy for the above pattern.

---


#### 1. 2 Bit Local Predictor

##### Input Command

```bash
./build/X86/gem5.opt ./configs/example/se.py -c /home/sudarshan/Desktop/gem5_sim/bench/blocked_matmul --cpu-type=DerivO3CPU --caches --l1d_size=128kB --l1i_size=128kB --l2_size=1MB --l1d_assoc=2 --l1i_assoc=2 --l2_assoc=4 --cacheline_size=64 --bp-type=LocalBP
```


##### Output

```bash
gem5 Simulator System.  http://gem5.org
gem5 is copyrighted software; use the --copyright option for details.

gem5 compiled May 30 2020 18:51:42
gem5 started May 30 2020 19:05:18
gem5 executing on sudarshan-HP-Pavilion-Notebook, pid 8739
command line: ./build/X86/gem5.opt ./configs/example/se.py -c /home/sudarshan/Desktop/gem5_sim/bench/blocked_matmul --cpu-type=DerivO3CPU --caches --l1d_size=128kB --l1i_size=128kB --l2_size=1MB --l1d_assoc=2 --l1i_assoc=2 --l2_assoc=4 --cacheline_size=64 --bp-type=LocalBP

Global frequency set at 1000000000000 ticks per second
warn: DRAM device capacity (8192 Mbytes) does not match the address range assigned (512 Mbytes)
0: system.remote_gdb: listening for remote gdb on port 7000
**** REAL SIMULATION ****
info: Entering event queue @ 0.  Starting simulation...
warn: ignoring syscall access(...)
warn: ignoring syscall access(...)
warn: ignoring syscall access(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
info: Increasing stack size by one page.
Iteration 0: 4439507 cycles
Iteration 1: 4530531 cycles
Iteration 2: 4438588 cycles
Iteration 3: 4438710 cycles
Exiting @ tick 9566953500 because exiting with last active thread context
```

##### Configuration

```python
class LocalBP(BranchPredictor):
    type = 'LocalBP'
    cxx_class = 'LocalBP'
    cxx_header = "cpu/pred/2bit_local.hh"

    localPredictorSize = Param.Unsigned(16384, "Size of local predictor")
    localCtrBits = Param.Unsigned(2, "Bits per counter")
```

---

#### 2. Tournament Predictor

##### Input Command
```bash
./build/X86/gem5.opt ./configs/example/se.py -c /home/sudarshan/Desktop/gem5_sim/bench/blocked_matmul --cpu-type=DerivO3CPU --caches --l1d_size=128kB --l1i_size=128kB --l2_size=1MB --l1d_assoc=2 --l1i_assoc=2 --l2_assoc=4 --cacheline_size=64 --bp-type=TournamentBP

```

##### Output


```bash
gem5 Simulator System.  http://gem5.org
gem5 is copyrighted software; use the --copyright option for details.

gem5 compiled May 30 2020 18:51:42
gem5 started May 30 2020 19:59:04
gem5 executing on sudarshan-HP-Pavilion-Notebook, pid 25468
command line: ./build/X86/gem5.opt ./configs/example/se.py -c /home/sudarshan/Desktop/gem5_sim/bench/blocked_matmul --cpu-type=DerivO3CPU --caches --l1d_size=128kB --l1i_size=128kB --l2_size=1MB --l1d_assoc=2 --l1i_assoc=2 --l2_assoc=4 --cacheline_size=64 --bp-type=TournamentBP

Global frequency set at 1000000000000 ticks per second
warn: DRAM device capacity (8192 Mbytes) does not match the address range assigned (512 Mbytes)
0: system.remote_gdb: listening for remote gdb on port 7000
**** REAL SIMULATION ****
info: Entering event queue @ 0.  Starting simulation...
warn: ignoring syscall access(...)
warn: ignoring syscall access(...)
warn: ignoring syscall access(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
info: Increasing stack size by one page.
Iteration 0: 4439667 cycles
Iteration 1: 4530270 cycles
Iteration 2: 4438559 cycles
Iteration 3: 4568508 cycles
Exiting @ tick 9576490500 because exiting with last active thread context
```


##### Configuration

```python
class TournamentBP(BranchPredictor):
    type = 'TournamentBP'
    cxx_class = 'TournamentBP'
    cxx_header = "cpu/pred/tournament.hh"

    localPredictorSize = Param.Unsigned(2048, "Size of local predictor")
    localCtrBits = Param.Unsigned(2, "Bits per counter")
    localHistoryTableSize = Param.Unsigned(1024, "size of local history table")
    globalPredictorSize = Param.Unsigned(4096, "Size of global predictor")
    globalCtrBits = Param.Unsigned(2, "Bits per counter")
    choicePredictorSize = Param.Unsigned(4096, "Size of choice predictor")
    choiceCtrBits = Param.Unsigned(2, "Bits of choice counters")
```

---

#### 3. GShare Predictor

##### Input
```bash
./build/X86/gem5.opt ./configs/example/se.py -c /home/sudarshan/Desktop/gem5_sim/bench/blocked_matmul --cpu-type=DerivO3CPU --caches --l1d_size=128kB --l1i_size=128kB --l2_size=1MB --l1d_assoc=2 --l1i_assoc=2 --l2_assoc=4 --cacheline_size=64 --bp-type=GshareBP
```


##### Output

``` bash
gem5 Simulator System.  http://gem5.org
gem5 is copyrighted software; use the --copyright option for details.

gem5 compiled May 30 2020 18:51:42
gem5 started May 30 2020 20:14:25
gem5 executing on sudarshan-HP-Pavilion-Notebook, pid 30227
command line: ./build/X86/gem5.opt ./configs/example/se.py -c /home/sudarshan/Desktop/gem5_sim/bench/blocked_matmul --cpu-type=DerivO3CPU --caches --l1d_size=128kB --l1i_size=128kB --l2_size=1MB --l1d_assoc=2 --l1i_assoc=2 --l2_assoc=4 --cacheline_size=64 --bp-type=GshareBP

Global frequency set at 1000000000000 ticks per second
warn: DRAM device capacity (8192 Mbytes) does not match the address range assigned (512 Mbytes)
0: system.remote_gdb: listening for remote gdb on port 7000
**** REAL SIMULATION ****
info: Entering event queue @ 0.  Starting simulation...
warn: ignoring syscall access(...)
warn: ignoring syscall access(...)
warn: ignoring syscall access(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
warn: ignoring syscall mprotect(...)
info: Increasing stack size by one page.
Iteration 0: 4499386 cycles
Iteration 1: 4700625 cycles
Iteration 2: 4607151 cycles
Iteration 3: 4494626 cycles
Exiting @ tick 9736627500 because exiting with last active thread context

```

##### Configuration

```python
	class GshareBP(BranchPredictor):type = 'GshareBP'
	cxx_class = 'GshareBP'
	cxx_header = "cpu/pred/gshare.hh"
	globalHistoryBits=Param.Unsigned(16, "Size of global hitstory reg")
	localPredictorSize = Param.Unsigned(16384, "Size of local predictor")
	localCtrBits = Param.Unsigned(2, "Bits per counter")
```



The m5out of each of the simulation can be found in the folder named Q4. 

---

### Observations

The table shows the Prediction Accuracy of the three branch predictors

| Predictor        | Number of Branches           | Mispredicted Branches | Misprediction Accuracy | Prediction Accuracy|
| ------------- |:----------:| -----:|------:|-----:|
| Local 2 bit Predictor|561676 | 14221 | 2.531886|97.468114 |
| Tournament Predictor|561676 | 15178 | 2.702269| 97.297731|
| GShare Predictor|561676 | 22287 | 3.967945|96.032055 |



The GShare predictor has the worst accuracy among the three because the bench as explained above doesn't have correlation branches. We know that since the Gshare predictor uses the global history it works well with the correlation branches like the conditional branches eg. if/else etc.

The accuracy of the 2 bit predictor and the Tournament predictor is fairly same with very slight difference. We know that the 2 bit predictor works very well with the loop intensive programs and the benchmark as explained above is loop intensive. Moreover, – n-bit predictors (n>2) are not much better for loop based programs. 

Tournament predictors used the best of both the Pshare and the Gshare using a choice predictor which selects which prediction to consider. The small performance glitch of the tournament counter as compared to the local 2 bit counter could be due to the saturating nature of the choice predictor.



---