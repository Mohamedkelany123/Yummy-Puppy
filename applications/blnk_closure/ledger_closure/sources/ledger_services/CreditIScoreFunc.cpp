#include <CreditIScoreFunc.h>

void CreditIScoreFunc (map<string,PSQLAbstractORM *> * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((CreditIScoreStruct *) extras)->blnkTemplateManager, partition_number);

    credit_app_inquirylog_primitive_orm * cail_orm = ORM(credit_app_inquirylog,orms_list);
    ekyc_app_onboardingsession_primitive_orm * eyobs_orm = ORM(ekyc_app_onboardingsession,orms_list);

    CreditIScore creditIScore(orms_list, ((CreditIScoreStruct *) extras)->expense_fee);
    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&creditIScore);


    creditIScore.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    if (ledgerAmounts != nullptr)
    {
        localTemplateManager->setEntryData(ledgerAmounts);

        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(cail_orm->get_created_at());
        ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();
        cout << la_orm->get_amount() << endl;
        if (entry && la_orm) {
            creditIScore.stampORMs(entry, la_orm);
        } else {
            cerr << "Cannot stamp ORM objects\n";
            exit(1);
        }
    }

    delete localTemplateManager;
    delete ledgerClosureService;
};
