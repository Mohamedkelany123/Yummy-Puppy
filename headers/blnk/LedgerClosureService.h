#ifndef _LEDGER_CLOSURE_SERVICE_H_
#define _LEDGER_CLOSURE_SERVICE_H_

#include <common.h>
#include <LedgerClosureStep.h>


class LedgerClosureService {

    private:
            map <string,funcPtr> funcMap;
            LedgerClosureStep * ledgerClosureStep;
    public:
        LedgerClosureService (LedgerClosureStep * _ledgerClosureStep);
        void addHandler (string legName, funcPtr func);
        map <string,LedgerAmount> inference ();
};


#endif