#include <lambdas.h>

// function<void(map<string, PSQLAbstractORM*>*, int, mutex*)> DisburseLoanFunc(BlnkTemplateManager* blnkTemplateManager, float current_provision_percentage) {
//     return [blnkTemplateManager, current_provision_percentage](map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock) {
        
//         BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(*blnkTemplateManager);
//         DisburseLoan disburseLoan(orms, current_provision_percentage);
//         LedgerClosureService* ledgerClosureService = new LedgerClosureService(&disburseLoan);

//         disburseLoan.setupLedgerClosureService(ledgerClosureService);
//         map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
//         localTemplateManager->setEntryData(ledgerAmounts);

//         ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate("2024-05-15"));
//         ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();

//         if (entry && la_orm) {
//             disburseLoan.stampORMs(entry, la_orm);
//         } else {
//             cerr << "Cannot stamp ORM objects\n";
//             exit(1);
//         }

//         delete localTemplateManager;
//         delete ledgerClosureService;
//     };
// }

void DisburseLoanFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {

        BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((DisburseLoanStruct *) extras)->blnkTemplateManager);
        DisburseLoan disburseLoan(orms, ((DisburseLoanStruct *) extras)->current_provision_percentage);
        LedgerClosureService* ledgerClosureService = new LedgerClosureService(&disburseLoan);

        disburseLoan.setupLedgerClosureService(ledgerClosureService);
        map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
        localTemplateManager->setEntryData(ledgerAmounts);

        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate("2024-05-15"));
        ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();

        if (entry && la_orm) {
            disburseLoan.stampORMs(entry, la_orm);
        } else {
            cerr << "Cannot stamp ORM objects\n";
            exit(1);
        }

        delete localTemplateManager;
        delete ledgerClosureService;
    };
