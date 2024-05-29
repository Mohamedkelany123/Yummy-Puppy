#include <DisburseLoans.h>


DisburseLoan::DisburseLoan(map <string,PSQLAbstractORM *> * _orms, float _percentage):LedgerClosureStep ()
{
    loan_app_loan_bl_orm * _lal_orm = ORMBL(loan_app_loan,_orms);
    loan_app_loanproduct_primitive_orm * _lalp_orm = ORM(loan_app_loanproduct,_orms);
    crm_app_customer_primitive_orm * _cac_orm = ORM(crm_app_customer,_orms);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    float _short_term_principal = gorm->toFloat("short_term_principal");
    float _long_term_principal = gorm->toFloat("long_term_principal");
    bool _is_rescheduled = gorm->toBool("is_rescheduled");   
    lal_orm = _lal_orm;    
    cac_orm = _cac_orm;
    lalp_orm = _lalp_orm;
    prov_percentage = _percentage;
    short_term_principal = _short_term_principal;
    is_rescheduled = _is_rescheduled;

}


void DisburseLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    if(is_rescheduled){
        ledgerClosureService->addHandler("Booking rescheduled loan - long term, if applicable", DisburseLoan::_calc_long_term_receivable_balance);
        ledgerClosureService->addHandler("Booking rescheduled loan - short term; and", DisburseLoan::_calc_short_term_receivable_balance);
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


LedgerAmount*  DisburseLoan::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCashierId(lal_orm->get_cashier_id());
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());

    return lg;
}

void DisburseLoan::stampORMs(ledger_entry_primitive_orm *entry, ledger_amount_primitive_orm *la_orm){
    // cout << "Disbursing loan id: " << la_orm->get_loan_id() << endl;
    lal_orm->setUpdateRefernce("loan_creation_ledger_entry_id", entry);
    lal_orm->set_lms_closure_status(ledger_status::DISBURSE_LOAN);
    vector <new_lms_installmentextension_primitive_orm *> * ie_list = lal_orm->get_new_lms_installmentextension_loan_id();
    if (la_orm != NULL)
    {
        for ( auto i : *ie_list)
        {
            if(!i->get_is_long_term()){
                i->setUpdateRefernce("short_term_ledger_amount_id", la_orm);
            }

        }
    }
    else cout << "ERROR in fetching first leg of the entry " << endl;
}

float DisburseLoan::_calculate_loan_upfront_fee(){
    loan_app_loan_bl_orm* lal_orm = get_loan_app_loan();
    crm_app_customer_primitive_orm* cac_orm = get_crm_app_customer();
    loan_app_loanproduct_primitive_orm* lp_orm = get_loan_app_loanproduct();
    json upfront_fee;  
    float fee = 0.0;

    if (cac_orm->get_limit_source() == 1) {
        upfront_fee = lp_orm-> get_transaction_upfront_income_banked();
    }
    else {
        upfront_fee = lp_orm-> get_transaction_upfront_income_unbanked();
    }
    if (upfront_fee["type"] == "Paid in Cash") {
        if (upfront_fee["data"]["option"] == "flat_fee"){
            fee = float(upfront_fee["data"]["flat_fee"]);
        }
        else if (upfront_fee["data"]["option"] == "percentage"){
            fee = ROUND((float(upfront_fee["data"]["percentage"])) / 100 * (lal_orm->get_principle()));
            if (upfront_fee["data"].contains("floor") && fee < float(upfront_fee["data"]["floor"]))
                fee = float(upfront_fee["data"]["floor"]);
            if (upfront_fee["data"].contains("cap") && fee > float(upfront_fee["data"]["cap"]))
                fee = float(upfront_fee["data"]["cap"]);
            else if (upfront_fee["data"]["option"] == "both"){
                fee = float(upfront_fee["data"]["flat_fee_bo"]) + ROUND(float(upfront_fee["data"]["percentage_bo"])) / 100 * (lal_orm->get_principle());
                if (upfront_fee["data"].contains("floor_bo") && fee < float(upfront_fee["data"]["floor_bo"]))
                    fee = float(upfront_fee["data"]["floor_bo"]);
                if (upfront_fee["data"].contains("cap_bo") && fee > float(upfront_fee["data"]["cap_bo"]))
                    fee = float(upfront_fee["data"]["cap_bo"]);
            }
        }
    }
    return fee;
    
}

loan_app_loan_bl_orm* DisburseLoan::get_loan_app_loan()  {
    return lal_orm;
}
loan_app_loanproduct_primitive_orm *DisburseLoan::get_loan_app_loanproduct()
{
    return lalp_orm;
}
crm_app_customer_primitive_orm *DisburseLoan::get_crm_app_customer()
{
    return cac_orm;
}
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

void DisburseLoan::set_loan_app_loan(loan_app_loan_bl_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

void DisburseLoan::set_loan_app_loanproduct(loan_app_loanproduct_primitive_orm *_lalp_orm)
{
    lalp_orm = _lalp_orm;
}
void DisburseLoan::set_crm_app_customer(crm_app_customer_primitive_orm *_cac_orm)
{
    cac_orm = _cac_orm;
}


float DisburseLoan::get_long_term_principal()
{
    return long_term_principal;
}


void DisburseLoan::set_long_term_principal(float _long_term_principal)
{
    long_term_principal = _long_term_principal;
}

bool DisburseLoan::get_is_rescheduled()  {
    return is_rescheduled;
}
void DisburseLoan::set_is_rescheduled(bool _is_rescheduled)
{
    is_rescheduled =  _is_rescheduled;
}


LedgerAmount * DisburseLoan::_calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan)
 {  
    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la->setAmount(ROUND(((DisburseLoan*)disburseLoan)->get_short_term_principal()));
    return la;

}


LedgerAmount * DisburseLoan::_calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_bl_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double principal = loan_orm->get_principle();
    double merchant_to_blnk_fee = loan_orm->get_mer_t_bl_fee();
    double amount = ROUND(principal * (merchant_to_blnk_fee / 100));
    LedgerAmount * ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    ledgerAmount->setAmount(amount);
    return ledgerAmount;
}
LedgerAmount * DisburseLoan::_calc_provision_percentage(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_bl_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double perc = ((DisburseLoan*)disburseLoan)->get_provision_percentage()/100;
    double amount = ROUND(loan_orm->get_principle()*perc);

    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la->setAmount(amount);
    

    return la;
}
LedgerAmount * DisburseLoan::_calc_cashier_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_bl_orm* lal_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    
    float cashier_fee = (lal_orm->get_principle() * (lal_orm->get_cashier_fee()/ 100));
    la->setAmount(ROUND(cashier_fee));
 
    return la;
}
LedgerAmount * DisburseLoan::_calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan)
{
    loan_app_loan_bl_orm* loan_orm = ((DisburseLoan*)disburseLoan)->get_loan_app_loan();
    double perc = loan_orm->get_bl_t_mer_fee()/100;
    double amount = ROUND(loan_orm->get_principle()*perc);
    LedgerAmount * la = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    la->setAmount(amount);
    return la;
}
LedgerAmount * DisburseLoan::_calc_loan_upfront_fee(LedgerClosureStep *disburseLoan)
{
    LedgerAmount  * ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    ledgerAmount->setAmount(((DisburseLoan*)disburseLoan)->_calculate_loan_upfront_fee());
    return ledgerAmount;
}
LedgerAmount * DisburseLoan::_calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan)
{
    LedgerAmount * ledgerAmount = ((DisburseLoan*)disburseLoan)->_init_ledger_amount();
    float long_term_principal = ROUND(((DisburseLoan*)disburseLoan)->get_long_term_principal());
    ledgerAmount->setAmount(long_term_principal);
    return ledgerAmount;
}

