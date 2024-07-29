#ifndef DUE_TO_SEC_BOND_H
#define DUE_TO_SEC_BOND_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>

class DueToSecuritizationBond : public LedgerClosureStep
{
    private:
        int loan_id;
        int bond_id;
        float amount;

    public:
        map<string, funcPtr> funcMap;
        DueToSecuritizationBond(int _loan_id,float _amount, int _bond_id);

        LedgerAmount * _init_ledger_amount();
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        //static methods
        static LedgerAmount* _get_sec_amount(LedgerClosureStep *dueToSecuritizationBond);

        //Helpers
        float get_paid_sec_amount();

        ~DueToSecuritizationBond();
};  

#endif