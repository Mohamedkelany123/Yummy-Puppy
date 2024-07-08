#include <MarginaliseIncome.h>
#include "MarginalizeIncome.h"

void MarginalizeIncome::setupLedgerClosureService(LedgerClosureService *ledgerClosureService)
{
    ledgerClosureService->addHandler("Interest income becoming overdue", DueToOverdue::_get_installment_insterest);
    ledgerClosureService->addHandler("Loan principal becoming overdue", DueToOverdue::_get_installment_principal);
    ledgerClosureService->addHandler("Late repayment fee income accrual",DueToOverdue::_calc_installment_late_fees);
}

PSQLJoinQueryIterator *MarginalizeIncome::aggregator(string _closure_date_string)
{

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator("main",
        {
            new loan_app_loan_primitive_orm("main"), 
            new loan_app_installment_primitive_orm("main"), 
            new new_lms_installmentextension_primitive_orm("main"),
            new new_lms_installmentlatefees_primitive_orm("main")
        },
        {
            {{"loan_app_loan", "id"}, {"crm_app_purchase", "loan_id"}}, 
            {{"loan_app_loan", "id"}, {"loan_app_installment", "loan_id"}}, 
            {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}},
            {{"new_lms_installmentextension", "installment_ptr_id"}, {"new_lms_installmentlatefees", "installment_extension_id"}}

        });

psqlQueryJoin->filter(
    ((is_marginalized == False) AND(marginalization_date != None))
        ANDOperator(
            new OROperator(
                new UnaryOperator("new_lms_installmentextension.is_marginalized", eq, true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.is_marginalized", eq, false),
                    new UnaryOperator("new_lms_installmentextension.marginalization_date", isnotnull, "", true))) new UnaryOperator("new_lms_installmentextension.marginalization_date", lte, _closure_date_string),
            new UnaryOperator("new_lms_installmentextension.marginalization_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.accrual_ledger_amount_id", isnotnull, "", true)

            // new UnaryOperator ("loan_app_loan.closure_status",eq,ledger_status::MARGINALIZE_INCOME-1),

            new OROperator(
                new UnaryOperator("new_lms_installmentlatefees.is_marginalized", eq, true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentlatefees.is_marginalized", eq, false),
                    new UnaryOperator("new_lms_installmentlatefees.marginalization_date", isnotnull, "", true)))

                new UnaryOperator("new_lms_installmentlatefees.marginalization_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentlatefees.marginalization_date", lte, _closure_date_string)));

psqlQueryJoin->setAggregates(
    {{"loan_app_loan", {"id", 1}},
    {"new_lms_installmentextension", {"marginalization_date", 2}}
});

psqlQueryJoin->setOrderBy("crm_app_customer.id asc ,loan_app_loan.id asc,  crm_app_purchase.id asc");

return psqlQueryJoin;
}
void MarginalizeIncome::update_step()
{
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::MARGINALIZE_INCOME)}}

        );
    psqlUpdateQuery.update(); 
}
MarginalizeIncome::MarginaliseIncome()
{
}

MarginalizeIncome::MarginaliseIncome(loan_app_loan_primitive_orm *_lal_orm, loan_app_installment_primitive_orm *_lai_orm, new_lms_installmentextension_primitive_orm *_nlie_orm, new_lms_installmentlatefees_primitive_orm *_nlilf_orm, BDate _marginalization_date)
{
    lal_orm = _lal_orm;
    lai_orm = _lai_orm;
    nlie_orm = _nlie_orm;
    nlilf_orm = _nlilf_orm;
    marginalization_date = _marginalization_date;
}

void MarginalizeIncome::set_loan_app_loan(loan_app_loan_bl_orm *_lal_orm)
{
    lal_orm = _lal_orm;
}

void MarginalizeIncome::set_loan_app_installment(loan_app_installment_primitive_orm *_lai_orm)
{
    lai_orm = _lai_orm;
}

void MarginalizeIncome::set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nlie_orm) {
    nlie_orm = _nlie_orm;
}

void MarginalizeIncome::set_new_lms_installmentlatefees(new_lms_installmentlatefees_primitive_orm *_nlilf_orm)
{
    nlilf_orm = _nlilf_orm;
}

void MarginalizeIncome::set_template_id(int _template_id)
{
    template_id = _template_id;
}

void MarginalizeIncome::set_closing_day(BDate _closing_day)
{
    closing_day = _closing_day;
}

void MarginalizeIncome::set_marginalization_date(BDate _due_to_overdue_date)
{
    due_to_overdue_date = _due_to_overdue_date;
}

loan_app_loan_primitive_orm *MarginalizeIncome::get_loan_app_loan()
{
    return lal_orm;
}

loan_app_installment_primitive_orm* MarginalizeIncome::get_loan_app_installment()
{
    return lai_orm;
}

new_lms_installmentextension_primitive_orm* MarginalizeIncome::get_new_lms_installment_extention()
{
    return nlie_orm;
}

new_lms_installmentlatefees_primitive_orm *MarginalizeIncome::get_new_lms_installmentlatefees()
{
    return nlilf_orm;
}

int MarginalizeIncome::get_template_id()
{
    return template_id;
}

BDate MarginalizeIncome::get_closing_day()
{
    return closing_day;
}

BDate MarginalizeIncome::get_marginalization_date()
{
    return due_to_overdue_date;
}