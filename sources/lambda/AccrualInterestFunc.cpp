#include <AccrualInterestFunc.h>

void AccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((AccrualInterestStruct *) extras)->blnkTemplateManager,partition_number);
    new_lms_installmentextension_primitive_orm * nli_orm = ORM(new_lms_installmentextension,orms);
    loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
    string accrual_date = nli_orm->get_accrual_date();
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
    string settled_history = gorm->get("settled_history");
    AccrualInterest accrualInterest(orms, 1);
    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&accrualInterest);
    accrualInterest.setupLedgerClosureService(ledgerClosureService);
    map <string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();
    localTemplateManager->setEntryData(ledgerAmounts);
    string stamping_date = accrual_date;
    if (settled_history != "" && nli_orm->get_payment_status() == 1 && BDate(settled_history)() < BDate(nli_orm->get_undue_to_due_date())()) {
        stamping_date = settled_history;
    }

    ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(stamping_date));
    map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

    if (entry) {
        accrualInterest.stampORMs(leg_amounts);
    }
    else {
        cerr << "Can not stamp ORM objects\n";
        exit(1);
    }
    lal_orm->set_closure_status(ledger_status::INTEREST_ACCRUAL);

    delete (ledgerClosureService);
};


void PartialAccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((AccrualInterestStruct *) extras)->blnkTemplateManager,partition_number);
    new_lms_installmentextension_primitive_orm * nli_orm = ORM(new_lms_installmentextension,orms);
    loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
    string stamping_date = nli_orm->get_partial_accrual_date();
    AccrualInterest accrualInterest(orms, 2);
    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&accrualInterest);
    accrualInterest.setupLedgerClosureService(ledgerClosureService);
    map <string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();
    localTemplateManager->setEntryData(ledgerAmounts);
    ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(stamping_date));
    map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

    if (entry) {
        accrualInterest.stampORMs(leg_amounts);
    }
    else {
        cerr << "Can not stamp ORM objects\n";
        exit(1);
    }
    lal_orm->set_closure_status(ledger_status::PARTIAL_INTEREST_ACCRUAL);

    delete (ledgerClosureService);
};

void SettlementAccrualInterestFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((AccrualInterestStruct *) extras)->blnkTemplateManager,partition_number);
    new_lms_installmentextension_primitive_orm * nli_orm = ORM(new_lms_installmentextension,orms);
    loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
    string stamping_date = nli_orm->get_settlement_accrual_interest_date();
    AccrualInterest accrualInterest(orms, 3);
    LedgerClosureService * ledgerClosureService = new LedgerClosureService(&accrualInterest);
    accrualInterest.setupLedgerClosureService(ledgerClosureService);
    map <string,LedgerAmount*> * ledgerAmounts = ledgerClosureService->inference();
    localTemplateManager->setEntryData(ledgerAmounts);
    ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(stamping_date));
    map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

    if (entry) {
        accrualInterest.stampORMs(leg_amounts);
    }
    else {
        cerr << "Can not stamp ORM objects\n";
        exit(1);
    }
    lal_orm->set_closure_status(ledger_status::SETTLEMENT_INTEREST_ACCRUAL);

    delete (ledgerClosureService);
};