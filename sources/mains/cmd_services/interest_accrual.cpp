#include <PSQLController.h>
#include <TemplateManager.h>
#include <AccrualInterest.h>
#include <common_orm.h>
#include <common.h>

int main(int argc, char** argv) {

    int threads_count = 1;
    // Accrue interest aggregator

    PSQLJoinQueryIterator * accrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});

    string closure_date_string = "2024-05-15"; 
    
    accrualQuery->filter(
        ANDOperator (
            new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::INTEREST_ACCRUAL-1),
            OROperator (
                new UnaryOperator("new_lms_installmentextension.accrual_date", lte, closure_date_string),
                ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.payment_status", eq, 1),
                    new UnaryOperator("new_lms_installmentextension.accrual_date", eq, closure_date_string)
                )
            ),
            new UnaryOperator("new_lms_installmentextension.accrual_ledger_amount", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_loan.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0)
        )
    );

    accrualQuery->setOrderBy("id");

    accrualQuery->addExtraFromField("(select day from loan_app_loanstatushistroy lalsh  where loan_app_loan.id=lalsh.loan_id and status_type =0 and previous_status_id =loan_app_loan.marginalization_bucket_id and lalsh.status_id>loan_app_loan.marginalization_bucket_id and day <= new_lms_installmentextension.partial_accrual_date and status_id not in (6,7,8,12,13,14,15,16) order by id desc limit 1)","marginalization_history");
    accrualQuery->addExtraFromField("(select paid_at from payments_loanorder plo where plo.status=1 AND loan_app_loan.id=plo.loan_id order by paid_at desc limit 1)","last_order_date");
    accrualQuery->addExtraFromField("(select day from loan_app_loanstatushistroy lalsh where loan_app_loan.id=lalsh.loan_id and lalsh.reversal_order_id is null and lalsh.status_type=0 and lalsh.status_id=8 order by id desc limit 1)","settled_history");

    BlnkTemplateManager * accrualTemplateManager = new BlnkTemplateManager(8);

    accrualQuery->process(threads_count, [accrualTemplateManager](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
        AccrualInterest accrualInterest(orms, 1);
        // DisburseLoan disburseLoan (orms, current_provision_percentage);
        // LedgerClosureService * ledgerClosureService = new LedgerClosureService(&disburseLoan);
        // disburseLoan.setupLedgerClosureService(ledgerClosureService);
        // map <string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference ();
        // ledger_entry_primitive_orm* entry = blnkTemplateManager->buildEntry(BDate("2024-05-15"), ledgerAmounts);
        // ledger_amount_primitive_orm * la_orm =  blnkTemplateManager->getFirstLedgerAmountORM();
        // if (entry && la_orm) {
        //     disburseLoan.stampORMs(entry, la_orm);
        // }
        // else {
        //     cerr << "Can not stamp ORM objects\n";
        //     exit(1);
        // }
        // delete (ledgerClosureService);
    });


    // Partial accrue interest aggregator
    PSQLJoinQueryIterator * partialAccrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});
    
    partialAccrualQuery->filter(
        ANDOperator (
            new UnaryOperator("new_lms_installmentextension.partial_accrual_date", lte, closure_date_string),
            new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::PARTIAL_INTEREST_ACCRUAL-1),
            new UnaryOperator("new_lms_installmentextension.partial_accrual_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.expected_partial_accrual_amount", ne, 0),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
            OROperator(
                new UnaryOperator("new_lms_installmentextension.is_interest_paid", eq, true),
                ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.status_id", nin, "8, 15"),
                    OROperator(
                        new UnaryOperator("new_lms_installmentextension.status_id", ne, 16),
                        new UnaryOperator("new_lms_installmentextension.payment_status", ne, 3)
                    )
                )
            ),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("new_lms_installmentextension.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.partial_accrual_date", ne, "new_lms_installmentextension.accrual_date")
        )
    );
    partialAccrualQuery->setOrderBy("id");

    // Settlement accrue interest aggregator
    
    PSQLJoinQueryIterator * settlementccrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});
    
    settlementccrualQuery->filter(
        ANDOperator (
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", lte, closure_date_string),
            new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_INTEREST_ACCRUAL-1),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.status", nin, "12, 13"),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_amount", ne, 0),
            new UnaryOperator("loan_app_loan.status_id", nin, "12, 13"),
            new UnaryOperator("new_lms_installmentextension.interest_expected", ne, 0)
        )
    );

    settlementccrualQuery->setOrderBy("id");

}