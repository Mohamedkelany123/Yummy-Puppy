#include <UpdatingProvisionsFunc.h>

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