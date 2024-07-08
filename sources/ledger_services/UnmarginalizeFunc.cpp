#include <UnmarginalizeFunc.h>

void UnmarginalizeFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UnmarginalizeStruct *) extras)->blnkTemplateManager, partition_number);
    // DisburseLoan disburseLoan(orms_list, ((DisburseLoanStruct *) extras)->current_provision_percentage);
    // LedgerClosureService* ledgerClosureService = new LedgerClosureService(&disburseLoan);

    // loan_app_loan_primitive_orm * lal_orm = ORML(loan_app_loan,orms_list,0);



    // disburseLoan.setupLedgerClosureService(ledgerClosureService);
    // map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

    // if (ledgerAmounts != nullptr)
    // {
    //     localTemplateManager->setEntryData(ledgerAmounts);

    //     ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(lal_orm->get_loan_booking_day());
    //     ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();

    //     if (entry && la_orm) {
    //         disburseLoan.stampORMs(entry, la_orm);
    //     } else {
    //         cerr << "Cannot stamp ORM objects\n";
    //         exit(1);
    //     }
    // }

    delete localTemplateManager;
    // delete ledgerClosureService;
};
