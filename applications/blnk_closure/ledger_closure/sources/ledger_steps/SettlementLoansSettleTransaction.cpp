#include "SettlementLoansSettleTransaction.h"

SettleTransaction::SettleTransaction(int _merchant_id, int _bank, int _bank_to, int _payment_request_id, int _category, double _amount)
{
    bank = _bank;
    bank_to = _bank_to;
    amount = _amount;
    merchant_id = _merchant_id;
    payment_request_id = _payment_request_id;
    category = _category;
}

LedgerAmount *SettleTransaction::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(0);
    lg->setLoanId(0);
    lg->setMerchantId(merchant_id);
    lg->setMerchantPaymentRequestId(payment_request_id);
    lg->setReversalBool(false);
    if (category == 0) {
        lg->setCreditAccountId(bank);
    }
    else {
        lg->setDebitAccountId(bank_to);
    }
    return lg;
}

void SettleTransaction::setupLedgerClosureService(LedgerClosureService* ledgerClosureService)
{
    if (category == 0) {
        ledgerClosureService->addHandler("Payment of cash to settle with merchant for reconciliation", SettleTransaction::_get_amount);
    }
    else {
        ledgerClosureService->addHandler("Recieving of cash to settle from merchant for reconciliation:", SettleTransaction::_get_amount);
    }
}

LedgerAmount *SettleTransaction::_get_amount(LedgerClosureStep *settleTransaction)
{
    LedgerAmount* ledgerAmount = ((SettleTransaction*)settleTransaction)->_init_ledger_amount();
    double amount = ((SettleTransaction*) settleTransaction)->amount;
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}
