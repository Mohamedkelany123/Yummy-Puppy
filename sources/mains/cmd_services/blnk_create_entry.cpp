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
#include <DueToOverdueFunc.h>
#include <DueToOverdue.h>
#include <MarginalizeIncome.h>
#include <MarginalizeIncomeFunc.h>
#include <PSQLUpdateQuery.h>

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
    const char * step = "marginalizeIncome"; 
    string closure_date_string = "2024-07-22"; 
    int threadsCount = 1;
    bool connect = psqlController.addDataSource("main","192.168.65.216",5432,"django_ostaz_02072024_abdallah2","development","5k6MLFM9CLN3bD1");
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

    if ( strcmp (step,"accrual") == 0 || strcmp (step,"full_closure") == 0 || 1)
    {
        //Partial accrue interest aggregator
        PSQLJoinQueryIterator*  partialAccrualQuery = AccrualInterest::aggregator(closure_date_string, 1);

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
        PSQLJoinQueryIterator*  accrualQuery = AccrualInterest::aggregator(closure_date_string, 2);
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
        PSQLJoinQueryIterator*  settlementAccrualQuery = AccrualInterest::aggregator(closure_date_string, 3);
        BlnkTemplateManager * settlementAccrualTemplateManager = new BlnkTemplateManager(8, -1);
        AccrualInterestStruct settlementAccrualInterestStruct = {
            settlementAccrualTemplateManager
        };
        settlementAccrualQuery->process(threadsCount, SettlementAccrualInterestFunc, (void*)&settlementAccrualInterestStruct);
        delete(settlementAccrualTemplateManager);
        delete(settlementAccrualQuery);
        psqlController.ORMCommit(true,true,true, "main");  
        AccrualInterest::update_step();
    }


    if ( strcmp (step,"undueToDue") == 0 || strcmp (step,"full_closure") == 0)
    {
        PSQLJoinQueryIterator*  installments_becoming_due_iterator = UndueToDue::aggregator(closure_date_string, 1);
        BlnkTemplateManager * undueToDueTemplateManager = new BlnkTemplateManager(10, -1);
        UndueToDueStruct undueToDueStruct;
        undueToDueStruct.blnkTemplateManager = undueToDueTemplateManager;
        undueToDueStruct.closing_day = BDate(closure_date_string);
        
        installments_becoming_due_iterator->process(threadsCount, InstallmentBecomingDueFunc, (void *)&undueToDueStruct);
        
        delete(installments_becoming_due_iterator);
        psqlController.ORMCommit(true,true,true, "main");  

        //----------------------------------------------------------------------------------------//

        PSQLJoinQueryIterator*  sticky_installments_becoming_due_iterator = UndueToDue::aggregator(closure_date_string, 2);
        UndueToDueStruct stickyUndueToDueStruct;
        stickyUndueToDueStruct.blnkTemplateManager = undueToDueTemplateManager;
        stickyUndueToDueStruct.closing_day = BDate(closure_date_string);
        
        sticky_installments_becoming_due_iterator->process(threadsCount, StickyInstallmentBecomingDueFunc, (void *)&stickyUndueToDueStruct);
        
        delete(sticky_installments_becoming_due_iterator);
        delete(undueToDueTemplateManager);
        psqlController.ORMCommit(true,true,true, "main");  
        UndueToDue::update_step(); 
    }

    if (strcmp(step, "duetooverdue")==0 || strcmp(step, "full_closure")==0) {
        PSQLJoinQueryIterator*  installmentsBecomingOverdueIterator = DueToOverdue::aggregator(closure_date_string);
        BlnkTemplateManager* dueToOverdueTemplateManager = new BlnkTemplateManager(12, -1);
        DueToOverdueStruct dueToOverdueStruct;
        dueToOverdueStruct.blnkTemplateManager = dueToOverdueTemplateManager;
        dueToOverdueStruct.closing_day = BDate(closure_date_string);
        installmentsBecomingOverdueIterator->process_aggregate(threadsCount, InstallmentBecomingOverdueFunc, (void *)&dueToOverdueStruct);

        delete(installmentsBecomingOverdueIterator);
        psqlController.ORMCommit(true, false, true, "main");
        delete(dueToOverdueTemplateManager);
        DueToOverdue::update_step();
    }

     if (strcmp(step, "marginalizeIncome")==0 || strcmp(step, "full_closure")==0 || 1) {
        PSQLJoinQueryIterator*  marginalizeIncomeIterator = MarginalizeIncome::aggregator(closure_date_string);
        BlnkTemplateManager* marginalizeIncomeTemplateManager = new BlnkTemplateManager(34, -1);
        MarginalizeIncomeStruct marginalizeIncomeStruct;
        marginalizeIncomeStruct.blnkTemplateManager = marginalizeIncomeTemplateManager;
        marginalizeIncomeStruct.marginalization_day = BDate(closure_date_string);
        marginalizeIncomeIterator->process_aggregate(threadsCount, MarginalizeIncomeFunc, (void *)&marginalizeIncomeStruct);

        delete(marginalizeIncomeIterator);
        psqlController.ORMCommit(true, false, true, "main");
        delete(marginalizeIncomeTemplateManager);
        MarginalizeIncome::update_step();
    }


    return 0;
}

