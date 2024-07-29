#include <DueToSecBond.h>

DueToSecuritizationBond::DueToSecuritizationBond(int _loan_id, float _amount, int _bond_id)
{
    loan_id = _loan_id;
    amount = _amount;
    bond_id = _bond_id;
}

LedgerAmount *DueToSecuritizationBond::_init_ledger_amount()
{
    LedgerAmount * lg = new LedgerAmount();
    lg->setLoanId(loan_id);
    lg->setBondId(bond_id);
    return lg;
}

void DueToSecuritizationBond::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("due to securatization bond", DueToSecuritizationBond::_get_sec_amount);
}

LedgerAmount *DueToSecuritizationBond::_get_sec_amount(LedgerClosureStep *dueToSecuritizationBond)
{
    double paid_sec_amt = ((DueToSecuritizationBond*)dueToSecuritizationBond)->get_paid_sec_amount();

    LedgerAmount * la = ((DueToSecuritizationBond*)dueToSecuritizationBond)->_init_ledger_amount();
    la->setAmount(paid_sec_amt);
    
    return la;
}

float DueToSecuritizationBond::get_paid_sec_amount()
{
    return amount;
}

DueToSecuritizationBond::~DueToSecuritizationBond()
{
}
