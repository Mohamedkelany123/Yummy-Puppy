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
#include <InitialLoanInterestAccrualFunc.h>
#include <InitialLoanInterestAccrual.h>
#include <LongToShortTerm.h>
#include <LongToShortTermFunc.h>
#include <IScoreNidInquiry.h>
#include <IScoreNidInquiryFunc.h>
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

float get_iscore_nid_inquiry_fee(){
    ledger_global_primitive_orm_iterator * it = new ledger_global_primitive_orm_iterator("main");
    it->filter(
        ANDOperator(
        new UnaryOperator("ledger_global.name",eq,"iscore_nid_expense_fee")
        )
    );
    it->execute();
    ledger_global_primitive_orm * global_orm = it->next(true);
    if((global_orm->get_value())["amount"] != NULL){
        return global_orm->get_value()["amount"];
    }
    else cout << "ERROR in fetching NID iScore inquiry amount" << endl;

    return -1;
}

int main (int argc, char ** argv)
{
    // const char * step = "full_closure"; 
    const char * step = "iScoreNidInquiry"; 
    string closure_date_string = "2024-07-03"; 
    int threadsCount = 1;
    string databaseName = "django_ostaz_02072024_aliaclosure";
    bool connect = psqlController.addDataSource("main","192.168.65.216",5432,databaseName,"development","5k6MLFM9CLN3bD1");
    if (connect){
        cout << "--------------------------------------------------------" << endl;
        cout << "Connected to DATABASE->[" << databaseName << "]" << endl;
        cout << "Threads Count->[" << threadsCount << "]" << endl;
        cout << "Step[" << step << "]" << endl;
        cout << "--------------------------------------------------------" << endl;
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


    if ( strcmp (step,"ledger_accruel_initial_interest") == 0 || strcmp (step,"full_closure") == 0)
    {
        cout << "First Accrual" << endl;
        //FIRST ACCRUAL
        loan_app_loan_primitive_orm_iterator*  loans_to_get_first_accrual_agg = InitialLoanInterestAccrual::aggregator(closure_date_string, 1);
        
        BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(135, -1);

        InitialLoanInterestAccrualStruct initialLoanInterestFirstAccrualStruct;
        initialLoanInterestFirstAccrualStruct.blnkTemplateManager = blnkTemplateManager;
        initialLoanInterestFirstAccrualStruct.is_first_date = true;

        loans_to_get_first_accrual_agg->process(threadsCount, InitialLoanInterestAccrualFunc,(void *)&initialLoanInterestFirstAccrualStruct);

        delete(loans_to_get_first_accrual_agg);

        psqlController.ORMCommit(true,true,true, "main");  
        //-----------------------------------------------------------------------------------

        //SECOND ACCRUAL
        cout << "Second Accrual" << endl;
        loan_app_loan_primitive_orm_iterator*  loans_to_get_second_accrual_agg = InitialLoanInterestAccrual::aggregator(closure_date_string, 2);
        
        InitialLoanInterestAccrualStruct initialLoanInterestSecondAccrualStruct;
        initialLoanInterestSecondAccrualStruct.blnkTemplateManager = blnkTemplateManager;
        initialLoanInterestSecondAccrualStruct.is_first_date = false;

        loans_to_get_second_accrual_agg->process(threadsCount, InitialLoanInterestAccrualFunc,(void *)&initialLoanInterestSecondAccrualStruct);

        delete(blnkTemplateManager);
        delete(loans_to_get_second_accrual_agg);

        psqlController.ORMCommit(true,true,true, "main");  
        InitialLoanInterestAccrual::update_step();
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

    if ( strcmp (step,"longToShort") == 0 || strcmp (step,"full_closure") == 0)
    {
        //Partial accrue interest aggregator
        PSQLJoinQueryIterator*  longToShortTermQuery = LongToShortTerm::aggregator(closure_date_string);

        BlnkTemplateManager * longToShortTermTemplateManager = new BlnkTemplateManager(11, -1);
        LongToShortTermStruct longToShortTermStruct = {
            longToShortTermTemplateManager
        };
        longToShortTermQuery->process(threadsCount, LongToShortTermFunc, (void*)&longToShortTermStruct);
        delete(longToShortTermTemplateManager);
        delete(longToShortTermQuery);
        psqlController.ORMCommit(true,true,true, "main"); 


        LongToShortTerm::update_step();
    }

    if ( strcmp (step,"iScoreNidInquiry") == 0 || strcmp (step,"full_closure") == 0)
    {
        //Partial accrue interest aggregator
        cout << "Starting IScore NID inquiry step!" << endl;
        PSQLJoinQueryIterator*  iScoreNidInquiryQuery = IScoreNidInquiry::aggregator(closure_date_string);
        BlnkTemplateManager * iScoreNidInquiryTemplateManager = new BlnkTemplateManager(3, -1);
        IScoreNidInquiryStruct iScoreNidInquiryStruct;
        iScoreNidInquiryStruct.blnkTemplateManager = iScoreNidInquiryTemplateManager;
        cout << "IScore inquiry fee :";
        cout << get_iscore_nid_inquiry_fee() << endl;
        iScoreNidInquiryStruct.inquiryFee = get_iscore_nid_inquiry_fee();
        iScoreNidInquiryQuery->process(threadsCount, IScoreNidInquiryFunc, (void*)&iScoreNidInquiryStruct);
        delete(iScoreNidInquiryTemplateManager);
        delete(iScoreNidInquiryQuery);
        psqlController.ORMCommit(true,true,true, "main"); 
        LongToShortTerm::update_step();
    }


    return 0;
}