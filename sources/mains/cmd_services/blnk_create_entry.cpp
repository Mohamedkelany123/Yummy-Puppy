#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>
#include <Disbursefunc.h>
#include <CancelFunc.h>
#include <AccrualInterest.h>
#include <AccrualInterestFunc.h>
#include <UndueToDueFunc.h>
#include <PSQLUpdateQuery.h>



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
    // const char * step = "full_closure"; 




    const char * step = "disburse"; 
    string closure_date_string = "2024-06-10"; 
    int threadsCount = 1;
    bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus","postgres","postgres");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);


    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312")
        ),
        {{"closure_status",to_string(ledger_status::LEDGER_START)}}
        );
    psqlUpdateQuery.update();



    if ( strcmp (step,"disburse") == 0 || strcmp (step,"full_closure") == 0)
    {
        
        
        PSQLJoinQueryIterator*  psqlQueryJoin = DisburseLoan::aggregator(closure_date_string);

        BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4, -1);
        map<int,float> status_provision_percentage =  get_loan_status_provisions_percentage();

        DisburseLoanStruct disburseLoanStruct;
        disburseLoanStruct.blnkTemplateManager = blnkTemplateManager;
        disburseLoanStruct.current_provision_percentage = status_provision_percentage[1];

        psqlQueryJoin->process_aggregate(threadsCount, DisburseLoanFunc,(void *)&disburseLoanStruct);

        delete(blnkTemplateManager);
        delete(psqlQueryJoin);

        psqlController.ORMCommit(true,true,true, "main");  
        DisburseLoan::update_step();
    }



    if ( strcmp (step,"cancel") == 0 || strcmp (step,"full_closure") == 0)
    {
        PSQLJoinQueryIterator*  psqlQueryJoin = CancelLoan::aggregator(closure_date_string);

        CancelLoanStruct cancelLoanStruct;
        BlnkTemplateManager *  blnkTemplateManager_cancel = new BlnkTemplateManager(5, -1);
        cancelLoanStruct.blnkTemplateManager_cancel = blnkTemplateManager_cancel;
        BlnkTemplateManager * blnkTemplateManager_reverse = new BlnkTemplateManager(6, -1);
        cancelLoanStruct.blnkTemplateManager_reverse = blnkTemplateManager_reverse;
        
        psqlQueryJoin->process_aggregate(threadsCount, CancelLoanFunc,(void *)&cancelLoanStruct);

        delete(blnkTemplateManager_cancel);
        delete(blnkTemplateManager_reverse);
        delete(psqlQueryJoin);
        
        psqlController.ORMCommit(true,true,true, "main"); 
        CancelLoan::update_step();
    }

    if ( strcmp (step,"accrual") == 0 || strcmp (step,"full_closure") == 0)
    {
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
                new OROperator(
                    new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", true),
                    new ANDOperator(
                        new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", false),
                        new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date",gte, "new_lms_installmentextension.partial_accrual_date", true),
                        new UnaryOperator("loan_app_loan.status_id", eq,15)
                    ),
                    new ANDOperator(
                        new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date", isnull, "", false),
                        new UnaryOperator("new_lms_installmentextension.settlement_accrual_interest_date",gt, "new_lms_installmentextension.partial_accrual_date", true),
                        new UnaryOperator("loan_app_loan.status_id", ne,15)
                    )
                ),
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
                new UnaryOperator("loan_app_loan.status_id", nin, "12,13"),
                new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
                new UnaryOperator("new_lms_installmentextension.partial_accrual_date", ne, "new_lms_installmentextension.accrual_date", true),
                new UnaryOperator ("loan_app_loan.id",ne,"14312")
            )
        );
        partialAccrualQuery->setOrderBy("loan_app_loan.id");
        BlnkTemplateManager * partialAccrualTemplateManager = new BlnkTemplateManager(8, -1);

        AccrualInterestStruct partialAccrualInterestStruct = {
            partialAccrualTemplateManager
        };

        partialAccrualQuery->process(threadsCount, PartialAccrualInterestFunc, (void*)&partialAccrualInterestStruct);

        delete(partialAccrualTemplateManager);
        delete(partialAccrualQuery);

        psqlController.ORMCommit(true,true,true, "main"); 

        //-------------------------------------------------------------------------------------------------------------------------------------------
        // Accrue interest aggregator

        PSQLJoinQueryIterator * accrualQuery = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main")},
        {{{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, {{"loan_app_installment", "id"}, {"new_lms_installmentextension", "installment_ptr_id"}}});

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
                new UnaryOperator("new_lms_installmentextension.status_id", nin, "8, 15, 16"),
                new UnaryOperator ("loan_app_loan.id",ne,"14312")
            )
        );

        accrualQuery->setOrderBy("loan_app_loan.id");
        
        BlnkTemplateManager * accrualTemplateManager = new BlnkTemplateManager(8, -1);

        AccrualInterestStruct accrualInterestStruct = {
            accrualTemplateManager
        };
        
        accrualQuery->process(threadsCount, AccrualInterestFunc, (void*)&accrualInterestStruct);

        delete(accrualTemplateManager);
        delete(accrualQuery);
        
        psqlController.ORMCommit(true,true,true, "main");  

        //-------------------------------------------------------------------------------------------------------------------------------------------
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
                new UnaryOperator("loan_app_installment.interest_expected", ne, 0),
                new UnaryOperator ("loan_app_loan.id",ne,"14312")
            )
        );

        settlementAccrualQuery->setOrderBy("loan_app_loan.id");

        BlnkTemplateManager * settlementAccrualTemplateManager = new BlnkTemplateManager(8, -1);

        AccrualInterestStruct settlementAccrualInterestStruct = {
            settlementAccrualTemplateManager
        };

        settlementAccrualQuery->process(threadsCount, SettlementAccrualInterestFunc, (void*)&settlementAccrualInterestStruct);

        delete(settlementAccrualTemplateManager);
        delete(settlementAccrualQuery);

        psqlController.ORMCommit(true,true,true, "main");  

        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::SETTLEMENT_INTEREST_ACCRUAL)}}

        );
        psqlUpdateQuery.update(); 
    }


    if ( strcmp (step,"undueToDue") == 0 || strcmp (step,"full_closure") == 0)
    {
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

        delete(installments_becoming_due_iterator);
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

        delete(sticky_installments_becoming_due_iterator);
        delete(undueToDueTemplateManager);

        psqlController.ORMCommit(true,true,true, "main");  
        
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator(
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.closure_status",gte,0)
            ),
            {{"closure_status",to_string(ledger_status::LEDGER_UNDUE_TO_DUE)}}

            );
        psqlUpdateQuery.update();  
    }


    return 0;
}