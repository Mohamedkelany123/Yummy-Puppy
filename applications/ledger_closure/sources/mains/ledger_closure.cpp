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
#include <Unmarginalize.h>
#include <UnmarginalizeFunc.h>
#include <UpdatingProvisions.h>
#include <UpdatingProvisionsFunc.h>
#include <DueToOverdueFunc.h>
#include <DueToOverdue.h>
#include <DueForSettlementWithMerchant.h>
#include <DueForSettlementWithMerchantFunc.h>
#include <MarginalizeIncome.h>
#include <MarginalizeIncomeFunc.h>

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


vector<string> get_start_and_end_fiscal_year(){
    ledger_global_primitive_orm_iterator * it = new ledger_global_primitive_orm_iterator("main");
    it->filter(
        OROperator(
        new UnaryOperator("ledger_global.name",eq,"start_fiscal_year"),
        new UnaryOperator("ledger_global.name",eq,"end_fiscal_year")
        )
    );
    it->execute();
    vector<string> vector(2);
    for(int i=0; i< 2;i++){
        ledger_global_primitive_orm * global_orm = it->next(true);
        if(global_orm->get_name()== "start_fiscal_year"){
            vector[0] = global_orm->get_value()["date"];
        }
        else{
            vector[1] = global_orm->get_value()["date"];
        }
    }
    return vector;
}

int main (int argc, char ** argv)
{
    // const char * step = "full_closure"; 
    const char * step = "unmarginalize_income"; 
    string databaseName = "c_plus_plus";
    string closure_date_string = "2024-07-18"; 
    int threadsCount = 1;
    bool connect = psqlController.addDataSource("main","192.168.1.51",5432,databaseName,"postgres","postgres");
    if (connect){
        cout << "--------------------------------------------------------" << endl;
        cout << "Connected to DATABASE->[" << databaseName << "]" << endl;
        cout << "Threads Count->[" << threadsCount << "]" << endl;
        cout << "Step[" << step << "]" << endl;
        cout << "Closing Day[" << closure_date_string << "]" << endl;
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


    if (strcmp(step, "due_to_overdue")==0 || strcmp(step, "full_closure")==0) {
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

    if ( strcmp (step,"iscore_nid_inquiry") == 0 || strcmp (step,"full_closure") == 0)
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

    if ( strcmp (step,"unmarginalize_income") == 0 || strcmp (step,"full_closure") == 0){
        cout << "Unmarginalize Income" << endl;
        PSQLJoinQueryIterator*  UnmarginalizeQuery = Unmarginalize::aggregator(closure_date_string);
        BlnkTemplateManager * UnmarginalizeIncomeTemplateManager = new BlnkTemplateManager(33, -1);

        UnmarginalizeStruct unmarginalizeIncomeStruct;
        unmarginalizeIncomeStruct.blnkTemplateManager = UnmarginalizeIncomeTemplateManager;
        unmarginalizeIncomeStruct.closure_date_string = closure_date_string;

        UnmarginalizeQuery->process_aggregate(threadsCount, UnmarginalizeFunc, (void*)&unmarginalizeIncomeStruct);
        
        psqlController.ORMCommit(true,true,true, "main");  
        OnboardingCommission::update_step(); 
    }

    if (strcmp(step, "receive_customer_payments") == 0 || strcmp(step, "full_closure") == 0) {
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

    if ( strcmp (step,"updating_provisions") == 0 || strcmp (step,"full_closure") == 0){
        cout << "Updating Provisions" << endl;
        std::vector<std::string> dates = get_start_and_end_fiscal_year();
        BlnkTemplateManager * updatingProvisionsTemplateManager = new BlnkTemplateManager(22, -1);
        UpdatingProvisionsStruct updatingProvisionsStruct;
        updatingProvisionsStruct.blnkTemplateManager = updatingProvisionsTemplateManager;
        updatingProvisionsStruct.closingDate = closure_date_string;
        updatingProvisionsStruct.startDate = dates[0];
        updatingProvisionsStruct.endDate = dates[1];
        // PSQLJoinQueryIterator*  updating_provisions_iterator = UpdatingProvisions::aggregator(closure_date_string,dates[0],dates[1],closure_date_string);
        // loan_app_loan_primitive_orm_iterator*  updating_provisions_iterator = UpdatingProvisions::aggregator(closure_date_string,dates[0],dates[1],closure_date_string);
        //ON Balance
        loan_app_loan_primitive_orm_iterator* updating_provisions_onbalance_iterator = UpdatingProvisions::aggregator_onbalance(closure_date_string,dates[0],dates[1],closure_date_string);
        updating_provisions_onbalance_iterator->process(threadsCount, UpdatingProvisionsFuncOn, (void *)&updatingProvisionsStruct);
        delete updating_provisions_onbalance_iterator;
        psqlController.ORMCommit(true,true,true, "main");  
        //OFF Balance
        PSQLJoinQueryIterator* updating_provisions_offbalance_iterator = UpdatingProvisions::aggregator_offbalance(closure_date_string,dates[0],dates[1],closure_date_string);
        updating_provisions_offbalance_iterator->process(threadsCount, UpdatingProvisionsFuncOff, (void *)&updatingProvisionsStruct);
        // updating_provisions_iterator->process_aggregate(threadsCount, UpdatingProvisionsFunc, (void *)&updatingProvisionsStruct);
        delete updating_provisions_offbalance_iterator;
        psqlController.ORMCommit(true,true,true, "main");  
        OnboardingCommission::update_step(); 
        delete updatingProvisionsTemplateManager;
    }
    
    if ( strcmp (step,"credit_iscore") == 0 || strcmp (step,"full_closure") == 0)
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

    if (strcmp(step, "marginalize_income")==0 || strcmp(step, "full_closure")==0 || 1) {
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


    if ( strcmp (step,"due_for_settlement_with_merchant") == 0 || strcmp (step,"full_closure") == 0)
    {   
        vector<string> fascal_year_vars = get_start_and_end_fiscal_year();
        loan_app_loan_primitive_orm_iterator*  dueForSettlementIterator = DueForSettlement::aggregator(closure_date_string, fascal_year_vars[0]);

        BlnkTemplateManager *  blnkTemplateManager = new BlnkTemplateManager(27, -1);
        DueForSettlementStruct dueForSettlementStruct;
        dueForSettlementStruct.blnkTemplateManager = blnkTemplateManager;

        dueForSettlementIterator->process(threadsCount, dueForSettlementWithMerchantFunc,(void *)&dueForSettlementStruct);


        delete(blnkTemplateManager);
        delete(dueForSettlementIterator);
        psqlController.ORMCommit(true,true,true, "main"); 
    
        DueForSettlement::update_step();

    }
    return 0;
}