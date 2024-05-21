#include <UndueToDue.h>

void InstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    // loan_app_installment_primitive_orm * lai_orm = ORM(loan_app_loan,orms);




    // BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((DisburseLoanStruct *) extras)->blnkTemplateManager);
    // DisburseLoan disburseLoan(orms, ((DisburseLoanStruct *) extras)->current_provision_percentage);
    // LedgerClosureService* ledgerClosureService = new LedgerClosureService(&disburseLoan);

    // disburseLoan.setupLedgerClosureService(ledgerClosureService);
    // map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    // localTemplateManager->setEntryData(ledgerAmounts);

    // ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate("2024-05-15"));
    // ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();

    // if (entry && la_orm) {
    //     disburseLoan.stampORMs(entry, la_orm);
    // } else {
    //     cerr << "Cannot stamp ORM objects\n";
    //     exit(1);
    // }

    // delete localTemplateManager;
    // delete ledgerClosureService;
};
