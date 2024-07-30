#ifndef _LEDGER_CLOSURE_STEP_H_
#define _LEDGER_CLOSURE_STEP_H_

#include <LedgerAmount.h>

class LedgerClosureService;

typedef struct{
    bool isLoanSpecific;
    bool isMultiMachine;
    
    int mod_value;
    int offset;

    string loan_ids;
    string closure_date_string;
}QueryExtraFeilds;

class LedgerClosureStep {
    protected:
    public:
        LedgerClosureStep(){}
        virtual void setupLedgerClosureService (LedgerClosureService * ledgerClosureService) = 0;
        virtual ~LedgerClosureStep(){}

};

typedef LedgerAmount * (*funcPtr)(LedgerClosureStep *);


#endif