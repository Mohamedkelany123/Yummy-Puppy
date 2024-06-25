#include "DueToOverdueFunc.h"

void InstallmentBecomingOverdueFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    loan_app_loan_primitive_orm * _lal_orm  = ORML(loan_app_loan,orms_list,0);
    PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms_list,0);
    string last_entry_date_string = gorm->get("last_entry_date");

    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((DueToOverdueStruct *) extras)->blnkTemplateManager,partition_number);
    
    vector<loan_app_installment_primitive_orm*>* lai_orms;
    vector<new_lms_installmentextension_primitive_orm*> * nlie_orms;
    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        lai_orms->push_back(ORML(loan_app_installment, orms_list, i));
    }
    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        nlie_orms->push_back(ORML(new_lms_installmentextension, orms_list, i));
    }

    DueToOverdue dueToOverdue(_lal_orm, lai_orms, nlie_orms);


    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&dueToOverdue);
    dueToOverdue.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

    if (ledgerAmounts != nullptr) {
        // localTemplateManager->
    }
    else {
        cout << "No Due to overdue amounts created\n";
    }

    if (last_entry_date_string == "") {
        last_entry_date_string = (*nlie_orms)[0]->get_due_to_overdue_date();
    }
    BDate due_to_overdue_date = BDate(last_entry_date_string);
    for (int i=0; i<36; i++) {
        due_to_overdue_date.inc_months(i);
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(due_to_overdue_date);
    }


}