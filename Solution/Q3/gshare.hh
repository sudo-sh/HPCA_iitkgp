

#ifndef __CPU_PRED_GSHARE_PRED_HH__
#define __CPU_PRED_GSHARE_PRED_HH__


#include <vector>
#include "cpu/pred/bpred_unit.hh"
#include "base/sat_counter.hh"
#include "base/types.hh"
#include "params/GshareBP.hh"


class GshareBP : public BPredUnit
{
  public:
    GshareBP(const GshareBPParams *params);
    
    void uncondBranch(ThreadID tid, Addr pc, void * &bp_history);

    void squash(ThreadID tid, void *bp_history);

    bool lookup(ThreadID tid, Addr branch_addr, void * &bp_history);

    void btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history);

    void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history, bool squashed, const StaticInstPtr & inst, Addr corrTarget);

  private:
    void updateGlobalHistReg(bool taken);

    struct BPHistory {
        unsigned globalHistoryReg;
        
        bool finalPred;
    };

   
    //storing the bits of the global history register.
    unsigned globalHistoryReg;
    //indicates the size (length) of the global history register.
    unsigned globalHistoryBits;

    /** Mask to control how much history is stored. All of it might not be
     *  used. */
    unsigned historyRegisterMask;

    /** Local counters, each element of localCtrs is a Saturating counter */
    /** Number of counters in the local predictor. */
    unsigned localPredictorSize;

    /** Number of bits of the local predictor's counters. */
    unsigned localCtrBits;

    std::vector<SatCounter> localCtrs;

    
    /** Thresholds for the counter value; above the threshold is taken,
     *  equal to or below the threshold is not taken.
     */
    unsigned localThreshold;
};

#endif // __CPU_PRED_GSHARE_PRED_HH__