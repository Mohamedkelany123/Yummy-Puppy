#ifndef _LEDGER_CLOSURE_STEP_H_
#define _LEDGER_CLOSURE_STEP_H_

#include <LedgerAmount.h>

class LedgerClosureService;

class LedgerClosureStep {
    protected:
    public:
        LedgerClosureStep(){}
        virtual void setupLedgerClosureService (LedgerClosureService * ledgerClosureService) = 0;
        virtual ~LedgerClosureStep(){}

};

typedef LedgerAmount (*funcPtr)(LedgerClosureStep *);


#endif