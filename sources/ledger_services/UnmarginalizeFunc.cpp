#include <UnmarginalizeFunc.h>

void UnmarginalizeFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    loan_app_loan_primitive_orm * lal_orm  = ORML(loan_app_loan,orms_list,0);
    // PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms_list,0);
    // string last_entry_date_string = gorm->get("last_entry_date");

    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UnmarginalizeStruct *) extras)->blnkTemplateManager,partition_number);
    map <string,map<int,pair<new_lms_installmentextension_primitive_orm*,vector<vector<new_lms_installmentlatefees_primitive_orm*> *> *> *> *> * date_map = Unmarginalize::get_date_map(orms_list);

    //write nested for loop on map for each run the inference
    for (auto itr = date_map->cbegin(); itr != date_map->cend(); ++itr) 
    {
        for (auto itr1 = itr->second->cbegin(); itr1 != itr->second->cend(); ++itr1) 
        {
            Unmarginalize unmarginalize = Unmarginalize(itr1->second->first, itr1->second->second);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&unmarginalize);
            unmarginalize.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

        }
        //create entry after finishing the inside loop

    }
   
    
    // loan_app_installment_primitive_orm* lai_orm;
    // new_lms_installmentextension_primitive_orm* nlie_orm = ORML(new_lms_installmentextension, orms_list, 0);
    // new_lms_installmentlatefees_primitive_orm* nlilf_orm;

    
    // map<string, LedgerAmount*>* ledgerAmounts;

    // ledger_entry_primitive_orm* entry = nullptr;
    // BlnkTemplateManager* loopTemplateManager;

    
    // for (int i=0; i<ORML_SIZE(orms_list); i++) {
    //     loopTemplateManager = new BlnkTemplateManager(localTemplateManager, partition_number);
    //     lai_orm = ORML(loan_app_installment, orms_list, i);
    //     nlie_orm = ORML(new_lms_installmentextension, orms_list, i);
    //     nlilf_orm = ORML(new_lms_installmentlatefees, orms_list, i);

    //     if (nlilf_orm == nullptr) {
    //         cout << "nlilf_orm is nullptr at iteration ";
    //         continue; 
    //         cout << "after continue " ;

    //     }
    //     BDate due_to_overdue_date(nlilf_orm->get_day());
    //     int payment_status = nlie_orm->get_payment_status();
    //     BDate principal_paid_at(nlie_orm->get_principal_paid_at());
    //     BDate late_fee_paid_at(nlilf_orm->get_paid_at());
    //     bool late_fee_is_paid = nlilf_orm->get_is_paid();
    //     BDate late_fee_cancellation_date(nlilf_orm->get_cancellation_date());
    //     bool late_fee_is_cancelled = nlilf_orm->get_is_cancelled();
    //     BDate installment_due_to_overdue_date(nlie_orm->get_due_to_overdue_date());
    //     if (
    //         (payment_status == 2 && principal_paid_at() >= due_to_overdue_date())
    //         || (payment_status != 2)
    //         || (late_fee_is_paid && late_fee_paid_at() >= due_to_overdue_date())
    //         || (late_fee_is_cancelled && late_fee_cancellation_date() >= due_to_overdue_date())
    //     ) {
    //         if (entry == nullptr) {
    //             localTemplateManager->createEntry(due_to_overdue_date);
    //             entry = localTemplateManager->get_entry();
    //         }
    //         loopTemplateManager->setEntry(entry);
    //         DueToOverdue dueToOverdue(lal_orm, lai_orm, nlie_orm, nlilf_orm, due_to_overdue_date);
    //         LedgerClosureService* ledgerClosureService = new LedgerClosureService(&dueToOverdue);
    //         dueToOverdue.setupLedgerClosureService(ledgerClosureService);
    //         ledgerAmounts = ledgerClosureService->inference();
    //         if (ledgerAmounts != nullptr) {
    //             loopTemplateManager->setEntryData(ledgerAmounts);
    //             loopTemplateManager->buildEntry(due_to_overdue_date);
    //             map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms = loopTemplateManager->get_ledger_amount_orms();
    //             if (ledger_amount_orms->size()) {
    //                 dueToOverdue.stampORMs(ledger_amount_orms);
    //                 // delete(ledger_amount_orms);
    //             }
    //             else {
    //                 cerr << "No legs created\n";
    //                 // delete(ledger_amount_orms);
    //                 exit(1);
    //             }
    //         }
    //         else {
    //             cout << "No Due to overdue amounts created\n";
    //         }
    //     }

        
    //     // delete(loopTemplateManager);
    //     // delete(ledgerClosureService);
    //     // delete(ledgerAmounts);
    // }
    delete(localTemplateManager);
};
