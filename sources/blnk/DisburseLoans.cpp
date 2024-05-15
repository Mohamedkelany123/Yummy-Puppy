#include <DisburseLoans.h>


DisburseLoan::DisburseLoan(BlnkTemplateManager * _temp_manager, loan_app_installment_primitive_orm * _lai_orm, new_lms_installmentextension_primitive_orm * _nli_orm,loan_app_loan_primitive_orm * _lal_orm)
{
    temp_manager = _temp_manager;
    lai_orm = _lai_orm;
    nli_orm = _nli_orm;
    lal_orm = _lal_orm;    
    generateFuncMap();
}

void DisburseLoan::generateFuncMap()
{
    funcMap["_calc_long_term_receivable_balance"] = DisburseLoan::_calc_long_term_receivable_balance;
    funcMap["_calc_short_term_receivable_balance"] = DisburseLoan::_calc_short_term_receivable_balance;
    funcMap["_calc_mer_t_bl_fee"] = DisburseLoan::_calc_mer_t_bl_fee;
    funcMap["_calc_provision_percentage"] = DisburseLoan::_calc_provision_percentage;
    funcMap["_calc_cashier_fee"] = DisburseLoan::_calc_cashier_fee;
    funcMap["_calc_bl_t_mer_fee"] = DisburseLoan::_calc_bl_t_mer_fee;
    funcMap["_calc_loan_upfront_fee"] = DisburseLoan::_calc_loan_upfront_fee;
}

DisburseLoan::~DisburseLoan(){}



void DisburseLoan::set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm) {
    lai_orm = _lai_orm;
}
loan_app_installment_primitive_orm* DisburseLoan::get_loan_app_installment()  {
    return lai_orm;
}

new_lms_installmentextension_primitive_orm* DisburseLoan::get_new_lms_installmentextension()  {
    return nli_orm;
}
void DisburseLoan::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nli_orm) {
    nli_orm = _nli_orm;
}

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

float get_provisions_percentage()
{
        //Query to return percentage from loan_app_provision
        PSQLJoinQueryIterator * psqlQueryJoinProvisions = new PSQLJoinQueryIterator ("main",
        {new loan_app_loanstatus_primitive_orm("main"),new loan_app_provision_primitive_orm("main")},
        {{{"loan_app_loanstatus","id"},{"loan_app_provision","status_id"}}});

        psqlQueryJoinProvisions->filter(
            UnaryOperator ("loan_app_loanstatus.name",eq,"CURRENT")
        );

        psqlQueryJoinProvisions->execute();
        map <string,PSQLAbstractORM *> * orms =  psqlQueryJoinProvisions->next();
        if (orms == nullptr){
            throw std::runtime_error( "Query Returns Null");
        }
        loan_app_provision_primitive_orm * lap_orm = ORM(loan_app_provision,orms);
        float percentage = lap_orm->get_percentage();
        // cout << "PERCENTAGE: " << percentage << endl;

        return percentage;
}



LedgerAmount DisburseLoan::_calc_short_term_receivable_balance(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_mer_t_bl_fee(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_provision_percentage(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_cashier_fee(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_bl_t_mer_fee(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_loan_upfront_fee(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
LedgerAmount DisburseLoan::_calc_long_term_receivable_balance(DisburseLoan *disburseLoan)
{
    return LedgerAmount();
}
