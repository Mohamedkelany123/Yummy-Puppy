#include <LongToShortTermFunc.h>

void LongToShortTermFunc(map<string,PSQLAbstractORM*>*orms, int partition_number, mutex* shared_lock,void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((LongToShortTermStruct *) extras)->blnkTemplateManager,partition_number);
    new_lms_installmentextension_primitive_orm * nli_orm = ORM(new_lms_installmentextension, orms);

    LongToShortTerm longToShortTerm(orms);
    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&longToShortTerm);
    longToShortTerm.setupLedgerClosureService(ledgerClosureService);
    map<string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();

    if(ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);
        string stamping_date = nli_orm->get_long_to_short_term_date();

        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(stamping_date));
        map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

        if (entry){
            longToShortTerm.stampORMs(leg_amounts);
        }
        else {
            cerr << "Can not stamp ORM objects\n";
            exit(1);
        }
    }

    delete (ledgerClosureService);


};