#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>
#include <UndueToDueFunc.h>

//TODO: create special type for 

//<BuckedId,Percentage>
map<int,float> get_loan_status_provisions_percentage()
{
        //Query to return percentage from loan_app_provision
        PSQLJoinQueryIterator * psqlQueryJoinProvisions = new PSQLJoinQueryIterator ("main",
        {new loan_app_loanstatus_primitive_orm("main"),new loan_app_provision_primitive_orm("main")},
        {{{"loan_app_loanstatus","id"},{"loan_app_provision","status_id"}}});

        
        map<int,float> bucket_percentage;

        psqlQueryJoinProvisions->execute();
        map<string, PSQLAbstractORM *>* orms = psqlQueryJoinProvisions->next(true);
        loan_app_loanstatus_primitive_orm * lals_orm;
        loan_app_provision_primitive_orm * lap_orm; 
        while (orms != NULL){
            lals_orm = ORM(loan_app_loanstatus,orms);
            lap_orm = ORM(loan_app_provision,orms);
            bucket_percentage[lals_orm->get_id()] = lap_orm->get_percentage();
            delete(lals_orm);
            delete(lap_orm);
            delete(orms);
            orms = psqlQueryJoinProvisions->next(true);
        }
        delete (psqlQueryJoinProvisions);
        

        return bucket_percentage;
}


int main (int argc, char ** argv)
{

    int threadsCount = 10;
    bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"django_ostaz_before_closure","postgres","postgres");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);
    string closure_date_string = "2024-06-01"; 


    PSQLJoinQueryIterator * installments_becoming_due_iterator = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_bl_orm("main"), new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});


    installments_becoming_due_iterator->filter(
        ANDOperator 
        (
            new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,closure_date_string),
            // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::LEDGER_UNDUE_TO_DUE-1)),
            new OROperator (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_ledger_amount_id ",isnull,"",true),
                new ANDOperator (
                    new UnaryOperator ("new_lms_installmentextension.undue_to_due_interest_ledger_amount_id ",isnull,"",true),
                    new UnaryOperator ("loan_app_installment.interest_expected",ne,"0")
                )
            ),
            new UnaryOperator ("loan_app_loan.status_id",nin,"12,13"),
            new UnaryOperator ("new_lms_installmentextension.status_id",nin,"8,15,16,12,13")
        )
    );
    
    installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=8 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id desc limit 1)","settled_paid_off_day");
    installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=15 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id desc limit 1)","settled_charge_off_day_status");
    installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 26 and le.reverse_entry_id is null order by la.id desc limit 1)","undue_to_due_amount");
    installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null order by la.id desc limit 1)","undue_to_due_interest_amount");
    
    BlnkTemplateManager * undueToDueTemplateManager = new BlnkTemplateManager(10, -1);

    UndueToDueStruct undueToDueStruct;
    undueToDueStruct.blnkTemplateManager = undueToDueTemplateManager;
    undueToDueStruct.closing_day = BDate(closure_date_string);

    installments_becoming_due_iterator->process(threadsCount, InstallmentBecomingDueFunc, (void *)&undueToDueStruct);

    psqlController.ORMCommit(true,true,true, "main");  


    //----------------------------------------------------------------------------------------//
    //----------------------------------------------------------------------------------------//
    //----------------------------------------------------------------------------------------//

    PSQLJoinQueryIterator * sticky_installments_becoming_due_iterator = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_bl_orm("main"), new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

    
    sticky_installments_becoming_due_iterator->filter(
        ANDOperator 
        (
            new OROperator (
                // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::LEDGER_UNDUE_TO_DUE-1)),
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",gt,closure_date_string),
                new ANDOperator(
                    new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,true),
                    new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",gt,"interest_paid_at", true)
                )
            ),

            new UnaryOperator ("new_lms_installmentextension.payment_status",in,"2,4"),
            new UnaryOperator ("new_lms_installmentextension.is_principal_paid",eq,true),
            new UnaryOperator ("new_lms_installmentextension.principal_paid_at",lte,closure_date_string),

            new OROperator(
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_ledger_amount_id",isnull,"", true),
                
                new ANDOperator(
                    new UnaryOperator ("new_lms_installmentextension.settlement_accrual_interest_amount",gt,"0"),
                    new UnaryOperator ("new_lms_installmentextension.undue_to_due_interest_ledger_amount_id",isnull,"",true)
                )
            ),

            new UnaryOperator ("loan_app_loan.status_id",nin,"12,13"),



            new OROperator(
                new UnaryOperator ("new_lms_installmentextension.status_id",nin,"8,12,13"),
                new ANDOperator(
                    new UnaryOperator ("new_lms_installmentextension.status_id",ne,"16"),
                    new UnaryOperator ("new_lms_installmentextension.payment_status",ne,"3")
                )                
            )
        )
    );

    sticky_installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=8 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id desc limit 1)","settled_paid_off_day");
    sticky_installments_becoming_due_iterator->addExtraFromField("(select lal.day from loan_app_loanstatushistroy lal where lal.status_id=15 and lal.reversal_order_id is null and lal.status_type = 0 and lal.loan_id = loan_app_loan.id order by id desc limit 1)","settled_charge_off_day_status");
    sticky_installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 27 and le.reverse_entry_id is null order by la.id desc limit 1)","undue_to_due_amount");
    sticky_installments_becoming_due_iterator->addExtraFromField("(select la.id from ledger_amount la inner join ledger_entry le on le.id  = la.entry_id where la.installment_id = loan_app_installment.id and le.template_id = 10 and reversal_bool = false and account_id = 32 and le.reverse_entry_id is null order by la.id desc limit 1)","undue_to_due_interest_amount");

    UndueToDueStruct stickyUndueToDueStruct;
    stickyUndueToDueStruct.blnkTemplateManager = undueToDueTemplateManager;
    stickyUndueToDueStruct.closing_day = BDate(closure_date_string);

    sticky_installments_becoming_due_iterator->process(threadsCount, StickyInstallmentBecomingDueFunc, (void *)&stickyUndueToDueStruct);

    delete(undueToDueTemplateManager);

    psqlController.ORMCommit(true,true,true, "main");  

    


    return 0;
}
