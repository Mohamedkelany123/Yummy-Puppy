#include <UndueToDueFunc.h>

void InstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UndueToDueStruct *) extras)->blnkTemplateManager);
    BDate closing_day = ((UndueToDueStruct *) extras)->closing_day;
    
    UndueToDue undueToDue = UndueToDue(orms, closing_day);

    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&undueToDue);    
    undueToDue.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();



    localTemplateManager->setEntryData(ledgerAmounts);

    ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(closing_day);
    ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();

    if (entry && la_orm) {
        undueToDue.stampORMs(entry, la_orm);
    } else {
        cerr << "Cannot stamp ORM objects\n";
        //Stamp installment as failed in ledger step
    }

    delete localTemplateManager;
    delete ledgerClosureService;
};


