#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>
#include <Disbursefunc.h>
#include <CancelFunc.h>
#include <AccrualInterest.h>
#include <AccrualInterestFunc.h>
#include <CancelLateFeesFunc.h>
#include <CancelLateFees.h>
#include <WalletPrepaidFunc.h>
#include <WalletPrepaid.h>
#include <UndueToDueFunc.h>
#include <InitialLoanInterestAccrualFunc.h>
#include <InitialLoanInterestAccrual.h>
#include <CreditIScore.h>
#include <CreditIScoreFunc.h>
#include <LongToShortTerm.h>
#include <LongToShortTermFunc.h>
#include <IScoreNidInquiry.h>
#include <IScoreNidInquiryFunc.h>
#include <PSQLUpdateQuery.h>
#include <CustomerPayment.h>
#include <CustomerPaymentFunc.h>
#include <OnboardingCommission.h>
#include <OnboardingCommissionFunc.h>
#include <DueToOverdueFunc.h>
#include <DueToOverdue.h>
#include <SettlementLoansWithMerchant.h>
#include <ReverseSettlementLoansWithMerchantFunc.h>



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

float get_iscore_credit_expense_fee(){
    ledger_global_primitive_orm_iterator * it = new ledger_global_primitive_orm_iterator("main");
    it->filter(
        ANDOperator(
        new UnaryOperator("ledger_global.name",eq,"iscore_credit_expense_fee")
        )
    );
    it->execute();
    ledger_global_primitive_orm * global_orm = it->next(true);
    if((global_orm->get_value())["amount"] != NULL){
        return global_orm->get_value()["amount"];
    }
    else cout << "ERROR in fetching iScore Credit expense amount" << endl;

    return -1;
}

int main (int argc, char ** argv)
{
    // const char * step = "full_closure"; 
    const char * step = "settlementLoansWithMerchant"; 
    string closure_date_string = "2024-07-06"; 
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

//accrual-undue_to_due

    if ( strcmp (step,"disburse") == 0 || strcmp (step,"full_closure") == 0)
    {
        cout << "Start: Disburse" << endl;
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
        cout << "End: Disburse" << endl;

    }


    if ( strcmp (step,"ledger_accruel_initial_interest") == 0 || strcmp (step,"full_closure") == 0)
    {
        cout << "Start: First Accrual" << endl;
        //FIRST ACCRUAL
        loan_app_loan_primitive_orm_iterator*  loans_to_get_first_accrual_agg = InitialLoanInterestAccrual::aggregator(closure_date_string, 1);
        
        BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(135, -1);

        InitialLoanInterestAccrualStruct initialLoanInterestFirstAccrualStruct;
        initialLoanInterestFirstAccrualStruct.blnkTemplateManager = blnkTemplateManager;
        initialLoanInterestFirstAccrualStruct.is_first_date = true;

        loans_to_get_first_accrual_agg->process(threadsCount, InitialLoanInterestAccrualFunc,(void *)&initialLoanInterestFirstAccrualStruct);

        delete(loans_to_get_first_accrual_agg);

        psqlController.ORMCommit(true,true,true, "main");  
        cout << "End: First Accrual" << endl;

        //-----------------------------------------------------------------------------------

        //SECOND ACCRUAL
        cout << "Start: Second Accrual" << endl;
        loan_app_loan_primitive_orm_iterator*  loans_to_get_second_accrual_agg = InitialLoanInterestAccrual::aggregator(closure_date_string, 2);
        
        InitialLoanInterestAccrualStruct initialLoanInterestSecondAccrualStruct;
        initialLoanInterestSecondAccrualStruct.blnkTemplateManager = blnkTemplateManager;
        initialLoanInterestSecondAccrualStruct.is_first_date = false;

        loans_to_get_second_accrual_agg->process(threadsCount, InitialLoanInterestAccrualFunc,(void *)&initialLoanInterestSecondAccrualStruct);

        delete(blnkTemplateManager);
        delete(loans_to_get_second_accrual_agg);

        psqlController.ORMCommit(true,true,true, "main");  
        InitialLoanInterestAccrual::update_step();
        cout << "End: Second Accrual" << endl;

    }

    if ( strcmp (step,"cancel_loan") == 0 || strcmp (step,"full_closure") == 0)
    {
        cout << "Start: Cancel Loan" << endl;
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
        cout << "End: Cancel Loan" << endl;

    }

    if ( strcmp (step,"accrual") == 0 || strcmp (step,"full_closure") == 0)
    {
        //Partial accrue interest aggregator
        PSQLJoinQueryIterator*  partialAccrualQuery = AccrualInterest::aggregator(closure_date_string, 1);

        BlnkTemplateManager * accrualTemplateManager = new BlnkTemplateManager(8, -1);
        AccrualInterestStruct partialAccrualInterestStruct = {
        accrualTemplateManager
        };
        partialAccrualQuery->process(threadsCount, PartialAccrualInterestFunc, (void*)&partialAccrualInterestStruct);
        delete(partialAccrualQuery);
        psqlController.ORMCommit(true,true,true, "main"); 

        //-------------------------------------------------------------------------------------------------------------------------------------------
        // Accrue interest aggregator
        PSQLJoinQueryIterator*  accrualQuery = AccrualInterest::aggregator(closure_date_string, 2);
        AccrualInterestStruct accrualInterestStruct = {
            accrualTemplateManager
        };
        accrualQuery->process(threadsCount, AccrualInterestFunc, (void*)&accrualInterestStruct);
        delete(accrualQuery);
        psqlController.ORMCommit(true,true,true, "main");  

        //-------------------------------------------------------------------------------------------------------------------------------------------
        // Settlement accrue interest aggregator
        PSQLJoinQueryIterator*  settlementAccrualQuery = AccrualInterest::aggregator(closure_date_string, 3);
        AccrualInterestStruct settlementAccrualInterestStruct = {
        accrualTemplateManager
        };
        settlementAccrualQuery->process(threadsCount, SettlementAccrualInterestFunc, (void*)&settlementAccrualInterestStruct);
        delete(accrualTemplateManager);
        delete(settlementAccrualQuery);
        psqlController.ORMCommit(true,true,true, "main");  
        AccrualInterest::update_step();
    }


    if ( strcmp (step,"undue_to_due") == 0 || strcmp (step,"full_closure") == 0)
    {
        cout << "Undue To Due" << endl;
        cout << "-Intallments Becoming Due" << endl;
        PSQLJoinQueryIterator*  installments_becoming_due_iterator = UndueToDue::aggregator(closure_date_string, 1);
        BlnkTemplateManager * undueToDueTemplateManager = new BlnkTemplateManager(10, -1);
        UndueToDueStruct undueToDueStruct;
        undueToDueStruct.blnkTemplateManager = undueToDueTemplateManager;
        undueToDueStruct.closing_day = BDate(closure_date_string);
        
        installments_becoming_due_iterator->process(threadsCount, InstallmentBecomingDueFunc, (void *)&undueToDueStruct);
        
        delete(installments_becoming_due_iterator);
        psqlController.ORMCommit(true,true,true, "main");  

        //----------------------------------------------------------------------------------------//
        cout << "-Sticky Installments Becoming Due" << endl;
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

    if ( strcmp (step,"cancel_latefees") == 0 || strcmp (step,"full_closure") == 0)
    {
        PSQLJoinQueryIterator*  cancel_late_fees_iterator = CancelLateFees::aggregator(closure_date_string);       
        BlnkTemplateManager * cancelLateFeesManager = new BlnkTemplateManager(50, -1);
        CancelLateFeesStruct cancelLateFeesStruct;
        cancelLateFeesStruct.blnkTemplateManager = cancelLateFeesManager;
        cancel_late_fees_iterator->process_aggregate(threadsCount, CancelLateFeesFunc, (void *)&cancelLateFeesStruct);
        delete(cancel_late_fees_iterator);
        psqlController.ORMCommit(true,true,true, "main");  
        CancelLateFees::update_step(); 
    }

    if ( strcmp (step,"wallet_prepaid") == 0 || strcmp (step,"full_closure") == 0)
    {
        
        new_lms_customerwallettransaction_primitive_orm_iterator*  wallet_prepaid_iterator = WalletPrepaid::aggregator(closure_date_string);       
        BlnkTemplateManager * walletPrepaidManager = new BlnkTemplateManager(134, -1);
        WalletPrepaidStruct walletPrepaidStruct;
        walletPrepaidStruct.blnkTemplateManager = walletPrepaidManager;
        wallet_prepaid_iterator->process(threadsCount, WalletPrepaidFunc, (void *)&walletPrepaidStruct);
        delete(wallet_prepaid_iterator);
        psqlController.ORMCommit(true,true,true, "main");  
        WalletPrepaid::update_step(); 
    }


    if ( strcmp (step,"long_to_short") == 0 || strcmp (step,"full_closure") == 0)
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

    if ( strcmp (step,"i_score_nid_inquiry") == 0 || strcmp (step,"full_closure") == 0)
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


    if ( strcmp (step,"onboarding_commission") == 0 || strcmp (step,"full_closure") == 0){
        cout << "Onboarding Commission" << endl;
        PSQLJoinQueryIterator*  onboarding_commissions_iterator = OnboardingCommission::aggregator(closure_date_string);
        BlnkTemplateManager * onboardingCommissionsTemplateManager = new BlnkTemplateManager(68, -1);

        OnboardingCommissionStruct onboardingCommissionStruct;
        onboardingCommissionStruct.blnkTemplateManager = onboardingCommissionsTemplateManager;

        onboarding_commissions_iterator->process(threadsCount, OnboardingCommissionFunc, (void *)&onboardingCommissionStruct);
        
        psqlController.ORMCommit(true,true,true, "main");  
        OnboardingCommission::update_step(); 
    }

    if (strcmp(step, "receiveCustomerPayments") == 0 || strcmp(step, "full_closure") == 0) {
        PSQLJoinQueryIterator* psqlJoinQueryIterator = CustomerPayment::aggregator(closure_date_string);
        map<int, BlnkTemplateManager*>* blnkTemplateManagerMap = new map<int, BlnkTemplateManager*>;
        int template_ids[] = {18, 19, 44, 165, 53, 119, 133};
        BlnkTemplateManager* blnkTemplateManager = nullptr;
        for (auto template_id : template_ids) {
            blnkTemplateManager = new BlnkTemplateManager(template_id, -1);
            blnkTemplateManagerMap->operator[](template_id) = blnkTemplateManager;
        }
        ReceiveCustomerPaymentStruct receiveCustomerPaymentStruct;
        receiveCustomerPaymentStruct.blnkTemplateManagerMap = blnkTemplateManagerMap;
        receiveCustomerPaymentStruct.closing_date = BDate(closure_date_string);
        psqlJoinQueryIterator->process(threadsCount, receiveCustomerPaymentFunc, (void*)&receiveCustomerPaymentStruct);
        delete psqlJoinQueryIterator;
        for(auto it=blnkTemplateManagerMap->begin(); it!=blnkTemplateManagerMap->end(); it++) {
            delete it->second;
        }
        psqlController.ORMCommit(true, true, true, "main");
        CustomerPayment::update_step();
    }
    
    if ( strcmp (step,"creditIScore") == 0 || strcmp (step,"full_closure") == 0)
    {
        PSQLJoinQueryIterator*  psqlQueryJoin = CreditIScore::aggregator(closure_date_string);

        CreditIScoreStruct creditIScoreStruct;
        BlnkTemplateManager *  blnkTemplateManager = new BlnkTemplateManager(1, -1);
        creditIScoreStruct.blnkTemplateManager = blnkTemplateManager;
        creditIScoreStruct.expense_fee = get_iscore_credit_expense_fee();
        psqlQueryJoin->process(threadsCount, CreditIScoreFunc,(void *)&creditIScoreStruct);
        
        delete(blnkTemplateManager);
        delete(psqlQueryJoin);
        psqlController.ORMCommit(true,true,true, "main"); 
    }

    if ( strcmp (step,"settlementLoansWithMerchant") == 0 || strcmp (step,"full_closure") == 0)
    {
        PSQLJoinQueryIterator*  psqlQueryJoin = SettlementLoansWithMerchant::paymentRequestAggregator(closure_date_string);
        
        SettlementLoansWithMerchantStruct settlementLoansWithMerchantStruct;
        SettlementLoansWithMerchant::unstampLoans();
        BlnkTemplateManager *  blnkTemplateManager = new BlnkTemplateManager(6, -1);
        reverseSettlementLoansWithMerchantStruct.blnkTemplateManager = blnkTemplateManager;
        set<int>* loan_ids = new set<int>;
        psqlQueryJoin->process_aggregate(threadsCount, getMerchantPaymentRequestLoansFunc,(void *)loan_ids);
        for(auto id : *loan_ids) {
            cout<<"loan id: "<<id<<endl;
        }
        PSQLJoinQueryIterator* psqlQueryJoinLoans = SettlementLoansWithMerchant::loanAggregator(closure_date_string, loan_ids);
        map<int, loan_app_loan_primitive_orm*> *loanMap = new map<int, loan_app_loan_primitive_orm*>;
        psqlQueryJoinLoans->process_aggregate(threadsCount, processLoanOrms,(void*)loanMap);

        psqlQueryJoin->process_aggregate(threadsCount, settleLoansWithMerchant, (void*) settlementLoansWithMerchantStruct);
        delete(blnkTemplateManager);
        delete(psqlQueryJoin);
        psqlController.ORMCommit(true,true,true, "main"); 
    }

    return 0;
}