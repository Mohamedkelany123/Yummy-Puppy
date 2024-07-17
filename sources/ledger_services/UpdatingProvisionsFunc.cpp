#include <UpdatingProvisionsFunc.h>

void UpdatingProvisionsFunc(vector<map<string,PSQLAbstractORM*>*>* orms,int partition_number, mutex * shared_lock , void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UpdatingProvisionsStruct *) extras)->blnkTemplateManager,partition_number);

    cout << "-------------------IN FUNC----------------" << endl;
    // loan_app_loan_primitive_orm * lal = ORML(loan_app_loan, orms,0);
    loan_app_installment_primitive_orm *  lai = ORML(loan_app_installment,orms,0);
    cout << "LOAN ID : " << lai->get_loan_id() << endl;
    // loan_app_installment_primitive_orm * lai = ORML(loan_app_installment, orms);
    tms_app_loaninstallmentfundingrequest_primitive_orm * tlai = ORML(tms_app_loaninstallmentfundingrequest, orms,0);

    // cout << "BOND ID->" <<  tlai->get_funding_facility_id() << endl;
    PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms,0);
    cout << "loans_rec_balance_on : " << gorm->toFloat("loans_rec_balance_on")<< endl;
    cout << "loans_rec_balance_off : " << gorm->toFloat("loans_rec_balance_off")<< endl;
    cout << "long_term_balance_on : " << gorm->toFloat("long_term_balance_on")<< endl;
    cout << "long_term_balance_off : " << gorm->toFloat("long_term_balance_off")<< endl;
    cout << "Impairment provisions balance on : " << gorm->toFloat("impairment_provisions_balance_on") << endl;
    cout << "Impairment provisions balance off : " << gorm->toFloat("impairment_provisions_balance_off") << endl;
    // if(tlai->get_funding_facility_id() == 0){
    // cout << "impairment_provisions_balance TO TAKE IS ON BALANCE : " << gorm->toFloat("impairment_provisions_balance_on")<< endl;
    // }
    // else{
    //         cout << "impairment_provisions_balance TO TAKE IS OFF BALANCE : " << gorm->toFloat("impairment_provisions_balance_off")<< endl;
    // }
    cout << "history_provision_percentage : " << gorm->toInt("history_provision_percentage")<< endl;
    cout << "loan_provision_percentage : " << gorm->toInt("loan_provision_percentage")<< endl;
    // UpdatingProvisions updatingProvisions(orms,((UpdatingProvisionsStruct *) extras)->startDate,((UpdatingProvisionsStruct *) extras)->endDate);

    // LedgerClosureService * ledgerClosureService = new LedgerClosureService(&updatingProvisions);
    // updatingProvisions.setupLedgerClosureService(ledgerClosureService);
    // updatingProvisions.setMissingProvisions();
    // map<string,LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    
    // if(ledgerAmounts != nullptr)
    // {
    //     localTemplateManager->setEntryData(ledgerAmounts);
    //     ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(((UpdatingProvisionsStruct *) extras)->closingDate));

    //     delete (ledgerClosureService);
    // }
};

void UpdatingProvisionsFuncOn(loan_app_loan_primitive_orm* loan,int partition_number, mutex * shared_lock , void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UpdatingProvisionsStruct *) extras)->blnkTemplateManager,partition_number);

    UpdatingProvisions updatingProvisions(loan,((UpdatingProvisionsStruct *) extras)->startDate,((UpdatingProvisionsStruct *) extras)->endDate);

    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&updatingProvisions);
    updatingProvisions.setupLedgerClosureService(ledgerClosureService);

    map<string,LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    if(ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(((UpdatingProvisionsStruct *) extras)->closingDate));

        delete (ledgerClosureService);
    }
};

void UpdatingProvisionsFuncOff(map<string,PSQLAbstractORM*>* orms,int partition_number, mutex * shared_lock , void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UpdatingProvisionsStruct *) extras)->blnkTemplateManager,partition_number);

    UpdatingProvisions updatingProvisions(orms,((UpdatingProvisionsStruct *) extras)->startDate,((UpdatingProvisionsStruct *) extras)->endDate);

    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&updatingProvisions);
    updatingProvisions.setupLedgerClosureService(ledgerClosureService);

    map<string,LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    
    if(ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(((UpdatingProvisionsStruct *) extras)->closingDate));

        delete (ledgerClosureService);
    }
};