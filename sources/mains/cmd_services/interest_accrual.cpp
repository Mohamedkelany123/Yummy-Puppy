#include <PSQLController.h>
#include <TemplateManager.h>
#include <AccrualInterest.h>
#include <AccrualInterestFunc.h>
#include <common_orm.h>
#include <common.h>

int main(int argc, char** argv) {

    int threads_count = 1;

    bool connect = psqlController.addDataSource("main","192.168.65.216",5432,"ledger_closure_omneya","postgres","8ZozYD6DhNJgW7a");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threads_count);

    // Accrue interest aggregator

    PSQLJoinQueryIterator * accrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});

    string closure_date_string = "2024-05-20"; 

    accrualQuery->addExtraFromField("(select day from loan_app_loanstatushistroy lalsh  where loan_app_loan.id=lalsh.loan_id and status_type =0 and previous_status_id =loan_app_loan.marginalization_bucket_id and lalsh.status_id>loan_app_loan.marginalization_bucket_id and day <= new_lms_installmentextension.partial_accrual_date and status_id not in (6,7,8,12,13,14,15,16) order by id desc limit 1)","marginalization_history");
    accrualQuery->addExtraFromField("(select paid_at from payments_loanorder plo where plo.status=1 AND loan_app_loan.id=plo.loan_id order by paid_at desc limit 1)","last_order_date");
    accrualQuery->addExtraFromField("(select day from loan_app_loanstatushistroy lalsh where loan_app_loan.id=lalsh.loan_id and lalsh.reversal_order_id is null and lalsh.status_type=0 and lalsh.status_id=8 order by id desc limit 1)","settled_history");

    
    accrualQuery->filter(
        ANDOperator (
            // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::INTEREST_ACCRUAL-1),
            new OROperator (
                new UnaryOperator("new_lms_installmentextension.accrual_date", lte, closure_date_string),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.payment_status", eq, 1),
                    new UnaryOperator("new_lms_installmentextension.accrual_date", gt, closure_date_string)
                )
            ),
            new UnaryOperator("new_lms_installmentextension.accrual_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_loan.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "8, 15, 16")
        )
    );

    accrualQuery->setOrderBy("loan_app_loan.id");
    
    BlnkTemplateManager * accrualTemplateManager = new BlnkTemplateManager(8);

    AccrualInterestStruct accrualInterestStruct = {
        accrualTemplateManager
    };
    accrualQuery->process(threads_count, AccrualInterestFunc, (void*)&accrualInterestStruct);  
    delete(accrualTemplateManager);
    psqlController.ORMCommit(true,true,true, "main");  

    // Partial accrue interest aggregator
    PSQLJoinQueryIterator * partialAccrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});
    
    partialAccrualQuery->filter(
        ANDOperator (
            new UnaryOperator("new_lms_installmentextension.partial_accrual_date", lte, closure_date_string),
            // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::PARTIAL_INTEREST_ACCRUAL-1),
            new UnaryOperator("new_lms_installmentextension.partial_accrual_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.expected_partial_accrual_amount", ne, 0),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
            new OROperator(
                new UnaryOperator("new_lms_installmentextension.is_interest_paid", eq, true),
                new ANDOperator(
                    new UnaryOperator("new_lms_installmentextension.status_id", nin, "8, 15"),
                    new OROperator(
                        new UnaryOperator("new_lms_installmentextension.status_id", ne, 16),
                        new UnaryOperator("new_lms_installmentextension.payment_status", ne, 3)
                    )
                )
            ),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
            new UnaryOperator("new_lms_installmentextension.partial_accrual_date", ne, "new_lms_installmentextension.accrual_date", true)
        )
    );
    partialAccrualQuery->setOrderBy("loan_app_loan.id");

    BlnkTemplateManager * partialAccrualTemplateManager = new BlnkTemplateManager(8);

    AccrualInterestStruct partialAccrualInterestStruct = {
        partialAccrualTemplateManager
    };

    partialAccrualQuery->process(threads_count, PartialAccrualInterestFunc, (void*)&partialAccrualInterestStruct);
    delete(partialAccrualTemplateManager);
    psqlController.ORMCommit(true,true,true, "main");  
    // Settlement accrue interest aggregator
    
    PSQLJoinQueryIterator * settlementAccrualQuery = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});
    
    settlementAccrualQuery->filter(
        ANDOperator (
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", lte, closure_date_string),
            // new UnaryOperator("loan_app_loan.closure_status", eq, ledger_status::SETTLEMENT_INTEREST_ACCRUAL-1),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_ledger_amount_id", isnull, "", true),
            new UnaryOperator("new_lms_installmentextension.status_id", nin, "12, 13"),
            new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_amount", ne, 0),
            new UnaryOperator("loan_app_loan.status_id", nin, "12, 13"),
            new UnaryOperator("loan_app_installment.interest_expected", ne, 0)
        )
    );

    settlementAccrualQuery->setOrderBy("loan_app_loan.id");

    BlnkTemplateManager * settlementAccrualTemplateManager = new BlnkTemplateManager(8);

    AccrualInterestStruct settlementAccrualInterestStruct = {
        settlementAccrualTemplateManager
    };

    settlementAccrualQuery->process(threads_count, SettlementAccrualInterestFunc, (void*)&settlementAccrualInterestStruct);

    delete(settlementAccrualTemplateManager);
    psqlController.ORMCommit(true,true,true, "main");  


}