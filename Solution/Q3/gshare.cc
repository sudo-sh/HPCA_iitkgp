
#include "cpu/pred/gshare.hh"
#include "base/bitfield.hh"
#include "base/intmath.hh"

/*
 * Constructor for gshare BP
 */
GshareBP::GshareBP(const GshareBPParams *params)
    : BPredUnit(params), 
      globalHistoryReg(0), //initilize the global History registor to 0
      globalHistoryBits(ceilLog2(params->globalHistoryBits)),  //initilize the size of the global history register to be log2(localPredictorSize)
      localPredictorSize(params->localPredictorSize),
      localCtrBits(params->localCtrBits),
      localCtrs(localPredictorSize, SatCounter(localCtrBits))


{
	if (!isPowerOf2(localPredictorSize))
		fatal("Invalid local predictor size.\n");

	//set the mask of the global history register, to ensure the bits above globalHistoryBits are 0s.
	historyRegisterMask = mask(globalHistoryBits);


	//setting the threshold for the value in local counter to indicates a taken branch
	// This is equivalent to (2^(Ctr))/2 - 1
    localThreshold  = (unsigned) (ULL(1) << (localCtrBits  - 1)) - 1;

   

}


/*
 * Actions for an unconditional branch
 	1. create new record of bpHistory, and return it via bpHistory
 	2. update the record of global history register.
 */

void
GshareBP::uncondBranch(ThreadID tid, Addr pc, void * &bp_history)
{
	BPHistory *history = new BPHistory;
	//store the current global history register to the returning history
	history->globalHistoryReg = globalHistoryReg;
	//treat unconditional branch as a predict-to-take branch
	history->finalPred = true;
	//return the content in history to bpHistory
	bp_history = static_cast<void *>(history);
	updateGlobalHistReg(true);

	return ;
}

/*
 * Lookup the actual branch prediction.
 */
bool
GshareBP::lookup(ThreadID tid, Addr branch_addr, void * &bp_history)
{
	//hash the branchAddr with the global history register to get the index into the table of counter.
	unsigned localCtrsIdx = ((branch_addr >> instShiftAmt) ^ globalHistoryReg) & historyRegisterMask;
    assert(localCtrsIdx < localPredictorSize);
    
    //read the value from the local counters, and assign the judgement into the final_prediction
    bool final_prediction = localCtrs[localCtrsIdx] > localThreshold;

    //update the bpHistory
    BPHistory *history = new BPHistory;
    history->finalPred = final_prediction;
    history->globalHistoryReg = globalHistoryReg;
    bp_history = static_cast<void *>(history);

    //speculatively update the global history register.
    updateGlobalHistReg(final_prediction);

    return final_prediction;
}

/*
 * BTB Update actions, called when a BTB miss happen
 */
void
GshareBP::btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history)
{
	//force set the last prediction made to be 0
	globalHistoryReg &= (historyRegisterMask & ~ULL(1));
}

/*
 * Update data structures after getting actual decison 
 */
void
GshareBP::update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history, bool squashed, const StaticInstPtr & inst, Addr corrTarget)
{
	if(bp_history)
	{
		//case that the branch history is not null
		BPHistory *history = static_cast<BPHistory *>(bp_history);
		//1. get the index to the local counter for that branch address at that bpHistory time
		unsigned localCtrsIdx = ((branch_addr >> instShiftAmt) ^ history->globalHistoryReg) & historyRegisterMask;
		assert(localCtrsIdx < localPredictorSize);

		//2. update the local counter by the acutal judgement of the conditional branch
		if(taken)
		{
			localCtrs[localCtrsIdx]++;
		}
		else
		{
			localCtrs[localCtrsIdx]--;
		}

		//if the branch is mis-predicted
		if(squashed)
		{
			if(taken)
				globalHistoryReg = (history->globalHistoryReg << 1) | 1;
			else
				globalHistoryReg = (history->globalHistoryReg << 1);
				globalHistoryReg &= historyRegisterMask;
		}
		else
		{
			//the globalHistoryReg is already updated when lookup() is called.
			delete history;
		}
	}
	//otherwise do nothing
}

/*
 * Global History Registor Update 
 */
void
GshareBP::updateGlobalHistReg(bool taken)
{
	//shift the register and insert the new value.
	globalHistoryReg = taken ? (globalHistoryReg << 1) | 1 :
								(globalHistoryReg << 1);
	globalHistoryReg &= historyRegisterMask;
}

/*
 * Actions for squash
 */
void
GshareBP::squash(ThreadID tid, void *bp_history) {
	//retrieve the data from the bpHistory
	BPHistory *history = static_cast<BPHistory*>(bp_history);
	globalHistoryReg = history->globalHistoryReg;
	//release the memory
	delete history;
}


GshareBP*
GshareBPParams::create()
{
    return new GshareBP(this);
}