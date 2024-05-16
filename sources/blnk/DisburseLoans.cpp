#include <DisburseLoans.h>


DisburseLoan::DisburseLoan(loan_app_loan_primitive_orm * _lal_orm, float _short_term_principal, float long_term_principal, float _percentage, bool _is_rescheduled):LedgerClosureStep ()
{
    lal_orm = _lal_orm;    
    template_id = 4;
    prov_percentage = _percentage;
    short_term_principal = _short_term_principal;
    is_rescheduled = _is_rescheduled;
    //setupLedgerCloslaureService(this);
}


void DisburseLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    if(is_rescheduled){
        ledgerClosureService->addHandler("Booking rescheduled loan - long term, if applicable", DisburseLoan::_calc_long_term_receivable_balance_reschedled);
        ledgerClosureService->addHandler("Booking rescheduled loan - short term; and", DisburseLoan::_calc_short_term_receivable_balance_reschedled);
    }
 
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

// float DisburseLoan::_calculate_loan_upfront_fee(){
//     loan_app_loan_primitive_orm* lal_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
//     crm_app_customer_primitive_orm* customer_orm = ((DisburseLoan*)disburseLoan)->get_crm_app_customer();
//     loan_app_loanproduct_primitive_orm* lp_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loanproduct();
//     json upfront_fee;  
//     float fee = 0.0;
//     if (customer_orm.get_limit_source() == 1) {
//         upfront_fee = lp_orm-> get_transaction_upfront_income_banked();
//     }
//     else {
//         upfront_fee = lp_orm-> get_transaction_upfront_income_unbanked();
//     }
//     if (upfront_data["type"] == "Paid in Cash") {
//         if upfront_data["data"]["option"] == "flat_fee":
//             fee = upfront_data["data"]["flat_fee"];
//     }
//     else if (upfront_data["data"]["option"] == "percentage"){
//         fee = ROUND((upfront_data["data"]["percentage"]) / 100 * (lal_orm->get_principle()));
//         if (upfront_data["data"].contains("floor") && fee < upfront_data["data"]["floor"])
//             {fee = upfront_data["data"]["floor"];}
//         if (upfront_data["data"].contains("cap") && fee > upfront_data["data"]["cap"])
//             {fee = upfront_data["data"]["cap"];}
//         else if (upfront_data["data"]["option"] == "both"){
//             fee = upfront_data["data"]["flat_fee_bo"] + ROUND(upfront_data["data"]["percentage_bo"]) / 100 * (lal_orm->get_principle());
//             if upfront_data["data"].contains("floor_bo") && fee < upfront_data["data"]["floor_bo"]
//                 {fee = upfront_data["data"]["floor_bo"];}
//             if upfront_data["data"].contains("cap_bo") && fee > upfront_data["data"]["cap_bo"]
//                 {fee = upfront_data["data"]["cap_bo"];}
//         }
//     }
//     return fee;
    
// }

loan_app_loan_primitive_orm* DisburseLoan::get_loan_app_loan()  {
    return lal_orm;
}
loan_app_loanproduct_primitive_orm *DisburseLoan::get_loan_app_loanproduct()
{
    return lalp_orm;
}
// crm_app_customer_primitive_orm *DisburseLoan::get_crm_app_customer()
// {
//     return cac_orm;
// }
float DisburseLoan::get_provision_percentage()
{
    return prov_percentage;
}
void DisburseLoan::set_provision_percentage(float _provision_percentage){
    prov_percentage = _provision_percentage;
}
float DisburseLoan::get_short_term_principal()
{
    return short_term_principal;
}
void DisburseLoan::set_short_term_principal(float _short_term_principal){
    short_term_principal = _short_term_principal;
}

void DisburseLoan::set_loan_app_loan(loan_app_loan_primitive_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

void DisburseLoan::set_loan_app_loanproduct(loan_app_loanproduct_primitive_orm *_lalp_orm)
{
    lalp_orm = _lalp_orm;
}
// void DisburseLoan::set_crm_app_customer(crm_app_customer_primitive_orm *_cac_orm)
// {
//     cac_orm = _cac_orm;
// }

int DisburseLoan::get_template_id()  {
    return template_id;
}
void DisburseLoan::set_template_id(int _template_id)
{
    template_id = _template_id;
}

bool DisburseLoan::get_is_recheduled()  {
    return is_rescheduled;
}
void DisburseLoan::set_is_recheduled(bool _is_rescheduled)
{
    template_id =  _is_rescheduled;
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
    LedgerAmount la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la.setAmount(round(((DisburseLoan*)disburseLoan)->get_short_term_principal()));
    return la;

}

LedgerAmount DisburseLoan::_calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan)
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
    // LedgerAmount ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    // ledgerAmount.setAmount(((DisburseLoan*)disburseLoan)->calculate_loan_upfront_fee());
    return LedgerAmount();

}
