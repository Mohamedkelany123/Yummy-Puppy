#ifndef _SETTLEMENT_LOANS_SETTLE_TRANSACTION_H_
#define _SETTLEMENT_LOANS_SETTLE_TRANSACTION_H_

#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>


class SettleTransaction : public LedgerClosureStep {
    private:
        int bank, bank_to, merchant_id, payment_request_id, category;
        double amount;
    public:
    SettleTransaction(int _merchant_id, int _bank, int _bank_to, int _payment_request_id, int _category, double _amount);
    LedgerAmount * _init_ledger_amount();
    void setupLedgerClosureService (LedgerClosureService* ledgerClosureService);
    static LedgerAmount* _get_amount(LedgerClosureStep* settleTransaction);
};



#endif