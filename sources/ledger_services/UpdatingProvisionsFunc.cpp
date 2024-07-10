#include <UpdatingProvisionsFunc.h>

void UpdatingProvisionsFunc(map<string,PSQLAbstractORM*>* orms,int partition_number, mutex * shared_lock , void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UpdatingProvisionsStruct *) extras)->blnkTemplateManager,partition_number);

    UpdatingProvisions updatingProvisions(orms,((UpdatingProvisionsStruct *) extras)->startDate,((UpdatingProvisionsStruct *) extras)->endDate);

    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&updatingProvisions);
    updatingProvisions.setupLedgerClosureService(ledgerClosureService);
    updatingProvisions.setMissingProvisions();
    map<string,LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

    if(ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(((UpdatingProvisionsStruct *) extras)->closingDate));

        delete (ledgerClosureService);
    }
};