#include <DisburseLoans.h>


DisburseLoan::DisburseLoan(BlnkTemplateManager * _temp_manager, loan_app_loan_primitive_orm * _lal_orm, float short_term_principal, float long_term_principal, float percentage):LedgerClosureStep ()
{
    temp_manager = _temp_manager;
    lal_orm = _lal_orm;    
    template_id=4;
    //setupLedgerClosureService(this);
}

void DisburseLoan::setupLedgerClosureService (LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("_calc_long_term_receivable_balance",DisburseLoan::_calc_long_term_receivable_balance);
    ledgerClosureService->addHandler("_calc_short_term_receivable_balance",DisburseLoan::_calc_short_term_receivable_balance);
    ledgerClosureService->addHandler("_calc_mer_t_bl_fee",DisburseLoan::_calc_mer_t_bl_fee);
    ledgerClosureService->addHandler("_calc_provision_percentage",DisburseLoan::_calc_provision_percentage);
    ledgerClosureService->addHandler("_calc_cashier_fee",DisburseLoan::_calc_cashier_fee);
    ledgerClosureService->addHandler("_calc_bl_t_mer_fee",DisburseLoan::_calc_bl_t_mer_fee);
    ledgerClosureService->addHandler("_calc_loan_upfront_fee",DisburseLoan::_calc_loan_upfront_fee);
}

DisburseLoan::~DisburseLoan(){}




loan_app_loan_primitive_orm* DisburseLoan::get_loan_app_loan()  {
    return lal_orm;
}
void DisburseLoan::set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm) {
    lal_orm = _lal_orm;
}

BlnkTemplateManager* DisburseLoan::get_template_manager()  {
    return temp_manager;
}
void DisburseLoan::set_template_manager(BlnkTemplateManager* _temp_manager) {
    temp_manager = _temp_manager;
}

int DisburseLoan::get_template_id()  {
    return template_id;
}
void DisburseLoan::set_template_id(int _template_id)
{
    template_id = _template_id;
}




LedgerAmount DisburseLoan::_calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_provision_percentage(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_cashier_fee(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_loan_upfront_fee(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan)
{
    return LedgerAmount();
}
