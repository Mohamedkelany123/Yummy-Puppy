#include <DueToOverdue.h>

PSQLJoinQueryIterator* DueToOverdue::installments_becoming_overdue_agg(string _closure_date_string)
{
     PSQLJoinQueryIterator * installments_becoming_due_iterator = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_bl_orm("main"), new new_lms_installmentlatefees_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
        {{{"new_lms_installmentlatefees", "installment_extension_id"}, {"new_lms_installmentextension", "installment_ptr_id"}}, {{"loan_app_loan", "id"}, {"new_lms_installmentextension", "loan_id"}}}
        );


        installments_becoming_due_iterator->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,_closure_date_string),
                new UnaryOperator("new_lms_installmentlatefees.day", lte, _closure_date_string),
                new UnaryOperator("new_lms_installmentlatefees.accrual_ledger_amount", isnull, true),
                new UnaryOperator("loan_app_loan.closure_status", eq, closure_status::DUE_TO_OVERDUE-1)
            )
        );
        
        vector<pair<string, string>> distinct_map = {
            {"loan_app_loan", "id"},
        };

        installments_becoming_due_iterator->setDistinct(distinct_map);
        
        return installments_becoming_due_iterator;
}

PSQLJoinQueryIterator *DueToOverdue::aggregator(string _closure_date_string)
{
    DueToOverdue dueToOverdue;
    return dueToOverdue.installments_becoming_overdue_agg(_closure_date_string);
}
