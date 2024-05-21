#include <CancelLoans.h>


CancelLoan::CancelLoan(loan_app_loan_bl_orm * _lal_orm):LedgerClosureStep ()
{

    lal_orm = _lal_orm;    
    
    template_id = 5;
   

}


void CancelLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Booking the transaction upfront fee", CancelLoan::_get_upfront_fee);
}

CancelLoan::~CancelLoan(){}


LedgerAmount*  CancelLoan::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCashierId(lal_orm->get_cashier_id());
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}




loan_app_loan_bl_orm* CancelLoan::get_loan_app_loan()  {
    return lal_orm;
}

void CancelLoan::set_loan_app_loan(loan_app_loan_bl_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}



int CancelLoan::get_template_id()  {
    return template_id;
}



void CancelLoan::set_template_id(int _template_id)
{
    template_id = _template_id;
}





LedgerAmount * CancelLoan::_get_upfront_fee(LedgerClosureStep *cancelLoan)
{
    loan_app_loan_bl_orm* loan_orm = ((CancelLoan*)cancelLoan)->get_loan_app_loan();
    double amount = ROUND(loan_orm->get_loan_upfront_fee() * !(loan_orm->get_refund_upfront_fee_bool()));
    LedgerAmount * ledgerAmount = ((CancelLoan*)cancelLoan)->_init_ledger_amount();
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}
