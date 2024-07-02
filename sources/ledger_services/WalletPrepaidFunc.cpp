#include <WalletPrepaidFunc.h>

void WalletPrepaidFunc (new_lms_customerwallettransaction_primitive_orm*  transaction_orm, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((WalletPrepaidStruct *) extras)->blnkTemplateManager, partition_number);
    BDate closing_day = ((WalletPrepaidStruct *) extras)->closing_day;
    
    WalletPrepaid walletPrepaid = WalletPrepaid(transaction_orm, closing_day);

    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&walletPrepaid);    
    walletPrepaid.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    cout<<"ammout"<<ledgerAmounts->begin()->second->getAmount()<<endl;
    if (ledgerAmounts != nullptr)
    {    
        localTemplateManager->setEntryData(ledgerAmounts);
        
        localTemplateManager->buildEntry(transaction_orm->get_created_at());
        ledger_amount_primitive_orm *   la_orm = localTemplateManager->getFirstLedgerAmountORM();
        walletPrepaid.stampORMs(la_orm);
       
    }
    delete localTemplateManager;
    delete ledgerClosureService;
};

