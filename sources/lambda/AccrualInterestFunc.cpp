#include <AccrualInterestFunc.h>

void AccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((AccrualInterestStruct *) extras)->blnkTemplateManager);

    AccrualInterest accrualInterest(orms, 1);
    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&accrualInterest);
    accrualInterest.setupLedgerClosureService(ledgerClosureService);
    map <string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();
    ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate("2024-05-15"));
    map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

    if (entry) {
        accrualInterest.stampORMs(leg_amounts);
    }
    else {
        cerr << "Can not stamp ORM objects\n";
        exit(1);
    }
    delete (ledgerClosureService);
};
