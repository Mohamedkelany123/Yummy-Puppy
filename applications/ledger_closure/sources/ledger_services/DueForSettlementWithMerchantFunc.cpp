#include <DueForSettlementWithMerchantFunc.h>


void dueForSettlementWithMerchantFunc(loan_app_loan_primitive_orm * _lal_orm, int partition_number, mutex* shared_lock,void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((DueForSettlementStruct *) extras)->blnkTemplateManager, partition_number);
    
    DueForSettlement dueForSettlementWithMerchant(_lal_orm);
    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&dueForSettlementWithMerchant);
    
    dueForSettlementWithMerchant.setupLedgerClosureService(ledgerClosureService);

    map<string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();


    if(ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);

        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(_lal_orm->get_settlement_to_merchant_date()));

        if (entry){
            dueForSettlementWithMerchant.stampORMS(entry);
        }
        else {
            cerr << "Can not stamp ORM objects\n";
        }
    }

    delete (ledgerClosureService);
    delete (localTemplateManager);
}
