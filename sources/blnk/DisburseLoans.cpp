#include <DisburseLoans.h>


DisburseLoan::DisburseLoan(loan_app_loan_primitive_orm * _lal_orm, float short_term_principal, float long_term_principal, float _percentage):LedgerClosureStep ()
{
    lal_orm = _lal_orm;    
    template_id = 4;
    prov_percentage = _percentage;
    //setupLedgerCloslaureService(this);
}


void DisburseLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    // if(lal_orm->get_created)
    ledgerClosureService->addHandler("Booking rescheduled loan - long term, if applicable", DisburseLoan::_calc_long_term_receivable_balance_reschedled);
    ledgerClosureService->addHandler("Booking rescheduled loan - short term; and", DisburseLoan::_calc_short_term_receivable_balance_reschedled);
    ledgerClosureService->addHandler("Booking new loan - long term, if applicable", DisburseLoan::_calc_long_term_receivable_balance);
    ledgerClosureService->addHandler("Booking new loan - short term; and", DisburseLoan::_calc_short_term_receivable_balance);
    ledgerClosureService->addHandler("Booking the merchant’s commission income", DisburseLoan::_calc_mer_t_bl_fee);
    ledgerClosureService->addHandler("Booking an impairment provision", DisburseLoan::_calc_provision_percentage);
    ledgerClosureService->addHandler("Booking the cashier's commission expense", DisburseLoan::_calc_cashier_fee);
    ledgerClosureService->addHandler("Booking the merchant's commission expense", DisburseLoan::_calc_bl_t_mer_fee);
    ledgerClosureService->addHandler("Booking the transaction upfront fee", DisburseLoan::_calc_loan_upfront_fee);
}

DisburseLoan::~DisburseLoan(){}


LedgerAmount DisburseLoan::_init_ledger_amount(){
    LedgerAmount lg;
    lg.setCashierId(lal_orm->get_cashier_id());
    lg.setCustomerId(lal_orm->get_customer_id());
    lg.setLoanId(lal_orm->get_id());
    lg.setMerchantId(lal_orm->get_merchant_id());

    return lg;
}


loan_app_loan_primitive_orm* DisburseLoan::get_loan_app_loan()  {
    return lal_orm;
}
loan_app_loanproduct_primitive_orm *DisburseLoan::get_loan_app_loanproduct()
{
    return lalp_orm;
}
float DisburseLoan::get_provision_percentage()
{
    return prov_percentage;
}
void DisburseLoan::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

void DisburseLoan::set_loan_app_loanproduct(loan_app_loanproduct_primitive_orm *_lalp_orm)
{
    lalp_orm = _lalp_orm;
}

int DisburseLoan::get_template_id()  {
    return template_id;
}
void DisburseLoan::set_template_id(int _template_id)
{
    template_id = _template_id;
}

LedgerAmount DisburseLoan::_calc_short_term_receivable_balance_reschedled(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}

LedgerAmount DisburseLoan::_calc_long_term_receivable_balance_reschedled(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}

LedgerAmount DisburseLoan::_calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double principal = loan_orm->get_principle();
    double merchant_to_blnk_fee = loan_orm->get_mer_t_bl_fee();
    double amount = ROUND(principal * (merchant_to_blnk_fee / 100));
    LedgerAmount ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    ledgerAmount.setAmount(amount);
    return ledgerAmount;
}
LedgerAmount DisburseLoan::_calc_provision_percentage(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double perc = ((DisburseLoan*)disburseLoan)->get_provision_percentage()/100;
    double amount = round(loan_orm->get_principle()*perc);

    LedgerAmount la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la.setAmount(amount);
    

    return la;
}
LedgerAmount DisburseLoan::_calc_cashier_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_primitive_orm* lal_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    LedgerAmount la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    
    float cashier_fee = (lal_orm->get_principle() * (lal_orm->get_cashier_fee()/ 100));
    la.setCashierId(ROUND(cashier_fee));
 
    return la;
}
LedgerAmount DisburseLoan::_calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double perc = loan_orm->get_bl_t_mer_fee()/100;
    double amount = round(loan_orm->get_principle()*perc);
    LedgerAmount la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la.setAmount(amount);
}
LedgerAmount DisburseLoan::_calc_loan_upfront_fee(LedgerClosureStep *disburseLoan)
{
    // loan_app_loan_primitive_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();

    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
